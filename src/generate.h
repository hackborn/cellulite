#ifndef CS_GENERATE_H_
#define CS_GENERATE_H_

#include "kt/async/worker_thread.h"
#include "generator.h"

namespace kt { class Cns; }
namespace cs {
class Settings;

/**
 * @class cs::Generate
 * @brief Manage the generation process.
 * @description I manage the parts that actually generate the new curve values.
 */
class Generate {
public:
	Generate() = delete;
	Generate(const Generate&) = delete;
	Generate(const kt::Cns&, const cs::Settings&);

	void					start(const ParticleList&);
	void					update();

	bool					hasFrame() const { return mHasFrame; }
	void					getFrame(ParticleList&);

private:
	GeneratorRef			nextGenerator();

	class Op;
	void					handle(Op&);

	class Op {
	public:
		Op();

		bool				replace(Op&, int&) { return false; }
		void				run(int&);

		kt::math::Cube		mWorldBounds;
		// Handle generating the new values
		GeneratorRef		mGenerator;
		ParticleList		mParticles;
	};

	const kt::Cns&			mCns;
	const cs::Settings&		mSettings;
	bool					mHasFrame = false;
	ParticleList			mFrame;
	GeneratorRef			mRndGenerator;
	GeneratorRef			mLineGenerator;
	GeneratorRef			mCurrentGenerator;
	kt::async::OperatorThread<Op, int>	mWorker;
};

} // namespace cs

#endif
