#ifndef CS_NOISE_H_
#define CS_NOISE_H_

#include <random>
#include <cinder/Vector.h>

namespace cs {
class Noise;

/**
 * @func midpoint_displacement
 * @brief Fill a vector using midpoint displacement. Resulting range is -1 to 1.
 */
void				midpoint_displacement(std::vector<float>&);
void				midpoint_displacement(Noise&, std::vector<float>&);

/**
 * @class cs::Noise
 * @brief Encapsulate a random number generator.
 */
class Noise {
public:
	Noise(const float distribution_min = -1.0f, const float distribution_max = 1.0f);

	float									nextFloat();
	glm::vec3								nextVec();

private:
	std::random_device						mDevice;
	std::ranlux24_base						mEngine;
	std::uniform_real_distribution<float>	mDistribution;
};

/**
 * @class cs::InterpCube
 * @brief Fill a volume space with random values.
 * @description This is a very simple class that just generates random data for
 * each axis and then interpolates a new value based on an incoming point.
 */
class InterpCube {
public:
	InterpCube();

	// Depth is the number of cells in each axis' data. The higher the
	// depth, the more fine-grained the result.
	void				fill(const size_t depth = 100);

	glm::vec3			at(const glm::vec3 &unit) const;

private:
	void				fillAxis(const size_t depth, std::vector<float>&);
	float				atAxis(const float unit, const std::vector<float>&) const;

	Noise				mNoise;
	std::vector<float>	mX, mY, mZ;
};

} // namespace cs

#endif
