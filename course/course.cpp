#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include <list>
#include <random>
#include <algorithm>
#include <vector>

// Obby Number
const int OBBY_NUM = 0;

// Screen Resolution
const int WINDOW_WIDTH = 215;
const int WINDOW_HEIGHT = 466;

// Plinko Obby Size
const int PLINKO_SIZE = 12;

// Shifter Obby Size
const int SHIFTER_SIZE = 22;

// Ball Size
const int BALL_SIZE = 18;

/****************************************************************
* Name: converter						*
* Parameters: N/A						*
* Purpose: To convert pixels to meters, meters to pixels,	*
* radians to degrees, or degrees to radians			*
****************************************************************/
namespace converter {
	constexpr double PIXELS_PER_METERS = 30.0;
	constexpr double PI = 3.14159;

	template<typename T> constexpr T pixelsToMeters(const T& x){return x / PIXELS_PER_METERS;};

	template<typename T> constexpr T metersToPixels(const T& x){return x * PIXELS_PER_METERS;};

	template<typename T> constexpr T degToRad(const T& x){return PI * x / 180.0;};

	template<typename T> constexpr T radToDeg(const T& x){return 180.0 * x / PI;};

}

/********************************************************
* Name: creator						*
* Parameters: N/A					*
* Purpose: To be the holy one to create all aspects of	*
* the world						*
********************************************************/
namespace creator {


	/*********************************************************
	* Name: createBox					 *
	* Parameters: Physical world, initial x position, initial*
	* y position, box width, box height, body type		 *
	* Purpose: To create a box body and shape in the physical*
	* world.						 *
	*********************************************************/
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

	/*********************************************************
	* Name: createPlinkoObby				 *
	* Parameters: Physical world, initial x position, initial*
	* y position, plinko width, plinko height, body type*	 *
	* Purpose: To spawn in a single plinko peg.		 *
	*********************************************************/
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

	/*********************************************************
	* Name: createBall					 *
	* Parameters: Physical world, initial x position, initial*
	* y position, ball radius, body type			 *
	* Purpose: To spawn in a single ball "player".		 *
	*********************************************************/
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

	/*********************************************************
	* Name: createShifterObby				 *
	* Parameters: Physical world, initial x position, initial*
	* y position, shifter width, shifter height, body type	 *
	* Purpose: To spawn in a single shifter obstacle.	 *
	*********************************************************/
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

		//std::cout << "Body's x position: " << converter::metersToPixels(b2Body_GetPosition(bodyId).x) << std::endl;
		b2Body_SetLinearVelocity(bodyId, (b2Vec2){1.0f, 0.0f});

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

/*********************************************************
* Name: worldInit					 *
* Parameters: Physical world, list of all physical bodies*
* Purpose: To spawn in the bounds of the playable world	 *
* (ground box, and wall boxes).				 *
*********************************************************/
void worldInit(b2WorldId &world, std::list<b2BodyId> &bodies) {

	// Ground box
	bodies.push_back(creator::createBox(world, WINDOW_WIDTH - 30, 3500, 800, 20, b2_staticBody));
	// Left wall box
	bodies.push_back(creator::createBox(world, 0, 0, 2, 7000, b2_staticBody));
	// Right wall box
	bodies.push_back(creator::createBox(world, WINDOW_WIDTH, 0, 2, 7000, b2_staticBody));

}


/********************************************************
* Name: ballsInit				 	*
* Parameters: Physical world, all bodies in the world,	*
* and balls list					*
* Purpose: To spawn in all ball players.		*
*********************************************************/
void ballsInit(b2WorldId &world, std::list<b2BodyId> &bodies, std::list<b2BodyId> &balls) {


	for (int i = 0; i <= 4; i++) {
		b2BodyId ball = creator::createBall(
			world,
			WINDOW_WIDTH / 2,
			-WINDOW_HEIGHT / 2,
			BALL_SIZE,
			b2_dynamicBody);

		bodies.push_back(ball);
		balls.push_back(ball);
	}
}


/********************************************************
* Name: plinkoObby				 	*
* Parameters: Physical world, all bodies in the world,	*
* and plinkoes list					*
* Purpose: To spawn in the plinko set.			*
*********************************************************/
void plinkoObby(int y, b2WorldId& world, std::list<b2BodyId> &obbies, std::list<b2BodyId> &plinkoes) {

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

			b2BodyId plinko = creator::createPlinkoObby(
					world,
					plinkoPos,
					(j * 42) + y,
					PLINKO_SIZE,
					PLINKO_SIZE,
					b2_staticBody);

			obbies.push_back(plinko);
			plinkoes.push_back(plinko);
		}
	}

}


/********************************************************
* Name: shifterObby				 	*
* Parameters: Physical world, all bodies in the world,	*
* and shifters list					*
* Purpose: To spawn in all shifter obbies.		*
*********************************************************/
void shifterObby(int y, b2WorldId& world, std::list<b2BodyId> &obbies, std::list<b2BodyId> &shifters) {

	// Spawn 4 shifter bars
	for (int i = 0; i <= 4; i++) {

		b2BodyId shifter = creator::createShifterObby(
			world,
			i * 55,
			(i * 60) + y,
			SHIFTER_SIZE * 4,
			SHIFTER_SIZE,
			b2_kinematicBody);

		obbies.push_back(shifter);
		shifters.push_back(shifter);
	}
}


