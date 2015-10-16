#include "background.h"

#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/scoped.h>
#include "kt/math/geometry.h"
#include "noise.h"
#include "settings.h"

namespace cs {

/**
 * @class cs::Background
 */
Background::Background(const cs::Settings &s, const glm::ivec2 &window_size)
		: mSettings(s)
		, mWorkerWindowSize(window_size)
		, mWorkerColor(s.mBackgroundColor) {
	// Setup
	mHasWorkerSurface = false;

	// Create the mesh
	const glm::vec2			tc_ul(0.0f, 0.0f),
							tc_ur(1.0f, 0.0f),
							tc_lr(1.0f, 1.0f),
							tc_ll(0.0f, 1.0f);
	const float				fw = static_cast<float>(window_size.x),
							fh = static_cast<float>(window_size.y);
	ci::gl::VboMeshRef		mesh = ci::gl::VboMesh::create(ci::geom::Rect(ci::Rectf(0, 0, fw, fh)).texCoords(tc_ul, tc_ur, tc_lr, tc_ll));
	if (!mesh) throw std::runtime_error("Background vbo can't create vbo mesh");

	// Create shader
	auto					shader = ci::gl::getStockShader(ci::gl::ShaderDef().texture());
	if (!shader) throw std::runtime_error("Background vbo can't create shader");
	
	// Create batch
	mBatch = ci::gl::Batch::create(mesh, shader);
	if (!mBatch) throw std::runtime_error("Background vbo can't create batch");

	// Generate background
	mThread = std::thread([this](){generateImage();});
}

Background::~Background() {
	try {
		mThread.join();
	} catch (std::exception const&) {
	}
}

void Background::update() {
	if (mHasWorkerSurface) {
		mHasWorkerSurface = false;

		if (mWorkerSurface.getWidth() > 0 && mWorkerSurface.getHeight() > 0) {
			if (mTexture && mWorkerSurface.getWidth() == mTexture->getWidth() && mWorkerSurface.getHeight() == mTexture->getHeight()) {
				mTexture->update(mWorkerSurface);
			} else {
				mTexture = ci::gl::Texture2d::create(mWorkerSurface);
			}
		}
	}
}

void Background::draw() {
	ci::gl::clear(mSettings.mBackgroundColor);
	if (mTexture) {
		ci::gl::color(1, 1, 1, 1);
		ci::gl::ScopedTextureBind	ts(mTexture);
		ci::gl::ScopedDepth			sd(false);
		mBatch->draw();
	}
}

namespace {

uint8_t			f_color_to_i(const float v) {
	int32_t		iv = static_cast<int32_t>(v*255.0f);
	if (iv <= 0) return 0;
	if (iv >= 255) return 255;
	return static_cast<uint8_t>(iv);
}

// Answer a value of 0 - 1, where 1 is the edge of the screen, 0 the center
float			border_value(const float x, const float y, const float w, const float h, const float edge_d) {
	// LT 
	if (x < edge_d && y < edge_d) {
		const float	d = glm::distance(glm::vec2(x, y), glm::vec2(edge_d, edge_d)) / edge_d;
		if (d >= 1.0f) return 1.0f;
		return d;			
	// RT
	} else if (x >= w-edge_d && y < edge_d) {
		const float	d = glm::distance(glm::vec2(x, y), glm::vec2(w-edge_d, edge_d)) / edge_d;
		if (d >= 1.0f) return 1.0f;
		return d;			
	// T
	} else if (y < edge_d) {
		return glm::distance(y, edge_d) / edge_d;
	// LB
	} else if (x < edge_d && y > h-edge_d) {
		const float	d = glm::distance(glm::vec2(x, y), glm::vec2(edge_d, h-edge_d)) / edge_d;
		if (d >= 1.0f) return 1.0f;
		return d;			
	// RB
	} else if (x >= w-edge_d && y > h-edge_d) {
		const float	d = glm::distance(glm::vec2(x, y), glm::vec2(w-edge_d, h-edge_d)) / edge_d;
		if (d >= 1.0f) return 1.0f;
		return d;			
	// B
	} else if (y > h-edge_d) {
		return glm::distance(y, h-edge_d) / edge_d;
	// L
	} else if (x < edge_d) {
		return glm::distance(x, edge_d) / edge_d;
	// R
	} else if (x > w-edge_d) {
		return glm::distance(x, w-edge_d) / edge_d;
	}
	return 0.0f;
}

}

void Background::generateImage() {
	if (mWorkerWindowSize.x < 1 || mWorkerWindowSize.y < 1) return;

	ci::Surface8u		s = ci::Surface8u(mWorkerWindowSize.x, mWorkerWindowSize.y, true);
	if (s.getWidth() != mWorkerWindowSize.x || s.getHeight() != mWorkerWindowSize.y) return;

	Noise				noise(-1.0f, 1.0f);
	const float			fw(static_cast<float>(s.getWidth())),
						fh(static_cast<float>(s.getHeight()));
	const float			edge_d( ((fw + fh) / 2.0f) * 0.1f);
	std::vector<float>	frac_t, frac_b, frac_l;
	frac_t.resize(mWorkerWindowSize.x/16);
	frac_b.resize(mWorkerWindowSize.x/16);
	frac_l.resize(mWorkerWindowSize.y);
	midpoint_displacement(noise, frac_t);
	midpoint_displacement(noise, frac_b);
	midpoint_displacement(noise, frac_l);

	float				saturate = 0.25f;

	auto				pix(s.getIter());
	while (pix.line()) {
		while (pix.pixel()) {
			const glm::vec2		fpt(static_cast<float>(pix.x()), static_cast<float>(pix.y()));
			const glm::vec2		unit_fpt(fpt.x / fw, fpt.y / fh);
			const float			warp = kt::math::hermite_at(unit_fpt.y, frac_l) * 0.005f;
			float				offset = unit_fpt.x + warp;
			if (offset < 0.0f) offset = 0.0f;
			else if (offset > 1.0f) offset = 1.0f;
//			const float			t = kt::math::hermite_at(unit_fpt.x*warp, frac_t),
//								b = kt::math::hermite_at(unit_fpt.x*warp, frac_b);
			const float			t = kt::math::hermite_at(offset, frac_t),
								b = kt::math::hermite_at(offset, frac_b);
			float				v = glm::mix(t, b, unit_fpt.y);
			if (v <= 0.0f) {
				v = 0.0f;
			} else {
				v = -(kt::math::s_curvef(v) * saturate);
			}
			// This fractal thing looks really lame, let's turn it off
			v = 0.0f;

			// Apply a little per-pixel noise
			const float			ppn = ((noise.nextFloat() + 1.0f) / 2.0f) * (-0.075f);

			// Apply a border
			const float			bv = border_value(fpt.x, fpt.y, fw, fh, edge_d) * -0.15f;

			// Set the colors
			pix.r() = f_color_to_i(mWorkerColor.r + v + ppn + bv);
			pix.g() = f_color_to_i(mWorkerColor.g + v + ppn + bv);
			pix.b() = f_color_to_i(mWorkerColor.b + v + ppn + bv);

#if 0
v = kt::math::linear_at(unit_fpt.x, frac_t);
//		v = glm::mix(t, b, unit_fpt.y);
v = (v + 1.0f) / 2.0f;
uint8_t c = static_cast<uint8_t>(v * 255.0f);
pix.r() = pix.b() = pix.g() = c;
#endif
			pix.a() = 255;
		}
	}

	mWorkerSurface = s;
	mHasWorkerSurface = true;
}

} // namespace cs
