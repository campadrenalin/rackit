#include "buffer.h"
#include "sdl_player.h"
const double PI = 3.141592653589793238463;
const double TAU = 2*PI;
const int AMPLITUDE = 13000;
const int SAMPLE_RATE = 44100;
const double sr = SAMPLE_RATE;
const SDL_AudioFormat FORMAT_WANTED = AUDIO_S16SYS;
typedef Sint16 FORMAT;

#include "modules/actor.c"
#include "modules/mixer.c"

Buffer buf_master;
Mixer mix_master = { 0, {}, &buf_master };

#include "modules/sine.c"
