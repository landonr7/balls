#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "entity.h"
#include "obby.h"
#include <vector>

// Screen Resolution
const int WINDOW_WIDTH = 215;
const int WINDOW_HEIGHT = 466;

// Update each entity's postion in  the world
void Obby::update(sf::Clock clock) {
    
    for (auto &entity : entities)
        entity->update(clock);
}

// Draw each entity on the window
void Obby::draw(sf::RenderWindow& window, float cameraY) {
    
    for (auto &entity : entities)
        entity->draw(window, cameraY);
}

// Build the boundaries obby in the world
void Boundary::build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) {
    // Left Wall Box
    entities.push_back(
        std::make_unique<Wall>(world, 0, 0, 2, 6000) 
    );

    // Right Wall Box
    entities.push_back(
        std::make_unique<Wall>(world, WINDOW_WIDTH, 0, 2, 6000)
    );
}

// Spawn the "Players" in the world
void Balls::build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) {

    // Spawn 5 balls
    for (int i = 0; i < 5; i++) {
       
       entities.push_back(
            std::make_unique<Ball>(
                world, 
                WINDOW_WIDTH / 2,
                startY,
                i
            )
        );
    }
}

// Build the dot pit obby in the world
void DotObby::build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) {
 
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
        }
    }
}

// Build the plinko obby in the world
void PlinkoObby::build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) {
    
    float plinkoPos = 0;

    // Three columns of plinko
    for (int i = 0; i < 3; i++) {

        // Eight rows of plinko
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
        }
    }  
} 

// Build the shifter obby in the world
void ShifterObby::build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) {

    // Five shifter bars
    for (int i = 0; i < 5; i++) {

        entities.push_back(
            std::make_unique<Shifter>(
            world,
            WINDOW_WIDTH / 2,
            (i * 70) + startY
            )
        );
    }
}

// Build the paddle obby in the world
void PaddleObby::build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) {

    // Three rows of paddles
    for (int i = 0; i < 3; i++) {

        int height = (i * 150) + startY;
        
        // One on the right side of the screen
        entities.push_back(
            std::make_unique<Paddle>(
                world,
                25 + WINDOW_WIDTH * 2 / 3,
                height,
                0
            )
        );

        // One on the left side of the screen
        entities.push_back(
            std::make_unique<Paddle>(
                world,
                (WINDOW_WIDTH / 3) - 25,
                height,
                1
            )
        );
    }
}

// Build the conveyer obby in the world
void ConveyerObby::build(b2WorldId world, std::vector<std::unique_ptr<Entity>> &entities) {

    // Five rows of conveyer belt
    for (int row = 0; row < 5; row++) {

        bool direction = row & 1;
        float side = 0.0f;

        // Five conveyers per belt
        for (int col = 0; col < 5; col++) { 

            if (direction) side = col * 30;
            else side = WINDOW_WIDTH - (col * 30);

            // Alternate rotating direction based on side of screen
            entities.push_back(
                std::make_unique<Conveyer>(
                world,
                side,
                col * 10 + (row * 80) + startY,
                direction
                )
            );
        }
    }
}
