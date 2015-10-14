#ifndef CS_VELOCITYCUBE_H_
#define CS_VELOCITYCUBE_H_

#include "velocity_plane.h"
#include "geometry.h"
#include "particle_list.h"

namespace cs {

/**
 * @class cs::VelocityCube
 * @brief A 3D velocity space, sorted by z value.
 */
class VelocityCube {
public:
	VelocityCube() { }

	void						clear();
	cs::math::Cube				getBounds() const;
	glm::vec3					getCenter() const;

	void						update(ParticleList&) const;
	
	std::vector<VelocityPlane>	mPlanes;
};

} // namespace cs

#endif
