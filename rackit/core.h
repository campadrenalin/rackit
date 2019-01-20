#pragma once
#import "stdlib.h"

const double PI = 3.141592653589793238463;
const double TAU = 2*PI;
const int SAMPLE_RATE = 44100;
const double SR = SAMPLE_RATE;
typedef double Sample;

// Buffer Class ================================================================
static const int BUFFER_SIZE = 2048;
typedef Sample Buffer[2048];

void Buffer_fill(Buffer *buf, Sample value);

// Port Class ==================================================================
typedef struct { int one;  struct Module *module; int port; } PortPatch;
typedef struct { int zero; Buffer buf;                      } PortBuffer;
typedef union _rackit_port {
    int is_patched;
    PortPatch patch;
    PortBuffer buf;
} Port;

void Port_set_patch(Port *p, struct Module *module, int port);
void Port_set_constant(Port *p, Sample value);
void Port_reset(Port *p);

Buffer *Port_find_buffer(Port *p);

// Module Class ================================================================

typedef union _rackit_module_data {
    Sample phase;
} ModuleData;

typedef void(*ModuleCallback) (struct Module *self, int length);
typedef struct Module {
    ModuleCallback process_cb;
    long time;
    ModuleData data;
    int num_ports;
    Port ports[];
} Module;

#define ModuleSize(num_ports) sizeof(Module) + num_ports*sizeof(Port)
Module *Module_new(ModuleCallback callback, int num_ports);
Buffer *Module_buffer(struct Module *self, int n);
void Module_process(struct Module *m, int length, long time);

#define MC(name) ModuleCallback_ ## name
#define DEF_MC(name) void MC(name)(struct Module *m, int length)
#define NEW_MODULE(callback, size) Module_new(& MC(callback), size)
