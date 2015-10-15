#ifndef KT_APP_CNS_H_
#define KT_APP_CNS_H_

#include "../math/geometry.h"

namespace kt {

/**
 * @class kt::Cns
 * @brief Encapsulate globals to be sent throughout the system.
 * @description This is a stand-in for my normal CNS class,
 * which provides abstract services for letting apps install
 * whatever data and behaviour they need.
 */
class Cns {
public:
	Cns() { }

	// Size in pixels of a single cell at z of 0.
	glm::ivec2			mCellSizeInPixelsRaw = glm::ivec2(0, 0);
	glm::vec2			mCellSizeInPixels = glm::vec2(0, 0);

	// Size of a single particle (which, at 1,1, is the same size as a cell).
	glm::vec2			mParticleSize = glm::vec2(1, 1);

	// The visible world bounds. The standard bounds extend beyond the edges,
	// but the exact bounds fit the screen exactly.
	kt::math::Cube		mWorldBounds;
	kt::math::Cube		mExactWorldBounds;
};

} // namespace kt

#endif
