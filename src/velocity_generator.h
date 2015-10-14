#ifndef CS_VELOCITYGENERATOR_H_
#define CS_VELOCITYGENERATOR_H_

#include <cinder/PolyLine.h>
#include <cinder/Rand.h>
#include "velocity_plane.h"

namespace cs {
class VelocityCube;

/**
 * @class cs::VelocityGenerator
 * @brief Abstract behaviour for filling a velocity cube.
 */
class VelocityGenerator {
public:
	virtual ~VelocityGenerator() { }

	void				update(VelocityCube&);

protected:
	VelocityGenerator() { }
	virtual void		onUpdate(VelocityCube&) = 0;
};

/**
 * @class cs::RandomGenerator
 * @brief Fill with random velocities.
 */
class RandomGenerator : public VelocityGenerator {
public:
	RandomGenerator() { }
	RandomGenerator(const float min, const float max) : mMin(min), mMax(max) { }

	void				onUpdate(VelocityCube&) override;

private:
	cinder::Rand		mRand;
	const float			mMin = -0.5f,
						mMax = 0.05f;
};

/**
 * @class cs::PolyLineGenerator
 * @brief Fill with attractors to a polyline.
 */
class PolyLineGenerator : public VelocityGenerator {
public:
	PolyLineGenerator() { }
	PolyLineGenerator(const ci::PolyLine3f &line, const float min, const float max) : mLine(line), mMin(min), mMax(max) { }

	void				onUpdate(VelocityCube&) override;

private:
	std::vector<ci::PolyLine3f> mLines;
	ci::PolyLine3f		mLine;
	cinder::Rand		mRand;
	const float			mMin = -0.5f,
						mMax = 0.05f;
	const float			mDistance = 10.0f;
	const float			mLineMin = 0.25f,
						mLineMax = 32.0f;
};

} // namespace cs

#endif
