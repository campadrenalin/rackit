lib = require "rackit"

buf = lib.Buffer()
assert(type(buf) == 'userdata', "Buffer is a userdata")
assert(type(getmetatable(buf).peek) == 'function', "Metatable is not nil")
assert(buf:peek() == 0, "First element is zero")

buf = lib.Buffer(32)
assert(buf:peek() == 32, "First element is 32")

lib.sdl_init()

sin1 = lib.OscSine(440)
sin2 = lib.OscSine(450)
assert(sin1.freq == 440)
assert(type(sin1.buffer) == 'userdata')

print("Playing sound")
lib.sdl_play(1000)
lib.sdl_finish()
print("Tests pass - OK")
