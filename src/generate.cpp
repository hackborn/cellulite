#include "generate.h"

#include "kt/app/kt_cns.h"
#include "settings.h"

namespace cs {

/**
 * @class cs::Generate
 */
Generate::Generate(const kt::Cns &cns, const cs::Settings &s)
		: mCns(cns)
		, mSettings(s)
		, mWorker([this](Op &op){handle(op);}) {
	mRndGenerator.reset(new RandomGenerator());
	mLineGenerator.reset(new RandomLineGenerator());
	mImageGenerator.reset(new ImageGenerator());
}

void Generate::start(const ParticleList &list) {
	mParams.setTo(mCns);

	mWorker.run([this, &list](Op &op) {
		op.mParams = mParams;
		op.mGenerator = nextGenerator();
		op.mParticles = list;
	});
}

void Generate::update() {
	mWorker.update();
}

void Generate::handle(Op &op) {
	mHasFrame = true;
	mFrame.swap(op.mParticles);
	mFrame.mMaxCurveLength = op.mParticles.mMaxCurveLength;
	mFrame.mAverageCurveLength = op.mParticles.mAverageCurveLength;
	mFrame.mDuration = op.mParticles.mDuration;
}

void Generate::getFrame(ParticleList &out) {
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
	out.mMaxCurveLength = mFrame.mMaxCurveLength;
	out.mAverageCurveLength = mFrame.mAverageCurveLength;
	out.mDuration = mFrame.mDuration;

	// Generate the next frame
	mHasFrame = false;
	mWorker.run([this](Op &op) {
		op.mParams = mParams;
		op.mGenerator = nextGenerator();
		op.mParticles.swap(mFrame);
	});
}

GeneratorRef Generate::nextGenerator() {
	if (mCurrentGenerator == mLineGenerator) {
		mCurrentGenerator = mRndGenerator;
	} else if (mCurrentGenerator == mRndGenerator) {
		mCurrentGenerator = mImageGenerator;
	} else {
		mCurrentGenerator = mLineGenerator;
	}
	return mCurrentGenerator;
}

/**
 * @class cs::Generate::Op
 */
Generate::Op::Op() {
}

void Generate::Op::run(int&) {
	if (!mGenerator) return;

	mGenerator->update(mParams, mParticles);
}

} // namespace cs
