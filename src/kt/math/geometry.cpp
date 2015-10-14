#include "geometry.h"

namespace kt {
namespace math {

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
 * @class kt::Cube
 */
glm::vec3 Cube::atUnit(const glm::vec3 &unit) const {
	glm::vec2		ll(glm::mix(mNearLL, mFarLL, unit.z));
	glm::vec2		ur(glm::mix(mNearUR, mFarUR, unit.z));
	return glm::vec3(	glm::mix(ll.x, ur.x, unit.x),
						glm::mix(ll.y, ur.y, unit.y),
						glm::mix(mNearLL.z, mFarLL.z, unit.z));
}

} // namespace math
} // namespace kt
