#ifndef CS_APP_H_
#define CS_APP_H_

#include <cinder/gl/Batch.h>
#include <cinder/gl/Fbo.h>
#include "particle_render.h"
#include "kt_app.h"
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
	void						createVelocityCube(const int32_t max_z, VelocityCube&) const;

	using base = kt::App;

	cs::Settings				mSettings;
	Picker3d					mPicker;

	// Drawing
	ci::gl::FboRef				mFbo;
	ci::gl::BatchRef			mBatch;
	ParticleRender				mParticleRender;
	ci::CameraOrtho				mCameraOrtho;
};

} // namespace cs

#endif