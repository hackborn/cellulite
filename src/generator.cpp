#include "generator.h"

namespace cs {

/**
 * @class cs::Generator
 */

/**
 * @class cs::RandomGenerator
 */
void RandomGenerator::update(const kt::math::Cube &cube, std::vector<Particle> &list) {
	for (auto& p : list) {
		// Continue from the previous end point
		p.mCurve.mP0 = p.mCurve.mP3;
		p.mCurve.mP1 = nextPt(cube);
		p.mCurve.mP2 = nextPt(cube);
		p.mCurve.mP3 = nextPt(cube);
	}
}

glm::vec3 RandomGenerator::nextPt(const kt::math::Cube &cube) {
	glm::vec3	unit = glm::vec3(	mRand.nextFloat(),
									mRand.nextFloat(),
									mRand.nextFloat());
	return cube.atUnit(unit);
}

/**
 * @class cs::PolyLineGenerator
 */
void PolyLineGenerator::update(const kt::math::Cube &cube, std::vector<Particle> &l) {
	// Make up a line for now
	if (mLine.getPoints().empty()) {
		mLine.push_back(glm::vec3(cube.atUnit(glm::vec3(0.0f, 0.0f, 0.9f))));
		mLine.push_back(glm::vec3(cube.atUnit(glm::vec3(1.0f, 1.0f, 0.1f))));

		// Test
		mLines.push_back(mLine);

		ci::PolyLine3f		line;
		line.push_back(glm::vec3(cube.atUnit(glm::vec3(1.0f, 0.0f, 0.1f))));
		line.push_back(glm::vec3(cube.atUnit(glm::vec3(0.0f, 1.0f, 0.9f))));
		mLines.push_back(line);
	}

	for (auto& p : l) {
		glm::vec3		closest_pt;
		// Continue from the previous end point
		const float		d = kt::math::distance_seg(p.mCurve.mP3, mLines, &closest_pt);
		p.mCurve.mP0 = p.mCurve.mP3;
		p.mCurve.mP3 = closest_pt;

		p.mCurve.mP1 = glm::vec3(0, 0, -5);
		p.mCurve.mP2 = glm::vec3(0, 0, -5);
	}
}

/**
 * @class cs::RandomLineGenerator
 */
void RandomLineGenerator::update(const kt::math::Cube &cube, std::vector<Particle> &l) {
	nextLines(cube);

	for (auto& p : l) {
		glm::vec3		closest_pt;
		// Continue from the previous end point
		const float		d = kt::math::distance_seg(p.mCurve.mP3, mLines, &closest_pt);
		p.mCurve.mP0 = p.mCurve.mP3;
		p.mCurve.mP3 = closest_pt;

		p.mCurve.mP1 = glm::vec3(0, 0, -5);
		p.mCurve.mP2 = glm::vec3(0, 0, -5);
	}
}

void RandomLineGenerator::nextLines(const kt::math::Cube &cube) {
	mLines.clear();

	const int32_t		groups = mRand.nextInt(1, 8);
	for (int32_t g=0; g<groups; ++g) {
		const int32_t	lines = mRand.nextInt(10, 50);
		ci::PolyLine3f	poly;
		glm::vec3		last_pt = nextPt(cube);
		poly.push_back(last_pt);
		for (int32_t k=0; k<lines; ++k) {
			glm::vec3	next_pt = nextPt(cube);
			poly.push_back(next_pt);
		}
		mLines.push_back(poly);
	}
}

glm::vec3 RandomLineGenerator::nextPt(const kt::math::Cube &cube) {
	glm::vec3	unit = glm::vec3(	mRand.nextFloat(),
									mRand.nextFloat(),
									mRand.nextFloat());
	return cube.atUnit(unit);
}

} // namespace cs
