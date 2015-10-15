#ifndef CS_FEEDER_H_
#define CS_FEEDER_H_

#include "kt/async/worker_thread.h"
#include "generator.h"

namespace cs {
class Settings;

/**
 * @class cs::Feeder
 * @brief Manage the generation process.
 * @description I manage the parts that actually generate new particle paths, feeding
 * that info to the render.
 */
class Feeder {
public:
	Feeder() = delete;
	Feeder(const Feeder&) = delete;
	Feeder(const kt::Cns&, const cs::Settings&);

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

		GeneratorParams		mParams;
		GeneratorRef		mGenerator;
		ParticleList		mParticles;
	};

	const kt::Cns&			mCns;
	const cs::Settings&		mSettings;
	bool					mHasFrame = false;
	ParticleList			mFrame;
	GeneratorParams			mParams;
	std::vector<GeneratorRef> mGeneratorList;
	size_t					mCurrentGenerator = 0;
//	GeneratorRef			mRndGenerator;
//	GeneratorRef			mLineGenerator;
//	GeneratorRef			mImageGenerator;
//	GeneratorRef			mCurrentGenerator;
	kt::async::OperatorThread<Op, int>	mWorker;
};

} // namespace cs

#endif
