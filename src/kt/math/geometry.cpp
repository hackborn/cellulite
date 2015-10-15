#include "geometry.h"

namespace kt {
namespace math {

namespace {
// symmetric rounding
inline int round(const double d) {
	return static_cast<int>(d > 0.0 ? floor(d + 0.5) : ceil(d - 0.5));
}

void get_index(const float idx, const size_t size, size_t& idxI, float& idxF);
}

/**
 * @func distance_seg
 */
float								distance_seg(	const glm::vec3 &pt, const glm::vec3 &a, const glm::vec3 &b,
													glm::vec3* out_pt) {
	// From stackoverflow:
	// http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
	const float			l2(distanceSquared(a, b));
	if (l2 == 0.0f) {
		if (out_pt) *out_pt = a;
		return glm::distance(pt, a);
	}

	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(pt-mA) . (mB-mA)] / |mB-mA|^2
	const float t = glm::dot(pt-a, b-a) / l2;
	if (t < 0.0) {
		if (out_pt) *out_pt = a;
		return glm::distance(pt, a);						// Beyond the 'v' end of the segment
	} else if (t > 1.0) {
		if (out_pt) *out_pt = b;
		return glm::distance(pt, b);				// Beyond the 'w' end of the segment
	}
	const auto projection = a + t * (b - a);		// Projection falls on the segment
	if (out_pt) *out_pt = projection;
	return glm::distance(pt, projection);
}

/**
 * @func distance_seg
 */
float								distance_seg(	const glm::vec3 &pt, const ci::PolyLine3f &poly,
													glm::vec3* out_pt) {
	if (poly.getPoints().size() < 2) return -1.0f;
	glm::vec3						a = (poly.isClosed() ? poly.getPoints().back() : poly.getPoints().front());
	bool							has_ans = false;
	float							ans = 0.0f;
	for (const auto& b : poly.getPoints()) {
		glm::vec3					found_pt;
		const float					d = distance_seg(pt, a, b, &found_pt);
		if (!has_ans || d < ans) {
			has_ans = true;
			ans = d;
			if (out_pt) *out_pt = found_pt;
		}
		a = b;
	}
	if (has_ans) return ans;
	return -1.0f;
}

float								distance_seg(	const glm::vec3 &pt, const std::vector<ci::PolyLine3f> &polies,
													glm::vec3* out_pt) {
	bool							has_ans = false;
	float							ans = -1.0f;
	for (const auto& poly : polies) {
		glm::vec3					found_pt;
		const float					d = distance_seg(pt, poly, &found_pt);
		if (!has_ans || d < ans) {
			has_ans = true;
			ans = d;
			if (out_pt) *out_pt = found_pt;
		}
	}
	return ans;
}

/**
 * @func linear_at()
 */
float				linear_at(const float index, const std::vector<float> &vec) {
	const size_t	s = vec.size();
	if (s < 2) {
		if (s < 1) return 0.0f;
		return vec[0];
	}

	size_t				idxI;
	float				idxF;
	get_index(index, s, idxI, idxF);

	int					x1 = (idxI >= s-1 ? 0 : idxI+1);
	return (vec[idxI] * (1-idxF)) + (vec[x1] * idxF);}

/**
 * @func hermite_at()
 */
float				hermite_at(const float index, const std::vector<float> &vec) {
	const size_t		s = vec.size();
	assert(s >= 4);
	if (s < 4) return 0;
	size_t				idxI;
	float				idxF;
	get_index(index, s, idxI, idxF);

	// Load up the four values we use for hermite interpolation
	int					hermiteI = idxI-1;
	const float			hermiteXm1 = vec[hermiteI < 0 ? s-1 : hermiteI];
	const float			hermiteX0 = vec[idxI];
	hermiteI = idxI+1;
	const float			hermiteX1 = vec[hermiteI >= (int)s ? hermiteI-(int)s : hermiteI];
	hermiteI = idxI+2;
	const float			hermiteX2 = vec[hermiteI >= (int)s ? hermiteI-(int)s : hermiteI];
	// 4-point, 3rd-order Hermite (x-form)
	const float			hermiteC1 = 0.5f * (hermiteX1 - hermiteXm1);
	const float			hermiteC3 = 1.5f * (hermiteX0 - hermiteX1) + 0.5f * (hermiteX2 - hermiteXm1);
	const float			hermiteC2 = hermiteXm1 - hermiteX0 + hermiteC1 - hermiteC3;
	return ((hermiteC3 * idxF + hermiteC2) * idxF + hermiteC1) * idxF + hermiteX0;
}

/**
 * @class kt::Cube
 */
glm::vec3 Cube::atUnit(const glm::vec3 &unit) const {
	glm::vec2		ll(glm::mix(mNearLL, mFarLL, unit.z));
	glm::vec2		ur(glm::mix(mNearUR, mFarUR, unit.z));
	return glm::vec3(	glm::mix(ll.x, ur.x, unit.x),
						glm::mix(ll.y, ur.y, unit.y),
						glm::mix(mNearLL.z, mFarLL.z, unit.z));
}

namespace {
float				to_unit(const float v, const float min, const float max) {
	if (v <= min) return min;
	if (v >= max) return max;
	const float		r = max-min;
	if (r == 0) return min;
	return (v-min) / r;
}
}

glm::vec3 Cube::toUnit(glm::vec3 &pt) const {
	const float		z = to_unit(pt.z, mFarLL.z, mNearLL.z);
	glm::vec2		ll(glm::mix(mFarLL, mNearLL, z));
	glm::vec2		ur(glm::mix(mFarUR, mNearUR, z));
	return glm::vec3(	to_unit(pt.x, ll.x, ur.x),
						to_unit(pt.y, ll.y, ur.y),
						z);
}

namespace {

void get_index(const float idx, const size_t size, size_t& idxI, float& idxF) {
	assert(size > 0);
	if (idx <= 0) {
		idxI = 0;
		idxF = 0;
	} else if (idx >= 1) {
		idxI = size-1;
		idxF = 0;
	} else {
		const float		scale = idx*float(size-1);
		const float		scaleFloor = floorf(scale);
		idxI = round(scaleFloor);
		if (idxI >= size-1) idxF = 0;
	}
}

} // anonymous namespace

} // namespace math
} // namespace kt
