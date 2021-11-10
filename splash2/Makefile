all: fft radix

fft: fft.ll
	clang -lpthread -lm -o $@ $< 

radix: radix.ll
	clang -lpthread -o $@ $< 

%.ll: %.C
	clang -g3 -S -emit-llvm -o $@ $<

clean:
	find . -type f -name "*.ll" -print -delete
	rm -f fft radix