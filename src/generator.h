#ifndef CS_GENERATOR_H_
#define CS_GENERATOR_H_

#include <cinder/PolyLine.h>
#include <cinder/Rand.h>
#include "kt/math/geometry.h"
#include "particle_list.h"

namespace cs {
class Generator;
using GeneratorRef = std::shared_ptr<Generator>;

/**
 * @class cs::Generator
 * @brief Abstract behaviour for filling particle data.
 */
class Generator {
public:
	virtual ~Generator() { }

	// Update the curve. Ideally, treat the curve's endpoint
	// as the particle's current position.
	virtual void		update(const kt::math::Cube&, std::vector<Particle>&) = 0;

protected:
	Generator() { }
};

/**
 * @class cs::RandomGenerator
 * @brief Fill with random velocities.
 */
class RandomGenerator : public Generator {
public:
	RandomGenerator() { }

	void				update(const kt::math::Cube&, std::vector<Particle>&) override;

private:
	glm::vec3			nextPt(const kt::math::Cube&);

	cinder::Rand		mRand;
};

/**
 * @class cs::PolyLineGenerator
 * @brief Fill with attractors to a polyline.
 */
class PolyLineGenerator : public Generator {
public:
	PolyLineGenerator() { }
	PolyLineGenerator(const ci::PolyLine3f &line, const float min, const float max) : mLine(line), mMin(min), mMax(max) { }

	void				update(const kt::math::Cube&, std::vector<Particle>&) override;

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

/**
 * @class cs::RandomLineGenerator
 * @brief Fill with attractors to random polylines.
 */
class RandomLineGenerator : public Generator {
public:
	RandomLineGenerator() { }

	void				update(const kt::math::Cube&, std::vector<Particle>&) override;

private:
	void				nextLines(const kt::math::Cube&);
	glm::vec3			nextPt(const kt::math::Cube&);

	std::vector<ci::PolyLine3f> mLines;
	cinder::Rand		mRand;
};

} // namespace cs

#endif
