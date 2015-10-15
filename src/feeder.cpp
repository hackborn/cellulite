#include "feeder.h"

#include "kt/app/kt_cns.h"
#include "settings.h"

namespace cs {

namespace {
void			add_gen(GeneratorRef g, std::vector<GeneratorRef> &out) {
	if (g) out.push_back(g);
}
}

/**
 * @class cs::Feeder
 */
Feeder::Feeder(const kt::Cns &cns, const cs::Settings &s)
		: mCns(cns)
		, mSettings(s)
		, mWorker([this](Op &op){handle(op);}) {
	add_gen(GeneratorRef(new RandomLineGenerator()), mGeneratorList);
	add_gen(GeneratorRef(new RandomGenerator()), mGeneratorList);
	add_gen(GeneratorRef(new ImageGenerator()), mGeneratorList);
	add_gen(GeneratorRef(new RandomGenerator()), mGeneratorList);
	mCurrentGenerator = mGeneratorList.size();
}

void Feeder::start(const ParticleList &list) {
	mParams.setTo(mCns);

	mWorker.run([this, &list](Op &op) {
		op.mParams = mParams;
		op.mGenerator = nextGenerator();
		op.mParticles = list;
	});
}

void Feeder::update() {
	mWorker.update();
}

void Feeder::handle(Op &op) {
	mHasFrame = true;
	mFrame.swap(op.mParticles);
	mFrame.setParametersFrom(op.mParticles);
}

void Feeder::getFrame(ParticleList &out) {
	if (!mHasFrame) return;

	const size_t		size = (out.size() <= mFrame.size() ? out.size() : mFrame.size());
	if (size < 1) return;

	// Copy in the new data
	Particle*			src(&mFrame.front());
	Particle*			src_end = src + size;
	Particle*			dst(&out.front());
	while (src < src_end) {
		src->mCurve.mP0 = dst->mPosition;
		dst->mCurve = src->mCurve;

		++src;
		++dst;
	}
	out.setParametersFrom(mFrame);

	// Generate the next frame
	mHasFrame = false;
	mWorker.run([this](Op &op) {
		op.mParams = mParams;
		op.mGenerator = nextGenerator();
		op.mParticles.swap(mFrame);
	});
}

GeneratorRef Feeder::nextGenerator() {
	if (mGeneratorList.empty()) return nullptr;

	++mCurrentGenerator;
	if (mCurrentGenerator >= mGeneratorList.size()) mCurrentGenerator = 0;
	return mGeneratorList[mCurrentGenerator];
}

/**
 * @class cs::Feeder::Op
 */
Feeder::Op::Op() {
}

void Feeder::Op::run(int&) {
	if (!mGenerator) return;

	mGenerator->update(mParams, mParticles);
}

} // namespace cs
