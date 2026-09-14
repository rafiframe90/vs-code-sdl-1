#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL could not initialize: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("My First SDL3 App", 800, 600, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

     // 1. Pick a background color (Red, Green, Blue, Alpha)
     SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

     // 2. Clear the screen with that color
     SDL_RenderClear(renderer);

     // 3. Show what we drew on the screen
     SDL_RenderPresent(renderer);

     // 4. Pause for 3000 milliseconds (3 seconds)
     // --- MAIN LOOP START ---
bool is_running = true;
SDL_Event event;

while (is_running)
{
    // Check for OS events (like clicking the window's close button)
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            is_running = false; 
        }
    }

    
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

    
    SDL_RenderClear(renderer);

    
    SDL_RenderPresent(renderer);
}


     SDL_DestroyRenderer(renderer);
     SDL_DestroyWindow(window);
     SDL_Quit();

    return 0;
}