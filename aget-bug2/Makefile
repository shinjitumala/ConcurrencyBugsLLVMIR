LLS=aget-devel/Aget.ll aget-devel/Resume.ll aget-devel/Misc.ll aget-devel/loadrc.ll aget-devel/Ftp.ll aget-devel/Download.ll aget-devel/Signal.ll aget-devel/Head.ll aget-devel/main.ll 

%.ll: %.c
	clang -g3 -S -emit-llvm $< -o $@

aget.ll: $(LLS)
	llvm-link -S -o $@ $^

clean:
	find . -type f -name "*.ll" -print -delete