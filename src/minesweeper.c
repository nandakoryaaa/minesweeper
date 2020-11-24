#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "An SDL2 window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

//    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
//    SDL_SetRenderDrawColor(renderer, 80, 60, 90, 255);
//    SDL_RenderClear(renderer);

    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);

    SDL_Rect rect = {100, 100, 100, 100};
    SDL_FillRect(screenSurface, &rect, SDL_MapRGB(screenSurface->format, 255, 0, 0));
    SDL_UpdateWindowSurface(window);
//    SDL_RenderPresent(renderer);

    SDL_Event event;

    while (1) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
