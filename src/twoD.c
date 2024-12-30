#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <math.h>
#include <stdio.h>

#define WIDTH_INIT 900
#define HEIGHT_INIT 600
#define COLOR_WHITE 0xFFFFA500
#define COLOR_BLACK 0x000000FF
#define COLOR_GREEN 0x00FF00FF
#define COLOR_RED 0xFF0000FF
#define RAYS_NUMBER 100

struct Circle {
  double x, y, radius;
};

struct Ray {
  double x_start, y_start, x_end, y_end, angle;
};

int WIDTH = WIDTH_INIT;
int HEIGHT = HEIGHT_INIT;

void FillCircle(SDL_Renderer *renderer, struct Circle circle, Uint32 color) {
  SDL_SetRenderDrawColor(renderer, (color >> 24) & 0xFF, (color >> 16) & 0xFF,
                         (color >> 8) & 0xFF, color & 0xFF);
  for (int x = circle.x - circle.radius; x < circle.x + circle.radius; x++) {
    for (int y = circle.y - circle.radius; y < circle.y + circle.radius; y++) {
      if (sqrt(pow(x - circle.x, 2) + pow(y - circle.y, 2)) < circle.radius) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
          SDL_RenderDrawPoint(renderer, x, y);
        }
      }
    }
  }
}

// Function to check if a ray intersects a circle
int ray_intersects_circle(struct Ray ray, struct Circle circle,
                          double *intersection_x, double *intersection_y) {
  double cx = circle.x;
  double cy = circle.y;
  double r = circle.radius;
  double ox = ray.x_start;
  double oy = ray.y_start;
  double dx = cos(ray.angle);
  double dy = sin(ray.angle);

  double a = dx * dx + dy * dy;
  double b = 2 * (dx * (ox - cx) + dy * (oy - cy));
  double c = (ox - cx) * (ox - cx) + (oy - cy) * (oy - cy) - r * r;

  double discriminant = b * b - 4 * a * c;
  if (discriminant < 0) {
    return 0; // No intersection
  } else {
    double t1 = (-b - sqrt(discriminant)) / (2 * a);
    double t2 = (-b + sqrt(discriminant)) / (2 * a);

    // Consider only intersections in the forward direction of the ray (t > 0)
    if (t1 >= 0) {
      *intersection_x = ox + t1 * dx;
      *intersection_y = oy + t1 * dy;
      return 1;
    } else if (t2 >= 0) {
      *intersection_x = ox + t2 * dx;
      *intersection_y = oy + t2 * dy;
      return 1;
    } else {
      return 0;
    }
  }
}

void generate_rays(struct Circle circle, struct Ray rays[RAYS_NUMBER]) {
  double pi = acos(-1);
  for (int i = 0; i < RAYS_NUMBER; i++) {
    rays[i].x_start = circle.x;
    rays[i].y_start = circle.y;
    rays[i].angle = 2 * pi * i / RAYS_NUMBER;
    // Calculate a long initial endpoint, we'll shorten it if it hits something
    rays[i].x_end = circle.x + 2000 * cos(rays[i].angle);
    rays[i].y_end = circle.y + 2000 * sin(rays[i].angle);
  }
}

void draw_rays(SDL_Renderer *renderer, struct Ray rays[RAYS_NUMBER],
               struct Circle shadowCircle) {
  SDL_SetRenderDrawColor(renderer, (COLOR_RED >> 24) & 0xFF,
                         (COLOR_RED >> 16) & 0xFF, (COLOR_RED >> 8) & 0xFF,
                         COLOR_RED & 0xFF);
  for (int i = 0; i < RAYS_NUMBER; i++) {
    double intersection_x, intersection_y;
    if (ray_intersects_circle(rays[i], shadowCircle, &intersection_x,
                              &intersection_y)) {
      SDL_RenderDrawLine(renderer, rays[i].x_start, rays[i].y_start,
                         intersection_x, intersection_y);
    } else {
      // If no intersection, draw the ray to the window edge (or a sufficiently
      // far point) We'll keep the long endpoint calculated in generate_rays for
      // simplicity here
      SDL_RenderDrawLine(renderer, rays[i].x_start, rays[i].y_start,
                         rays[i].x_end, rays[i].y_end);
    }
  }
}

int is_point_in_circle(int x, int y, struct Circle circle) {
  return sqrt(pow(x - circle.x, 2) + pow(y - circle.y, 2)) < circle.radius;
}

int twoDmain(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("RayTracing", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
                                        SDL_WINDOW_RESIZABLE);

  if (!window) {
    SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  struct Circle circle = {200, 200, 30};
  struct Circle shadowCircle = {700, 300, 60};
  struct Ray rays[RAYS_NUMBER];

  int simulate = 1;
  SDL_Event event;
  int dragging_circle1 = 0;
  int dragging_circle2 = 0;
  int mouse_x, mouse_y;

  while (simulate) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        simulate = 0;
      } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        mouse_x = event.button.x;
        mouse_y = event.button.y;
        if (is_point_in_circle(mouse_x, mouse_y, circle)) {
          dragging_circle1 = 1;
        } else if (is_point_in_circle(mouse_x, mouse_y, shadowCircle)) {
          dragging_circle2 = 1;
        }
      } else if (event.type == SDL_MOUSEBUTTONUP) {
        dragging_circle1 = 0;
        dragging_circle2 = 0;
      } else if (event.type == SDL_MOUSEMOTION) {
        mouse_x = event.motion.x;
        mouse_y = event.motion.y;
        if (dragging_circle1) {
          if (mouse_x - circle.radius > 0 && mouse_x + circle.radius < WIDTH &&
              mouse_y - circle.radius > 0 && mouse_y + circle.radius < HEIGHT) {
            circle.x = mouse_x;
            circle.y = mouse_y;
          }
        } else if (dragging_circle2) {
          if (mouse_x - shadowCircle.radius > 0 &&
              mouse_x + shadowCircle.radius < WIDTH &&
              mouse_y - shadowCircle.radius > 0 &&
              mouse_y + shadowCircle.radius < HEIGHT) {
            shadowCircle.x = mouse_x;
            shadowCircle.y = mouse_y;
          }
        }
      } else if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          WIDTH = event.window.data1;
          HEIGHT = event.window.data2;
          SDL_Log("Window resized to: %d x %d", WIDTH, HEIGHT);
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, (COLOR_BLACK >> 24) & 0xFF,
                           (COLOR_BLACK >> 16) & 0xFF,
                           (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF);
    SDL_RenderClear(renderer);

    FillCircle(renderer, circle, COLOR_WHITE);
    FillCircle(renderer, shadowCircle, COLOR_GREEN);
    generate_rays(circle, rays);
    draw_rays(renderer, rays, shadowCircle); // Pass shadowCircle to draw_rays

    SDL_RenderPresent(renderer);

    SDL_Delay(10);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  printf("2D RayTracing Done.\n");
  return 0;
}
