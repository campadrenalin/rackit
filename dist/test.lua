lib = require "rackit"

buf = lib.Buffer()
assert(type(buf) == 'userdata', "Buffer is a userdata")
assert(type(getmetatable(buf).peek) == 'function', "Metatable is not nil")
assert(buf:peek() == 0, "First element is zero")

buf = lib.Buffer(32)
assert(buf:peek() == 32, "First element is 32")

lib.sdl_init()

lfo_osc = lib.OscSine(440/8)
lfo = lib.LFO(lfo_osc.out, 440)
assert(lfo.scale == 1)
lfo.scale = 80;

sin1 = lib.OscSine(440) --, lfo.out)
sin2 = lib.OscSine(450)
-- lib.MixMaster(0.3, sin1.out)
lib.MixMaster(1, sin2.out)

assert(type(sin1.out) == 'userdata')
assert(type(sin1.freq) == 'userdata')
assert(sin1.freq:peek(0)    == 440)
assert(sin1.freq:peek(2047) == 440)

print("Playing sound")
sin2.freq = lfo.out;
-- for freq=880,440,-1 do
    -- sin1.freq:fill(freq)
    -- lib.sdl_play(10)
-- end
for power=0,300,1 do
    lfo.scale = power
    lib.sdl_play(10)
end
-- lib.sdl_play(2000)
lib.sdl_finish()
print("Tests pass - OK")
