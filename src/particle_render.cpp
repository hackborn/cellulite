#include "particle_render.h"

#include <cinder/gl/Batch.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/scoped.h>
#include <cinder/CinderMath.h>
#include <cinder/ImageIo.h>
#include "kt_cns.h"
#include "kt_environment.h"
#include "settings.h"

namespace cs {

namespace {

/**
 * @func make_texture
 * @brief Create the standard jot texture.
 */
ci::gl::TextureRef	make_texture(const kt::Cns&);

}

/**
 * @class cs::ParticleRender
 */
ParticleRender::ParticleRender(const kt::Cns &cns, const cs::Settings &settings)
		: mCns(cns)
		, mSettings(settings) {
	// Load the texture
	mTexture = make_texture(cns);
	if (!mTexture) throw std::runtime_error("ParticleRender vbo can't create texture");

	// Load the shader
	mGlsl = ci::gl::GlslProg::create(	ci::loadFile(kt::env::expand("$(DATA)/shaders/particle_instanced.vert")),
										ci::loadFile(kt::env::expand("$(DATA)/shaders/particle_instanced.frag")) );
	if (!mGlsl) throw std::runtime_error("ParticleRender vbo can't create shader");

	// Create the mesh
	const glm::vec2			tc_ul(0.0f, 0.0f),
							tc_ur(1.0f, 0.0f),
							tc_lr(1.0f, 1.0f),
							tc_ll(0.0f, 1.0f);
	const float				hs = cns.mParticleSize.x/2.0f;
	ci::gl::VboMeshRef		mesh = ci::gl::VboMesh::create(ci::geom::Rect(ci::Rectf(-hs, -hs, hs, hs)).texCoords(tc_ul, tc_ur, tc_lr, tc_ll));
	if (!mesh) throw std::runtime_error("ParticleRender vbo can't create vbo mesh");

	// create an array of per-instance positions
	std::vector<glm::vec4>	data;
	data.resize(BUFFER_SIZE);

	// create the VBO which will contain per-instance (rather than per-vertex) data
	mInstanceDataVbo = ci::gl::Vbo::create( GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec4), data.data(), GL_DYNAMIC_DRAW );

	// we need a geom::BufferLayout to describe this data as mapping to the CUSTOM_0 semantic, and the 1 (rather than 0) as the last param indicates per-instance (rather than per-vertex)
	ci::geom::BufferLayout instanceDataLayout;
	instanceDataLayout.append(ci::geom::Attrib::CUSTOM_0, 4, 0, 0, 1 /* per instance */ );
	
	// now add it to the VboMesh we already made of the Teapot
	mesh->appendVbo( instanceDataLayout, mInstanceDataVbo );

	// and finally, build our batch, mapping our CUSTOM_0 attribute to the "vInstancePosition" GLSL vertex attribute
	mBatch = ci::gl::Batch::create( mesh, mGlsl, { { ci::geom::Attrib::CUSTOM_0, "vInstancePosition" } } );
}

void ParticleRender::push_back(const ParticleRef &j) {
	mParticles.push_back(j);
}

void ParticleRender::erase(const ParticleRef &j) {
	if (!j) return;
	mParticles.erase(j);
}

void ParticleRender::erase(const ParticleList &list) {
	mParticles -= list;
}

void ParticleRender::update() {
//return;
	mCns.mVelocities.update(mParticles);
	for (auto& p : mParticles) {
		if (!p) continue;
		p->mVelocity *= mSettings.mFriction;
		p->mPosition += p->mVelocity;
	}
}

void ParticleRender::draw() {
	ci::gl::ScopedTextureBind	stb(mTexture);
	// Prevent writing to the depth buffer, which will block out
	// pixels that are supposed to be transparent.
	ci::gl::ScopedDepthWrite	sdw(false);
	ci::gl::ScopedBlendAlpha	sba;
	ci::gl::color(1.0f, 1.0f, 1.0f, 1.0f);
	size_t						k = 0, size = mParticles.size();
	while (size >= BUFFER_SIZE) {
		drawParticles(k, k + BUFFER_SIZE);
		k += BUFFER_SIZE;
		size -= BUFFER_SIZE;
	}
	drawParticles(k, mParticles.size());
}

void ParticleRender::drawParticles(size_t start, size_t end) {
	if (start >= mParticles.size()) return;
	if (end > mParticles.size()) end = mParticles.size();

	// update our instance positions; map our instance data VBO, write new positions, unmap
	glm::vec4 *data = (glm::vec4*)mInstanceDataVbo->mapReplace();
	for (size_t k=start; k<end; ++k) {
		const ParticleRef&		p(mParticles[k]);
		data->x = p->mPosition.x;
		data->y = p->mPosition.y;
		data->z = p->mPosition.z;
		data->w = p->mColor;
		*data++;
	}
	mInstanceDataVbo->unmap();

	// and draw
	mBatch->drawInstanced(end-start);
}

#if 0
void JotManagerView::onUpdate(const kt::UpdateParams &p) {
	bool					has_delete = false;
	ci::Rectf				expand(-10.0f, +10.0f, +10.0f, -10.0f);
	ci::Rectf				out_of_frame(mMetrics.mWorldFrame + expand);
	std::swap(out_of_frame.y1, out_of_frame.y2);
	for (auto& j : mMoving) {
		glm::vec3&		pos(j->mPosition);
		pos += j->mVelocity;
		if (!out_of_frame.contains(kt::math::xy(pos))) {
			j->mDelete = true;
			has_delete = true;
			// XXX Want to recycle these
			erase(j);
		}
	}
	if (has_delete) {
		const auto cnd = [](const JotRef &j)->bool { return j->mDelete; };
		mMoving.erase(std::remove_if(mMoving.begin(), mMoving.end(), cnd), mMoving.end());
	}
}
#endif

namespace {

/**
 * @func make_texture
 */
ci::gl::TextureRef	make_texture(const kt::Cns &cns) {
	ci::Surface8u				src(cns.mCellSizeInPixelsRaw.x, cns.mCellSizeInPixelsRaw.y, true);
	auto						pit(src.getIter());
	const glm::vec2				cen(static_cast<float>(src.getWidth()) / 2.0f, static_cast<float>(src.getHeight()) / 2.0f);
	const float					r = fminf(cen.x, cen.y);
	const float					max_d(cen.x / 2.0f + cen.y / 2.0f);
	const float					min_d(max_d-(max_d * 0.15f));
	while (pit.line()) {
		while (pit.pixel()) {
			const glm::vec2		fpt(static_cast<float>(pit.x()), static_cast<float>(pit.y()));
			const float			d = glm::distance(fpt, cen);

			// COLOR
			uint8_t				a = 0;
			if (d < r) {
				a = static_cast<uint8_t>((1.0f-(d/r))*255.0f);			
			}
			pit.r() = pit.g() = pit.b() = 255;
			pit.a() = a;
		}
	}
	return ci::gl::Texture::create(src);
}

}

} // namespace cs
