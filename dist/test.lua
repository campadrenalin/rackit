lib = require "rackit"

buf = lib.Buffer()
assert(type(buf) == 'userdata', "Buffer is a userdata")
assert(type(getmetatable(buf).peek) == 'function', "Metatable is not nil")
assert(buf:peek() == 0, "First element is zero")

buf = lib.Buffer(32)
assert(buf:peek() == 32, "First element is 32")

lib.sdl_init()

-- lfo_osc = lib.OscSquare(440/8)
-- lfo = lib.LFO(lfo_osc.out, 440)
lfo_osc = lib.OscSine(0.8)
lfo = lib.LFO(lfo_osc.out, 0.5)
assert(lfo.scale == 1)
lfo.scale = 0.2;

sin1 = lib.OscSine(440) --, lfo.out)
sin2 = lib.OscSquare(440/2, nil, lfo.out)

assert(type(sin1.out) == 'userdata')
assert(type(sin1.freq) == 'userdata')
assert(sin1.freq:peek(0)    == 440)
assert(sin1.freq:peek(2047) == 440)

function play_note()
    -- lib.MixMaster(0.3, sin1.out)
    lib.MixMaster(1, sin2.out)
    lib.sdl_play(2000)
end

function play_ramp(res)
    lib.MixMaster(1, sin2.out)
    for pw=0,1,0.01*res do
        --lfo.scale = power
        sin2.pw:fill(pw)
        lib.sdl_play(res*20)
    end
end

print("Playing sound")
play_note()
-- play_ramp(1)
lib.sdl_finish()
print("Tests pass - OK")
