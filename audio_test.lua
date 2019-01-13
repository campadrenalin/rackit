lib = require "audio_lua"

a, b, c = lib.reverse('c', 'b', 'a')
assert(a == 'a')
assert(b == 'b')
assert(c == 'c')

buf = lib.Buffer()
assert(type(buf) == 'userdata', "Buffer is a userdata")
assert(type(getmetatable(buf).peek) == 'function', "Metatable is not nil")
assert(buf:peek() == 0, "First element is zero")
print("Tests pass - OK")

buf = lib.Buffer(32)
assert(buf:peek() == 32, "First element is 32")
