#include "generator.h"

#include <cinder/ImageIo.h>
#include <cinder/Surface.h>
#include "kt/app/kt_cns.h"
#include "kt/app/kt_environment.h"

namespace cs {

namespace { 
const float			WHITE = 1.0f;
}

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
//	std::cout << "generator " << typeid(*this).name() << std::endl;

	list.mTransitionDuration = 2.0;
	list.mHoldDuration = 0.2;

	onUpdate(p, list);

	// Assign 20 random accent generators.
	for (auto& p : list) p.mHasAccents = false;
	for (size_t k=0; k<20; ++k) {
		size_t		idx = mRand.nextUint(list.size()-1);
		list[idx].mHasAccents = true;
	}

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

glm::vec3 Generator::nextPt(const kt::math::Cube &cube) {
	glm::vec3	unit = glm::vec3(	mRand.nextFloat(),
									mRand.nextFloat(),
									mRand.nextFloat());
	return cube.atUnit(unit);
}

/**
 * @class cs::RandomGenerator
 */
void RandomGenerator::onUpdate(const GeneratorParams &gp, ParticleList &list) {
	list.mHoldDuration = 0.0;

	// Assign the start and end points.
	if (mMode == Mode::kClosest) onUpdateClosest(gp, list);
	else onUpdateAnywhere(gp, list);

	// Randomize the control points, but don't let it get toooo crazy.
	for (auto& p : list) {
		// Alpha
		p.mStartAlpha = p.mEndAlpha;
		p.mEndAlpha = 1.0f;		

		// Curve
		kt::math::Bezier3f&		c(p.mCurve);

		const auto				mid = glm::mix(c.mP0, c.mP3, 0.5f);
		const float				d1 = glm::distance(c.mP0, mid),
								d2 = glm::distance(c.mP3, mid);
		const float				d = (d1 <= d2 ? d1 : d2) * 0.2f;
		c.mP1 = glm::mix(c.mP0, mid, 0.75f) + nextOffset(d);
		c.mP2 = glm::mix(c.mP3, mid, 0.75f) + nextOffset(d);
	}
}

void RandomGenerator::onUpdateAnywhere(const GeneratorParams &gp, ParticleList &list) {
	for (auto& p : list) {
		kt::math::Bezier3f&		c(p.mCurve);

		// Continue from the previous end point
		c.mP0 = c.mP3;
		c.mP3 = nextPt(gp.mWorldBounds);
	}
}

void RandomGenerator::onUpdateClosest(const GeneratorParams &gp, ParticleList &list) {
	if (list.empty()) return;
	mClosestPts.resize(list.size());

	for (auto& p : mClosestPts) {
		p = nextPt(gp.mWorldBounds);
	}

	// Each point picks its closest, eliminating as it goes. Not the best possible
	// results, but hopefully decent for a reasonable performance trade off.
	for (auto& p : list) {
		kt::math::Bezier3f&		c(p.mCurve);

		// Continue from the previous end point
		c.mP0 = c.mP3;
		c.mP3 = popClosest(c.mP0, mClosestPts);
	}
}

glm::vec3 RandomGenerator::nextOffset(const float scale) {
	glm::vec3	pt = glm::vec3(	mRand.nextFloat(),
									mRand.nextFloat(),
									mRand.nextFloat());
	pt = glm::normalize(pt);
	pt *= scale;
	return pt;
}

glm::vec3 RandomGenerator::popClosest(const glm::vec3 &pt, PtList &list) const {
	if (list.empty()) return pt;
	size_t			idx = 0, k = 0;
	float			dist = kt::math::distanceSquared(pt, list.front());
	for (const auto& b : list) {
		const float	d = kt::math::distanceSquared(pt, b);
		if (d < dist) {
			dist = d;
			idx = k;
		}
		++k;
	}
	const glm::vec3	ans = list[idx];
	if (list.size() > 1 && idx < list.size()-1) {
		list[idx] = list.back();
	}
	list.pop_back();
	return ans;
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
		// Alpha
		p.mStartAlpha = p.mEndAlpha;
		p.mEndAlpha = 1.0f;		

		// Curve
		kt::math::Bezier3f&		c(p.mCurve);
		glm::vec3				closest_pt;
		// Continue from the previous end point
		const float				d = kt::math::distance_seg(c.mP3, mLines, &closest_pt);
		c.mP0 = p.mCurve.mP3;
		c.mP3 = closest_pt;

		c.mP1 = glm::vec3(0, 0, -5);
		c.mP2 = glm::vec3(0, 0, -5);
	}
}

/**
 * @class cs::RandomLineGenerator
 */
void RandomLineGenerator::onUpdate(const GeneratorParams &gp, ParticleList &l) {
	nextLines(gp.mWorldBounds);

	for (auto& p : l) {
		// Alpha
		p.mStartAlpha = p.mEndAlpha;
		p.mEndAlpha = 1.0f;		

		// Curve
		kt::math::Bezier3f&		c(p.mCurve);
		glm::vec3				closest_pt;
		// Continue from the previous end point
		const float				d = kt::math::distance_seg(c.mP3, mLines, &closest_pt);
		c.mP0 = p.mCurve.mP3;
		c.mP3 = closest_pt;

		c.mP1 = glm::vec3(0, 0, -5);
		c.mP2 = glm::vec3(0, 0, -5);
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

/**
 * @class cs::ImageGenerator
 */
void ImageGenerator::onUpdate(const GeneratorParams &gp, ParticleList &l) {
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
		// Alpha
		p.mStartAlpha = p.mEndAlpha;
		p.mEndAlpha = 1.0f;		

		// Curve
		// Get coords
		glm::vec2				fpt(static_cast<float>(x) / (static_cast<float>(cols-1)),
									static_cast<float>(y) / (static_cast<float>(rows-1)));
		glm::ivec2				src_pt(static_cast<int32_t>(fpt.x*src_w), static_cast<int32_t>(fpt.y*src_h));
		if (src_pt.x < 0) src_pt.x = 0;
		else if (src_pt.x >= s.getWidth()) src_pt.x = s.getWidth()-1;
		if (src_pt.y < 0) src_pt.y = 0;
		else if (src_pt.y >= s.getHeight()) src_pt.y = s.getHeight()-1;

		// Src value
		const auto				clr = s.getPixel(src_pt);
		const float				v = 1.0f - (static_cast<float>(clr.r + clr.g + clr.b) / (255.0f * 3.0f));

		glm::vec3				pt = gp.mExactWorldBounds.atUnit(glm::vec3(fpt.x, fpt.y, v));

		// Continue from the previous end point
		kt::math::Bezier3f&		c(p.mCurve);
		c.mP0 = p.mCurve.mP3;
		c.mP3 = pt;

		c.mP1 = glm::vec3(0, 0, -5);
		c.mP2 = glm::vec3(0, 0, -5);

		// Blur things out a little on the blue, because why not, even though you
		// really can't tell.
		p.mEndAlpha = glm::mix(0.1f, 1.0f, static_cast<float>(clr.b)/255.0f);

		if (++x >= cols) {
			x = 0;
			++y;
		}
	}
}

} // namespace cs
