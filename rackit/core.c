#pragma once
#import "core.h"
#import "stdlib.h"

// Buffer Class ================================================================
void Buffer_fill(Buffer *buf, Sample value) {
    for (int i=0; i<BUFFER_SIZE; i++)
        (*buf)[i] = value;
}

// Port Class ==================================================================
void Port_set_patch(Port *p, struct Module *module, int port) {
    p->is_patched = 1;
    p->patch.module = module;
    p->patch.port = port;
}

void Port_set_constant(Port *p, Sample value) {
    p->is_patched = 0;
    Buffer_fill(&(p->buf.buf), value);
}

void Port_reset(Port *p) { Port_set_constant(p, 0); }

Buffer *Port_find_buffer(Port *p) {
    if (p->is_patched) {
        Port *foreign = &p->patch.module->ports[p->patch.port];
        return Port_find_buffer(foreign);
    } else {
        return &(p->buf.buf);
    }
}

// Module Class ================================================================

Module *Module_new(ModuleCallback callback, int num_ports) {
    Module *m = calloc(1, ModuleSize(num_ports));
    m->process_cb = callback;
    m->num_ports = num_ports;
    return m;
}

Buffer *Module_buffer(struct Module *self, int n) {
    return Port_find_buffer(&self->ports[n]);
}

void Module_process(struct Module *m, int length, long time) {
    // Recurse to all sources
    for (int p=0; p < m->num_ports; p++) {
        Port *port = &m->ports[p];
        if (port->is_patched)
            Module_process(port->patch.module, length, time);
    }

    // Don't double-process
    if (m->time == time) return;

    m->time = time;
    m->process_cb(m, length);
}

#define IN   (*in)[i]
#define OUT  (*out)[i]
#define FREQ (*freq)[i]
#define PW   (*pw)[i]
#define CENTER (*center)[i]
#define SCALE  (*scale)[i]

#define OSCILLATOR(formula) { \
    Buffer *out, *freq, *pw; \
    out = Module_buffer(m, 0); \
    freq = Module_buffer(m, 1); \
    if (m->num_ports >= 3) \
        pw = Module_buffer(m, 2); \
    Sample phase=0, p=0; \
    for (int i=0; i<length; i++) { \
        p += FREQ / SR; \
        phase = fmod(m->data.phase + p, 1); \
        OUT = formula; \
    } \
    m->data.phase = phase; \
}

#define MC(name) ModuleCallback_ ## name
#define DEF_MC(name) void MC(name)(struct Module *m, int length)
#define NEW_MODULE(callback, size) Module_new(& MC(callback), size)

DEF_MC(Sine)   OSCILLATOR(sin(phase * TAU));
DEF_MC(Saw)    OSCILLATOR(phase*2 -1);
DEF_MC(Square) OSCILLATOR( (phase>PW) *2 -1 );

DEF_MC(FMA) {
    Buffer *out    = Module_buffer(m, 0);
    Buffer *in     = Module_buffer(m, 1);
    Buffer *center = Module_buffer(m, 2);
    Buffer *scale  = Module_buffer(m, 3);
    for (int i=0; i<length; i++)
        OUT = (IN*SCALE) + CENTER;
};

// TODO: Container Module
