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
	void				update(const kt::math::Cube&, ParticleList&);

protected:
	virtual void		onUpdate(const kt::math::Cube&, ParticleList&) = 0;

	Generator() { }
};

/**
 * @class cs::RandomGenerator
 * @brief Fill with random velocities.
 */
class RandomGenerator : public Generator {
public:
	RandomGenerator() { }

	void				onUpdate(const kt::math::Cube&, ParticleList&) override;

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
	PolyLineGenerator(const ci::PolyLine3f &line) : mLine(line) { }

	void				onUpdate(const kt::math::Cube&, ParticleList&) override;

private:
	std::vector<ci::PolyLine3f> mLines;
	ci::PolyLine3f		mLine;
	cinder::Rand		mRand;
};

/**
 * @class cs::RandomLineGenerator
 * @brief Fill with attractors to random polylines.
 */
class RandomLineGenerator : public Generator {
public:
	RandomLineGenerator() { }

	void				onUpdate(const kt::math::Cube&, ParticleList&) override;

private:
	void				nextLines(const kt::math::Cube&);
	glm::vec3			nextPt(const kt::math::Cube&);

	std::vector<ci::PolyLine3f> mLines;
	cinder::Rand		mRand;
};

} // namespace cs

#endif
