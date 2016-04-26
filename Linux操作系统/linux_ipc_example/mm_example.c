/*-----------------------------------------------------------------------------
 * Linux IPC example by Michael Yang, April 2008
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>


/*-----------------------------------------------------------------------------
 * Macros
 *---------------------------------------------------------------------------*/
#define ERROR(...)    fprintf( stderr, __VA_ARGS__ )


/*-----------------------------------------------------------------------------
 * Constants
 *---------------------------------------------------------------------------*/
const char    file_name[]   = "./mm_temp";
const size_t  buffer_size   = 1024;

/*-----------------------------------------------------------------------------
 * Implementations
 *---------------------------------------------------------------------------*/
int main(int argc_, char* argv_[])
{
	struct stat fileStat;
	int file_fd;
	char localBuf[ buffer_size ];
	char* mmBuf;
	int rt;

	printf(">>> Memory Mapping Example >>>\n");
	printf("pid=%d\n", getpid());

	printf("File %s is specified\n", file_name);

	if (stat(file_name, &fileStat) < 0) {
		/* The file probably does not exist */
		file_fd = open(file_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		if (file_fd < 0) {
			ERROR("Failed to create file %s, err=%d\n", file_name, file_fd);
			return file_fd;
		}

		memset(localBuf, 0, buffer_size);
		rt = write(file_fd, localBuf, buffer_size);
		if (rt != buffer_size) {
			ERROR("Failed to initialize mapping file, err=%d\n", rt);
			close(file_fd);
			return rt;
		}
	} else {
		file_fd = open(file_name, O_RDWR, S_IRUSR | S_IWUSR);
		if (file_fd < 0) {
			ERROR("Failed to open file %s, err=%d\n", file_name, file_fd);
			return file_fd;
		}
	}

	/* Establish mapping */
	mmBuf = (char*)mmap(NULL,
	                    buffer_size,
	                    PROT_READ | PROT_WRITE,
	                    MAP_SHARED,
	                    file_fd,
	                    0);
	/* Never compare the result against 0 here !!! */
	if ((int)mmBuf == -1) {
		ERROR("Failed to map file, err=%d\n", (int)mmBuf);
		close(file_fd);
		return (int)mmBuf;
	}

	/* file_fd can actually be closed here */
	close(file_fd);

	printf("Mapped file to %p\n", mmBuf);

	for (;;) {
		printf("Enter: ");
		gets(localBuf);

		if (localBuf[ 0 ] == 0) {
			/* Dump the shared memory */
			printf("MM: %s\n", mmBuf);
		} else if (strcmp(localBuf, "exit") == 0) {
			/* Quit the example program */
			break;
		} else {
			/* Copy the data to shared memory */
			strncpy(mmBuf, localBuf, buffer_size);
			mmBuf[ buffer_size - 1 ] = 0;
		}
	}

	printf("Unmapping file %s at %p\n", file_name, mmBuf);
	rt = munmap(mmBuf, buffer_size);
	if (rt < 0) {
		ERROR("Failed to unmap file %s, err=%d\n", file_name, rt);
	}

	//close( file_fd );
	return rt;
}


