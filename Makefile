run: a.out
	./a.out

a.out: src.txt gen.py
	@cat src.txt | python gen.py | clang -x c++ -

.PHONY: audio
audio: audio.cpp
	clang audio.cpp -g -I/Library/Frameworks/SDL2.framework/ -lSDL2 -o audio
	time ./audio
