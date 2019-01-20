src = $(wildcard rackit/*.c) $(wildcard rackit/modules/*.c) \
      $(wildcard rackit/*.h) $(wildcard rackit/modules/*.h)

.PHONY: rackit_test test clean
rackit_test: dist/rackit.so
	@cd dist; lua test.lua

test:
	clang test/main.c vendor/unity/src/unity.c -I vendor/unity/src -o test/runner.out \
		-I/Library/Frameworks/SDL2.framework/ -lSDL2
	test/runner.out

clean:
	rm -f test/*.out dist/rackit.so

dist/rackit.so: $(src)
	time clang rackit/rackit.c -shared -o dist/rackit.so \
		-fPIC -llua -I/usr/local/include/lua \
		-I/Library/Frameworks/SDL2.framework/ -lSDL2
