#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <random>
#include <algorithm>
#include <cmath>

#pragma once

namespace converter {
	constexpr double PIXELS_PER_METERS = 30.0;
	constexpr double PI = 3.14159;

	template<typename T> constexpr T pixelsToMeters(const T& x){return x / PIXELS_PER_METERS;};

    template<typename T> constexpr T metersToPixels(const T& x){return x * PIXELS_PER_METERS;};

	template<typename T> constexpr T degToRad(const T& x){return PI * x / 180.0;};

	template<typename T> constexpr T radToDeg(const T& x){return 180.0 * x / PI;};

}

// Create Box
b2BodyId createBox (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y);

// Create Ball
b2BodyId createBall (b2WorldId& world, float pos_x, float pos_y, float radius, int pic);

// Create Plinko
b2BodyId createPlinko (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y);

// Create Shifter
b2BodyId createShifter (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y);

// Create Dot
b2BodyId createDot (b2WorldId& world, float pos_x, float pos_y, float radius);

// Create Paddle
b2BodyId createPaddle (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y, b2JointId &jointId, bool direction);

// Create Conveyer
b2BodyId createConveyer (b2WorldId& world, float pos_x, float pos_y, float radius, bool direction);
