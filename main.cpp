#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

// Structure to represent each falling red cube 🟥
struct RedCube {
    float x, y;
    float size;
    float speed;
};

// Simple AABB collision check 💥
bool checkCollision(float x1, float y1, float w1, float h1, 
                    float x2, float y2, float w2, float h2) {
    return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}

// Helper to reset the game state 🔄
void resetGame(float& playerX, float& playerY, float& velocityY, bool& isGrounded, 
               int& hearts, float& survivalTime, std::vector<RedCube>& cubes, 
               float& spawnTimer, float& spawnInterval, bool& isGameOver) {
    playerX = 375.0f;
    playerY = 500.0f;
    velocityY = 0.0f;
    isGrounded = true;
    hearts = 3;
    survivalTime = 0.0f;
    spawnTimer = 0.0f;
    spawnInterval = 1.0f;
    cubes.clear();
    isGameOver = false;
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!SDL_CreateWindowAndRenderer("Survival Dodge", 800, 600, 0, &window, &renderer)) {
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderVSync(renderer, 1);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // World Boundaries 🧱
    const float LEFT_WALL = 50.0f;
    const float RIGHT_WALL = 750.0f;
    const float FLOOR_Y = 500.0f;

    // Player Stats & Physics 🟩
    float playerX = 375.0f;
    float playerY = FLOOR_Y;
    float playerSize = 40.0f;
    float moveSpeed = 350.0f;
    float velocityY = 0.0f;
    float gravity = 1200.0f;       // Gravity force 🍎
    float jumpForce = -550.0f;     // Jump strength 🦘
    bool isGrounded = true;
    int hearts = 3; 

    // Enemy Spawner Variables 🟥
    std::vector<RedCube> cubes;
    float spawnTimer = 0.0f;
    float spawnInterval = 1.0f;

    // Game Stats & Highscore ⏱️
    float survivalTime = 0.0f;
    float highScore = 0.0f;
    bool isGameOver = false;

    bool isRunning = true;
    SDL_Event event;
    Uint64 lastTicks = SDL_GetTicks();

    while (isRunning) {
        // 1. Calculate Delta Time ⏱️
        Uint64 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        // 2. Event Handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }
        }

        const bool* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_ESCAPE]) isRunning = false;

        // Restart feature on R press 🔄
        if (state[SDL_SCANCODE_R]) {
            resetGame(playerX, playerY, velocityY, isGrounded, hearts, 
                      survivalTime, cubes, spawnTimer, spawnInterval, isGameOver);
        }

        if (!isGameOver) {
            // Update Survival Time
            survivalTime += deltaTime;
            if (survivalTime > highScore) {
                highScore = survivalTime;
            }

            // 3. Horizontal Movement ↔️
            if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
                playerX -= moveSpeed * deltaTime;
            }
            if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
                playerX += moveSpeed * deltaTime;
            }

            // Wall Collision Check 🧱
            if (playerX < LEFT_WALL) playerX = LEFT_WALL;
            if (playerX > RIGHT_WALL - playerSize) playerX = RIGHT_WALL - playerSize;

            // 4. Jump & Physics Logic 🦘
            if ((state[SDL_SCANCODE_W] || state[SDL_SCANCODE_SPACE]) && isGrounded) {
                velocityY = jumpForce;
                isGrounded = false;
            }

            velocityY += gravity * deltaTime;
            playerY += velocityY * deltaTime;

            // Floor Collision Check 🧱
            if (playerY >= FLOOR_Y) {
                playerY = FLOOR_Y;
                velocityY = 0.0f;
                isGrounded = true;
            }

            // 5. Cube Spawning Logic (Dynamic Difficulty Ramping) 🎲
            // As survival time increases, spawn interval drops down to a minimum of 0.2s!
            spawnInterval = 1.0f - (survivalTime * 0.02f);
            if (spawnInterval < 0.2f) spawnInterval = 0.2f; 

            spawnTimer += deltaTime;
            if (spawnTimer >= spawnInterval) {
                spawnTimer = 0.0f;
                RedCube newCube;
                newCube.size = 35.0f;
                // Spawn randomly strictly between left and right walls
                float minX = LEFT_WALL;
                float maxX = RIGHT_WALL - newCube.size;
                newCube.x = minX + static_cast<float>(rand() % static_cast<int>(maxX - minX));
                newCube.y = -newCube.size; 
                newCube.speed = 200.0f + static_cast<float>(rand() % 300); // Speeds 200 to 500
                cubes.push_back(newCube);
            }

            // 6. Update & Move Cubes ⬇️
            for (size_t i = 0; i < cubes.size(); ) {
                cubes[i].y += cubes[i].speed * deltaTime;

                // Check collision with player 💥
                if (checkCollision(playerX, playerY, playerSize, playerSize,
                                   cubes[i].x, cubes[i].y, cubes[i].size, cubes[i].size)) {
                    hearts--;
                    cubes.erase(cubes.begin() + i);
                    if (hearts <= 0) {
                        isGameOver = true;
                    }
                    continue;
                }

                // Remove off-screen cubes 🧹
                if (cubes[i].y > 600.0f) {
                    cubes.erase(cubes.begin() + i);
                } else {
                    i++;
                }
            }
        }

        // 7. Rendering 🎨
        if (isGameOver) {
            SDL_SetRenderDrawColor(renderer, 60, 10, 10, 255); // Dark Red background on Game Over
        } else {
            SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255); // Dark Blue background
        }
        SDL_RenderClear(renderer);

        // Draw Walls and Ground 🧱
        SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
        SDL_FRect leftWallRect = { 0.0f, 0.0f, LEFT_WALL, 600.0f };
        SDL_FRect rightWallRect = { RIGHT_WALL, 0.0f, 80.0f, 600.0f };
        SDL_FRect floorRect = { 0.0f, FLOOR_Y + playerSize, 800.0f, 100.0f };
        SDL_RenderFillRect(renderer, &leftWallRect);
        SDL_RenderFillRect(renderer, &rightWallRect);
        SDL_RenderFillRect(renderer, &floorRect);

        // Draw Player (Green Square) 🟩
        if (!isGameOver) {
            SDL_FRect playerRect = { playerX, playerY, playerSize, playerSize };
            SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
            SDL_RenderFillRect(renderer, &playerRect);
        }

        // Draw Falling Red Cubes 🟥
        SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
        for (const auto& cube : cubes) {
            SDL_FRect cubeRect = { cube.x, cube.y, cube.size, cube.size };
            SDL_RenderFillRect(renderer, &cubeRect);
        }

        // Draw Hearts (Red Squares in top-left) ❤️
        SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
        for (int i = 0; i < hearts; i++) {
            SDL_FRect heartRect = { 60.0f + (i * 25.0f), 20.0f, 18.0f, 18.0f };
            SDL_RenderFillRect(renderer, &heartRect);
        }

        // Draw Timer Bar (Blue bar at top) & Highscore Bar (Gold bar) ⏱️
        // Live Survival Bar
        SDL_SetRenderDrawColor(renderer, 0, 180, 255, 255);
        SDL_FRect timerBar = { 200.0f, 20.0f, survivalTime * 15.0f, 12.0f }; // Grows over time
        SDL_RenderFillRect(renderer, &timerBar);

        // High Score Marker Bar
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_FRect highScoreBar = { 200.0f, 36.0f, highScore * 15.0f, 6.0f };
        SDL_RenderFillRect(renderer, &highScoreBar);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}