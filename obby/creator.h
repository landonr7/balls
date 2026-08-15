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

// Creating Box
b2BodyId createBox (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y);

// Creating Ball
b2BodyId createBall (b2WorldId& world, float pos_x, float pos_y, float radius);

// Creating Shifter
b2BodyId createShifter (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y);

// Creating Dot
b2BodyId createDot (b2WorldId& world, float pos_x, float pos_y, float radius);

// Creating Paddle
b2BodyId createPaddle (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y, b2JointId &jointId, bool direction);

/*
namespace creator {
	b2BodyId createBox (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y, b2BodyType type) {

		// Define a body
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};

		// Define a shape
		b2Polygon box = b2MakeBox(converter::pixelsToMeters<float>(size_x / 2.0), converter::pixelsToMeters<float>(size_y / 2.0));
		b2ShapeDef shapeDef = b2DefaultShapeDef();

		// Create body
		b2BodyId bodyId = b2CreateBody(world, &bodyDef);
		b2Body_SetType(bodyId, type);

		// Create shape
		b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
		b2Shape_SetDensity(shapeId, 1.0f, 1);
		b2Shape_SetFriction(shapeId, 0.3f);

		// Create SFML shape
		sf::RectangleShape* shape = new sf::RectangleShape({size_x, size_y});
		shape->setOrigin({size_x / 2.0f, size_y / 2.0f});
		shape->setPosition({pos_x, pos_y});

		if (type == b2_dynamicBody) {
			shape->setFillColor(sf::Color::Red);
		} else {
			shape->setFillColor(sf::Color::White);
		}

		b2Body_SetUserData(bodyId, shape);

		return bodyId;
	}

	b2BodyId createBall (b2WorldId& world, float pos_x, float pos_y, float radius, b2BodyType type) {

		// Define a body
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};

		// Define a circle
		b2Circle circle;
		circle.center = (b2Vec2){0.0f, 0.0f};
		circle.radius = converter::pixelsToMeters<float>(radius);
		b2ShapeDef shapeDef = b2DefaultShapeDef();

		// Create a body
		b2BodyId bodyId = b2CreateBody(world, &bodyDef);
		b2Body_SetType(bodyId, type);

		// Create a circle
		b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
		b2Shape_SetDensity(shapeId, 1.0f, 1);
		b2Shape_SetFriction(shapeId, 0.3f);

		// Give each ball a random linear velocity
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(-1.0, 1.0);
		float randVelocity = dist(gen);
		b2Body_SetLinearVelocity(bodyId, (b2Vec2){(float)randVelocity, 0.0f});

		// Create SFML shape
		sf::CircleShape* shape = new sf::CircleShape(radius);
		shape->setOrigin({radius, radius});
		shape->setPosition({pos_x, pos_y});
		shape->setPointCount(100);

		shape->setFillColor(sf::Color::Red);

		b2Body_SetUserData(bodyId, shape);

		return bodyId;
	}

	// Creating Shifter Obby
	b2BodyId createShifter (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y, b2BodyType type) {

		// Define a body
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};
		bodyDef.rotation = b2MakeRot(converter::degToRad<float>(0));

		// Define a shape
		b2Polygon box = b2MakeBox(converter::pixelsToMeters<float>(size_x / 2.0), converter::pixelsToMeters<float>(size_y / 2.0));
		b2ShapeDef shapeDef = b2DefaultShapeDef();

		// Create body
		b2BodyId bodyId = b2CreateBody(world, &bodyDef);
		b2Body_SetType(bodyId, type);

		// Create shape
		b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
		b2Shape_SetDensity(shapeId, 1.0f, 1);
		b2Shape_SetFriction(shapeId, 0.3f);

		// Give each ball a random linear velocity
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(-3.0, 3.0);
		float randVelocity = dist(gen);
		b2Body_SetLinearVelocity(bodyId, (b2Vec2){(float)randVelocity, 0.0f});

		// Create SFML shape
		sf::RectangleShape* shape = new sf::RectangleShape({size_x, size_y});
		shape->setOrigin({size_x / 2.0f, size_y / 2.0f});
		shape->setPosition({pos_x, pos_y});

		b2Rot q = b2Body_GetRotation(bodyId);
		shape->setRotation(sf::radians(b2Rot_GetAngle(q)));

		if (type == b2_dynamicBody) {
			shape->setFillColor(sf::Color::Blue);
		} else {
			shape->setFillColor(sf::Color::White);
		}

		b2Body_SetUserData(bodyId, shape);

		return bodyId;
	}


	b2BodyId createPlinko (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y, b2BodyType type) {

		// Define a body
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};
		bodyDef.rotation = b2MakeRot(converter::degToRad<float>(45));

		// Define a shape
		b2Polygon box = b2MakeBox(converter::pixelsToMeters<float>(size_x / 2.0), converter::pixelsToMeters<float>(size_y / 2.0));
		b2ShapeDef shapeDef = b2DefaultShapeDef();

		// Create body
		b2BodyId bodyId = b2CreateBody(world, &bodyDef);
		b2Body_SetType(bodyId, type);

		// Create shape
		b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
		b2Shape_SetDensity(shapeId, 1.0f, 1);
		b2Shape_SetFriction(shapeId, 0.3f);

		// Create SFML shape
		sf::RectangleShape* shape = new sf::RectangleShape({size_x, size_y});
		shape->setOrigin({size_x / 2.0f, size_y / 2.0f});
		shape->setPosition({pos_x, pos_y});

		b2Rot q = b2Body_GetRotation(bodyId);
		shape->setRotation(sf::radians(b2Rot_GetAngle(q)));

		if (type == b2_dynamicBody) {
			shape->setFillColor(sf::Color::Blue);
		} else {
			shape->setFillColor(sf::Color::White);
		}

		b2Body_SetUserData(bodyId, shape);

		return bodyId;
	}
}
*/
