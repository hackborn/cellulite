#include "velocity_plane.h"

namespace cs {

/**
 * @class cs::VelocityPlane
 */
glm::ivec2 VelocityPlane::getBounds() const {
	return glm::ivec2(	(mUR.x-mLL.x) + 1,
						(mUR.y-mLL.y) + 1);
}

glm::vec3 VelocityPlane::at(const glm::vec2 &pt) const {
	return at(glm::ivec2(	static_cast<int32_t>(std::round(pt.x)),
							static_cast<int32_t>(std::round(pt.y))));
}

glm::vec3 VelocityPlane::at(const glm::ivec2 &pt) const {
	int32_t				x = pt.x,
						y = pt.y;
	if (x <= mLL.x) x = mLL.x;
	else if (x >= mUR.x) x = mUR.x;
	if (y <= mLL.y) y = mLL.y;
	else if (y >= mUR.y) y = mUR.y;

	x -= mLL.x;
	y -= mLL.y;

	const int32_t		w = (mUR.x-mLL.x) + 1;
	size_t				idx = static_cast<size_t>((y*w) + x);
	if (idx >= mVelocity.size()) {
		return glm::vec3(0);
	}
	return mVelocity[idx];
}

} // namespace cs
