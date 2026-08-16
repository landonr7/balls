#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "creator.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
#include <string>
#include <algorithm>
#include <utility>

// Screen Resolution
const int WINDOW_WIDTH = 215;
const int WINDOW_HEIGHT = 466;

// Plinko Size
const int PLINKO_SIZE = 12;

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

using namespace converter;

class Entity {

protected:
	b2BodyId body;
    b2BodyDef bodyDef;
    const char *name;
    std::unique_ptr<sf::Shape> shape;
    bool destroyed = false;

public:
	virtual ~Entity() = default;

	virtual void update(float dt) {}

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

	b2BodyId getBody() const { return body; }

    b2BodyDef getBodyDef() const { return bodyDef; }

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
        circle->setFillColor(sf::Color::Blue);

        // Transfer ownership of circle pointer to shape
        shape = std::move(circle);

		setBody(createDot(
			world,
			posX,
			posY,
			DOT_SIZE
        ));
	}

    void collision(Entity* a, Entity* b) override {

        if (strcmp(b2Body_GetName(a->getBody()), "Player") == 0 && strcmp(b2Body_GetName(b->getBody()), "Dot") == 0)
            b->destroy();
        else if (strcmp(b2Body_GetName(a->getBody()), "Dot") == 0 && strcmp(b2Body_GetName(b->getBody()), "Player") == 0)
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
        shape->setFillColor(sf::Color::Blue);

		setBody(createShifter(
			world,
			posX,
			posY,
			SHIFTER_SIZE * 5,
			SHIFTER_SIZE
        ));
	}

	void update(float dt) override {

		float x = converter::metersToPixels(
			b2Body_GetPosition(body).x
		);

        if (x <= 55)
			b2Body_SetLinearVelocity(body, -b2Body_GetLinearVelocity(body));
		else if (x >= 160)
			b2Body_SetLinearVelocity(body, -b2Body_GetLinearVelocity(body));
    }
};

class Plinko : public Entity {

public:
	Plinko(b2WorldId world, float posX, float posY) {

        shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(PLINKO_SIZE, PLINKO_SIZE));

        shape->setOrigin({PLINKO_SIZE / 2.0f, PLINKO_SIZE / 2.0f});
        shape->setPosition({posY, posY});
        shape->setFillColor(sf::Color::Blue);

		setBody(createPlinko(
			world,
			posX,
			posY,
			PLINKO_SIZE,
			PLINKO_SIZE
        ));	
	}
};

class Paddle : public Entity {

private:
    b2JointId joint;

public:

	Paddle(b2WorldId world, float posX, float posY, bool direction) {

        // Create SFML shape
        shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(PADDLE_SIZE * 5, PADDLE_SIZE));
        shape->setOrigin({PADDLE_SIZE * 5 / 2.0f, PADDLE_SIZE / 2.0f});
        shape->setPosition({posX, posY});

        shape->setFillColor(sf::Color::Blue);
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
	
    void update(float dt) override {
    
        float angle = converter::radToDeg<float>(b2RevoluteJoint_GetAngle(joint));

        float motorSpeed = b2RevoluteJoint_GetMotorSpeed(joint);

        if (angle <= -50.0f || angle >= 20.0f)
            b2RevoluteJoint_SetMotorSpeed(joint, -motorSpeed);
    }
};

class Conveyer : public Entity {

private:
    float timer = 0.0f;

public:

