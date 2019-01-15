.PHONY: audio_lua
audio_lua: audio_lua.so
	lua audio_test.lua

audio_lua.so: audio_lua.c
	clang audio_lua.c -shared -o audio_lua.so \
		-fPIC -llua -I/usr/local/include/lua \
		-I/Library/Frameworks/SDL2.framework/ -lSDL2

run: a.out
	./a.out

a.out: src.txt gen.py
	@cat src.txt | python gen.py | clang -x c++ -

