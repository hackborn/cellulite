#ifndef CS_PARTICLE_H_
#define CS_PARTICLE_H_

#include <memory>
#include <cinder/Color.h>
#include <cinder/Vector.h>

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

	glm::vec3		mPosition = glm::vec3(0, 0, 0),
					mVelocity = glm::vec3(0, 0, 0);
	// A 32-bit colour, encoded into a float
	float			mColor;
};

} // namespace cs

#endif
