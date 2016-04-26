/*-----------------------------------------------------------------------------
 * Linux IPC example by Michael Yang, April 2008
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


/*-----------------------------------------------------------------------------
 * Macros
 *---------------------------------------------------------------------------*/
#define ERROR(...)    fprintf( stderr, __VA_ARGS__ )


/*-----------------------------------------------------------------------------
 * Constants
 *---------------------------------------------------------------------------*/
const size_t  buffer_size      = 1024;


/*-----------------------------------------------------------------------------
 * Implementations
 *---------------------------------------------------------------------------*/
int main(int argc_, char* argv_[])
{
	int pipe_fds[ 2 ];
	pid_t pid;
	int rt;
	FILE* stream;
	char buf[ 1024 ];
	int len;

	printf(">>> Pipe Example >>>\n");
	printf("Parent: pid=%d\n", getpid());

	rt = pipe(pipe_fds);
	if (rt < 0) {
		ERROR("Failed to create pipe, err=%d\n", rt);
		return rt;
	}

	pid = fork();
	if (pid == 0) {
		/* Child process */
		printf(" Child: pid=%d ppid=%d\n", getpid(), getppid());

		close(pipe_fds[ 1 ]);

		stream = fdopen(pipe_fds[ 0 ], "r");
		if (stream == NULL) {
			ERROR(" Child: Failed to open stream\n");
			close(pipe_fds[ 0 ]);
			return -1;
		}

		while (fgets(buf, 1024, stream)) {
			len = strlen(buf);
			if (len <= 0) {
				printf(" Child: [Empty String]\n");
				continue;
			}

			/* Get rid of the tailing ENTER */
			if (buf[ len - 1 ] == '\n') {
				buf[ len - 1 ] = 0;
			}

			printf(" Child: %s\n", buf);

			if (strcmp(buf, "exit") == 0) {
				break;
			}
		}

		printf(" Child: Process terminating\n");
		fclose(stream);
		close(pipe_fds[ 0 ]);
	} else if (pid > 0) {
		close(pipe_fds[ 0 ]);

		stream = fdopen(pipe_fds[ 1 ], "w");
		if (stream == NULL) {
			ERROR("Parent: Failed to open stream\n");
			close(pipe_fds[ 1 ]);
			return -1;
		}

		while (printf("\nParent: "), gets(buf)) {
			fprintf(stream, "%s\n", buf);
			fflush(stream);

			sleep(1);

			if (strcmp(buf, "exit") == 0) {
				break;
			}
		}

		printf("Parent: Process terminating\n");
		fclose(stream);
		close(pipe_fds[ 1 ]);
	} else {
		ERROR("Failed to create child process, err=%d\n", pid);
		close(pipe_fds[ 0 ]);
		close(pipe_fds[ 1 ]);
		return rt;
	}

	return 0;
}


