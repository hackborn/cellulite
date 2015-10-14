#include "noise.h"

namespace cs {

/**
 * @class cs::Noise
 */
Noise::Noise()
		: mEngine(mDevice())
		, mDistribution(-1.0f, 1.0f) {
}

glm::vec3 Noise::next() {
	float		x = mDistribution(mEngine),
				y = mDistribution(mEngine),
				z = mDistribution(mEngine);
	return glm::vec3(x, y, z);
}

} // namespace cs
