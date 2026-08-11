#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "creator.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <utility>

// Screen Resolution
const int WINDOW_WIDTH = 215;
const int WINDOW_HEIGHT = 466;

// Plinko Size
const int PLINKO_SIZE = 12;

// Shifter Size
const int SHIFTER_SIZE = 22;

// Dot Size
const int DOT_SIZE = 10;

// Ball Size
const int BALL_SIZE = 18;

using namespace converter;

enum class EntityType {
    Wall,
    Ball,
    Plinko,
    Shifter,
    Dot
};

class Entity {

protected:
	b2BodyId body;
    EntityType type;
    std::unique_ptr<sf::Shape> shape;
    bool destroyed = false;

public:
	virtual ~Entity() = default;

	virtual void update() {}

    virtual float getBallY() { return 0.0f; }

	virtual void draw(sf::RenderWindow& window, float cameraY) {
        shape->setPosition({
			converter::metersToPixels<float>(b2Body_GetPosition(body).x),
			converter::metersToPixels<float>(b2Body_GetPosition(body).y) - (float)cameraY
		});

		b2Rot rot = b2Body_GetRotation(body);
		shape->setRotation(sf::radians(b2Rot_GetAngle(rot)));
		window.draw(*shape);
	}

    void setBody(b2BodyId newBody) {

        body = newBody;
        b2Body_SetUserData(body, this);
    }

	b2BodyId getBody() const {

		return body;
	}

    EntityType getType() const {
        
        return type;
    }

    bool isDestroyed() const { return destroyed; }

    void destroy() { destroyed = true; }

    virtual void collision(Entity* a, Entity* b) {} 
};

class Wall : public Entity {

public:

    Wall(b2WorldId world, float posX, float posY, float sizeX, float sizeY) {
  
        // Create SFML shape
        shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(sizeX, sizeY));
        shape->setOrigin({sizeX / 2.0f, sizeY / 2.0f});
        shape->setPosition({posY, posY});
        shape->setFillColor(sf::Color::White);

        type = EntityType::Wall;

        setBody(createBox(
            world,
            posX,
            posY,
            sizeX,
            sizeY
        ));
   }
};

class Ball : public Entity {

public:

	Ball(b2WorldId world, float posX, float posY) {

        // Create SFML shape
        auto circle = std::make_unique<sf::CircleShape>(BALL_SIZE);
        circle->setOrigin({BALL_SIZE, BALL_SIZE});
        circle->setPointCount(100);
        circle->setFillColor(sf::Color::Red);

        // Transfer ownership of circle pointer to shape
        shape = std::move(circle);

        type = EntityType::Ball;

		setBody(createBall(
			world,
			posX,
			posY,
			BALL_SIZE
		));
	}

    float getBallY() {
        
        return converter::metersToPixels(b2Body_GetPosition(body).y);
    }
};

class Dot : public Entity {

public:

	Dot(b2WorldId world, float posX, float posY) {

        // Create SFML shape
        auto circle = std::make_unique<sf::CircleShape>(DOT_SIZE);
        circle->setOrigin({DOT_SIZE, DOT_SIZE});
        circle->setPosition({posX, posX});
        circle->setPointCount(100);
        circle->setFillColor(sf::Color::White);

        // Transfer ownership of circle pointer to shape
        shape = std::move(circle);

        type = EntityType::Dot;

		setBody(createDot(
			world,
			posX,
			posY,
			DOT_SIZE
        ));
	}

    void collision(Entity* a, Entity* b) override {

        if (a->getType() == EntityType::Ball && b->getType() == EntityType::Dot)
            b->destroy();
        else if (a->getType() == EntityType::Dot && b->getType() == EntityType::Ball)
            a->destroy();            
    }

};

class Shifter : public Entity {

public:

	Shifter(b2WorldId world, float posX, float posY) {

        // Create SFML shape
        shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(SHIFTER_SIZE * 5, SHIFTER_SIZE));
        shape->setOrigin({SHIFTER_SIZE * 5 / 2.0f, SHIFTER_SIZE / 2.0f});
        shape->setPosition({posX, posY});

        shape->setFillColor(sf::Color::White);

        type = EntityType::Shifter;

		setBody(createShifter(
			world,
			posX,
			posY,
			SHIFTER_SIZE * 4,
			SHIFTER_SIZE
        ));
	}

	void update() override {

		float x = converter::metersToPixels(
			b2Body_GetPosition(body).x
		);

        if (x <= 55)
			b2Body_SetLinearVelocity(body, -b2Body_GetLinearVelocity(body));
		else if (x >= 160)
			b2Body_SetLinearVelocity(body, -b2Body_GetLinearVelocity(body));
    }
};

class Obby {

protected:
	float startY;

    std::vector<std::unique_ptr<Entity>> entities;

public:
    Obby(float y) : startY(y) {}

    virtual ~Obby() = default;

    virtual void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) = 0;

	virtual void update() {
        
        for (auto &entity : entities)
            entity->update();
    }

	virtual void draw(sf::RenderWindow& window, float cameraY) {
        
        for (auto &entity : entities)
            entity->draw(window, cameraY);
    }
};

