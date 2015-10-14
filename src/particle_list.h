#ifndef CS_PARTICLELIST_H_
#define CS_PARTICLELIST_H_

#include <vector>
#include "particle.h"

namespace cs {

/**
 * @class cs::ParticleList
 */
class ParticleList : public std::vector<Particle> {
public:
	ParticleList() { }

	// The longest curve length
	float			mMaxCurveLength = 0.0f;
	// The average curve length
	float			mAverageCurveLength = 0.0f;
	// Suggested duration (in seconds) to run this list
	double			mDuration = 2.0;
};

} // namespace cs

#endif
