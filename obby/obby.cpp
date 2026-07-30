#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include <list>
#include <random>
#include <algorithm>
#include <vector>
#include <cmath>
#include <string>
#include <utility>

// Screen Resolution
const int WINDOW_WIDTH = 215;
const int WINDOW_HEIGHT = 466;

// Plinko Obby Size
const int PLINKO_SIZE = 12;

// Shifter obby size
const int SHIFTER_SIZE = 22;

// Ball Size
const int BALL_SIZE = 18;

namespace converter {
	constexpr double PIXELS_PER_METERS = 30.0;
	constexpr double PI = 3.14159;

	template<typename T> constexpr T pixelsToMeters(const T& x){return x / PIXELS_PER_METERS;};

	template<typename T> constexpr T metersToPixels(const T& x){return x * PIXELS_PER_METERS;};

	template<typename T> constexpr T degToRad(const T& x){return PI * x / 180.0;};

	template<typename T> constexpr T radToDeg(const T& x){return 180.0 * x / PI;};

}

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
	b2BodyId createShifterObby (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y, b2BodyType type) {

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


	b2BodyId createPlinkoObby (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y, b2BodyType type) {

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



class Entity {

protected:
	b2BodyId body;

public:
	virtual ~Entity() {}

	virtual void update() {}

    virtual float getBallY() { return 0.0f; }

	virtual void draw(sf::RenderWindow& window, float cameraY) {

		sf::Shape* shape = static_cast<sf::Shape* >(b2Body_GetUserData(body));

		shape->setPosition({
			converter::metersToPixels<float>(b2Body_GetPosition(body).x),
			converter::metersToPixels<float>(b2Body_GetPosition(body).y) - (float)cameraY
		});

		b2Rot rot = b2Body_GetRotation(body);
		shape->setRotation(sf::radians(b2Rot_GetAngle(rot)));
		window.draw(*shape);
	}

	b2BodyId getBody() const {

		return body;
	}
};

class Wall : public Entity {

public:
    Wall(b2WorldId world, float posX, float posY, float sizeX, float sizeY) {
   
        body = creator::createBox(
            world,
            posX,
            posY,
            sizeX,
            sizeY,
            b2_staticBody
        );
   }
};

class Ball : public Entity {

public:
	Ball(b2WorldId world, float x, float y) {

		body = creator::createBall(
			world,
			x,
			y,
			BALL_SIZE,
			b2_dynamicBody
		);
	}

    float getBallY() {
        
        //float y = converter::metersToPixels(b2Body_GetPosition(ball).y);

        //float x = converter::metersToPixels(
		//	b2Body_GetPosition(body).x
		//);

        return converter::metersToPixels(b2Body_GetPosition(body).y);
    }
};

class Plinko : public Entity {

public:
	Plinko(b2WorldId world, float x, float y) {

		body = creator::createPlinkoObby(
			world,
			x,
			y,
			PLINKO_SIZE,
			PLINKO_SIZE,
			b2_staticBody);	
	}
};

class Shifter : public Entity {

public:
	Shifter(b2WorldId world, float x, float y) {

		body = creator::createShifterObby(
			world,
			x,
			y,
			SHIFTER_SIZE * 4,
			SHIFTER_SIZE,
			b2_kinematicBody);
	}

	void update() override {

		float x = converter::metersToPixels(
			b2Body_GetPosition(body).x
		);

		if (x <= 55)
			b2Body_SetLinearVelocity(body, (b2Vec2){1.0f, 0.0f});
		else if (x >= 160)
			b2Body_SetLinearVelocity(body, (b2Vec2){-1.0f, 0.0f});
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

class PlinkoObby : public Obby {

public:
	PlinkoObby(float y) : Obby(y) {}

    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override {
        
        float plinkoPos = 0;

		// One row of plinko
		for (int i = 0; i < 3; i++) {
			// One column of plinko
			for (int j = 0; j < 7; j++) {

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
			}
		}  
    } 
};

class ShifterObby : Obby {

public:
    ShifterObby(float y) : Obby(y) {}

    void build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) override {
   
        // Spawn 4 shifter bars
        for (int i = 0; i < 5; i++) {

            entities.push_back(
                std::make_unique<Shifter>(
                world,
                i * 55,
                (i * 60) + startY
                )
            );
        }
    }
};




/*
void shifterObby(int y, b2WorldId &worldId, std::list<b2BodyId> &obbies, std::list<b2BodyId> &shifters) {

	for (int i = 0; i <= 4; i++) {

		b2BodyId shifter = creator::createShifterObby(
			worldId,
			i * 55,
			(i * 60) + y,
			SHIFTER_SIZE * 4,
			SHIFTER_SIZE,
			b2_kinematicBody);

		obbies.push_back(shifter);
		shifters.push_back(shifter);
	}
}
*/

void ballsInit(b2WorldId &world, std::vector<std::unique_ptr<Entity>> &entities) {

	for (int i = 0; i <= 4; i++) {
        /*
		b2BodyId ball = creator::createBall(
			world,
			WINDOW_WIDTH / 2,
			20,
			BALL_SIZE,
			b2_dynamicBody);
        
		bodies.push_back(ball);
		balls.push_back(ball);
	    */
        entities.push_back(
            std::make_unique<Ball>(world, WINDOW_WIDTH / 2, 20)
        );
    }
}

/*
void boundingBox(b2WorldId &world, std::vector<std::unique_ptr<Entity>> &entities) {

    // Ground Box
    entities.push_back(
        std::make_unique<Wall>(world, WINDOW_WIDTH - 30, 100000, 800, 20)
    );

    // Left Wall Box
    entities.push_back(
        std::make_unique<Wall>(world, 0, 0, 2, 200000) 
    );

    // Right Wall Box
    entities.push_back(
        std::make_unique<Wall>(world, WINDOW_WIDTH, 0, 2, 200000)
    );
}
*/

float leadBall(const std::vector<std::unique_ptr<Entity>>& entities) {

	float leadY = WINDOW_HEIGHT * 1 / 3;

	for (const auto &entity: entities) {

        float y = entity->getBallY();

		if (y > leadY)
			leadY = y;
	}

	return leadY - (WINDOW_HEIGHT * 2 / 3);
}

/*
void updateShifters(const std::list<b2BodyId> &shifters) {

	for (const auto& shifter: shifters) {

		float x = converter::metersToPixels(b2Body_GetPosition(shifter).x);
		//std::cout << "Body's x position: " << x << std::endl;

		if (x <= 55)
			b2Body_SetLinearVelocity(shifter, (b2Vec2){3.0f, 0.0f});
		else if (x >= 160)
			b2Body_SetLinearVelocity(shifter, (b2Vec2){-3.0f, 0.0f});
	}
}
*/

void displayWorld(b2WorldId world, std::vector<std::unique_ptr<Entity>>& entities, sf::RenderWindow& render, float cameraY) {
    render.clear();

    for (auto& entity : entities) {
       entity->update();
    }

	b2World_Step(world, 1.0 / 60, 4);
	
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

	// List that holds all balls in the world
	//std::vector<make_unique<Ball>> balls;

	// List that holds all shifters in the world
	//std::list<b2BodyId> shifters;

    // Creating Bounding Boxes
    Boundary boundary(0);
    boundary.build(worldId, bodies);
    
 	// Creating balls
	ballsInit(worldId, bodies);

	// Creating plinko obby
	PlinkoObby plinkoObby(500);
    plinkoObby.build(worldId, bodies);
	
    // Creating shifter obby
    ShifterObby shifterObby(1000);
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
			else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
					int x = mouseButtonPressed->position.x;
					int y = mouseButtonPressed->position.y - (WINDOW_HEIGHT * 2 / 3);
					//b2BodyId player = creator::createBall(worldId, x, y, BALL_SIZE, b2_dynamicBody);
					//bodies.push_back(player);
					//balls.push_back(player);
				}
			}

		}

		// Update the movement of the shifter obbies
		//updateShifters(shifters);

		// Shift window view to track leader ball
		float leader = leadBall(bodies);

		// Continuously update the physical world frame by frame
		displayWorld(worldId, bodies, window, leader);
	}

    /*
	for (b2BodyId body : bodies) {
		delete static_cast<sf::RectangleShape*>(b2Body_GetUserData(body));
		b2DestroyBody(body);
	}
    */
	return 0;
}


