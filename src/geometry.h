#ifndef CS_MATH_GEOMETRY_H_
#define CS_MATH_GEOMETRY_H_

#include <cinder/PolyLine.h>
#include <cinder/Vector.h>

namespace cs {
namespace math {

/**
 * Add in some Cinder functions we lost in the move to glm
 */
inline glm::vec2	xy(const glm::vec3 &v) { return glm::vec2(v.x, v.y); }
inline float		lengthSquared(const glm::vec2 &v) { return v.x*v.x + v.y*v.y; }
inline float		lengthSquared(const glm::vec3 &v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
inline float		distanceSquared(const glm::vec2 &a, const glm::vec2 &b) { return lengthSquared(a - b); }
inline float		distanceSquared(const glm::vec3 &a, const glm::vec3 &b) { return lengthSquared(a - b); }

/**
 * @func distance_seg
 * @brief Distance from point pt to line segment a - b.
 */
float								distance_seg(	const glm::vec3 &pt, const glm::vec3 &a, const glm::vec3 &b,
													glm::vec3* out_pt = nullptr);

/**
 * @func distance_seg
 * @brief Nearest distance from point pt to line segments of polyine. Respect the isClosed state of the poly.
 * Return < 0 if no value was found.
 */
float								distance_seg(const glm::vec3 &pt, const ci::PolyLine3f&, glm::vec3* out_pt = nullptr);
float								distance_seg(const glm::vec3 &pt, const std::vector<ci::PolyLine3f>&, glm::vec3* out_pt = nullptr);

/**
 * @class cs::Cube
 * @brief Simple convenience cube.
 */
class Cube {
public:
	Cube() { }

	// Given a unit position (all values 0-1) translate to a position in the cube.
	glm::vec3					atUnit(const glm::vec3&) const;

	//a-b-c-d starts at the ll and proceeds clockwise (ul, ur, lr)
	glm::vec3					mNearLL, mNearUR,
								mFarLL, mFarUR;
};

} // namespace math
} // namespace cs

#endif
