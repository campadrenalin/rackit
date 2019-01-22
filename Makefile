src = $(wildcard rackit/*.c) $(wildcard rackit/modules/*.c) \
      $(wildcard rackit/*.h) $(wildcard rackit/modules/*.h)
lua_module=dist/rackit.so

.PHONY: test clean
all: test playback

playback: $(lua_module)
	@cd dist && lua test.lua

test:
	clang test/main.c vendor/unity/src/unity.c -I vendor/unity/src -o test/runner.out \
		-fPIC -llua -I/usr/local/include/lua \
		-I/Library/Frameworks/SDL2.framework/ -lSDL2
	test/runner.out

clean:
	rm -f test/*.out dist/rackit.so

$(lua_module): $(src)
	time clang rackit/lua.c -shared -o dist/rackit.so \
		-fPIC -llua -I/usr/local/include/lua \
		-I/Library/Frameworks/SDL2.framework/ -lSDL2
