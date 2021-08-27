LLS=./src/Aget.ll ./src/Resume.ll ./src/Misc.ll ./src/loadrc.ll ./src/Ftp.ll ./src/Download.ll ./src/Signal.ll ./src/Head.ll ./src/main.ll ./main.ll 

%.ll: %.c
	clang -g3 -S -emit-llvm $< -o $@ -fcommon

llvm: $(LLS)
	llvm-link -S $(LLS) -o aget_1_trigger.ll

clean:
	find . -type f -name "*.ll" -printf "%p\n" -delete