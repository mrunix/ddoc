/*-----------------------------------------------------------------------------
 * Linux IPC example by Michael Yang, April 2008
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


/*-----------------------------------------------------------------------------
 * Macros
 *---------------------------------------------------------------------------*/
#define ERROR(...)    fprintf( stderr, __VA_ARGS__ )


/*-----------------------------------------------------------------------------
 * Constants
 *---------------------------------------------------------------------------*/
const char    fifo_name[]   = "./fifo_temp";
const size_t  buffer_size   = 1024;

/*-----------------------------------------------------------------------------
 * Implementations
 *---------------------------------------------------------------------------*/
int main(int argc_, char* argv_[])
{
	const char* file_name;
	struct stat file_stat;
	int file_fd;
	char buf[ buffer_size ];
	int reader;
	FILE* stream;
	int len;

	printf(">>> FIFO Example >>>\n");
	printf("%s: pid=%d\n", argv_[ 0 ], getpid());

	if(argc_ < 1) {
		printf("argc_ < 1\n");
		exit(0);
	}

	if (strcmp(argv_[ 1 ], "r") == 0) {
		printf("Assuming the role of reader\n");
		reader = 1;
	} else if (strcmp(argv_[ 1 ], "w") == 0){
		printf("Assuming the role of writer\n");
		reader = 0;
	} else {
		printf("Specify the role\n");
		exit(0);
	}

	file_name = fifo_name;
	printf("File %s is specified\n", file_name);

	if (stat(file_name, &file_stat) < 0) {
		/* The file probably does not exist */
		file_fd = mkfifo(file_name, S_IRUSR | S_IWUSR);
		if (file_fd < 0) {
			ERROR("Failed to create fifo %s, err=%d\n", file_name, file_fd);
			return file_fd;
		}
	}
	
	stream = fopen(file_name, (reader ? "r" : "w"));
	if (stream == NULL) {
		ERROR("Failed to open fifo %s\n", file_name);
		return file_fd;
	}

	if (reader) {
		while (fgets(buf, buffer_size, stream)) {
			len = strlen(buf);
			if (len <= 0) {
				continue;
			}

			/* get rid of the tailing ENTER */
			if (buf[ len - 1 ] == '\n') {
				buf[ len - 1 ] = 0;
			}

			printf("%s\n", buf);

			if (strcmp(buf, "exit") == 0) {
				break;
			}
		}
	} else {
		while (printf("Enter: "), gets(buf)) {
			fprintf(stream, "%s\n", buf);
			fflush(stream);
			if (strcmp(buf, "exit") == 0) {
				break;
			}
		}
	}
	fclose(stream);
	return 0;
}
