#include "velocity_cube.h"

namespace cs {

namespace {
void						get_bounds(const VelocityPlane &p, glm::vec3 &ll, glm::vec3 &ur);
}

/**
 * @class cs::VelocityCube
 */
void VelocityCube::clear() {
	mPlanes.clear();
}

cs::math::Cube VelocityCube::getBounds() const {
	math::Cube					ans;
	if (!mPlanes.empty()) {
		get_bounds(mPlanes.front(), ans.mNearLL, ans.mNearUR);
		get_bounds(mPlanes.back(), ans.mFarLL, ans.mFarUR);
	}
	return ans;
}

glm::vec3 VelocityCube::getCenter() const {
	return getBounds().atUnit(glm::vec3(0.5f, 0.5f, 0.5f));
}

void VelocityCube::update(ParticleList &list) const {
	if (mPlanes.empty()) return;

	for (auto& it : list) {
		if (!it) continue;

		
		size_t			z_index = static_cast<size_t>(-(std::round(it->mPosition.z)));
		if (z_index >= mPlanes.size()) z_index = mPlanes.size()-1;
		it->mVelocity += mPlanes[z_index].at(glm::vec2(it->mPosition.x, it->mPosition.y));
	}
}

namespace {

void						get_bounds(const VelocityPlane &p, glm::vec3 &ll, glm::vec3 &ur) {
	ll = glm::vec3(p.mOutOfBounds.x1, p.mOutOfBounds.y1, p.mZ);
	ur = glm::vec3(p.mOutOfBounds.x2, p.mOutOfBounds.y2, p.mZ);
}

}

} // namespace cs
