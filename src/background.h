#ifndef CS_BACKGROUND_H_
#define CS_BACKGROUND_H_

#include <atomic>
#include <thread>
#include <cinder/Surface.h>
#include <cinder/Vector.h>
#include <cinder/gl/Batch.h>
#include <cinder/gl/Texture.h>

namespace cs {
class Settings;

/**
 * @class cs::Background
 * @brief Draw the background.
 * @description Most of this class is actually devoted to rendering the background image.
 */
class Background {
public:
	Background() = delete;
	Background(const Background&) = delete;
	Background(const cs::Settings&, const glm::ivec2 &window_size);
	~Background();

	void					update();
	void					draw();

private:
	// Run on worker thread
	void					generateImage();

	const cs::Settings&		mSettings;
	ci::gl::Texture2dRef	mTexture;
	ci::gl::BatchRef		mBatch;

	std::thread				mThread;
	glm::ivec2				mWorkerWindowSize = glm::ivec2(0);
	ci::Color				mWorkerColor;
	ci::Surface8u			mWorkerSurface;
	std::atomic_bool		mHasWorkerSurface;
};

} // namespace cs

#endif
