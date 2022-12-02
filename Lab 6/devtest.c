#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	unsigned int result;
	int fd; // file descriptor
	int i, j; // loop variables
	
	char input = 0;

	int read_i, read_j;
	unsigned int* factors = (unsigned int*)malloc(2 * sizeof(int));
	char* outputBuffer = (char*)malloc(3 * sizeof(int));
	
	int* output_int;

	// open device file for reading and writing
	// use "open" to open '/dev/multiplier'
	fd = open("/dev/multiplier", O_RDWR);
	
	// handle error opening file
	if (fd == -1) {
		printf("Failed to open device file!\n");
		return -1;	
	}

	printf("\n");
	
	while (input != 'q') {
		for (i = 0; i <= 16; i++) {
			for (j = 0; j <= 16; j++) {
				// write values to registers using char dev
				factors[0] = i;
				factors[1] = j;
				char* loadBuffer = (char*)factors;

				// use write to write i and j to peripheral
				write(fd, loadBuffer, 2 * sizeof(int));

				// read i, j, and result using char dev
				read(fd, outputBuffer, 3 * sizeof(int));
				output_int = (int*)outputBuffer; // convert to int array

				read_i = output_int[0];
				read_j = output_int[1];
				result = output_int[2];

				// use read to read from peripheral
				// print unsigned ints to screen
				printf("%u * %u = %u\n", read_i, read_j, result);

				// validate result
				if (result == (i * j)) {
					printf("Result correct!\n");
				}
				else {
					printf("Result incorrect\n!");
				}

				// read from terminal
				input = getchar();
			}		
		}
	}
	close(fd);
	free(factors);
	free(outputBuffer);
	return 0;
}
