#include "generator.h"

#include <cinder/ImageIo.h>
#include <cinder/Surface.h>
#include "kt/app/kt_cns.h"
#include "kt/app/kt_environment.h"

namespace cs {

/**
 * @class cs::GeneratorParams
 */
void GeneratorParams::setTo(const kt::Cns &cns) {
	mWorldBounds = cns.mWorldBounds;
	mExactWorldBounds = cns.mExactWorldBounds;
}

/**
 * @class cs::Generator
 */
void Generator::update(const GeneratorParams &p, ParticleList &list) {
	list.mDuration = 2.0;

	onUpdate(p, list);

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
void RandomGenerator::onUpdate(const GeneratorParams &gp, ParticleList &list) {
	list.mDuration = 6.0;

	for (auto& p : list) {
		// Continue from the previous end point
		p.mCurve.mP0 = p.mCurve.mP3;
		p.mCurve.mP3 = nextPt(gp.mWorldBounds);
		// Randomize the control points, but don't let it get toooo crazy
		p.mCurve.mP1 = glm::mix(p.mCurve.mP0, nextPt(gp.mWorldBounds), 0.25f);
		p.mCurve.mP2 = glm::mix(p.mCurve.mP3, nextPt(gp.mWorldBounds), 0.25f);
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
void PolyLineGenerator::onUpdate(const GeneratorParams &gp, ParticleList &l) {
	// Make up a line for now
	if (mLine.getPoints().empty()) {
		mLine.push_back(glm::vec3(gp.mWorldBounds.atUnit(glm::vec3(0.0f, 0.0f, 0.9f))));
		mLine.push_back(glm::vec3(gp.mWorldBounds.atUnit(glm::vec3(1.0f, 1.0f, 0.1f))));

		// Test
		mLines.push_back(mLine);

		ci::PolyLine3f		line;
		line.push_back(glm::vec3(gp.mWorldBounds.atUnit(glm::vec3(1.0f, 0.0f, 0.1f))));
		line.push_back(glm::vec3(gp.mWorldBounds.atUnit(glm::vec3(0.0f, 1.0f, 0.9f))));
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
void RandomLineGenerator::onUpdate(const GeneratorParams &gp, ParticleList &l) {
	nextLines(gp.mWorldBounds);

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

/**
 * @class cs::ImageGenerator
 */
void ImageGenerator::onUpdate(const GeneratorParams &gp, ParticleList &l) {
	l.mDuration = 6.0;
	ci::Surface8u		s(ci::loadImage(kt::env::expand("$(DATA)/images/Eagle 1.jpg")));

	const int			total_src_size  = s.getWidth() * s.getHeight();
	const float			src_w(static_cast<float>(s.getWidth())),
						src_h(static_cast<float>(s.getHeight()));
	const float			aspect = src_w / src_h;
	const float			sqr = floorf(ci::math<float>::sqrt(static_cast<float>(l.size())));
	float				w = 1.0f, h = 1.0f;
	int32_t				rows = static_cast<int32_t>(sqr), cols = static_cast<int32_t>(sqr);
	if (aspect >= 1.0f) {
		h = w / aspect;
	} else {
		h = w / aspect;
	}

	int32_t				y = 0, x = 0;
	for (auto& p : l) {
		// Get coords
		glm::vec2		fpt(static_cast<float>(x) / (static_cast<float>(cols-1)),
							static_cast<float>(y) / (static_cast<float>(rows-1)));
		glm::ivec2		src_pt(static_cast<int32_t>(fpt.x*src_w), static_cast<int32_t>(fpt.y*src_h));
		if (src_pt.x < 0) src_pt.x = 0;
		else if (src_pt.x >= s.getWidth()) src_pt.x = s.getWidth()-1;
		if (src_pt.y < 0) src_pt.y = 0;
		else if (src_pt.y >= s.getHeight()) src_pt.y = s.getHeight()-1;

		// Src value
		const auto		clr = s.getPixel(src_pt);
		const float		v = 1.0f - (static_cast<float>(clr.r + clr.g + clr.b) / (255.0f * 3.0f));

		glm::vec3		pt = gp.mExactWorldBounds.atUnit(glm::vec3(fpt.x, fpt.y, v));

		// Continue from the previous end point
		p.mCurve.mP0 = p.mCurve.mP3;
		p.mCurve.mP3 = pt;

		p.mCurve.mP1 = glm::vec3(0, 0, -5);
		p.mCurve.mP2 = glm::vec3(0, 0, -5);

		if (++x >= cols) {
			x = 0;
			++y;
		}
	}
}

} // namespace cs
