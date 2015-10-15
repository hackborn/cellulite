#ifndef CS_GENERATOR_H_
#define CS_GENERATOR_H_

#include <cinder/PolyLine.h>
#include <cinder/Rand.h>
#include "kt/math/geometry.h"
#include "particle_list.h"

namespace kt { class Cns; }
namespace cs {
class Generator;
using GeneratorRef = std::shared_ptr<Generator>;

/**
 * @class cs::GeneratorParams
 */
class GeneratorParams {
public:
	GeneratorParams() { }
	GeneratorParams(const kt::Cns &cns) { setTo(cns); }

	void				setTo(const kt::Cns&);

	kt::math::Cube		mWorldBounds,
						mExactWorldBounds;
};

/**
 * @class cs::Generator
 * @brief Abstract behaviour for filling particle data.
 */
class Generator {
public:
	virtual ~Generator() { }

	// Update the curve. Ideally, treat the curve's endpoint
	// as the particle's current position.
	void				update(const GeneratorParams&, ParticleList&);

protected:
	virtual void		onUpdate(const GeneratorParams&, ParticleList&) = 0;

	Generator() { }
};

/**
 * @class cs::RandomGenerator
 * @brief Fill with random velocities.
 */
class RandomGenerator : public Generator {
public:
	enum class Mode		{ kAnywhere, kClosest };
	RandomGenerator(const Mode m = Mode::kClosest) : mMode(m) { }

	void				onUpdate(const GeneratorParams&, ParticleList&) override;

private:
	void				onUpdateAnywhere(const GeneratorParams&, ParticleList&);
	void				onUpdateClosest(const GeneratorParams&, ParticleList&);

	using PtList = std::vector<glm::vec3>;
	glm::vec3			nextPt(const kt::math::Cube&);
	glm::vec3			nextOffset(const float scale);
	glm::vec3			popClosest(const glm::vec3&, PtList&) const;

	const Mode			mMode;
	cinder::Rand		mRand;
	PtList				mClosestPts;
};

/**
 * @class cs::PolyLineGenerator
 * @brief Fill with attractors to a polyline.
 */
class PolyLineGenerator : public Generator {
public:
	PolyLineGenerator() { }
	PolyLineGenerator(const ci::PolyLine3f &line) : mLine(line) { }

	void				onUpdate(const GeneratorParams&, ParticleList&) override;

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

	void				onUpdate(const GeneratorParams&, ParticleList&) override;

private:
	void				nextLines(const kt::math::Cube&);
	glm::vec3			nextPt(const kt::math::Cube&);

	std::vector<ci::PolyLine3f> mLines;
	cinder::Rand		mRand;
};


/**
 * @class cs::ImageGenerator
 * @brief Fill with attractors to an image.
 */
class ImageGenerator : public Generator {
public:
	ImageGenerator() { }

	void				onUpdate(const GeneratorParams&, ParticleList&) override;
};

} // namespace cs

#endif