class Boundary : public Obby {

public:
    Boundary(float y) : Obby(y) {}

    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override {
        // Ground Box
        entities.push_back(
            std::make_unique<Wall>(world, WINDOW_WIDTH - 30, 3500, 800, 20)
        );

        // Left Wall Box
        entities.push_back(
            std::make_unique<Wall>(world, 0, 0, 2, 7000) 
        );

        // Right Wall Box
        entities.push_back(
            std::make_unique<Wall>(world, WINDOW_WIDTH, 0, 2, 7000)
        );
    }
};

class DotObby : public Obby {

public:
    DotObby(float y) : Obby(y) {}

    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override {
   
        // Spawn 10 shifter bars
        for (int row = 0; row < 10; row++) {
            
            for (int col = 0; col < 10; col++) {
                
                entities.push_back(
                    std::make_unique<Dot>(
                    world,
                    row * (WINDOW_WIDTH / 10) + 13,
                    col * (WINDOW_WIDTH / 10) + startY
                    )
                );
            }
        }
    }
};

class ShifterObby : public Obby {

public:
    ShifterObby(float y) : Obby(y) {}

    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override {
   
        // Spawn 10 shifter bars
        for (int i = 0; i < 10; i++) {

            entities.push_back(
                std::make_unique<Shifter>(
                world,
                WINDOW_WIDTH / 2,
                (i * 70) + startY
                )
            );
        }
    }
};

void ballsInit(b2WorldId &world, std::vector<std::unique_ptr<Entity>> &entities) {

	for (int i = 0; i <= 4; i++) {
       
       entities.push_back(
            std::make_unique<Ball>(world, WINDOW_WIDTH / 2, 20)
        );
    }
}

float leadBall(const std::vector<std::unique_ptr<Entity>>& entities) {

	float leadY = WINDOW_HEIGHT * 1 / 3;

	for (const auto &entity: entities) {

        float y = entity->getBallY();

		if (y > leadY)
			leadY = y;
	}

	return leadY - (WINDOW_HEIGHT * 2 / 3);
}

void shapesContact(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) {

    b2ContactEvents contactEvents = b2World_GetContactEvents(world);

    for (int i = 0; i < contactEvents.beginCount; ++i) {

        b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;

        if (!b2Shape_IsValid(beginEvent->shapeIdA) ||
            !b2Shape_IsValid(beginEvent->shapeIdB))
            continue;
        
        b2BodyId bodyA = b2Shape_GetBody(beginEvent->shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(beginEvent->shapeIdB);

        Entity *entityA = static_cast<Entity*>(b2Body_GetUserData(bodyA)); 
        Entity *entityB = static_cast<Entity*>(b2Body_GetUserData(bodyB));

        entityA->collision(entityA, entityB);
        entityB->collision(entityA, entityB);        
    }

   for (auto it = entities.begin(); it != entities.end(); ) {

        if ((*it)->isDestroyed()) {

            b2DestroyBody((*it)->getBody());
            it = entities.erase(it);
        }
        else {

            ++it;
        }
    }
}

void displayWorld(b2WorldId &world, std::vector<std::unique_ptr<Entity>>& entities, sf::RenderWindow& render, float cameraY) {
    render.clear();

    for (auto& entity : entities) {
       entity->update();
    }

	b2World_Step(world, 1.0 / 60, 4);

    shapesContact(world, entities);
    
    for (auto& entity : entities) {

        entity->draw(render, cameraY);
    }

	render.display();
}

int main() {

	std::cout << "Hello, World!" << std::endl;

	// Create main window
	sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "balls");
	window.setFramerateLimit(60);

	// Set to desktop mode
	auto desktop = sf::VideoMode::getDesktopMode();

	// Get screen dimensions
	int screenWidth = desktop.size.x;
	int screenHeight = desktop.size.y;
	int windowWidth = window.getSize().x;
	int windowHeight = window.getSize().y;

	// Setting window position relative to screen size
	sf::Vector2i window_pos = {
		(screenWidth / 2) - (windowWidth / 2),
		(screenHeight / 2) - (windowHeight / 2)
	};
	window.setPosition(window_pos);

	// Box2d World
	b2WorldDef worldDef = b2DefaultWorldDef();
	b2Vec2 gravity = {0.0f, 9.81f};
	worldDef.gravity = gravity;
	b2WorldId worldId = b2CreateWorld(&worldDef);

	// List that holds all bodies in the world
	std::vector<std::unique_ptr<Entity>> bodies;

    // Creating Bounding Boxes
    Boundary boundary(0);
    boundary.build(worldId, bodies);
 	// Creating balls
	ballsInit(worldId, bodies);

    // Creating dot obby
    DotObby dotObby(200);
    dotObby.build(worldId, bodies);

    // Creating shifter obby
    ShifterObby shifterObby(500);
    shifterObby.build(worldId, bodies);

    // Main loop
	while (window.isOpen()) {

		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->code == sf::Keyboard::Key::Escape) {
					window.close();
				}
			}	
		}

		// Shift window view to track leader ball
		float leader = leadBall(bodies);

        // Continuously update the physical world frame by frame
		displayWorld(worldId, bodies, window, leader);
	}

	return 0;
}
