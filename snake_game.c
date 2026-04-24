#include <SDL3/SDL.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }

    if (!SDL_CreateWindowAndRenderer("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                                     &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return 1;
    }

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = 0;
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE ||
                    event.key.scancode == SDL_SCANCODE_Q) {
                    running = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
