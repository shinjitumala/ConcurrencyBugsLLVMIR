test: test.ll sqlite3.ll
	clang -lpthread -ldl -o $@ $^

%.ll: %.c
	clang -g3 -I . -S -emit-llvm -o $@ $<

clean:
	find . -type f -name "*.ll" -print -delete
	rm -f test