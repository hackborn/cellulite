#ifndef KT_APP_APP_H_
#define KT_APP_APP_H_

#include <cinder/app/App.h>
#include <cinder/Camera.h>
#include "kt_cns.h"

namespace kt {

/**
 * @class kt::App
 * @brief A stand-in for where I'd normally have my framework root
 * app class, to handle any initialization needed before the app is
 * constructed.
 */
class App : public ci::app::App {
private:
	const bool					mEnvInitialized;

public:
	App();

	void						update() override;
	void						draw() override;

protected:
	virtual void				onUpdate() { }
	virtual void				onDraw() { }

	Cns							mCns;
	// Standard distance from my camera to the scene.
	const float					mStdDist = 10.0f;
	ci::CameraPersp				mCamera;
};

} // namespace kt

#endif