
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "headers.h"
#include "error_handling.h"
#include "printing.h"

void parse_LFH(FILE* file) {
	// from version to extra_field_len there are 26 bytes
	uint8_t bytes[26];
	sure_fread(bytes, sizeof(bytes), file);

	LFH* lfh = (LFH*) sure_malloc(sizeof(LFH));

	lfh->signature         = LFH_SIGNATURE;
	lfh->version           = *(uint16_t *)bytes;
	lfh->flags             = *(uint16_t *)(bytes+2);
	lfh->compression       = *(uint16_t *)(bytes+4);
	lfh->mod_time          = *(uint16_t *)(bytes+6);
	lfh->mod_date          = *(uint16_t *)(bytes+8);
	lfh->CRC               = *(uint32_t *)(bytes+10);
	lfh->compressed_size   = *(uint32_t *)(bytes+14);
	lfh->uncompressed_size = *(uint32_t *)(bytes+18);
	lfh->filename_len      = *(uint16_t *)(bytes+22);
	lfh->extra_field_len   = *(uint16_t *)(bytes+24);

	lfh->filename = (uint8_t*) sure_malloc(lfh->filename_len);
	sure_fread(lfh->filename, lfh->filename_len, file);

	lfh->extra_field = (uint8_t*) sure_malloc(lfh->extra_field_len);
	sure_fread(lfh->extra_field, lfh->extra_field_len, file);

	// TODO check encryption or data descriptor flag are not set

	// read compressed data
	lfh->compressed_data = (uint8_t*) sure_malloc(lfh->compressed_size);
	sure_fread(lfh->compressed_data, lfh->compressed_size, file);


	// temporary debug printf
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


void parse_CDH(FILE* file) {
	// from version_made_by to relative_offset there are 42 bytes
	uint8_t bytes[42];
	size_t c = 0;
	sure_fread(bytes, sizeof(bytes), file);

	CDH* cdh = (CDH*) sure_malloc(sizeof(CDH));
	cdh->signature         = CDH_SIGNATURE;
	cdh->version_made_by   = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->version_extract   = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->flags             = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->compression       = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->mod_time          = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->mod_date          = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->CRC               = *(uint32_t *)(bytes+c); c += sizeof(uint32_t);
	cdh->compressed_size   = *(uint32_t *)(bytes+c); c += sizeof(uint32_t);
	cdh->uncompressed_size = *(uint32_t *)(bytes+c); c += sizeof(uint32_t);
	cdh->filename_len      = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->extra_field_len   = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->comment_len       = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->disk_num          = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->internal_attr     = *(uint16_t *)(bytes+c); c += sizeof(uint16_t);
	cdh->external_attr     = *(uint32_t *)(bytes+c); c += sizeof(uint32_t);
	cdh->relative_offset   = *(uint32_t *)(bytes+c); c += sizeof(uint32_t);
}

void parse_all_LFH(FILE* file, pk_zip* zip) {

}
void parse_all_CDH(FILE* file, pk_zip* zip) {

}

void parse_ECD(FILE* file, pk_zip* zip) {

}


pk_zip* parse_zip(FILE* file) {
	pk_zip* zip = (pk_zip*) sure_malloc(sizeof(pk_zip));
	// TODO
	parse_all_LFH(file, zip);
	parse_all_CDH(file, zip);
	parse_ECD(file, zip);

	return zip;
}

// int parse_block(FILE* file, pk_zip* zip) {

//     // read 4 bytes for the first signature and recognize the block typez
//     uint32_t signature;
//     fread(&signature, sizeof(signature), 1, file);
//     // TODO check the amount read

//     switch(signature) {
//         case LFH_SIGNATURE:
//             parse_LFH(file);
//             return 0;
//         break;
//         case CDH_SIGNATURE:
//             parse_CDH(file);
//             return 0;

//         default:
//             die_with_message("Found unknown signature 0x%08x at offset %08x\n", signature, ftell(file)-4);
//     }

//     return 1;
// }