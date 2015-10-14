#include "cs_app.h"

#include <cinder/app/RendererGl.h>
#include <cinder/gl/gl.h>
#include "kt_environment.h"
#include "velocity_generator.h"

namespace cs {

BasicApp::BasicApp()
		: mPicker(mCamera)
		, mParticleRender(mCns, mSettings) {
	
	// SETUP PICKER
	const glm::vec2		window_size(static_cast<float>(getWindowWidth()), static_cast<float>(getWindowHeight()));
	mPicker.setTo(window_size);

	// SETUP FBO
	ci::gl::Fbo::Format format;
	mFbo = ci::gl::Fbo::create(getWindowWidth(), getWindowHeight(), true, false, false); //format.disableDepth() );
	if (!mFbo) throw std::runtime_error("App can't create FBO");

	// SETUP BATCH
	const glm::vec2			tc_ul(0.0f, 0.0f),
							tc_ur(1.0f, 0.0f),
							tc_lr(1.0f, 1.0f),
							tc_ll(0.0f, 1.0f);
	ci::gl::VboMeshRef		mesh = ci::gl::VboMesh::create(ci::geom::Rect(ci::Rectf(0, 0, window_size.x, window_size.y)).texCoords(tc_ul, tc_ur, tc_lr, tc_ll));
	auto glsl = ci::gl::GlslProg::create(	ci::loadFile(kt::env::expand("$(DATA)/shaders/saturate.vert")),
											ci::loadFile(kt::env::expand("$(DATA)/shaders/saturate.frag")) );
	if (!glsl) throw std::runtime_error("App can't create shader");
	mBatch = ci::gl::Batch::create(mesh, glsl);

	// SETUP CAMERA
	mCameraOrtho.setOrtho(0.0f, window_size.x, window_size.y, 0.0f, -1.0f, 1.0f);

	// SETUP VELOCITY VOXELS
	createVelocityCube(mSettings.mPlaneDepth, mCns.mVelocities);
//	RandomGenerator			gen(mSettings.mRndMin, mSettings.mRndMax);
	PolyLineGenerator		gen(ci::PolyLine3f(), mSettings.mRndMin, mSettings.mRndMax);
	gen.update(mCns.mVelocities);

	// SETUP PARTICLES
	math::Cube				cube(mCns.mVelocities.getBounds());
	for (size_t k=0; k<mSettings.mParticleCount; ++k) {
		const glm::vec3		unit_pos(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat());
		const glm::vec3		pos(cube.atUnit(unit_pos));
		ParticleRef			p = std::make_shared<Particle>(pos);
		if (!p) continue;
		p->mColor = Particle::encodeColor(ci::ColorA(1, 1, 1, 1));
		mParticleRender.push_back(p);
	}
}

void BasicApp::prepareSettings(Settings* s) {
	if (s) {
//		s->setTitle("C. Clara Run");
		s->setWindowSize(glm::ivec2(1920, 1080));
//		s->setFullScreen(true);
//		s->setConsoleWindowEnabled(true);
	}
}

void BasicApp::setup() {
	base::setup();

#if 0
	for (const auto& plane : mCns.mVelocities.mPlanes) {
		int32_t w = plane.mUR.x - plane.mLL.x + 1;
		int32_t h = plane.mUR.y - plane.mLL.y + 1;
		std::cout << "z=" << plane.mZ << " w=" << w << " h=" << h << " size=" << (w*h) << std::endl;
	}
#endif
}

void BasicApp::mouseDrag(ci::app::MouseEvent event ) {
}

void BasicApp::keyDown(ci::app::KeyEvent event ) {
	if( event.getChar() == 'f' ) {
		// Toggle full screen when the user presses the 'f' key.
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == ci::app::KeyEvent::KEY_ESCAPE ) {
		// Exit full screen, or quit the application, when the user presses the ESC key.
		if( isFullScreen() )
			setFullScreen( false );
		else
			quit();
	}
}

void BasicApp::onUpdate() {
	mParticleRender.update();
}

void BasicApp::onDraw() {
	// Draw to framebuffer
	{
		ci::gl::ScopedFramebuffer fbScp(mFbo);
		ci::gl::clear();
		ci::gl::clear(ci::ColorA(0, 0, 0, 0));
		ci::gl::ScopedViewport scpVp(glm::ivec2(0), mFbo->getSize());
		ci::gl::setMatrices(mCamera);
		mParticleRender.draw();
	}

	// Draw the framebuffer
	ci::gl::setMatrices(mCameraOrtho);
	ci::gl::clear(ci::Color(0.11f, 0.88f, 0.08f));
	mFbo->bindTexture();
	ci::gl::color(1, 1, 1);
	mBatch->draw();
	mFbo->unbindTexture();
}

void BasicApp::createVelocityCube(const int32_t max_z, VelocityCube &cube) const {
	// Turn my perspective view into a voxel space.
	cube.clear();
	if (max_z <= 0) {
		assert(false);
		return;
	}

	const glm::ivec2		screen_ll(0, getWindowHeight()),
							screen_ur(getWindowWidth(), 0);
	glm::vec3				world_ll,
							world_ur;
	for (int32_t z = 0; z<max_z; ++z) {
		const float			world_z = static_cast<float>(-z);
		if (mPicker.pick(screen_ll, world_z, world_ll)
				&& mPicker.pick(screen_ur, world_z, world_ur)) {
			cube.mPlanes.push_back(VelocityPlane());
			VelocityPlane&	plane(cube.mPlanes.back());
			plane.mZ = world_z;
			// Give a little buffer so particles can float off
			plane.mLL = glm::ivec2(	static_cast<int>(std::lround(floorf(world_ll.x))) - 1,
									static_cast<int>(std::lround(floorf(world_ll.y))) - 1);
			plane.mUR = glm::ivec2(	static_cast<int>(std::lround(ceilf(world_ur.x))) + 1,
									static_cast<int>(std::lround(ceilf(world_ur.y))) + 1);
			plane.mOutOfBounds = ci::Rectf(	static_cast<float>(plane.mLL.x), static_cast<float>(plane.mLL.y),
											static_cast<float>(plane.mUR.x + 1), static_cast<float>(plane.mUR.y + 1));
		}	
	}
}

} // namespace cs

// This line tells Cinder to actually create and run the application.
CINDER_APP(cs::BasicApp, ci::app::RendererGl, cs::BasicApp::prepareSettings)