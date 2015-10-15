#ifndef CS_PARTICLE_H_
#define CS_PARTICLE_H_

#include <memory>
#include <cinder/Color.h>
#include <cinder/Vector.h>
#include "kt/math/bezier.h"

namespace cs {

/**
 * @class cs::Particle
 */
class Particle {
public:
	Particle() { }
	explicit Particle(const glm::vec3 &pos) : mPosition(pos) { }
	explicit Particle(const glm::vec3 &pos, const float a) : mPosition(pos), mAlpha(a) { }

	// Computed values during rendering.
	glm::vec3			mPosition = glm::vec3(0);
	float				mAlpha = 1.0f;

	// Alpha level of the particle.
	float				mStartAlpha = 1.0f,
						mEndAlpha = 1.0f;

	kt::math::Bezier3f	mCurve;
	float				mCurveLength = 0.0f;

	// Used to signify I can create accent particles
	bool				mHasAccents = false;
};

} // namespace cs

#endif
