#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "entity.h"
#include "obby.h"
#include <iostream>
#include <vector>
#include <string>
#include <tuple>

// Screen Resolution
const int WINDOW_WIDTH = 215;
const int WINDOW_HEIGHT = 466;

using namespace converter;

// Store y level of a "Player", name of "Player", and time "Player" took the lead
std::tuple<float, std::string, float> leadBall(const std::vector<std::unique_ptr<Entity>>& entities, sf::Clock clock, std:: string&prevLead) {

    // Set initial y level
    float leadY = WINDOW_HEIGHT / 3.0f;

    // Current leader name
    std::string curLead = "";

    // Time current leader took lead
    float leadTime = 0.0f;

    for (const auto &entity: entities) {

        float y = entity->getBallY();

		if (y > leadY) {
            // Set highesy y level to current leader y level
			leadY = y;
            // Get the entity's name
            curLead = entity->getName();
        }
	}

    // Store current leader name and time they took the lead
    if (curLead != prevLead && leadY < 2500.0f ) {

        prevLead = curLead;
        leadTime = clock.getElapsedTime().asSeconds();

        std::cout << curLead << ", " << leadTime << std::endl;
    }

    leadY -= WINDOW_HEIGHT * 2.0f / 3.0f;

    // Set camera y level to bottom of game
    if (leadY >= 3000.0f - WINDOW_HEIGHT) {

        leadY = 3000.0f - WINDOW_HEIGHT;
        
    }

    return {leadY, curLead, leadTime};
}

