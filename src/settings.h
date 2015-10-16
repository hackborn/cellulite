#ifndef CS_SETTINGS_H_
#define CS_SETTINGS_H_

#include <cinder/Color.h>
#include "kt/math/range.h"

namespace cs {

/**
 * @class cs::Settings
 * @brief All the adjustable app parameters.
 * @description Normally these would be loaded from a file, but this is
 * a small project without any of my typical machinery.
 */
class Settings {
public:
	Settings() { }

	// Total number of main particles
	size_t				mParticleCount = 5000;
//	size_t				mParticleCount = 10;
//	size_t				mParticleCount = 1;
	// Total number of accent particles
	size_t				mAccentParticleCount = 10000;

	// The far and near z planes that enclose the particles.
	kt::math::Rangef	mRangeZ = kt::math::Rangef(-80.0f, 0.0f);

	// Amount of deceleration (per frame; should be per second)
	float				mFriction = 0.9f;

	// Range of velocities for the random generator
	float				mRndMin = -0.5f,
						mRndMax = 0.5f;

	ci::Color			mBackgroundColor = ci::Color(0.11f, 0.78f, 0.08f);
};

} // namespace cs

#endif
