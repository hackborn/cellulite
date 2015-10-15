#include "cs_app.h"

#include <cinder/app/RendererGl.h>
#include <cinder/gl/gl.h>
#include "kt/app/kt_environment.h"

namespace cs {

BasicApp::BasicApp()
		: mPicker(mCamera)
		, mFeeder(mCns, mSettings)
		, mParticleRender(mCns, mSettings, mFeeder, mParticles) {
	
	// SETUP PICKER
	const glm::vec2		window_size(static_cast<float>(getWindowWidth()), static_cast<float>(getWindowHeight()));
	mPicker.setTo(window_size);

	// SETUP CAMERA
	mCameraOrtho.setOrtho(0.0f, window_size.x, window_size.y, 0.0f, -1.0f, 1.0f);

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

	// SETUP METRICS
	setupWorldBounds(0.0f, -80.0f, mCns);

	// SETUP PARTICLES
	GeneratorParams			gen_params(mCns);
	RandomGenerator			gen(RandomGenerator::Mode::kAnywhere);
	mParticles.resize(mSettings.mParticleCount);
	gen.update(gen_params, mParticles);
	for (auto& p : mParticles) {
		p.mCurve.mP0 = p.mCurve.mP3;
	}

	mFeeder.start(mParticles);
}

void BasicApp::prepareSettings(Settings* s) {
	if (s) {
//		s->setTitle("C. Clara Run");
		s->setWindowSize(glm::ivec2(1920, 1080));
		s->setFullScreen(true);
//		s->setConsoleWindowEnabled(true);
	}
}

void BasicApp::setup() {
	base::setup();
	// Printing during construction creates an error, so clear that out, in case anyone did.
	std::cout.clear();
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
	mFeeder.update();
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

void BasicApp::setupWorldBounds(const float near_z, const float far_z, kt::Cns &cns) const {
	const glm::ivec2		screen_ll(0, getWindowHeight()),
							screen_ur(getWindowWidth(), 0);
	kt::math::Cube&			cube(cns.mExactWorldBounds);
	mPicker.pick(screen_ll, near_z, cube.mNearLL);
	mPicker.pick(screen_ur, near_z, cube.mNearUR);
	mPicker.pick(screen_ll, far_z, cube.mFarLL);
	mPicker.pick(screen_ur, far_z, cube.mFarUR);

	// Expand so I don't have borders
	const float				exp_frac = 0.25f;
	const glm::vec3			near_exp(	(cube.mNearUR.x-cube.mNearLL.x) * exp_frac,
										(cube.mNearUR.y-cube.mNearLL.y) * exp_frac,
										0.0f);
	const glm::vec3			far_exp(	(cube.mFarUR.x-cube.mFarLL.x) * exp_frac,
										(cube.mFarUR.y-cube.mFarLL.y) * exp_frac,
										0.0f);

	cns.mWorldBounds = cns.mExactWorldBounds;
	cns.mWorldBounds.mNearLL -= near_exp;
	cns.mWorldBounds.mNearUR += near_exp;
	cns.mWorldBounds.mFarLL -= far_exp;
	cns.mWorldBounds.mFarUR += far_exp;
}

} // namespace cs

// This line tells Cinder to actually create and run the application.
CINDER_APP(cs::BasicApp, ci::app::RendererGl, cs::BasicApp::prepareSettings)