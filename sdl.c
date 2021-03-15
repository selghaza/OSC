/* sdl.c: read mouse position with sdl */
#include <SDL2/SDL.h>
#include <signal.h>

static volatile int done = 0;

static void
die(const char *msg)
{
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", msg);
  exit(1);
}

static void
on_signal(int signum)
{
  (void)signum;
  done = 1;
}

int
main(int argc, char *argv[])
{
  int x = 0, y = 0, prev_x = 0, prev_y = 0;
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    die("fail to init sdl");

  /* catch signal for grace shutdown */
  signal(SIGINT, on_signal);

  while (!done) {
    SDL_Event evt;

    while (SDL_PollEvent(&evt));

    SDL_GetGlobalMouseState(&x, &y);
    if (x != prev_x || y != prev_y) {
      printf("mouse: %d %d\n", x, y);
      prev_x = x;
      prev_y = y;
    }
  }

  SDL_Quit();
  return 0;
}
