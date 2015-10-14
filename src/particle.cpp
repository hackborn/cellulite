#include "particle.h"

namespace cs {

/**
 * @class cs::Particle
 */
float Particle::encodeColor(const ci::ColorA &clr) {
	int32_t				eclr(	static_cast<uint8_t>(clr.a*255.0f) << 24
								| static_cast<uint8_t>(clr.r*255.0f) << 16
								| static_cast<uint8_t>(clr.g*255.0f) << 8
								| static_cast<uint8_t>(clr.b*255.0f) << 0);
	return static_cast<float>(eclr);
}

} // namespace cs
