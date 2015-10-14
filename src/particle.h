#ifndef CS_PARTICLE_H_
#define CS_PARTICLE_H_

#include <memory>
#include <cinder/Color.h>
#include <cinder/Vector.h>
#include "kt/math/bezier.h"

namespace cs {
class Particle;
using ParticleRef = std::shared_ptr<Particle>;

/**
 * @class cs::Particle
 */
class Particle {
public:
	static float	encodeColor(const ci::ColorA&);

	Particle() { }
	explicit Particle(const glm::vec3 &pos) : mPosition(pos) { }

	glm::vec3			mPosition = glm::vec3(0),
						mVelocity = glm::vec3(0);
	// A 32-bit colour, encoded into a float
	float				mColor;
	size_t				mCount = 0;

//	glm::vec3			mStart = glm::vec3(0),
//						mTarget = glm::vec3(0);

	kt::math::Bezier3f	mCurve;
};

} // namespace cs

#endif
