#ifndef CS_PARTICLERENDER_H_
#define CS_PARTICLERENDER_H_

#include <cinder/gl/Batch.h>
#include <cinder/gl/Texture.h>
#include "particle_list.h"

namespace kt { class Cns; }
namespace cs {
class Settings;

/**
 * @class cs::ParticleRender
 * @brief Draw all particles.
 */
class ParticleRender {
public:
	ParticleRender() = delete;
	ParticleRender(const ParticleRender&) = delete;
	ParticleRender(const kt::Cns&, const cs::Settings&);

	void						push_back(const ParticleRef&);
	void						erase(const ParticleRef&);
	void						erase(const ParticleList&);

	void						update();
	void						draw();

private:
	void						drawParticles(size_t start, size_t end);

	const kt::Cns&				mCns;
	const cs::Settings&			mSettings;
	ParticleList				mParticles;

	const size_t				BUFFER_SIZE = 2000;
	ci::gl::VboRef				mInstanceDataVbo;
	ci::gl::TextureRef			mTexture;
	ci::gl::GlslProgRef			mGlsl;
	ci::gl::BatchRef			mBatch;
};

} // namespace cs

#endif