	Conveyer(b2WorldId world, float posX, float posY, bool direction) {

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

    void update(float dt) override {
    
        timer += dt;

        if (timer >= 300.0f) {

            b2Body_SetAngularVelocity(body, -b2Body_GetAngularVelocity(body));
            timer = 0.0f;
        }
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

	virtual void update(float dt) {
        
        for (auto &entity : entities)
            entity->update(dt);
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
     
        float bottom = 0.0f;
        
        // Spawn 10 dot rows
        for (int row = 0; row < 10; row++) {
           
            // Spawn 10 dot columns
            for (int col = 0; col < 15; col++) {
                
                entities.push_back(
                    std::make_unique<Dot>(
                    world,
                    row * (WINDOW_WIDTH / 10) + 13,
                    col * (WINDOW_WIDTH / 10) + startY
                    )
                );
                bottom = std::max(col * (WINDOW_WIDTH / 10) + startY, bottom);
            }
        }
    
        std::cout << "dot length: " << bottom - startY << std::endl;
    }
};

class PlinkoObby : public Obby {

public:
	PlinkoObby(float y) : Obby(y) {}

    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override {
        
        float plinkoPos = 0;

        float bottom = 0.0f;

        // One column of plinko
		for (int i = 0; i < 3; i++) {

			// One row of plinko
			for (int j = 0; j < 8; j++) {

				// Offset alternate rows
				if (j % 2 == 0)
					plinkoPos = i * (WINDOW_WIDTH / 2);
				else
					plinkoPos  = i * (WINDOW_WIDTH / 2) + (WINDOW_WIDTH / 4);

				entities.push_back(
					std::make_unique<Plinko>(
						world,
						plinkoPos,
						(j * 42) + startY
					)
				);
                bottom = std::max((j * 42) + startY, bottom);
            }
		}  
        std::cout << "plinko length: " << bottom - startY << std::endl;
    } 
};

class ShifterObby : public Obby {

public:
    ShifterObby(float y) : Obby(y) {}

    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override {
  
        float bottom = 0.0f;

        // Spawn 5 shifter bars
        for (int i = 0; i < 5; i++) {

            entities.push_back(
                std::make_unique<Shifter>(
                world,
                WINDOW_WIDTH / 2,
                (i * 70) + startY
                )
            );
            bottom = std::max((i * 70) + startY, bottom);
        }
        std::cout << "shifter length: " << bottom - startY << std::endl;
    }
};

class PaddleObby : public Obby {

public:
    PaddleObby(float y) : Obby(y) {}

    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override {

        int bottom = 0;

        for (int i = 0; i < 3; i++) {

            int height = (i * 150) + startY;
             
            entities.push_back(
                std::make_unique<Paddle>(
                    world,
                    25 + WINDOW_WIDTH * 2 / 3,
                    height,
                    0
                )
            );

            entities.push_back(
                std::make_unique<Paddle>(
                    world,
                    (WINDOW_WIDTH / 3) - 25,
                    height,
                    1
                )
            );
            bottom = std::max(height, bottom);
        }
        std::cout << "paddle length: " << bottom - startY << std::endl;
    }
};

class ConveyerObby : public Obby {

public:
    ConveyerObby(float y) : Obby(y) {}

    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override {

        float bottom = 0.0f;

        for (int row = 0; row < 4; row++) {

            bool direction = row & 1;
            float side = 0.0f;

            for (int col = 0; col < 5; col++) { 

                if (direction) side = col * 30;
                else side = WINDOW_WIDTH - (col * 30);

                entities.push_back(
                    std::make_unique<Conveyer>(
                    world,
                    side,
                    col * 10 + (row * 100) + startY,
                    direction
                    )
                );
                bottom = std::max(col * 10 + (row * 100) + startY, bottom);
            }
        }
        std::cout << "conveyer length: " << bottom - startY << std::endl;
    }
};

void ballsInit(b2WorldId &world, std::vector<std::unique_ptr<Entity>> &entities) {

	for (int i = 0; i <= 4; i++) {
       
       entities.push_back(
            std::make_unique<Ball>(
                world, 
                WINDOW_WIDTH / 2,
                20
            )
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

void displayWorld(b2WorldId &world, sf::Clock clock, std::vector<std::unique_ptr<Entity>>& entities, sf::RenderWindow& render, float cameraY) {
    render.clear();

    float dt = clock.restart().asSeconds();

    for (auto& entity : entities) {
    
        entity->update(dt);
    }

	b2World_Step(world, 1.0f / 60, 4);

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

    // Creating game clock
    sf::Clock clock;

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

    // Creating Conveyer obby
    ConveyerObby conveyerObby(300);
    conveyerObby.build(worldId, bodies);

    // Creating paddle obby
    PaddleObby paddleObby(800);
    paddleObby.build(worldId, bodies);

    // Creating dot obby
    DotObby dotObby(1300);
    dotObby.build(worldId, bodies);

    // Creating shifter obby
    ShifterObby shifterObby(1800);
    shifterObby.build(worldId, bodies);

    // Creating plinko obby
	PlinkoObby plinkoObby(2500);
    plinkoObby.build(worldId, bodies);
	 
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
		displayWorld(worldId, clock, bodies, window, leader);
	}

	return 0;
}
