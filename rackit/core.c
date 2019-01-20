#pragma once
#import "core.h"
#import "stdlib.h"

// Buffer Class ================================================================
void Buffer_fill(Buffer *buf, Sample value) {
    for (int i=0; i<BUFFER_SIZE; i++)
        (*buf)[i] = value;
}

// Port Class ==================================================================
void Port_set_patch(Port *p, void *module, int port) {
    p->is_patched = 1;
    p->patch.module = module;
    p->patch.port = port;
}

void Port_set_constant(Port *p, Sample value) {
    p->is_patched = 0;
    Buffer_fill(&(p->buf.buf), value);
}

void Port_reset(Port *p) { Port_set_constant(p, 0); }

// TODO: use a header file to get declarations early
Buffer *Port_find_buffer(Port *p) {
    if (p->is_patched) {
        Port *foreign = &p->patch.module->ports[p->patch.port];
        return Port_find_buffer(foreign);
    } else {
        return &(p->buf.buf);
    }
}

// Module Class ================================================================

#define ModuleSize(num_ports) sizeof(Module) + num_ports*sizeof(Port)
Module *Module_new(int num_ports) {
    Module *m = calloc(1, ModuleSize(num_ports));
    m->num_ports = num_ports;
    return m;
}
