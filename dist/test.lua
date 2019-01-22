lib = require "rackit"

lib.sdl_init()

lfo_osc = lib.Saw(2)
lfo = lib.FMA(lfo_osc.out, 0.5)
assert(lfo.scale:value() == 1)
lfo.scale = -0.4;

C = 261.63
D = 293.66
E = 329.63
G = 392.00

sin1 = lib.Sine()
sin2 = lib.Square(440/4, lfo.out)
assert(sin1.freq:value() == 440)
assert(sin1.freq:value() == 440)
assert(sin2.pw:is_patched() == true)

function play_note()
    lib.sdl_play(sin2, 2000)
end

function play_ramp(res)
    for pw=0,1,0.01*res do
        --lfo.scale = power
        sin2.pw = pw
        lib.sdl_play(sin2, res*20)
    end
end

notes = {
    E, D, C, D, E, E, E, C/2,
    D, D, D, C/2, E, G, G, C/2,
    E, D, C, D, E, E, E, C/2,
    D, D, E, D, C, C, C, C/2,
}
function play_notes()
    sin2.pw = lfo.out
    for i, note in ipairs(notes) do
        sin2.freq = note
        lib.sdl_play(sin2, 500)
    end
end

print("Playing sound")
play_note()
play_ramp(1)
play_notes()
lib.sdl_finish()
print("Tests pass - OK")
