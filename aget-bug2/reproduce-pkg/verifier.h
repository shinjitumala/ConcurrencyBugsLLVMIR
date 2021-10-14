#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <pthread.h>

unsigned int maple_bwritten;

int maple_read_log(struct hist_data *h)
{
	char *logfile;
	FILE *fp;
        char *user_home;
        int i;
	long total_bwritten;

	logfile = (char *)calloc(255, sizeof(char));

	// user_home = getenv("HOME");
	// snprintf(logfile, 255, "%s/aget.file-ageth.log", user_home);

	// if ((fp = fopen(logfile, "r")) == NULL) {
	// 	printf("cannot open log file\n");
	// 	exit(-1);
	// }

	// fread(h, sizeof(struct hist_data), 1, fp);
	maple_bwritten = h->bwritten;
	// fclose(fp);

	// if ((unlink(logfile)) == -1) {
	// 	printf("cannot delete log file\n");
	// 	exit(-1);
	// }
		
	free(logfile);

	total_bwritten = 0;
        for (i = 0; i < h->nthreads; i++) {
		total_bwritten += (h->wthread[i].offset - h->wthread[i].soffset);
	}
	assert(total_bwritten == maple_bwritten);

	return 0;
}

void maple_verify(struct hist_data *h) {
	maple_read_log(h);
	printf("results ok!\n");
}

