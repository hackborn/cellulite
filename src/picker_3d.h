#ifndef CS_APP_PICKER3D_H_
#define CS_APP_PICKER3D_H_

#include <memory>
#include <cinder/Camera.h>
#include <cinder/Vector.h>

namespace cs {

/**
 * @class cs::Picker3d
 * @brief Utility for picking in a 3D scene.
 */
class Picker3d {
public:
	// Construct empty
	Picker3d() { }
	// Construct with internal data
	Picker3d(ci::CameraPersp&);

	void					setTo(const glm::vec2 &window_size);

	// Pos is window coordinates (x, y).
	// z is the desired z-value in pickedPosition.
	bool					pick(	const glm::ivec2 &input, const float z,
									glm::vec3 &pickedPosition) const;

private:
	class Data;
	std::shared_ptr<Data>	mData;
};

} // namespace cs

#endif
