#include "bezier.h"

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

} // namespace math
} // namespace kt
