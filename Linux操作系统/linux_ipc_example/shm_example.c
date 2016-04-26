/*-----------------------------------------------------------------------------
 * Linux IPC example by Michael Yang, April 2008
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>


/*-----------------------------------------------------------------------------
 * Macros
 *---------------------------------------------------------------------------*/
#define ERROR(...)    fprintf( stderr, __VA_ARGS__ )


/*-----------------------------------------------------------------------------
 * Constants
 *---------------------------------------------------------------------------*/
const size_t buffer_size = 1024;


/*-----------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------*/
int CreateShm(size_t size_);
void* AttachShm(int segment_id);
int DetachShm(void* addr_);
int RemoveShm(int segment_id);


/*-----------------------------------------------------------------------------
 * Implementations
 *---------------------------------------------------------------------------*/
int main(int argc_, char* argv_[])
{
	int owner;
	int segment_id;
	char* shm_buf;
	char local_buf[ buffer_size ];
	int rt = 0;

	printf(">>> Shared Memory Example >>>\n");
	printf("pid=%d\n", getpid());

	if (argc_ > 1) {
		owner = 0;
		sscanf(argv_[ 1 ], "%d", &segment_id);
		printf("Shared memory %d was specified\n", segment_id);
	} else {
		/* Create shared memory */
		owner = 1;
		segment_id = CreateShm(buffer_size);
		if (segment_id < 0) {
			ERROR("Failed to create shared memory, err=%d\n", segment_id);
			return segment_id;
		}
		printf("Shared memory %d was created\n", segment_id);
	}

	if (argc_ > 2 && strcmp(argv_[ 2 ], "remove") == 0) {
		printf("Removing shared memory\n");
		rt = RemoveShm(segment_id);
		if (rt < 0) {
			ERROR("Failed to remove shared memory, err=%d\n", rt);
		}
		return rt;
	}

	printf("Attaching to shared memory %d\n", segment_id);
	shm_buf = (char*)AttachShm(segment_id);
	if (shm_buf == NULL) {
		ERROR("Failed to attach to shared memory\n");
		return -1;
	}

	if (owner) {
		memset(shm_buf, 0, buffer_size);
	}

	for (;;) {
		printf("Enter: ");
		gets(local_buf);

		if (local_buf[ 0 ] == 0) {
			/* Dump the shared memory */
			printf("SHM: %s\n", shm_buf);
		} else if (strcmp(local_buf, "exit") == 0) {
			/* Quit the example program */
			break;
		} else {
			/* Copy the data to shared memory */
			strncpy(shm_buf, local_buf, buffer_size);
			shm_buf[ buffer_size - 1 ] = 0;
		}
	}

	printf("Detaching shared memory\n");
	rt = DetachShm(shm_buf);
	if (rt < 0) {
		ERROR("Failed to detach shared memory, err=%d\n", rt);
	}

	if (owner) {
		printf("Removing shared memory\n");
		rt = RemoveShm(segment_id);
		if (rt < 0) {
			ERROR("Failed to remove shared memory, err=%d\n", rt);
		}
	}

	return rt;
}
/*---------------------------------------------------------------------------*/
int CreateShm(size_t size_)
{
	return shmget(IPC_PRIVATE,
	              size_,
	              IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
}
/*---------------------------------------------------------------------------*/
void* AttachShm(int segment_id)
{
	return shmat(segment_id, NULL, 0);
}
/*---------------------------------------------------------------------------*/
int DetachShm(void* addr_)
{
	return shmdt(addr_);
}
/*---------------------------------------------------------------------------*/
int RemoveShm(int segment_id)
{
	return shmctl(segment_id, IPC_RMID, 0);
}


