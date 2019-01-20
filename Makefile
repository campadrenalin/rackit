src = $(wildcard rackit/*.c) $(wildcard rackit/modules/*.c) \
      $(wildcard rackit/*.h) $(wildcard rackit/modules/*.h)

.PHONY: rackit_test test
rackit_test: dist/rackit.so
	@cd dist; lua test.lua

test:
	clang test/test_main.c vendor/unity/src/unity.c -I vendor/unity/src -o test/test.out
	test/test.out

dist/rackit.so: $(src)
	time clang rackit/rackit.c -shared -o dist/rackit.so \
		-fPIC -llua -I/usr/local/include/lua \
		-I/Library/Frameworks/SDL2.framework/ -lSDL2
