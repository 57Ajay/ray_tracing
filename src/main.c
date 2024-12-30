#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#define WIDTH 900
#define HEIGHT 600
#define COLOR_WHITE 0xFFFFA500
#define COLOR_BLACK 0x000000FF
#define COLOR_GREEN 0x00FF00FF

struct Circle {
  double x, y, radius;
};

void FillCircle(SDL_Surface *surface, struct Circle circle, Uint32 color) {
  // fills a circle with the help of math lib and for loops;
  for (int x = circle.x - circle.radius; x < circle.x + circle.radius; x++) {
    for (int y = circle.y - circle.radius; y < circle.y + circle.radius; y++) {
      if (sqrt(pow(x - circle.x, 2) + pow(y - circle.y, 2)) < circle.radius) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
          ((Uint32 *)surface->pixels)[y * WIDTH + x] = color;
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("RayTracing", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
  SDL_Surface *surface = SDL_GetWindowSurface(window);
  struct Circle circle = {200, 200, 80};
  struct Circle shadowCircle = {700, 300, 120};
  SDL_Rect erase_rect = {0, 0, WIDTH, HEIGHT};

  int simulate = 1;
  SDL_Event event;
  while (simulate) {

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        simulate = 0;
      }
      if (event.type == SDL_MOUSEMOTION && event.motion.state != 0) {
        circle.x = event.motion.x;
        circle.y = event.motion.y;
      }
    }

    SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
    FillCircle(surface, circle, COLOR_WHITE);
    FillCircle(surface, shadowCircle, COLOR_GREEN);
    SDL_UpdateWindowSurface(window);

    SDL_Delay(10);
  }

  SDL_Quit();
  printf("Success");
  return 0;
}
