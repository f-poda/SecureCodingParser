#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void print_hex_dump(FILE* stream, uint8_t* buffer, size_t len) {
	for (size_t i = 0; i < len; i += 16) {
		fprintf(stream, "%p: ", buffer+i);
		for (size_t j = 0; j < 16; ++j) {
			if (i+j < len) {
				fprintf(stream, "%02x ", buffer[i+j]);
			} else {
				fprintf(stream, "   ");
			}

			if (j == 7) {
				fprintf(stream, " ");
			}
		}

		fprintf(stream, " |");
		for (size_t j = 0; j < 16 && i+j < len; ++j) {
			if (buffer[i+j] < 32 || buffer[i+j]>126) {
				fprintf(stream, ".");
			} else {
				fprintf(stream, "%c", buffer[i+j]);
			}
		}
		fprintf(stream, "|\n");
	}
}
