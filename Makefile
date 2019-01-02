run: a.out
	./a.out

a.out: src.txt gen.py
	@cat src.txt | python gen.py | clang -x c++ -
