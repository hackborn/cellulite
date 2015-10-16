#include "particle_view.h"

#include <cinder/gl/Batch.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/scoped.h>
#include <cinder/CinderMath.h>
#include <cinder/ImageIo.h>
#include "kt/app/kt_cns.h"
#include "kt/app/kt_environment.h"
#include "feeder.h"
#include "settings.h"

namespace cs {

/**
 * @class cs::ParticleView
 */
ParticleView::ParticleView(const kt::Cns &cns, const cs::Settings &settings, Feeder &f)
		: mCns(cns)
		, mSettings(settings)
		, mFeeder(f)
		, mRender(cns, settings) {
	// SETUP ACCENTS
	mAccentForces.fill(128);
}

void ParticleView::initializeParticles() {
	// SETUP PARTICLES
	mParticles.resize(mSettings.mParticleCount);
	RandomGenerator			gen(RandomGenerator::Mode::kAnywhere);
	gen.update(GeneratorParams(mCns), mParticles);
	for (auto& p : mParticles) {
		p.mPosition = p.mCurve.mP0 = p.mCurve.mP3;
		p.mAlpha = p.mStartAlpha = p.mEndAlpha = 1.0f;
	}

	mFeeder.start(mParticles);
}

void ParticleView::update() {
	updateAccents();

	// Hold
	if (mStage == Stage::kHold) {
		if (mTimer.elapsed() >= mHoldDuration) {
			if (mFeeder.hasFrame()) {
				mFeeder.getFrame(mParticles);
				mTransitionDuration = mParticles.mTransitionDuration;
				mHoldDuration = mParticles.mHoldDuration;
				mStage = Stage::kTransition;
				mTimer.start();
			}
		}
	// Transition
	} else {
		if (mTimer.elapsed() >= mTransitionDuration) {
			mStage = Stage::kHold;
			mTimer.start();
		} else {
			const float		t = static_cast<float>(kt::math::s_curved(mTimer.elapsed() / mTransitionDuration));
			for (auto& p : mParticles) {
				p.mPosition = p.mCurve.point(t);
				p.mAlpha = glm::mix(p.mStartAlpha, p.mEndAlpha, t);

				// Blur out a little based on distance
				p.mAlpha *= mSettings.mRangeZ.convert(p.mPosition.z, kt::math::Rangef(0.1f, 1.0f));

				if (mAddAccentTick == 0 && p.mHasAccents && mAccentParticles.size() < mSettings.mAccentParticleCount) {
					mAccentParticles.push_back(Particle(p.mPosition, p.mAlpha * 0.25f));
				}
			}
		}
	}

	static const size_t		ADD_ACCENT_TICKS = 5;
	++mAddAccentTick;
	if (mAddAccentTick >= ADD_ACCENT_TICKS) mAddAccentTick = 0;
}

void ParticleView::draw() {
	mRender.drawParticles(mParticles);
	mRender.drawParticles(mAccentParticles);
}

void ParticleView::updateAccents() {
	// Accents always fall down and fade out, with a little random forces thrown in.

	for (auto& p : mAccentParticles) {
		p.mAlpha -= 0.002f;
		if (p.mAlpha <= 0.0f) {
			std::swap(p, mAccentParticles.back());
		} else {
			p.mPosition.y += 0.04f;
			// Apply forces.
			glm::vec3		unit = mCns.mWorldBounds.toUnit(p.mPosition);
			glm::vec3		force = mAccentForces.at(unit);
			p.mPosition += (force * 0.00000000015f);
		}
	}
	while (!mAccentParticles.empty() && mAccentParticles.back().mAlpha <= 0.0f) {
		mAccentParticles.pop_back();
	}
}

} // namespace cs
