#ifndef KT_MATH_GEOMETRY_H_
#define KT_MATH_GEOMETRY_H_

#include <cinder/PolyLine.h>
#include <cinder/Vector.h>

namespace kt {
namespace math {

/**
 * Add in some Cinder functions we lost in the move to glm
 */
inline glm::vec2	xy(const glm::vec3 &v) { return glm::vec2(v.x, v.y); }
inline float		lengthSquared(const glm::vec2 &v) { return v.x*v.x + v.y*v.y; }
inline float		lengthSquared(const glm::vec3 &v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
inline float		distanceSquared(const glm::vec2 &a, const glm::vec2 &b) { return lengthSquared(b - a); }
inline float		distanceSquared(const glm::vec3 &a, const glm::vec3 &b) { return lengthSquared(b - a); }

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
 * @func s_curve()
 * @brief Transform a 0-1 value to an s shape.
 */
template <typename T>
T					s_curve(const T v) { return (3.0f-2.0f*v)*v*v; }
auto const s_curvef = &s_curve<float>;
auto const s_curved = &s_curve<double>;

/**
 * @func linear_at()
 * @brief Linear interpolation. Index is 0-1.
 */
float				linear_at(const float index, const std::vector<float>&);

/**
 * @func hermite_at()
 * @brief Hermite interpolation. Index is 0-1.
 * Note that hermite interpolation requires 4 or more values in the vector.
 */
float				hermite_at(const float index, const std::vector<float>&);

/**
 * @class kt::Cube
 * @brief Simple convenience cube.
 */
class Cube {
public:
	Cube() { }

	// Given a unit position (all values 0-1) translate to a position in the cube.
	glm::vec3					atUnit(const glm::vec3&) const;
	// Given a point somewhere in my bounds, answer a unit point.
	glm::vec3					toUnit(glm::vec3&) const;

	// Four corners.
	glm::vec3					mNearLL, mNearUR,
								mFarLL, mFarUR;
};

} // namespace math
} // namespace kt

#endif
