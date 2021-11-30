all: test test-sanitized source-adce.ll

test: source.ll
	clang -lpthread -ldl -o $@ $^

test-sanitized: test.c sqlite3.c
	clang -g3 -I . -ldl -fsanitize=thread $^ -o $@

source.ll: test.ll sqlite3.ll
	llvm-link -S $^ -o $@

source-adce.ll: source.ll
	llvm-link -flto -S -flto $< -o $@

%.ll: %.c
	clang -g3 -I . -S -emit-llvm -o $@ $<

clean:
	find . -type f -name "*.ll" -print -delete
	rm -f test test-sanitized

clean-db:
	find . -type f -name "foobar.db*" -print -delete