#ifndef KT_MATH_BEZIER_H_
#define KT_MATH_BEZIER_H_

#include <cinder/Vector.h>

namespace kt {
namespace math {

/**
 * @class kt::math::Bezier3f
 * @brief A bezier curve (cubic).
 */
class Bezier3f {
public:
	Bezier3f();

	bool			empty() const;
	void			clear();

	glm::vec3		point(const float t /* 0 - 1*/) const;
	// Simple measuring utility, very approximate result
	float			length(const size_t steps = 100) const;

	// Start, control 1, control 2, end
	glm::vec3		mP0, mP1, mP2, mP3;
};

} // namespace math
} // namespace kt

#endif
