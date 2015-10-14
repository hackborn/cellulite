#include "bezier.h"

#include "geometry.h"

namespace kt {
namespace math {

/**
 * @class kt::math::Bezier3f
 */
Bezier3f::Bezier3f() {
	clear();
}

bool Bezier3f::empty() const {
	return mP0 == mP3;
}

void Bezier3f::clear() {
	mP0 = glm::vec3(0);
	mP3 = mP0;
}

glm::vec3 Bezier3f::point(const float t) const {
	// from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
	float u = 1.0f - t;
	float tt = t*t;
	float uu = u*u;
	float uuu = uu * u;
	float ttt = tt * t;
 
	glm::vec3 p = uuu * mP0; //first term
	p += 3 * uu * t * mP1; //second term
	p += 3 * u * tt * mP2; //third term
	p += ttt * mP3; //fourth term

	return p;
}

float Bezier3f::length(const size_t steps) const {
	float					len = 0.0f;
	glm::vec3				last_pt = mP0;
	const float				total_steps = static_cast<float>(steps-1);
	for (size_t k=0; k<steps; ++k) {
		const glm::vec3		next_pt = point(static_cast<float>(k) /total_steps);
		len += glm::distance(last_pt, next_pt);

		last_pt = next_pt;
	}
	return len;
}

} // namespace math
} // namespace kt