/****************************************
* Name: updateShifters			*
* Parameters: List of shifters		*
* Purpose: To change velocty of shifter *
* depending on position.		*
****************************************/
void updateShifters(std::list<b2BodyId> &shifters) {

	for (const auto& shifter: shifters) {

		float x = converter::metersToPixels(b2Body_GetPosition(shifter).x);
		//std::cout << "Body's x position: " << x << std::endl;

		if (x <= 55)
			b2Body_SetLinearVelocity(shifter, (b2Vec2){1.0f, 0.0f});
		else if (x >= 160)
			b2Body_SetLinearVelocity(shifter, (b2Vec2){-1.0f, 0.0f});
	}
}


/********************************************************
* Name: leadBall					*
* Parameters: List of balls				*
* Purpose: To store the y value of the ball in the lead.*
*********************************************************/
float leadBall(const std::list<b2BodyId> &balls) {

	float leadY = 0;

	for (const auto &ball: balls) {

		float y = converter::metersToPixels(b2Body_GetPosition(ball).y);

		if (y > leadY)
			leadY = y;

	}
	return leadY - (WINDOW_HEIGHT * 2 / 3);
}


/*********************************************************
* Name: displayWorld					 *
* Parameters: Physical world, all bodies in the world,	 *
* the application window, and balls list		 *
* Purpose: To display the application window, run physics*
* simulation over time, and attach physical position to  *
* window position.					 *
*********************************************************/
void displayWorld(b2WorldId world, std::list<b2BodyId> bodies, sf::RenderWindow& render, float leader) {
	b2World_Step(world, 1.0 / 60, 4);
	render.clear();

	for (b2BodyId b: bodies) {
		sf::Shape* shape = static_cast<sf::Shape* >(b2Body_GetUserData(b));
		shape->setPosition({converter::metersToPixels<float>(b2Body_GetPosition(b).x),converter::metersToPixels<float>(b2Body_GetPosition(b).y) - (float)leader});
		b2Rot q = b2Body_GetRotation(b);
		shape->setRotation(sf::radians(b2Rot_GetAngle(q)));
		render.draw(*shape);
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

	// Load sprite
	const sf::Texture texture("build/assets/images/cute_guy.jpeg");
	sf::Sprite sprite(texture);

	// Create graphical text to display
	const sf::Font font("build/assets/fonts/papyrus.ttf");
	sf::Text text(font, "Hi, everyone!", 50);

	// Box2d World
	b2WorldDef worldDef = b2DefaultWorldDef();
	b2Vec2 gravity = {0.0f, 9.81f};
	worldDef.gravity = gravity;
	b2WorldId worldId = b2CreateWorld(&worldDef);

	// List that stores all bodies in the world
	std::list<b2BodyId> bodies;

	// List that stores all balls in the world
	std::list<b2BodyId> balls;

	// List that stores all plinkoes in the world
	std::list<b2BodyId> plinkoes;

	// List that stores all shifters in the world
	std::list<b2BodyId> shifters;

	// Spawn the walls
	worldInit(worldId, bodies);

	// Spawn the balls
	ballsInit(worldId, bodies, balls);

	// Give first obby a starting height
	int height = 100;

	//Create an array of numbers
	std::vector<int> randNums = {0, 1, 2, 3, 4, 5};
	std::random_device rd;
	std::mt19937 gen(rd());
	//Shuffle them
	std::shuffle(randNums.begin(), randNums.end(), gen);

	// Generate 6 obbies
	for (int i = 0; i < 6; i++) {

		// Obby is asssigned in random order
		int obbyNum = randNums[i];
		//int obbyNum = OBBY_NUM;

		switch (obbyNum) {
			case 0:
				// Plinko obby
				plinkoObby(height, worldId, bodies, plinkoes);
				break;
			case 1:
				// Shifter obby
				shifterObby(height, worldId, bodies, shifters);
				break;
			case 2:
				//Another obby
				break;
			case 3:
				//Another obby
				break;
			case 4:
				//Another obby
				break;
			case 5:
				//Another obby
				break;
			default:
				std::cout << "cry" << std::endl;
		}


		// Add height for subsequent obby
		height += 450;
	}

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
					b2BodyId player = creator::createBall(worldId, x, y, BALL_SIZE, b2_dynamicBody);
					bodies.push_back(player);
					balls.push_back(player);
				}
			}

		}

		// Update shifter velocity
		updateShifters(shifters);

		// Store leader ball y position
		float leader = leadBall(balls);

		// Render the world
		displayWorld(worldId, bodies, window, leader);
	}

	// Destroy the world; considering destroying bodies off the screen but probably not needed
	for (b2BodyId body : bodies) {
		delete static_cast<sf::RectangleShape*>(b2Body_GetUserData(body));
		b2DestroyBody(body);
	}

	return 0;
}

















