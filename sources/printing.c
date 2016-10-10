#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "headers.h"

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

void print_LFH(LFH* lfh) {
	// TODO redo better printing
	printf("lfh->signature %08x\n",lfh->signature);
	printf("lfh->version %08x\n",lfh->version);
	printf("lfh->flags %08x\n",lfh->flags);
	printf("lfh->compression %08x\n",lfh->compression);
	printf("lfh->mod_time %08x\n",lfh->mod_time);
	printf("lfh->mod_date %08x\n",lfh->mod_date);
	printf("lfh->CRC %08x\n",lfh->CRC);
	printf("lfh->compressed_size %08x\n",lfh->compressed_size);
	printf("lfh->uncompressed_size %08x\n",lfh->uncompressed_size);
	printf("lfh->filename_len %08x\n",lfh->filename_len);
	printf("lfh->extra_field_len %08x\n",lfh->extra_field_len);

	printf("lfh->filename:\n");
	print_hex_dump(stdout, lfh->filename, lfh->filename_len);

	printf("lfh->extra_field:\n");
	print_hex_dump(stdout, lfh->extra_field, lfh->extra_field_len);

	printf("lfh->compressed_data:\n");
	print_hex_dump(stdout, lfh->compressed_data, lfh->compressed_size);
}