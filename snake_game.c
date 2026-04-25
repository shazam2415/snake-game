#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static TTF_Font *font = NULL;

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

void food_spawn(Point *food) {
  food->x = rand() % GRID_WIDTH;
  food->y = rand() % GRID_HEIGHT;
}

void snake_move(Snake *snake) {
  // gövdeyi kaydır (arkadan öne)
  for (int i = snake->length - 1; i > 0; i--) {
    snake->body[i] = snake->body[i - 1];
  }
  // kafayı yöne göre ilerlet
  switch (snake->dir) {
  case DIR_RIGHT:
    snake->body[0].x++;
    break;
  case DIR_LEFT:
    snake->body[0].x--;
    break;
  case DIR_UP:
    snake->body[0].y--;
    break;
  case DIR_DOWN:
    snake->body[0].y++;
    break;
  }
}

int main(int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return 1;
  }

  if (!TTF_Init()) {
    SDL_Log("Couldn't initialize TTF: %s", SDL_GetError());
    return 1;
  }

  font = TTF_OpenFont("/usr/share/fonts/TTF/JetBrainsMonoNerdFontMono-Bold.ttf",
                      24);
  if (!font) {
    SDL_Log("Couldn't load font: %s", SDL_GetError());
    return 1;
  }

  if (!SDL_CreateWindowAndRenderer("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                                   &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return 1;
  }

  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  Snake snake;
  Point food;
  snake_init(&snake);
  int score = 0;
  int game_over = 0;
  srand(SDL_GetTicks());
  food_spawn(&food);

  int running = 1;
  Uint64 last_move = SDL_GetTicks();
  SDL_Event event;

  while (running) {

    if (game_over) {
      // Q ile çık, R ile restart
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
          running = 0;
        if (event.type == SDL_EVENT_KEY_DOWN) {
          if (event.key.scancode == SDL_SCANCODE_Q)
            running = 0;
          if (event.key.scancode == SDL_SCANCODE_R) {
            snake_init(&snake);
            score = 0;
            game_over = 0;
            food_spawn(&food);
          }
        }
      }
      // game over ekranı çiz
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(renderer);

      char go_text[64];
      snprintf(go_text, sizeof(go_text), "GAME OVER - Skor: %d", score);
      SDL_Color red = {255, 0, 0, 255};
      SDL_Surface *surface = TTF_RenderText_Solid(font, go_text, 0, red);
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_FRect rect = {WINDOW_WIDTH / 2 - surface->w / 2,
                        WINDOW_HEIGHT / 2 - surface->h / 2, surface->w,
                        surface->h};
      SDL_DestroySurface(surface);
      SDL_RenderTexture(renderer, texture, NULL, &rect);
      SDL_DestroyTexture(texture);

      SDL_Color white = {255, 255, 255, 255};
      SDL_Surface *s2 =
          TTF_RenderText_Solid(font, "R: Yeniden Baslat  Q: Cik", 0, white);
      SDL_Texture *t2 = SDL_CreateTextureFromSurface(renderer, s2);
      SDL_FRect rect2 = {WINDOW_WIDTH / 2 - s2->w / 2, WINDOW_HEIGHT / 2 + 40,
                         s2->w, s2->h};
      SDL_DestroySurface(s2);
      SDL_RenderTexture(renderer, t2, NULL, &rect2);
      SDL_DestroyTexture(t2);

      SDL_RenderPresent(renderer);
      continue;
    }

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = 0;
      if (event.type == SDL_EVENT_KEY_DOWN) {
        SDL_Scancode sc = event.key.scancode;
        if (sc == SDL_SCANCODE_ESCAPE || sc == SDL_SCANCODE_Q)
          running = 0;
        else if (sc == SDL_SCANCODE_UP && snake.dir != DIR_DOWN)
          snake.dir = DIR_UP;
        else if (sc == SDL_SCANCODE_DOWN && snake.dir != DIR_UP)
          snake.dir = DIR_DOWN;
        else if (sc == SDL_SCANCODE_LEFT && snake.dir != DIR_RIGHT)
          snake.dir = DIR_LEFT;
        else if (sc == SDL_SCANCODE_RIGHT && snake.dir != DIR_LEFT)
          snake.dir = DIR_RIGHT;
      }
    }

    Uint64 now = SDL_GetTicks();
    if (now - last_move >= 150) {
      snake_move(&snake);
      if (snake.body[0].x < 0 || snake.body[0].x >= GRID_WIDTH ||
          snake.body[0].y < 0 || snake.body[0].y >= GRID_HEIGHT) {
        game_over = 1;
      }
      for (int i = 1; i < snake.length; i++) {
        if (snake.body[0].x == snake.body[i].x &&
            snake.body[0].y == snake.body[i].y) {
          game_over = 1;
        }
      }
      if (snake.body[0].x == food.x && snake.body[0].y == food.y) {
        snake.length++;
        score++;
        food_spawn(&food);
      }
      last_move = now;
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
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_FRect frect = {food.x * CELL_SIZE, food.y * CELL_SIZE, CELL_SIZE,
                       CELL_SIZE};
    SDL_RenderFillRect(renderer, &frect);

    char score_text[32];
    SDL_snprintf(score_text, sizeof(score_text), "Skor: %d", score);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, score_text, 0, white);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FRect score_rect = {10, 10, surface->w, surface->h};
    SDL_DestroySurface(surface);
    SDL_RenderTexture(renderer, texture, NULL, &score_rect);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer);
  }

  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
