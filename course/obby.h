#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <vector>
#pragma once

// Obby is the base class for all entities in the world
class Obby {

protected:
    // Starting height of the obby
	float startY;

    // List of all entities in the world at any time
    std::vector<std::unique_ptr<Entity>> entities;

public:
    // Obby constructor
    Obby(float y) : startY(y) {};

    // Obby destructor
    virtual ~Obby() = default;

    // Obby builder
    virtual void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) = 0;

    // Obby position and logic updater
	virtual void update(sf::Clock clock);
      
    // Obby drawer to the window
	virtual void draw(sf::RenderWindow& window, float cameraY);  
};

// Boundary class defines the walls of the game area
class Boundary : public Obby {

public:
    // Boundary constructor
    Boundary(float y) : Obby(y) {};

    // Boundary builder
    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override; 
};

// Balls class defines the "Players" in the world
class Balls : public Obby {

public:
    // Balls constructor
    Balls(float y) : Obby(y) {};

    // Balls spawner
    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override;
};

// DotObby class defines the dot pit obstacle in the game area 
class DotObby : public Obby {

public:
    // DotObby constructor
    DotObby(float y) : Obby(y) {};

    // DotObby builder
    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override; 
};

// PlinkoObby class defines the plinko obstacle in the game area
class PlinkoObby : public Obby {

public:
    // PlinkoObby constructor
	PlinkoObby(float y) : Obby(y) {};

    // PlinkoObby builder
    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override; 
};

// ShifterObby class defines the shifting bars obstacle in the game area
class ShifterObby : public Obby {

public:
    // ShifterObby constructor
    ShifterObby(float y) : Obby(y) {};

    // ShifterObby builder
    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override; 
};

// PaddleObby class defines the pinball paddle obstacle in the game area
class PaddleObby : public Obby {

public:
    // PaddleObby constructor
    PaddleObby(float y) : Obby(y) {};

    // PaddleObby builder
    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override;
};

// ConveyerObby class defines the diagonal rows of rotating balls obstacle in the game area
class ConveyerObby : public Obby {

public:
    // ConveyerObby constructor
    ConveyerObby(float y) : Obby(y) {};

    // ConveryerObby builder
    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override;
};
