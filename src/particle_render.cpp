#include "particle_render.h"

#include <cinder/gl/Batch.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/scoped.h>
#include <cinder/CinderMath.h>
#include <cinder/ImageIo.h>
#include "kt/app/kt_cns.h"
#include "kt/app/kt_environment.h"
#include "feeder.h"
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
ParticleRender::ParticleRender(const kt::Cns &cns, const cs::Settings &settings, Feeder &f, ParticleList &p)
		: mCns(cns)
		, mSettings(settings)
		, mFeeder(f)
		, mParticles(p) {
	// Setup
	mAccentForces.fill(128);

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

void ParticleRender::update() {
	updateAccents();

	// Hold
	if (mStage == Stage::kHold) {
		if (mTimer.elapsed() >= mHoldDuration) {
			if (mFeeder.hasFrame()) {
				mFeeder.getFrame(mParticles);
				mTransitionDuration = mParticles.mTransitionDuration;
				mHoldDuration = mParticles.mHoldDuration;
				mStage = Stage::kTransition;
				mTimer.start();
			}
		}
	// Transition
	} else {
		if (mTimer.elapsed() >= mTransitionDuration) {
			mStage = Stage::kHold;
			mTimer.start();
		} else {
			const float		t = static_cast<float>(kt::math::s_curved(mTimer.elapsed() / mTransitionDuration));
			for (auto& p : mParticles) {
				p.mPosition = p.mCurve.point(t);
				p.mAlpha = glm::mix(p.mStartAlpha, p.mEndAlpha, t);

				// Blur out a little based on distance
				p.mAlpha *= mSettings.mRangeZ.convert(p.mPosition.z, kt::math::Rangef(0.1f, 1.0f));

				if (mAddAccentTick == 0 && p.mHasAccents && mAccentParticles.size() < mSettings.mAccentParticleCount) {
					mAccentParticles.push_back(Particle(p.mPosition, p.mAlpha * 0.25f));
				}
			}
		}
	}

	static const size_t		ADD_ACCENT_TICKS = 5;
	++mAddAccentTick;
	if (mAddAccentTick >= ADD_ACCENT_TICKS) mAddAccentTick = 0;
}

void ParticleRender::draw() {
	ci::gl::ScopedTextureBind	stb(mTexture);
	// Prevent writing to the depth buffer, which will block out
	// pixels that are supposed to be transparent.
	ci::gl::ScopedDepthWrite	sdw(false);
	ci::gl::ScopedBlendAlpha	sba;
	ci::gl::color(1.0f, 1.0f, 1.0f, 1.0f);

	drawParticles(mParticles);
	drawParticles(mAccentParticles);
}

void ParticleRender::updateAccents() {
	// Accents always fall down and fade out, with a little random forces thrown in.

	for (auto& p : mAccentParticles) {
		p.mAlpha -= 0.002f;
		if (p.mAlpha <= 0.0f) {
			std::swap(p, mAccentParticles.back());
		} else {
			p.mPosition.y += 0.04f;
			// Apply forces.
			glm::vec3		unit = mCns.mWorldBounds.toUnit(p.mPosition);
			glm::vec3		force = mAccentForces.at(unit);
			p.mPosition += (force * 0.00000000015f);
		}
	}
	while (!mAccentParticles.empty() && mAccentParticles.back().mAlpha <= 0.0f) {
		mAccentParticles.pop_back();
	}
}

void ParticleRender::drawParticles(const ParticleList &particles) {
	size_t						k = 0, size = particles.size();
	while (size >= BUFFER_SIZE) {
		drawParticles(k, k + BUFFER_SIZE, particles);
		k += BUFFER_SIZE;
		size -= BUFFER_SIZE;
	}
	drawParticles(k, particles.size(), particles);
}

void ParticleRender::drawParticles(size_t start, size_t end, const ParticleList &particles) {
	if (start >= particles.size()) return;
	if (end > particles.size()) end = particles.size();

	// update our instance positions; map our instance data VBO, write new positions, unmap
	glm::vec4 *data = (glm::vec4*)mInstanceDataVbo->mapReplace();
	for (size_t k=start; k<end; ++k) {
		const Particle&			p(particles[k]);
		data->x = p.mPosition.x;
		data->y = p.mPosition.y;
		data->z = p.mPosition.z;
		data->w = p.mAlpha;
		*data++;
	}
	mInstanceDataVbo->unmap();

	// and draw
	mBatch->drawInstanced(end-start);
}

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
