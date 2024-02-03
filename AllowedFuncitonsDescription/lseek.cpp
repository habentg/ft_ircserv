/* The `lseek` function in C and C++ is used to reposition the file
offset of an open file descriptor. It is particularly useful for
moving the read/write pointer within a file.

Here's a brief overview of `lseek`:

### Syntax:
```c
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

### Parameters:
- `fd`: The file descriptor of the file.
- `offset`: The number of bytes to move the file pointer.
- `whence`: The reference point for the offset calculation
(`SEEK_SET`, `SEEK_CUR`, or `SEEK_END`).

### Return Value:
- On success, the new offset in bytes from the beginning of the file.
- On failure, `-1` is returned, and `errno` is set to indicate the error.

### Usage Example: */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	int fd;
	off_t new_offset;

	fd = open("example.txt", O_RDWR); // Open file for read/write
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	new_offset = lseek(fd, 50, SEEK_SET); // Move the file pointer 50 bytes from the start
	if (new_offset == -1) {
		perror("lseek");
		exit(EXIT_FAILURE);
	}

	printf("New file offset: %ld\n", new_offset);

	close(fd);
	return 0;
}

/* This example opens a file, moves the file pointer 50 bytes from the start
using `lseek`, and then prints the new file offset. The `whence` argument
(`SEEK_SET`) specifies that the offset is relative to the beginning of the file.

Remember to handle errors appropriately when using `lseek`, as failures
might occur due to permissions, file type, or other issues. */