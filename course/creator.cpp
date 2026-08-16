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
    bodyDef.name = "Box";

    // Define a shape
    b2Polygon box = b2MakeBox(converter::pixelsToMeters<float>(size_x / 2.0), converter::pixelsToMeters<float>(size_y / 2.0));
    b2ShapeDef shapeDef = b2DefaultShapeDef();

    // Create body
    b2BodyId bodyId = b2CreateBody(world, &bodyDef);
    b2Body_SetType(bodyId, b2_staticBody);

    // Create shape
    b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
    b2Shape_SetDensity(shapeId, 1.0f, 1);
    b2Shape_SetFriction(shapeId, 10.0f);

    return bodyId;
}

b2BodyId createPlinko (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y) {

    // Define a body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};
    bodyDef.rotation = b2MakeRot(converter::degToRad<float>(45));
    bodyDef.name = "Plinko";
    
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
    bodyDef.name = "Player";

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
    bodyDef.name = "Shifter";
    
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
    bodyDef.name = "Dot";

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

b2BodyId createPaddle (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y, b2JointId &jointId, bool direction) {

    // Define a body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};
    bodyDef.rotation = b2MakeRot(converter::degToRad<float>(50));
    bodyDef.type = b2_dynamicBody;
    //bodyDef.name = "Paddle";
    
    // Create body
    b2BodyId bodyId = b2CreateBody(world, &bodyDef);

    // Define a shape
    b2Polygon box = b2MakeBox(converter::pixelsToMeters<float>(size_x / 2.0), converter::pixelsToMeters<float>(size_y / 2.0));
    b2ShapeDef shapeDef = b2DefaultShapeDef();

    // Create shape
    b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
    b2Shape_SetDensity(shapeId, 1.0f, 1);
    b2Shape_SetFriction(shapeId, 0.3f);
    b2Shape_SetRestitution(shapeId, 1.0f);
    
    // Create pivot
    b2BodyDef pivotDef = b2DefaultBodyDef();
    pivotDef.type = b2_staticBody;

    float pivotX;
    if (direction)
        pivotX = pos_x - size_x / 2.0f;
    else
        pivotX = pos_x + size_x / 2.0f;

    pivotDef.position = {converter::pixelsToMeters<float>(pivotX), converter::pixelsToMeters<float>(pos_y)};

    b2BodyId pivotBody = b2CreateBody(world, &pivotDef);

    b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();

    jointDef.bodyIdA = pivotBody;
    jointDef.bodyIdB = bodyId;

    jointDef.localAnchorA = {0.0f, 0.0f};

    float anchorX;
    if (direction)
        jointDef.localAnchorB = {converter::pixelsToMeters<float>(-size_x / 2.0f), 0.0f};
    else
        jointDef.localAnchorB = {converter::pixelsToMeters<float>(size_x / 2.0f), 0.0f};

    jointDef.enableLimit = true;

    jointDef.lowerAngle = converter::degToRad<float>(-50.0f);
    jointDef.upperAngle = converter::degToRad<float>(20.0f);

    jointDef.enableMotor = true;

    jointDef.motorSpeed = converter::degToRad<float>(-200.0f);

    jointDef.maxMotorTorque = 1000.0f;

    jointId = b2CreateRevoluteJoint(world, &jointDef);
    
    return bodyId;
}

b2BodyId createConveyer (b2WorldId& world, float pos_x, float pos_y, float radius, bool direction) {

    // Define a body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = (b2Vec2){converter::pixelsToMeters<float>(pos_x), converter::pixelsToMeters<float>(pos_y)};
    bodyDef.name = "Dot";
    bodyDef.type = b2_kinematicBody;
    
    if (direction) bodyDef.angularVelocity = -50.0f;
    else bodyDef.angularVelocity = 50.0f;

    // Define a circle
    b2Circle circle;
    circle.center = (b2Vec2){0.0f, 0.0f};
    circle.radius = converter::pixelsToMeters<float>(radius);
    b2ShapeDef shapeDef = b2DefaultShapeDef();

    // Create a body
    b2BodyId bodyId = b2CreateBody(world, &bodyDef);

    // Create a circle
    b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
    b2Shape_SetDensity(shapeId, 1.0f, 1);
    b2Shape_SetFriction(shapeId, 10.0f);
    b2Shape_SetRestitution(shapeId, 1.0f);
    
    return bodyId;
}
