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

assert(type(sin1.out) == 'userdata')
assert(type(sin1.freq) == 'userdata')
assert(sin1.freq:peek(0)    == 440)
assert(sin1.freq:peek(2047) == 440)

print("Playing sound")
for freq=880,440,-1 do
    sin1.freq:fill(freq)
    lib.sdl_play(10)
end
lib.sdl_finish()
print("Tests pass - OK")
