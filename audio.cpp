#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <cmath>
#include <array>
#include <vector>

const double PI = 3.141592653589793238463;
const double TAU = 2*PI;

const int AMPLITUDE = 15000;
const int SAMPLE_RATE = 44100;
const double DELTA = 1/(double) SAMPLE_RATE;
const double sr = SAMPLE_RATE;
int cursor = 0;

double wave_saw(int cursor, double freq) {
    int wavelength = SAMPLE_RATE/freq;
    double phase = (cursor % wavelength) / (double)wavelength; // From 0-1
    return phase-0.5;
}
double wave_sin(int cursor, double freq) {
    double wavelength = (double)SAMPLE_RATE/freq;
    double phase = (double)cursor / (double)wavelength;
    return sin(phase);
}

typedef std::array<double, 2048> Buffer;

void fill_buffer(Buffer *b, double v) {
    std::fill(b->begin(), b->end(), v);
}

struct Const {
    Buffer values;
    Const(double v) {
        fill_buffer(&values, v);
    }
};

struct OscSine {
    double phase;
    Buffer *freq;
    Buffer values;

    OscSine(Buffer *f) {
        freq = f;
        phase=0;
    }

    void process(int length) {
        double p = 0;
        for(int i=0; i<length; i++) {
            double t = (double)i/sr;
            double f = (*freq)[i]*TAU;
            p = t*f;
            values[i] = sin(phase + p);
        }
        phase += p;
    }
};

struct LFO {
    Buffer *osc;
    Buffer values;
    double center, scale;

    LFO(Buffer *o, double c, double s) {
        osc = o;
        center = c;
        scale  = s;
    }

    void process(int length) {
        for (int i=0; i<length; i++) {
            values[i] = ((*osc)[i] * scale) + center;
        }
    }
};

struct Mixer {
    std::vector<Buffer*> buffers;
    Buffer values;

    void process(int length) {
        fill_buffer(&values, 0);
        int num_buffers = buffers.size();
        for (int b=0; b<num_buffers; b++) {
            auto buf = buffers[b];
            for (int i=0; i<length; i++)
                values[i] += (*buf)[i];
        }
        for (int i=0; i<length; i++)
            values[i] /= num_buffers;
    }
};

Const c2(0.5);
OscSine lfo(&c2.values);
LFO pitch(&lfo.values, 440, 5);
OscSine detuned(&pitch.values);

Const f440(440);
OscSine tuned(&f440.values);
//Mixer master;

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2;
    int &cursor(*(int*)user_data);

    lfo.process(length);
    pitch.process(length);
    detuned.process(length);
    tuned.process(length);
    //master.process(length);
    for (int i=0; i<length; i++, cursor++) {
        buffer[i] = (detuned.values[i] + tuned.values[i])*AMPLITUDE/2; //master.values[i]*AMPLITUDE;
    }
    // SDL_Log("Sample: %d", buffer[0]);
}

int err(const char *fmt) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, fmt, SDL_GetError());
    return 1;
}

int audio_initialize() {
    return SDL_Init(SDL_INIT_AUDIO)
        && err("Failed to initialize SDL: %s");
}

int audio_open() {
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = SAMPLE_RATE;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 2048;
    want.callback = audio_callback;
    want.userdata = &cursor;

    if (SDL_OpenAudio(&want, &have) != 0)
        return err("Failed to open audio: %s");
    if (want.format != have.format)
        return err("Failed to get the desired AudioSpec");
    return 0;
}

int audio_play() {
    SDL_PauseAudio(0); // Start
    SDL_Delay(3000);   // Playing
    SDL_PauseAudio(1); // Stop
    return 0;
}

int audio_finish() {
    SDL_CloseAudio();
    return 0;
}

int main(int argc, char *argv[]) {
    return audio_initialize()
        || audio_open()
        || audio_play()
        || audio_finish();
}
