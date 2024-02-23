/* The `fstat` function in C and C++ retrieves information about an open
file descriptor and writes it to a provided structure.

### Syntax:
```c
#include <sys/stat.h>
#include <unistd.h>

int fstat(int fd, struct stat *buf);
```

### Parameters:
- `fd`: The file descriptor for the open file.
- `buf`: A pointer to a `struct stat` where the file status information will be stored.

### Return Value:
- On success, `0` is returned.
- On failure, `-1` is returned, and `errno` is set to indicate the error.

### Usage Example: */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	int fd;
	struct stat file_info;

	fd = open("bind.cpp", O_RDONLY);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	if (fstat(fd, &file_info) == -1) {
		perror("fstat");
		exit(EXIT_FAILURE);
	}

	printf("File size: %lld bytes\n", (long long)file_info.st_size);
	printf("File permissions: %o\n", file_info.st_mode & 0777); // Mask for file permissions
	printf("File permissions: %o\n", file_info.st_mode & 0777); // Mask for file permissions

	close(fd);
	return 0;
}

/* This example opens a file, uses `fstat` to retrieve information about the
file (such as its size and permissions), and then prints that information.
The `struct stat` contains various fields like file size, permissions,
timestamps, etc., providing useful details about the file. */

//////////////////////////////////////////////////////////////////////////////////////
//example with steroids, here we go through all the contents of the struct

// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>

// int main() {
// 	int fd;
// 	struct stat file_info;

// 	fd = open("bind.cpp", O_RDONLY);
// 	if (fd == -1) {
// 		perror("open");
// 		exit(EXIT_FAILURE);
// 	}

// 	if (fstat(fd, &file_info) == -1) {
// 		perror("fstat");
// 		exit(EXIT_FAILURE);
// 	}

// 	printf("File ID: %d\n", (int)file_info.st_dev);
// 	printf("File inode: %lu\n", (unsigned long)file_info.st_ino);
// 	printf("File mode: %o\n", file_info.st_mode);
// 	printf("Number of hard links: %lu\n", (unsigned long)file_info.st_nlink);
// 	printf("User ID of owner: %u\n", (unsigned int)file_info.st_uid);
// 	printf("Group ID of owner: %u\n", (unsigned int)file_info.st_gid);
// 	printf("Device ID: %d\n", (int)file_info.st_rdev);
// 	printf("File size: %lld bytes\n", (long long)file_info.st_size);
// 	printf("File block size: %ld bytes\n", (long)file_info.st_blksize);
// 	printf("Blocks allocated: %lld\n", (long long)file_info.st_blocks);
// 	printf("Last file access: %ld\n", (long)file_info.st_atime);
// 	printf("Last file modification: %ld\n", (long)file_info.st_mtime);
// 	printf("Last status change: %ld\n", (long)file_info.st_ctime);

// 	close(fd);
// 	return 0;
// }


//////////////////////////////////////////////////////////////////////////////////////
