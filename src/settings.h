#ifndef CS_SETTINGS_H_
#define CS_SETTINGS_H_

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

	// Total number of particles
	size_t				mParticleCount = 1000;
//	size_t				mParticleCount = 10;
//	size_t				mParticleCount = 1;

	// Total number of planes (i.e. the voxel depth -- width and height are
	// determined by screen size).
	size_t				mPlaneDepth = 40;

	// Amount of deceleration (per frame; should be per second)
	float				mFriction = 0.005f;

	// Range of velocities for the random generator
	float				mRndMin = -0.5f,
						mRndMax = 0.5f;
};

} // namespace cs

#endif
