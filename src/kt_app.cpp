#include "kt_app.h"

#include <cinder/gl/gl.h>
#include <cinder/gl/scoped.h>
#include "kt_environment.h"

namespace kt {

/**
 * @class kt::App
 */
App::App()
		: mEnvInitialized(kt::env::initialize()) {

	// SETUP CAMERA
	const float			verticalFovDegrees = 60.0f,
						aspectRatio = getWindowAspectRatio(),
						nearPlane = 2.0f,
						farPlane = 3000.0f;
	mCamera.setPerspective(verticalFovDegrees, aspectRatio, nearPlane, farPlane);
	mCamera.setWorldUp(glm::vec3(0, 1, 0));
	mCamera.lookAt(glm::vec3(0, 0, mStdDist), glm::vec3(0, 0, 0));

	// SETUP METRICS
	const glm::vec2		window_size(static_cast<float>(getWindowWidth()), static_cast<float>(getWindowHeight()));
	// Figure out the pixel size of a single cell at my standard distance. This will
	// guide the size of assets I generate.
	glm::vec2			ll = mCamera.worldToScreen(glm::vec3(0, 0, 0), window_size.x, window_size.y),
						ur = mCamera.worldToScreen(glm::vec3(1, 1, 0), window_size.x, window_size.y);
	const glm::vec2		s(ur.x-ll.x, ll.y-ur.y);
	mCns.mCellSizeInPixelsRaw = s;
	mCns.mCellSizeInPixels = glm::ivec2(static_cast<int>(std::lround(floorf(s.x))),
										static_cast<int>(std::lround(floorf(s.y))));
}

void App::update() {
	onUpdate();
}

void App::draw() {
	ci::gl::setMatrices(mCamera);
	ci::gl::clear(ci::Color::gray( 0.0f ) );
	ci::gl::ScopedDepth			sd(true);
	ci::gl::ScopedBlendAlpha	sba;

	onDraw();
}

} // namespace kt
