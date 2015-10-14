#include "velocity_generator.h"

#include "velocity_cube.h"

namespace cs {

namespace {
glm::vec3			make_velocity(const glm::ivec2 &cell, const float z, const glm::vec3 &center);
glm::vec3			make_velocity(const glm::vec3 &cell, const glm::vec3 &center);
}

/**
 * @class cs::VelocityGenerator
 */
void VelocityGenerator::update(VelocityCube &c) {
	// Make sure I've got all my voxels
	for (auto& plane : c.mPlanes) {
		const int32_t	w = plane.mUR.x - plane.mLL.x + 1,
						h = plane.mUR.y - plane.mLL.y + 1;
		if (w > 0 && h > 0) {
			plane.mVelocity.resize(w * h);
		}
	}

	onUpdate(c);

	// Make sure the edges all point towards the center
	const glm::vec3			cube_cen(c.getCenter());
	for (auto& plane : c.mPlanes) {
		if (plane.mVelocity.empty()) continue;
		const bool			cap = (&plane == &c.mPlanes.front() || &plane == &c.mPlanes.back());
		const glm::vec3		cen(glm::mix(plane.mOutOfBounds.x1, plane.mOutOfBounds.x2, 0.5f),
								glm::mix(plane.mOutOfBounds.y1, plane.mOutOfBounds.y2, 0.5f),
								plane.mZ);
		const int32_t		w = (plane.mUR.x-plane.mLL.x) + 1;
		for (int32_t y=plane.mLL.y; y<=plane.mUR.y; ++y) {
			for (int32_t x=plane.mLL.x; x<=plane.mUR.x; ++x) {
				int32_t		pix = ((y-plane.mLL.y) * w) + (x-plane.mLL.x);
				if (cap) {
					plane.mVelocity[pix] = make_velocity(glm::ivec2(x, y), plane.mZ, cube_cen);
				} else {
					if (y <= plane.mLL.y || y >= plane.mUR.y) {
						plane.mVelocity[pix] = make_velocity(glm::ivec2(x, y), plane.mZ, cen);
					} else if (x <= plane.mLL.x) {
						plane.mVelocity[pix] = make_velocity(glm::ivec2(x, y), plane.mZ, cen);
					} else if (x >= plane.mUR.x) {
						plane.mVelocity[pix] = make_velocity(glm::ivec2(x, y), plane.mZ, cen);
					}
				}
			}		
		}
	}
}

/**
 * @class cs::RandomGenerator
 */
void RandomGenerator::onUpdate(VelocityCube &c) {
	float vel = glm::mix(mMin, mMax, ci::Rand::randFloat());
	for (auto& plane : c.mPlanes) {
		for (auto& vel : plane.mVelocity) {
			vel = glm::vec3(glm::mix(mMin, mMax, mRand.nextFloat()),
							glm::mix(mMin, mMax, mRand.nextFloat()),
							glm::mix(mMin, mMax, mRand.nextFloat()));
		}
	}
}

/**
 * @class cs::PolyLineGenerator
 */
void PolyLineGenerator::onUpdate(VelocityCube &c) {
	// Make up a line for now
	if (mLine.getPoints().empty()) {
		cs::math::Cube		cube = c.getBounds();
		mLine.push_back(glm::vec3(cube.atUnit(glm::vec3(0.0f, 0.0f, 0.9f))));
		mLine.push_back(glm::vec3(cube.atUnit(glm::vec3(1.0f, 1.0f, 0.1f))));

		// Test
		mLines.push_back(mLine);

		ci::PolyLine3f		line;
		line.push_back(glm::vec3(cube.atUnit(glm::vec3(1.0f, 0.0f, 0.1f))));
		line.push_back(glm::vec3(cube.atUnit(glm::vec3(0.0f, 1.0f, 0.9f))));
		mLines.push_back(line);
	}

	float vel = glm::mix(mMin, mMax, ci::Rand::randFloat());
	for (auto& plane : c.mPlanes) {
		int32_t				x = plane.mLL.x, y = plane.mLL.y;
		for (auto& vel : plane.mVelocity) {
			glm::vec3		pt(	static_cast<float>(x)+0.5f,
								static_cast<float>(y)+0.5f,
								plane.mZ);
			glm::vec3		closest_pt;
			const float		d = cs::math::distance_seg(pt, mLines, &closest_pt);
			if (d >= 0.0f) {
				vel = make_velocity(pt, closest_pt);
//				vel *= glm::mix(mLineMin, mLineMax, mRand.nextFloat());
				vel *= 16.0f;
			} else {
				vel = glm::vec3(glm::mix(mMin, mMax, mRand.nextFloat()),
								glm::mix(mMin, mMax, mRand.nextFloat()),
								glm::mix(mMin, mMax, mRand.nextFloat()));
			}

			++x;
			if (x > plane.mUR.x) {
				x = plane.mLL.x;
				++y;
			}
		}
	}
}

namespace {

glm::vec3			make_velocity(const glm::ivec2 &_cell, const float z, const glm::vec3 &center) {
	glm::vec3		cell(static_cast<float>(_cell.x) + 0.5f, static_cast<float>(_cell.y)+0.5f, z);
	// Shouldn't ever happen
	if (center == cell) return glm::vec3(0);

	glm::vec3		v = center - cell;
	v = glm::normalize(v);
	return v;
}

glm::vec3			make_velocity(const glm::vec3 &cell, const glm::vec3 &center) {
	// Shouldn't ever happen
	if (center == cell) return glm::vec3(0);

	glm::vec3		v = center - cell;
	v = glm::normalize(v);
	return v;
}

}

} // namespace cs
