typedef struct {
    double amount;
    Buffer *input;
} MixInput;

typedef struct {
    int n_ins;
    MixInput ins[500];
    Buffer *out;
} Mixer;

void Mixer_process(void *raw_mixer, int length) {
    Mixer *m = raw_mixer;
    Buffer *out = m->out;

    // Clear the buffer
    for(int i=0; i<length; i++)
        (*out)[i] = 0;

    // Accumulate inputs into out buffer
    for (int in=0; in<m->n_ins; in++) {
        MixInput mi = m->ins[in];
        for (int i=0; i<length; i++) 
            (*out)[i] += mi.amount * (*mi.input)[i];
    }
}

void Mixer_append(Mixer *m, double amount, Buffer *buf) {
    MixInput mi = { amount, buf };
    m->ins[m->n_ins++] = mi;
}
