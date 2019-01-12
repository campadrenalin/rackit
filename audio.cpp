#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

const int AMPLITUDE = 5000;
const int SAMPLE_RATE = 44100;
int cursor = 0;

Sint16 saw(int cursor, double freq) {
    int wavelength = SAMPLE_RATE/freq;
    double phase = (cursor % wavelength) / (double)wavelength; // From 0-1
    return (phase-0.5)*AMPLITUDE;
}

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2;
    int &cursor(*(int*)user_data);

    for (int i = 0; i < length; i++, cursor++) {
        buffer[i] = saw(cursor, 440);
    }
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
