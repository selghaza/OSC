/* sine_tbl.c: wavetable sine oscillator */
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <signal.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846


#endif

#include <jack/jack.h>
typedef jack_default_audio_sample_t sample_t;


#define map(val, x0, x1, y0, y1) \
  ((y0) + ((y1)-(y0)) * ((val)-(x0))/((float)(x1)-(x0)))

static jack_client_t *client = NULL;
static jack_port_t *port_in = NULL, *port_out = NULL;
static float phs=0;
static float freq = 440.f;
static jack_nframes_t sr;

/* ---------------- tbl ---------------- */

#define N 512
static sample_t tbl[N][N];


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
// static sample_t tbl[N];

/* init wavetable. this should be called before
 * activating client */
// static void
// tbl_init(void) {
//   /* divide [0,2π) into N evenly spaced samples */
//   float step = 2*M_PI / N;
//   size_t i;

//   for (i = 0; i < N; ++i){
//     tbl[i] = sin(i*step);
//   }
// }

float terrain_func(float x, float y){
  return (x-y)*(x-1)*(x+1)*(y-1)*(y+1);
}

static void
tbl_init(void) {
  size_t i,j;

  float x, y;
  for (i = 0; i < N; ++i) {
    x = map(i, 0, 100, -1, 1);
    for (j = 0; j < N; ++j) {
      y = map(j, 0, 100, -1, 1);
      tbl[i][j] = terrain_func(x,y); 
    }
  }
}


static sample_t
tbl_eval(float x, float y)
{

  sample_t v00, v10, v01, v11, u0, u1;
  float i_map, j_map;
  float i_fr, j_fr;
  size_t i, j;



  /* 1. wrap x,y to be in the range [-1,1) */
  while (x >= 1.0) x-=2;
  while (x < -1.0) x+=2;

  while (y >= 1.0) y-=2;
  while (y < -1.0) y+=2;


  /* 2. map x,y to float index i, j in the range [0, N)
   *    (you might need a new variable here) */
  if (phs < 0 || phs >= 1)
    return 0;

  i_map = fabsf(x*N);
  j_map = fabsf(y*N);



  /* 3. compute integer (i) and fractional part (fr) of float
   *    index i, j */
  
  i = (size_t) i_map;
  i_fr = i_map - (float)i;

  j = (size_t) j_map;
  j_fr = j_map - (float)j;


  /* 4. find neighboring values */
  v00 = tbl[i][j];
  v10 = tbl[(i+1) % N][j];
  v01 = tbl[i][(j+1) % N];
  v11 = tbl[(i+1) % N][(j+1) % N];

  /* 5. bilinear interpolation with i_fr and j_fr as weights */
  u0 = (1-i_fr)*v00 + i_fr*v10;
  u1 = (1-i_fr)*v01 + i_fr*v11;

  // return interpolate(v00, v10, v01, v11, i_fr, j_fr);
  return (1-j_fr)*u0 + j_fr*u1;;
}

/* ---------------- /tbl ---------------- */

static int
on_process(jack_nframes_t nframes, void *arg)
{
  sample_t *in, *out;
  jack_nframes_t i;
  static jack_nframes_t tick = 0;

  // static float phs = 0;
  // static float freq = 400;

  in  = jack_port_get_buffer(port_in,  nframes);
  out = jack_port_get_buffer(port_out, nframes);

  float r=0.8;

  // phs = mouse clicks
  for (i = 0; i < nframes; ++i) {
    // phs += freq/(float)nframes;
    // sample_t x = r*cos(2*M_PI*i);
    // sample_t y = r*sin(2*M_PI*i);

    out[i] = sin(2*M_PI * 440 * tick++/sr);

    // out[i] = tbl_eval(x, y);
  }

  return 0;
}

static void
jack_init(void)
{
  client = jack_client_open("sine", JackNoStartServer, NULL);

  sr = jack_get_sample_rate(client);

  jack_set_process_callback(client, on_process, NULL);

  port_in  = jack_port_register(client, "phs", JACK_DEFAULT_AUDIO_TYPE,
                                JackPortIsInput, 0);
  port_out = jack_port_register(client, "out", JACK_DEFAULT_AUDIO_TYPE,
                                JackPortIsOutput, 0);
}

static void
jack_finish(void)
{
  jack_deactivate(client);
  jack_client_close(client);
}


// int
// main(int argc, char *argv[])
// {
//   int x = 0, y = 0, prev_x = 0, prev_y = 0;
//   (void)argc;
//   (void)argv;

//   if (SDL_Init(SDL_INIT_VIDEO) != 0)
//     die("fail to init sdl");

//   /* catch signal for grace shutdown */
//   signal(SIGINT, on_signal);

//   while (!done) {
//     SDL_Event evt;

//     while (SDL_PollEvent(&evt));

//     SDL_GetGlobalMouseState(&x, &y);
//     if (x != prev_x || y != prev_y) {
//       printf("mouse: %d %d\n", x, y);
//       prev_x = x;
//       prev_y = y;
//     }
//   }

//   SDL_Quit();
//   return 0;
// }

int
main(void)
{
  jack_init();
  tbl_init();    /* <- must be called before activating client */
  jack_activate(client);

  /* idle main thread */
  getchar();

  jack_finish();
  return 0;
}
