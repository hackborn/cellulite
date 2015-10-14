#include "cs_app.h"

#include <cinder/app/RendererGl.h>
#include <cinder/gl/gl.h>
#include "kt/app/kt_environment.h"

namespace cs {

BasicApp::BasicApp()
		: mPicker(mCamera)
		, mGenerate(mCns, mSettings)
		, mParticleRender(mCns, mSettings, mGenerate, mParticles) {
	
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
	setupWorldBounds(0.0f, -80.0f, mCns.mWorldBounds);

	// SETUP PARTICLES
	RandomGenerator			gen;
	mParticles.resize(mSettings.mParticleCount);
	gen.update(mCns.mWorldBounds, mParticles);
	for (auto& p : mParticles) {
//		const glm::vec3		unit_pos(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat());
//		const glm::vec3		pos(mCns.mWorldBounds.atUnit(unit_pos));
		p.mCurve.mP0 = p.mCurve.mP3;
//		p.mCurve.mP3 = pos;
	}
#if 0
	size_t					pc = 0;
	for (size_t k=0; k<mSettings.mParticleCount; ++k) {
		const glm::vec3		unit_pos(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat());
		const glm::vec3		pos(mCns.mWorldBounds.atUnit(unit_pos));
		ParticleRef			p = std::make_shared<Particle>(pos);
		if (!p) continue;
		p->mCurve.mP0 = p->mPosition;
		p->mCurve.mP3 = p->mPosition;
		p->mColor = Particle::encodeColor(ci::ColorA(1, 1, 1, 1));
		p->mCount = (pc++);
		if (pc > 5) pc = 0;
		mParticleRender.push_back(p);
	}
#endif
	// SETUP VELOCITY VOXELS
//	RandomGenerator			gen(mSettings.mRndMin, mSettings.mRndMax);
//	PolyLineGenerator		gen(ci::PolyLine3f(), mSettings.mRndMin, mSettings.mRndMax);
//	gen.update(mCns.mWorldBounds, mParticleRender.mParticles);

	mGenerate.start(mParticles);
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
	mGenerate.update();
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

void BasicApp::setupWorldBounds(const float near_z, const float far_z, kt::math::Cube &cube) const {
	const glm::ivec2		screen_ll(0, getWindowHeight()),
							screen_ur(getWindowWidth(), 0);
	mPicker.pick(screen_ll, near_z, cube.mNearLL);
	mPicker.pick(screen_ur, near_z, cube.mNearUR);
	mPicker.pick(screen_ll, far_z, cube.mFarLL);
	mPicker.pick(screen_ur, far_z, cube.mFarUR);
}

} // namespace cs

// This line tells Cinder to actually create and run the application.
CINDER_APP(cs::BasicApp, ci::app::RendererGl, cs::BasicApp::prepareSettings)