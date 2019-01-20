src = $(wildcard rackit/*.c) $(wildcard rackit/modules/*.c) \
      $(wildcard rackit/*.h) $(wildcard rackit/modules/*.h)

.PHONY: rackit_test
rackit_test: dist/rackit.so
	@cd dist; lua test.lua

test: test_buffer test_port test_module

test_buffer:
	clang test/test_buffer.c vendor/unity/src/unity.c -I vendor/unity/src -o test/buffer.out
	test/buffer.out

test_port:
	clang test/test_port.c vendor/unity/src/unity.c -I vendor/unity/src -o test/port.out
	test/port.out

test_module:
	clang test/test_module.c vendor/unity/src/unity.c -I vendor/unity/src -o test/module.out
	test/module.out

dist/rackit.so: $(src)
	time clang rackit/rackit.c -shared -o dist/rackit.so \
		-fPIC -llua -I/usr/local/include/lua \
		-I/Library/Frameworks/SDL2.framework/ -lSDL2
