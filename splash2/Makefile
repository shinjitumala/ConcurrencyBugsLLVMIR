PROGRAMS=fft radix lu_contiguous_blocks lu_non_contiguous_blocks

all: $(PROGRMS)

fft: fft.ll
	clang -lpthread -lm -o $@ $< 

radix: radix.ll
	clang -lpthread -o $@ $< 

lu_contiguous_blocks: lu_contiguous_blocks.ll
	clang -lpthread -lm -o $@ $<

lu_non_contiguous_blocks: lu_non_contiguous_blocks.ll
	clang -lpthread -lm -o $@ $<
%.ll: %.C
	clang -g3 -S -emit-llvm -o $@ $<

clean:
	find . -type f -name "*.ll" -print -delete
	rm -f $(PROGRAMS)