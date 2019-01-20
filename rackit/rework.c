#pragma once
#import "stdlib.h"

// Buffer Class ================================================================
static const int BUFFER_SIZE = 2048;
typedef double Sample;
typedef Sample Buffer[2048];

void Buffer_fill(Buffer *buf, Sample value) {
    for (int i=0; i<BUFFER_SIZE; i++)
        (*buf)[i] = value;
}

// Port Class ==================================================================
typedef struct { int one;  void *module; int port; } PortPatch;
typedef struct { int zero; Buffer buf;             } PortBuffer;
typedef union _rackit_port {
    int is_patched;
    PortPatch patch;
    PortBuffer buf;
} Port;

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

// Module Class ================================================================

typedef union _rackit_module_data {
    Sample phase;
} ModuleData;

typedef void(*ModuleCallback) (void *self, int length);
typedef struct {
    ModuleCallback process_cb;
    long time;
    ModuleData data;
    int num_ports;
    Port ports[];
} Module;

#define ModuleSize(num_ports) sizeof(Module) + num_ports*sizeof(Port)
Module *Module_new(int num_ports) {
    Module *m = calloc(1, ModuleSize(num_ports));
    m->num_ports = num_ports;
    return m;
}
