src = $(wildcard rackit/*.c) $(wildcard rackit/modules/*.c)

.PHONY: rackit_test
rackit_test: dist/rackit.so
	@cd dist; lua test.lua

dist/rackit.so: $(src)
	time clang rackit/rackit.c -shared -o dist/rackit.so \
		-fPIC -llua -I/usr/local/include/lua \
		-I/Library/Frameworks/SDL2.framework/ -lSDL2
