#ifndef CS_NOISE_H_
#define CS_NOISE_H_

#include <random>
#include <cinder/Vector.h>

namespace cs {

/**
 * @class cs::Noise
 * @brief Encapsulate a random number generator.
 */
class Noise {
public:
	Noise();

	// Random vector
	glm::vec3								next();

private:
	std::random_device						mDevice;
	std::ranlux24_base						mEngine;
	std::uniform_real_distribution<float>	mDistribution;
};

} // namespace cs

#endif
