#include "picker_3d.h"

#include <cinder/Ray.h>

namespace cs {

/**
 * @class cs::Picker3d::Data
 */
class Picker3d::Data {
public:
	Data() = delete;
	Data(ci::CameraPersp &c) : mCamera(c) { }

	ci::CameraPersp&	mCamera;
	glm::vec2			mWindowSize = glm::vec2(0, 0);
};

/**
 * @class cs::Picker3d
 */
Picker3d::Picker3d(ci::CameraPersp &c) {
	mData.reset(new Data(c));
}

void Picker3d::setTo(const glm::vec2 &window_size) {
	if (!mData) return;
	mData->mWindowSize = window_size;
}

bool Picker3d::pick(const glm::ivec2 &input, const float z,
					glm::vec3 &pickedPosition) const {
	if (!mData) return false;

	ci::CameraPersp&	cam(mData->mCamera);
	// generate a ray from the camera into our world
	float				u = static_cast<float>(input.x) / mData->mWindowSize.x;
	float				v = static_cast<float>(input.y) / mData->mWindowSize.y;

	// because OpenGL and Cinder use a coordinate system
	// where (0, 0) is in the LOWERleft corner, we have to flip the v-coordinate
	ci::Ray				ray = cam.generateRay(u , 1.0f - v, cam.getAspectRatio() );

	glm::vec3			plane_origin(glm::vec3(0.0f, 0.0f, z)),
						plane_normal(glm::vec3(0.0f, 0.0f, 1.0f));
	float				t;
	if (ray.calcPlaneIntersection(plane_origin, plane_normal, &t)) {
		pickedPosition = ray.calcPosition(t);
		return true;
	}
	return false;
}

} // namespace cs
