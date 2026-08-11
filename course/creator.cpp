#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>

namespace converter {
	constexpr double PIXELS_PER_METERS = 30.0;
	constexpr double PI = 3.14159;

	template<typename T> constexpr T pixelsToMeters(const T& x){return x / PIXELS_PER_METERS;};

	template<typename T> constexpr T metersToPixels(const T& x){return x * PIXELS_PER_METERS;};

	template<typename T> constexpr T degToRad(const T& x){return PI * x / 180.0;};

	template<typename T> constexpr T radToDeg(const T& x){return 180.0 * x / PI;};

}

b2BodyId createBox (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y) {

    // Define a body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};

    // Define a shape
    b2Polygon box = b2MakeBox(converter::pixelsToMeters<float>(size_x / 2.0), converter::pixelsToMeters<float>(size_y / 2.0));
    b2ShapeDef shapeDef = b2DefaultShapeDef();

    // Create body
    b2BodyId bodyId = b2CreateBody(world, &bodyDef);
    b2Body_SetType(bodyId, b2_staticBody);

    // Create shape
    b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
    b2Shape_SetDensity(shapeId, 1.0f, 1);
    b2Shape_SetFriction(shapeId, 0.3f);

    return bodyId;
}

b2BodyId createPlinko (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y) {

    // Define a body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};
    bodyDef.rotation = b2MakeRot(converter::degToRad<float>(45));
    
    // Define a shape
    b2Polygon box = b2MakeBox(converter::pixelsToMeters<float>(size_x / 2.0), converter::pixelsToMeters<float>(size_y / 2.0));
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    
    // Create body
    b2BodyId bodyId = b2CreateBody(world, &bodyDef);
    b2Body_SetType(bodyId, b2_staticBody);

    // Create shape
    b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
    b2Shape_SetDensity(shapeId, 1.0f, 1);
    b2Shape_SetFriction(shapeId, 0.3f);
    b2Shape_SetRestitution(shapeId, .5f);
    
    return bodyId;
}

b2BodyId createBall (b2WorldId& world, float pos_x, float pos_y, float radius) {

    // Define a body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};

    // Define a circle
    b2Circle circle;
    circle.center = (b2Vec2){0.0f, 0.0f};
    circle.radius = converter::pixelsToMeters<float>(radius);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.enableContactEvents = true;

    // Create a body
    b2BodyId bodyId = b2CreateBody(world, &bodyDef);
    b2Body_SetType(bodyId, b2_dynamicBody);

    // Create a circle
    b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
    b2Shape_SetDensity(shapeId, 1.0f, 1);
    b2Shape_SetFriction(shapeId, 0.3f);
    b2Shape_SetRestitution(shapeId, .35f);

    // Give each ball a random linear velocity
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0, 1.0);
    float randVelocity = dist(gen);
    b2Body_SetLinearVelocity(bodyId, (b2Vec2){(float)randVelocity, 0.0f});

    return bodyId;
}

// Creating Shifter Obby
b2BodyId createShifter (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y) {

    // Define a body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};
    bodyDef.rotation = b2MakeRot(converter::degToRad<float>(0));

    // Define a shape
    b2Polygon box = b2MakeBox(converter::pixelsToMeters<float>(size_x / 2.0), converter::pixelsToMeters<float>(size_y / 2.0));
    b2ShapeDef shapeDef = b2DefaultShapeDef();

    // Create body
    b2BodyId bodyId = b2CreateBody(world, &bodyDef);
    b2Body_SetType(bodyId, b2_kinematicBody);

    // Create shape
    b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
    b2Shape_SetDensity(shapeId, 1.0f, 1);
    b2Shape_SetFriction(shapeId, 0.3f);

    // Give each ball a random linear velocity
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(1.0, 5.0);
    float randVelocity = dist(gen);
    b2Body_SetLinearVelocity(bodyId, (b2Vec2){(float)randVelocity, 0.0f});

    return bodyId;
}

b2BodyId createDot (b2WorldId& world, float pos_x, float pos_y, float radius) {

    // Define a body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};

    // Define a circle
    b2Circle circle;
    circle.center = (b2Vec2){0.0f, 0.0f};
    circle.radius = converter::pixelsToMeters<float>(radius);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.enableContactEvents = true;

    // Create a body
    b2BodyId bodyId = b2CreateBody(world, &bodyDef);
    b2Body_SetType(bodyId, b2_kinematicBody);

    // Create a circle
    b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
    b2Shape_SetDensity(shapeId, 1.0f, 1);
    b2Shape_SetFriction(shapeId, 0.3f);
    b2Shape_SetRestitution(shapeId, 1.0f);
    
    return bodyId;
}
