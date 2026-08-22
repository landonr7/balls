#include "entity.h"
#include <box2d/box2d.h>
#include "creator.h"
#include <iostream>
#include <cstring>
#include <vector>

// Screen Resolution
const int WINDOW_WIDTH = 215;
const int WINDOW_HEIGHT = 466;

using namespace converter;

// Plinko Size
const int PLINKO_SIZE = 10;

// Shifter Size
const int SHIFTER_SIZE = 22;

// Paddle Size
const int PADDLE_SIZE = 15;

// Dot Size
const int DOT_SIZE = 10;

// Conveyer Size
const int CONV_SIZE = PADDLE_SIZE;

// Ball Size
const int BALL_SIZE = 18;

// Entity updater
void Entity::update(sf::Clock clock) {}

// Entity ball y level getter
float Entity::getBallY() { return 0.0f; }

// Entity game getter
std::string Entity::getName() { return std::string(b2Body_GetName(body)); }

// Entity drawer
void Entity::draw(sf::RenderWindow& window, float cameraY) {
    shape->setPosition({
        converter::metersToPixels<float>(b2Body_GetPosition(body).x),
        converter::metersToPixels<float>(b2Body_GetPosition(body).y) - (float)cameraY
    });

    b2Rot rot = b2Body_GetRotation(body);
    shape->setRotation(sf::radians(b2Rot_GetAngle(rot)));
    window.draw(*shape);
}

// Entity body setter
void Entity::setBody(b2BodyId newBody) {

    body = newBody;
    b2Body_SetUserData(body, this);
}

// Entity body getter
b2BodyId Entity::getBody() const { return body; }

// Entity body is destroyed checker
bool Entity::isDestroyed() const { return destroyed; }

// Entity body destroyer
void Entity::destroy() { destroyed = true; }

// Entity collision handler
void Entity::collision(Entity* a, Entity* b) {} 

// Wall constructor
Wall::Wall(b2WorldId world, float posX, float posY, float sizeX, float sizeY) {

    // Create SFML shape
    shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(sizeX, sizeY));
    shape->setOrigin({sizeX / 2.0f, sizeY / 2.0f});
    shape->setPosition({posY, posY});
    shape->setFillColor(sf::Color::White);

    setBody(createBox(
        world,
        posX,
        posY,
        sizeX,
        sizeY
    ));
}

// Ball "Player" constructor
Ball::Ball(b2WorldId world, float posX, float posY, int pic) {

    // Image of player
    std::string picPath = "./build/assets/images/" + std::to_string(pic) + ".jpg";

    if (!texture.loadFromFile(picPath))
        std::cout << "Image not found!\n" << std::endl;

    // Create SFML shape
    auto circle = std::make_unique<sf::CircleShape>(BALL_SIZE);
    circle->setOrigin({BALL_SIZE, BALL_SIZE});
    circle->setPointCount(100);

    // Transfer ownership of circle pointer to shape
    shape = std::move(circle);
    shape->setTexture(&texture);

    setBody(createBall(
        world,
        posX,
        posY,
        BALL_SIZE,
        pic
    ));
}

// Ball y level getter
float Ball::getBallY()  {
    
    return converter::metersToPixels(b2Body_GetPosition(body).y);
} 

// Dot constructor
Dot::Dot(b2WorldId world, float posX, float posY) {

    // Create SFML shape
    auto circle = std::make_unique<sf::CircleShape>(DOT_SIZE);
    circle->setOrigin({DOT_SIZE, DOT_SIZE});
    circle->setPosition({posX, posX});
    circle->setPointCount(100);
    circle->setFillColor(sf::Color(255, 0, 255));

    // Transfer ownership of circle pointer to shape
    shape = std::move(circle);

    setBody(createDot(
        world,
        posX,
        posY,
        DOT_SIZE
    ));
}

