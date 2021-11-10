all: fft

fft: fft.ll
	clang -lpthread -lm -o $@ $< 

%.ll: %.c
	clang -g3 -S -emit-llvm -o $@ $<

clean:
	find . -type f -naem "*.ll"