// Handle contact between two entities
void shapesContact(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) {

    // Start storing contact events
    b2ContactEvents contactEvents = b2World_GetContactEvents(world);

    for (int i = 0; i < contactEvents.beginCount; ++i) {

        // Increment begin contact event on contact
        b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;

        // Check if both shapes are not destroyed
        if (!b2Shape_IsValid(beginEvent->shapeIdA) ||
            !b2Shape_IsValid(beginEvent->shapeIdB))
            continue;
        
        // Get body of both contact shapes
        b2BodyId bodyA = b2Shape_GetBody(beginEvent->shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(beginEvent->shapeIdB);

        // Get entity of both contact shapes
        Entity *entityA = static_cast<Entity*>(b2Body_GetUserData(bodyA)); 
        Entity *entityB = static_cast<Entity*>(b2Body_GetUserData(bodyB));

        // Handle collision between entities
        entityA->collision(entityA, entityB);
        entityB->collision(entityA, entityB);        
    }

   for (auto it = entities.begin(); it != entities.end(); ) {

        // If entity destoyed, remove it from the entities in the world
        if ((*it)->isDestroyed()) {

            b2DestroyBody((*it)->getBody());
            it = entities.erase(it);
        }
        else {

            ++it;
        }
    }
}

// Build obstacle course
void buildCourse(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) {

    // Create shuffled array where number corresponds to obstacle
	std::vector<int> randNums = {0, 1, 2, 3, 4};
	std::random_device rd;
	std::mt19937 gen(rd());
	//Shuffle them
	std::shuffle(randNums.begin(), randNums.end(), gen);

    // Set initial height of first obstacle
    int height = 200;

    for (int i = 0; i < randNums.size(); i++) {

        switch (randNums[i]) {
            case 0: {
                //Create Plinko obby
                PlinkoObby plinkoObby(height);
                plinkoObby.build(world, entities);
            break;
            }
            case 1: {
                // Create shifter obby
                ShifterObby shifterObby(height);
                shifterObby.build(world, entities);
                break;
            }
            case 2: {
                // Create dot obby
                DotObby dotObby(height);
                dotObby.build(world, entities);
                break;
            }
            case 3: {
                // Create paddle obby
                PaddleObby paddleObby(height);
                paddleObby.build(world, entities);
                break;
            }
            case 4: {
                // Create Conveyer obby
                ConveyerObby conveyerObby(height);
                conveyerObby.build(world, entities);
                break;
            }
            default:{
                break;
            }

        }
        // Increment obstacle start height between obstacles
        height += 500;
    }
}

// Display "winner" of the game's picture and name
void winnerDisplay(sf::RenderWindow &render, float leadY, const std::string &currentLeader, bool &first, std::string &winnerName) {

    // If ball has reached y level 2500 and no one else has won
    if (leadY >= 2500.0f && !first) {

        // "Player" has won and store their name
        first = true;
        winnerName = currentLeader;
    }

    // Return if "Player" has not won yet
    if (!first)
        return;

        // Init papyrus font
        sf::Font papyrus;
        // Find papyrus font file
        if (!papyrus.openFromFile("build/assets/fonts/papyrus.ttf"))
            std::cerr << "Error loading file!\n";
        // Define "Player" name text and "wins!" text
        sf::Text text(papyrus);
        sf::Text textWin(papyrus);

        // Define "Player" image
        sf::Texture texture;
        // Scale "Player" image to certain size
        sf::Vector2f picNewSize({180.0f, 180.0f});

        // Find "Player" picture
        std::string picPath = "./build/assets/images/" + winnerName.substr(0,1) + ".jpg";
        if (!texture.loadFromFile(picPath))
            std::cerr << "Image not found!\n";

        // Set winner name and wins text string
        text.setString(winnerName);
        textWin.setString("wins!");

        // Set character size for winner name and wins text string
        text.setCharacterSize(36);
        textWin.setCharacterSize(36);

        // Set color of winner name and wins text string
        text.setFillColor(sf::Color::White);
        textWin.setFillColor(sf::Color::White);
        
        // Create box of winner name text
        sf::FloatRect textBox = text.getLocalBounds();
        text.setOrigin({
            textBox.position.x + textBox.size.x / 2.0f,
            textBox.position.y + textBox.size.y / 2.0f
        });

        // Create box of wins text
        sf::FloatRect textBoxWin = textWin.getLocalBounds();
        textWin.setOrigin({
            textBoxWin.position.x + textBoxWin.size.x / 2.0f,
            textBoxWin.position.y + textBoxWin.size.y / 2.0f
        });

        // Center align winner name text
        text.setPosition({
            (WINDOW_WIDTH / 2.0f),
            (WINDOW_HEIGHT / 2.0f) + 110.0f
        });
        // Center align wins text
        textWin.setPosition({
            (WINDOW_WIDTH / 2.0f),
            (WINDOW_HEIGHT / 2.0f) + 140.0f
        });
 
        // Get size of original winner image
        sf::Vector2f picOrigSize = (sf::Vector2f)texture.getSize();
        
        // Define sprite
        sf::Sprite image(texture);
        // Set sprite position
        image.setPosition({(WINDOW_WIDTH / 2.0f) - 90.0f, (WINDOW_HEIGHT / 2.0f) - 100.0f});        
        image.setScale({
            picNewSize.x / picOrigSize.x,
            picNewSize.y / picOrigSize.y
        });

        // Draw winner image, winner name, and wins text to screen
        render.draw(image);
        render.draw(text);
        render.draw(textWin);
}

int main() {

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

    // Create game clock
    sf::Clock clock;
    // Time in seconds since start of game
    float dt = clock.restart().asSeconds();
	
    // Box2d World
	b2WorldDef worldDef = b2DefaultWorldDef();
	b2Vec2 gravity = {0.0f, 9.81f};
	worldDef.gravity = gravity;
	b2WorldId worldId = b2CreateWorld(&worldDef);

	// List that holds all entities in the world
	std::vector<std::unique_ptr<Entity>> bodies;

    // Save previous leader name
    std::string prevLead = "";

    // Has someone reached the bottom yet?
    bool first = false;
    // Name of winner
    std::string winnerName = "";

    // Create Bounding Boxes
    Boundary boundary(0);
    boundary.build(worldId, bodies);

    // Create balls
    Balls balls(20);
    balls.build(worldId, bodies);

    // Build all obstacle courses
    buildCourse(worldId, bodies);
	 
    // Main loop
	while (window.isOpen()) {

		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
            // If escape pressed, exit window
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->code == sf::Keyboard::Key::Escape) {
					window.close();
				}
			}	
		}

        // Clear window each time step
        window.clear();
        
        // Handle contact events
        shapesContact(worldId, bodies);

		// Shift window view to track leader ball
		auto [leadY, player, leadTime] = leadBall(bodies, clock, prevLead);

        // Display winner picture and name
        winnerDisplay(window, leadY, player, first, winnerName);

        // Update entity world position, logic, and position on window
        for (auto& body : bodies) {
        
            body->update(clock);
        
            body->draw(window, leadY);
        }

        // Increment world time step
        b2World_Step(worldId, 1.0f / 60.0f, 4);

        // Display all entities to window
        window.display();
    }

	return 0;
}
