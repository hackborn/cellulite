#ifndef CS_VELOCITYPLANE_H_
#define CS_VELOCITYPLANE_H_

#include <vector>
#include <cinder/Rect.h>
#include <cinder/Vector.h>

namespace cs {

/**
 * @class cs::VelocityPlane
 * @brief A single plane of velocity information.
 */
class VelocityPlane {
public:
	VelocityPlane() { }

	glm::ivec2				getBounds() const;

	// Answer the velocity at the x,y
	glm::vec3				at(const glm::vec2&) const;
	glm::vec3				at(const glm::ivec2&) const;

	// This plane's z value.
	float					mZ = 0;
	// Out-of-bounds area (screen space)
	ci::Rectf				mOutOfBounds;
	// Lower-left and upper-right cells (screen space). The
	// velocity array is the width and height of this area (inclusive).
	ci::ivec2				mLL, mUR;

	// My 2D (flattened) array of velocities.
	std::vector<glm::vec3>	mVelocity;
};

} // namespace cs

#endif
