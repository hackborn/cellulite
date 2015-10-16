#ifndef CS_PARTICLEVIEW_H_
#define CS_PARTICLEVIEW_H_

#include <cinder/gl/Batch.h>
#include <cinder/gl/Texture.h>
#include "kt/time/seconds.h"
#include "noise.h"
#include "particle_list.h"
#include "particle_render.h"

namespace kt { class Cns; }
namespace cs {
class Feeder;
class Settings;

/**
 * @class cs::ParticleView
 * @brief Update and draw all particles.
 */
class ParticleView {
public:
	ParticleView() = delete;
	ParticleView(const ParticleRender&) = delete;
	ParticleView(const kt::Cns&, const cs::Settings&, Feeder&);

	void						initializeParticles();

	void						update();
	void						draw();

private:
	void						updateAccents();

	const kt::Cns&				mCns;
	const cs::Settings&			mSettings;
	class Feeder&				mFeeder;
	Noise						mNoise;
	ParticleList				mParticles;
	ParticleList				mAccentParticles;
	cs::InterpCube				mAccentForces;
	size_t						mAddAccentTick = 0;
	kt::time::Seconds			mTimer;
	enum class Stage			{ kTransition, kHold };
	Stage						mStage = Stage::kHold;
	double						mTransitionDuration = 0.0,
								mHoldDuration = 0.0;

	ParticleRender				mRender;
};

} // namespace cs

#endif
