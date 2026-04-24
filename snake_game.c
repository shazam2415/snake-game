#include <SDL3/SDL.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define CELL_SIZE 20
#define GRID_WIDTH (WINDOW_WIDTH / CELL_SIZE)   // 32
#define GRID_HEIGHT (WINDOW_HEIGHT / CELL_SIZE) // 24
#define MAX_LENGTH (GRID_WIDTH * GRID_HEIGHT)   // 768

typedef struct {
  int x, y;
} Point;

typedef enum { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;

typedef struct {
  Point body[MAX_LENGTH];
  int length;
  Direction dir;
} Snake;

void snake_init(Snake *snake) {
  snake->length = 3;
  snake->dir = DIR_RIGHT;
  snake->body[0].x = GRID_WIDTH / 2;
  snake->body[0].y = GRID_HEIGHT / 2;
  snake->body[1].x = GRID_WIDTH / 2 - 1;
  snake->body[1].y = GRID_HEIGHT / 2;
  snake->body[2].x = GRID_WIDTH / 2 - 2;
  snake->body[2].y = GRID_HEIGHT / 2;
}

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

  Snake snake;
  snake_init(&snake);

  int running = 1;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = 0;
      if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.scancode == SDL_SCANCODE_ESCAPE ||
            event.key.scancode == SDL_SCANCODE_Q) {
          running = 0;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    // yılanı çiz
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE); // yeşil
    for (int i = 0; i < snake.length; i++) {
      SDL_FRect rect = {snake.body[i].x * CELL_SIZE,
                        snake.body[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
      SDL_RenderFillRect(renderer, &rect);
    }
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
