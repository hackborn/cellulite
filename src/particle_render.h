#ifndef CS_PARTICLERENDER_H_
#define CS_PARTICLERENDER_H_

#include <cinder/gl/Batch.h>
#include <cinder/gl/Texture.h>
#include "kt/time/seconds.h"
#include "noise.h"
#include "particle_list.h"

namespace kt { class Cns; }
namespace cs {
class Feeder;
class Settings;

/**
 * @class cs::ParticleRender
 * @brief Draw all particles.
 */
class ParticleRender {
public:
	ParticleRender() = delete;
	ParticleRender(const ParticleRender&) = delete;
	ParticleRender(const kt::Cns&, const cs::Settings&, Feeder&, ParticleList&);

	void						update();
	void						draw();

private:
	void						drawParticles(size_t start, size_t end);

	const kt::Cns&				mCns;
	const cs::Settings&			mSettings;
	class Feeder&				mFeeder;
	Noise						mNoise;
	ParticleList&				mParticles;
	kt::time::Seconds			mTimer;
	enum class Stage			{ kTransition, kHold };
	Stage						mStage = Stage::kHold;
	double						mTransitionDuration = 0.0,
								mHoldDuration = 0.0;
	bool						mHasFrame = false;

	const size_t				BUFFER_SIZE = 2000;
	ci::gl::VboRef				mInstanceDataVbo;
	ci::gl::TextureRef			mTexture;
	ci::gl::GlslProgRef			mGlsl;
	ci::gl::BatchRef			mBatch;
};

} // namespace cs

#endif
