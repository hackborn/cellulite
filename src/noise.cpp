#include "noise.h"

#include "kt/math/geometry.h"

namespace cs {

namespace {
const float			RND_DROPOFF = 0.9f;
void				generate_midpoint(Noise&, const size_t a, const size_t b, const float rnd, std::vector<float>& vec);
}

/**
 * @func midpoint_displacement
 */
void midpoint_displacement(std::vector<float>& vec) {
	if (vec.empty()) return;
	Noise			n(-1.0f, 1.0f);
	midpoint_displacement(n, vec);
}

void midpoint_displacement(Noise &n, std::vector<float>& vec) {
	if (vec.empty()) return;

	vec[0] = n.nextFloat();
	vec[vec.size()-1] = n.nextFloat();
	generate_midpoint(n, 0, vec.size()-1, RND_DROPOFF, vec);
}

/**
 * @class cs::Noise
 */
Noise::Noise(const float min, const float max)
		: mEngine(mDevice())
		, mDistribution(min, max) {
}

float Noise::nextFloat() {
	return mDistribution(mEngine);
}

glm::vec3 Noise::nextVec() {
	float		x = mDistribution(mEngine),
				y = mDistribution(mEngine),
				z = mDistribution(mEngine);
	return glm::vec3(x, y, z);
}

/**
 * @class cs::InterpCube
 */
InterpCube::InterpCube()
		: mNoise(-1.0f, 1.0f) {
}

void InterpCube::fill(const size_t depth) {
	fillAxis(depth, mX);
	fillAxis(depth, mY);
	fillAxis(depth, mZ);
}

glm::vec3 InterpCube::at(const glm::vec3 &unit) const {
	return glm::vec3(	atAxis(unit.x, mX),
						atAxis(unit.y, mY),
						atAxis(unit.z, mZ));
}

void InterpCube::fillAxis(const size_t depth, std::vector<float> &vec) {
	vec.resize(depth);
	midpoint_displacement(mNoise, vec);
}

float InterpCube::atAxis(const float unit, const std::vector<float> &vec) const {
	if (unit <= 0.0f) return kt::math::linear_at(0.0f, vec);
	if (unit >= 1.0f) return kt::math::linear_at(1.0f, vec);
	return kt::math::linear_at(unit, vec);
}

namespace {

void generate_midpoint(	Noise &n, const size_t a, const size_t b, const float rnd,
						std::vector<float>& vec) {
	const size_t			mid = a + ((b-a)/2);
	if (mid <= a || mid >= b) return;

	// Find the interpolated midpoint and displace
	vec[mid] = ((vec[a]*0.5f) + (vec[b]*0.5f)) + (n.nextFloat()*rnd);
	generate_midpoint(n, a, mid, rnd*RND_DROPOFF, vec);
	generate_midpoint(n, mid, b, rnd*RND_DROPOFF, vec);
}

} // anonymous namespace

} // namespace cs
