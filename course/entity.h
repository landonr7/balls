#include <box2d/box2d.h>
#include "creator.h"
#include <iostream>
#include <cstring>
#include <vector>

#pragma once

using namespace converter;

// Entity is the base class that defines all entities in the world
class Entity {

protected:
    // Body of entity
	b2BodyId body;
    // Bodydef of entity
    b2BodyDef bodyDef;
    // Name of entity body
    const char *name;
    // SFML shape of body
    std::unique_ptr<sf::Shape> shape;
    // Destroy body
    bool destroyed = false;

public:
    // Entity destructor
	virtual ~Entity() = default;

    // Entity updater
	virtual void update(sf::Clock clock);

    // Entity y level getter
    virtual float getBallY();

    // Entity body name getter
    virtual std::string getName();

    // Entity drawer
    virtual void draw(sf::RenderWindow& window, float cameraY);

    // Entity body setter
    void setBody(b2BodyId newBody);

    // Entity body getter
	b2BodyId getBody() const;

    // Entity destroyed checker
    bool isDestroyed() const;

    // Entity destoyer
    void destroy();

    // Entity collision handler
    virtual void collision(Entity* a, Entity* b);
};

// Wall class defines wall entity
class Wall : public Entity {

public:
    // Wall constructor
    Wall(b2WorldId world, float posX, float posY, float sizeX, float sizeY); 
};

// Ball class defines "Player" entity
class Ball : public Entity {

private:
    // Picture of "Player"
    sf::Texture texture;

public:
    // Ball constructor
	Ball(b2WorldId world, float posX, float posY, int pic);

    // Ball y level getter
    float getBallY() override;

};

// Dot class defines dot entity
class Dot : public Entity {

public:
    // Dot constructor
	Dot(b2WorldId world, float posX, float posY);

    // Dot collision with "Player" handler
    void collision(Entity* a, Entity* b) override;
};

// Shifter class defines shifter entity
class Shifter : public Entity {

public:
    // Shifter constructor
	Shifter(b2WorldId world, float posX, float posY);

    // Shifter updater
	void update(sf::Clock lock) override;
};

// Plinko class defines plinko entity
class Plinko : public Entity {

public:
    // Plinko constructor
	Plinko(b2WorldId world, float posX, float posY);
};

// Paddle class defines paddle entity
class Paddle : public Entity {

private:
    // Joint that connects paddle to revolute joint
    b2JointId joint;

public:
    // Paddle constructor
	Paddle(b2WorldId world, float posX, float posY, bool direction);

    // Paddle updater
    void update(sf::Clock clock) override; 
};

// Conveyer class defines conveyer entity
class Conveyer : public Entity {

private:
    // Keep track of time passing
    float timer = 0.0f;

public:
    // Conveyer constructor
	Conveyer(b2WorldId world, float posX, float posY, bool direction);

    // Conveyer updater
    void update(sf::Clock clock) override; 
};
