#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include <list>
#include <random>
#include <algorithm>
#include <vector>

namespace converter {
	constexpr double PIXELS_PER_METERS = 30.0;
	constexpr double PI = 3.14159;

	template<typename T> constexpr T pixelsToMeters(const T& x){return x / PIXELS_PER_METERS;};

	template<typename T> constexpr T metersToPixels(const T& x){return x * PIXELS_PER_METERS;};

	template<typename T> constexpr T degToRad(const T& x){return PI * x / 180.0;};

	template<typename T> constexpr T radToDeg(const T& x){return 180.0 * x / PI;};

}

namespace creator {
	b2BodyId createBox (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y, b2BodyType type = b2_dynamicBody) {

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
		shape->setOrigin({size_x / 2.0, size_y / 2.0});
		shape->setPosition({pos_x, pos_y});

		if (type == b2_dynamicBody) {
			shape->setFillColor(sf::Color::Red);
		} else {
			shape->setFillColor(sf::Color::White);
		}

		b2Body_SetUserData(bodyId, shape);

		return bodyId;
	}


	b2BodyId createObby (b2WorldId& world, float pos_x, float pos_y, float size_x, float size_y, b2BodyType type = b2_dynamicBody) {

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
		shape->setOrigin({size_x / 2.0, size_y / 2.0});
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

	b2BodyId createCircle (b2WorldId& world, float pos_x, float pos_y, float radius, b2BodyType type = b2_dynamicBody) {

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

		// Create SFML shape
		sf::CircleShape* shape = new sf::CircleShape(radius);
		shape->setOrigin({radius, radius});
		shape->setPosition({pos_x, pos_y});
		shape->setPointCount(100);

		if (type == b2_dynamicBody) {
			shape->setFillColor(sf::Color::Red);
		} else {
			shape->setFillColor(sf::Color::White);
		}

		b2Body_SetUserData(bodyId, shape);

		return bodyId;
	}
}

// x location you want it to start, y location you want it to start, bodt list
void plinkoObby(int y, b2WorldId &worldId, std::list<b2BodyId> &obbies) {
	// Plinko obby
	// One row of plinko
	for (int i = 0; i < 5; i++) {
		// Offset rows five times
		for (int j = 0; j < 5; j++) {

			if (j % 2 == 0) {
				obbies.emplace_back(creator::createObby(worldId, (i * 107.5), (j * 100) + y, 28, 28, b2_staticBody));
			} else {
				obbies.emplace_back(creator::createObby(worldId, (i * 107.5) + 53.75, (j * 100) + y, 28, 28, b2_staticBody));
			}
		}
	}

}

void displayWorld(b2WorldId world, std::list<b2BodyId> bodies, sf::RenderWindow& render) {
	b2World_Step(world, 1.0 / 60, 4);
	render.clear();

	for (b2BodyId b: bodies) {
		sf::Shape* shape = static_cast<sf::Shape* >(b2Body_GetUserData(b));
		shape->setPosition({converter::metersToPixels<float>(b2Body_GetPosition(b).x),converter::metersToPixels<float>(b2Body_GetPosition(b).y)});
		b2Rot q = b2Body_GetRotation(b);
		shape->setRotation(sf::radians(b2Rot_GetAngle(q)));
		render.draw(*shape);
	}

	render.display();
}

int main() {

	std::cout << "Hello, World!" << std::endl;

	// Create main window
	sf::RenderWindow window(sf::VideoMode({430, 932}), "balls");
	window.setFramerateLimit(60);

	// Load sprite
	const sf::Texture texture("assets/images/cute_guy.jpeg");
	sf::Sprite sprite(texture);

	// Create graphical text to display
	const sf::Font font("assets/fonts/PAPYRUS.ttf");
	sf::Text text(font, "Hi, everyone!", 50);

	// Box2d World
	b2WorldDef worldDef = b2DefaultWorldDef();
	b2Vec2 gravity = {0.0f, 9.81f};
	worldDef.gravity = gravity;
	b2WorldId worldId = b2CreateWorld(&worldDef);


	std::list<b2BodyId> bodies;
	// Ground box
	bodies.emplace_back(creator::createBox(worldId, 400, 942, 800, 20, b2_staticBody));
	// Left wall box
	bodies.emplace_back(creator::createBox(worldId, 0, 0, 2, 942 * 2, b2_staticBody));
	// Right wall box
	bodies.emplace_back(creator::createBox(worldId, 430, 0, 2, 942 * 2, b2_staticBody));

	// Give first obby a starting height
	int height = 300;

	//Create an array of numbers
	std::vector<int> rand_nums = {0, 1, 2, 3, 4, 5};
	std::random_device rd;
	std::mt19937 gen(rd());
	//Shuffle them
	std::shuffle(rand_nums.begin(), rand_nums.end(), gen);

	// Generate 6 obbies
	for (int i = 0; i < 6; i++) {

		// Obby is asssigned in random order
		int obby_num = rand_nums[i];

		std::cout << "obby_num = " << obby_num << std::endl;

		switch (obby_num) {
			case 0:
				//Plinko obby
				// IF 0 IS GENERATED TOWARD THE END IT IS JUST FAR DOWN... THE CODE WORKS
 				plinkoObby(height, worldId, bodies);
				break;
			case 1:
				// Another obby
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
		height += 500;

	}


	// Main loop
	while (window.isOpen()) {

		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
					int x = mouseButtonPressed->position.x;
					int y = mouseButtonPressed->position.y;
					bodies.emplace_back(creator::createCircle(worldId, x, y, 30));
				}
			}

		}

		displayWorld(worldId, bodies, window);

	}

	for (b2BodyId body : bodies) {
		delete static_cast<sf::RectangleShape*>(b2Body_GetUserData(body));
		b2DestroyBody(body);
	}

	return 0;
}

















