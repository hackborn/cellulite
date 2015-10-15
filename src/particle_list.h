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

	inline void		setParametersFrom(const ParticleList &l) {
		mMaxCurveLength = l.mMaxCurveLength;
		mAverageCurveLength = l.mAverageCurveLength;
		mTransitionDuration = l.mTransitionDuration,
		mHoldDuration = l.mHoldDuration;
	}

	// The longest curve length
	float			mMaxCurveLength = 0.0f;
	// The average curve length
	float			mAverageCurveLength = 0.0f;
	// Suggested duration (in seconds) to run and display this list
	double			mTransitionDuration = 2.0,
					mHoldDuration = 0.5;
};

} // namespace cs

#endif