// Dot collision handler
void Dot::collision(Entity* a, Entity* b)  {

    // Add "+ 1" to body name so strcmp skips the first character (which would be [01234]) but still finds \0
    if (strcmp(b2Body_GetName(a->getBody()) + 1, "Player") == 0 && strcmp(b2Body_GetName(b->getBody()), "Dot") == 0)
        b->destroy();
    else if (strcmp(b2Body_GetName(a->getBody()), "Dot") == 0 && strcmp(b2Body_GetName(b->getBody()) + 1, "Player") == 0)
        a->destroy();            
}

// Shifter constructor
Shifter::Shifter(b2WorldId world, float posX, float posY) {

    // Create SFML shape
    shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(SHIFTER_SIZE * 5, SHIFTER_SIZE));
    shape->setOrigin({SHIFTER_SIZE * 5 / 2.0f, SHIFTER_SIZE / 2.0f});
    shape->setPosition({posX, posY});
    shape->setFillColor(sf::Color(255, 165, 0));

    setBody(createShifter(
        world,
        posX,
        posY,
        SHIFTER_SIZE * 5,
        SHIFTER_SIZE
    ));
}

// Shifter direction updater
void Shifter::update(sf::Clock lock)  {

    float x = converter::metersToPixels(
        b2Body_GetPosition(body).x
    );

    if (x <= 55)
        b2Body_SetLinearVelocity(body, -b2Body_GetLinearVelocity(body));
    else if (x >= 160)
        b2Body_SetLinearVelocity(body, -b2Body_GetLinearVelocity(body));
}

// Plinko constructor
Plinko::Plinko(b2WorldId world, float posX, float posY) {

    shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(PLINKO_SIZE, PLINKO_SIZE));

    shape->setOrigin({PLINKO_SIZE / 2.0f, PLINKO_SIZE / 2.0f});
    shape->setPosition({posY, posY});
    shape->setFillColor(sf::Color::Yellow);

    setBody(createPlinko(
        world,
        posX,
        posY,
        PLINKO_SIZE,
        PLINKO_SIZE
    ));	
}

// Paddle constructor
Paddle::Paddle(b2WorldId world, float posX, float posY, bool direction) {

    // Create SFML shape
    shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(PADDLE_SIZE * 5, PADDLE_SIZE));
    shape->setOrigin({PADDLE_SIZE * 5 / 2.0f, PADDLE_SIZE / 2.0f});
    shape->setPosition({posX, posY});

    shape->setFillColor(sf::Color::Green);
    setBody(createPaddle(
        world,
        posX,
        posY,
        PADDLE_SIZE * 5,
        PADDLE_SIZE,
        joint,
        direction
    ));
}

// Paddle updater
void Paddle::update(sf::Clock clock)  {

    float angle = converter::radToDeg<float>(b2RevoluteJoint_GetAngle(joint));

    float motorSpeed = b2RevoluteJoint_GetMotorSpeed(joint);

    // If reached min or max rotation angle, invert motor speed
    if (angle <= -50.0f || angle >= 20.0f)
        b2RevoluteJoint_SetMotorSpeed(joint, -motorSpeed);
}

// Conveyer constructor
Conveyer::Conveyer(b2WorldId world, float posX, float posY, bool direction) {

    // Create SFML shape
    auto circle = std::make_unique<sf::CircleShape>(CONV_SIZE);
    circle->setOrigin({CONV_SIZE, CONV_SIZE});
    circle->setPosition({posX, posX});
    circle->setPointCount(6);
    circle->setFillColor(sf::Color::Blue);

    // Transfer ownership of circle pointer to shape
    shape = std::move(circle);

    setBody(createConveyer(
        world,
        posX,
        posY,
        CONV_SIZE,
        direction
    ));
}

// Conveyer updater
void Conveyer::update(sf::Clock clock)  {

    timer = clock.getElapsedTime().asSeconds();

    // If three seconds has passed, inverse the rotation
    if ((int)timer % 3 == 0) {
        b2Body_SetAngularVelocity(body, -b2Body_GetAngularVelocity(body));
    }
}
