#ifndef CS_APP_H_
#define CS_APP_H_

#include <cinder/gl/Batch.h>
#include <cinder/gl/Fbo.h>
#include "kt/app/kt_app.h"
#include "background.h"
#include "feeder.h"
#include "particle_render.h"
#include "picker_3d.h"
#include "settings.h"

namespace cs {

/**
 * @class cs::App
 */
class BasicApp : public kt::App {
public:
	BasicApp();

	static void					prepareSettings(Settings*);
	void						setup() override;

	void						mouseDrag(ci::app::MouseEvent) override;
	void						keyDown(ci::app::KeyEvent) override;


protected:
	void						onUpdate() override;
	void						onDraw() override;

private:
	void						setupWorldBounds(const kt::math::Rangef&, kt::Cns&) const;

	using base = kt::App;

	cs::Settings				mSettings;
	Picker3d					mPicker;
	Feeder						mFeeder;
	ParticleList				mParticles;

	// Drawing
	ci::CameraOrtho				mCameraOrtho;
	ci::gl::FboRef				mFbo;
	ci::gl::BatchRef			mBatch;
	ParticleRender				mParticleRender;

	Background					mBackground;
};

} // namespace cs

#endif