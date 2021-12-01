PHOENIX_SRCS=phoenix/tpool.ll phoenix/pt_mutex.ll phoenix/map_reduce.ll phoenix/synch.ll phoenix/taskQ.ll phoenix/locality.ll phoenix/mcs.ll phoenix/scheduler.ll phoenix/iterator.ll phoenix/processor.ll phoenix/memory.ll
PHOENIX_DEFINES=-D_LINUX_

PROGRAMS=pca word_count matrix_multiply string_match kmeans histogram linear_regression 

all: phoenix.ll $(PROGRAMS)

%_linked.ll: %.ll phoenix.ll
	llvm-link -S $^ -o $@

pca: pca_linked.ll
	clang -g3 -lpthread $^ -o $@

word_count: word_count_linked.ll
	clang -g3 -lpthread $^ -o $@

matrix_multiply: matrix_multiply_linked.ll
	clang -g3 -lpthread $^ -o $@

string_match: string_match_linked.ll
	clang -g3 -lpthread $^ -o $@

kmeans: kmeans_linked.ll
	clang -g3 -lpthread $^ -o $@

histogram: histogram_linked.ll
	clang -g3 -lpthread $^ -o $@

linear_regression: linear_regression_linked.ll
	clang -g3 -lpthread $^ -o $@

phoenix.ll: $(PHOENIX_SRCS)
	llvm-link -S $^ -o $@

%.ll: %.c
	clang -g3 -S -emit-llvm -I . $(PHOENIX_DEFINES) $< -o $@

clean:
	find -type f -name "*.ll" -delete -print
	rm -f $(PROGRAMS)