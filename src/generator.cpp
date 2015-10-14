#include "generator.h"

namespace cs {

/**
 * @class cs::Generator
 */
void Generator::update(const kt::math::Cube &cube, ParticleList &list) {
	list.mDuration = 2.0;

	onUpdate(cube, list);

	// Compute all the curve length
	// XXX This was supposed to tell me how long I should run the animation for, but
	// it did not in fact deliver any actionable information
#if 0
	list.mMaxCurveLength = 0.0f;
	double			avg_len = 0.0;
	for (auto& p : list) {
		p.mCurveLength = p.mCurve.length(25);
		avg_len += p.mCurveLength;

		if (p.mCurveLength > list.mMaxCurveLength) {
			list.mMaxCurveLength = p.mCurveLength;
		}
	}
	list.mAverageCurveLength = static_cast<float>(avg_len / static_cast<double>(list.size()));
#endif
}

/**
 * @class cs::RandomGenerator
 */
void RandomGenerator::onUpdate(const kt::math::Cube &cube, ParticleList &list) {
	list.mDuration = 6.0;

	for (auto& p : list) {
		// Continue from the previous end point
		p.mCurve.mP0 = p.mCurve.mP3;
		p.mCurve.mP3 = nextPt(cube);
		// Randomize the control points, but don't let it get toooo crazy
		p.mCurve.mP1 = glm::mix(p.mCurve.mP0, nextPt(cube), 0.25f);
		p.mCurve.mP2 = glm::mix(p.mCurve.mP3, nextPt(cube), 0.25f);
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
void PolyLineGenerator::onUpdate(const kt::math::Cube &cube, ParticleList &l) {
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
void RandomLineGenerator::onUpdate(const kt::math::Cube &cube, ParticleList &l) {
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
