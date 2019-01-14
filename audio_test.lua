lib = require "audio_lua"

buf = lib.Buffer()
assert(type(buf) == 'userdata', "Buffer is a userdata")
assert(type(getmetatable(buf).peek) == 'function', "Metatable is not nil")
assert(buf:peek() == 0, "First element is zero")

buf = lib.Buffer(32)
assert(buf:peek() == 32, "First element is 32")

print("Playing sound")
lib.sdl_init()
lib.OscSine()
lib.sdl_play(1000)
lib.sdl_finish()
print("Tests pass - OK")
