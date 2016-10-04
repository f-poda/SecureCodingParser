#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "local_file_header.h"

#define SIGNATURE "\x50\x4b\x03\x04"

/**
	Transfroms text representation of number into its decimal value ("0xabcd" -> 43 981).

	number		array with the text representation of the number 
*/
long hex_to_decimal(byte* number) {
	long result = 0;
	int size = strlen(number);

	for (int i = 0; i < size; i++) {
		//Byte shift of each element of the array
		//element is shifted by (size_of_array - i) Bytes where 'i' is place of the element in an array
		result = result | (number[i] << (8 * ((size-1) - i)));
	}
	return result;
}

/**
	Reverses bytes in byte array ("abc" -> "cba").

	buffer		array to be reversed
*/
void reverse_array(byte* buffer) {
	int buffer_len = strlen(buffer);
	for (int i = 0; i < buffer_len/2; i++) {
		byte tmp = buffer[i];
		buffer[i] = buffer[buffer_len - i - 1];
		buffer[buffer_len - i - 1] = tmp;
	}
}

/**
	Reads data from file and reverses order of read bytes. 

	buffer		array to load data into
	length		size fo the buffer
	file		filestream to read from
*/
void read(byte* buffer, int length, FILE* file) {
	fgets(buffer, length, file);
	byte* ptr = strchr(buffer, '\0');
	if ((int)(ptr - buffer + 1) != 2) {
		reverse_array(buffer);
	}
}

/**
	Parses the "Local File Header" part of one ZIP block.

	header		data structure to save data into
	file		filestream to read data from
*/
int parse_header(local_file_header* header, FILE* file) {
	byte* tmp_buffer = (byte*)malloc(sizeof(byte)*5);
	
	fgets(header->signature, 5, file);
	if (strcmp(header->signature, SIGNATURE)) {
		printf("Incorrect signature. Invalid ZIP file.");
		return -3;
	}
	read(header->version, 3, file);
	read(header->flags, 3, file);
	read(header->compression, 3, file);
	read(header->mod_time, 3, file);
	read(header->mod_date, 3, file);
	read(header->CRC, 5, file);

	//reading and transforming header values
	read(tmp_buffer, 5, file);
	header->compressed_size = hex_to_decimal(tmp_buffer);
	read(tmp_buffer, 5, file);
	header->uncompressed_size = hex_to_decimal(tmp_buffer); 
	
	//reallocating buffer size, as we need to parse only 2 Byte values 
	tmp_buffer = (byte*)realloc(tmp_buffer, sizeof(byte) * 3);
	
	read(tmp_buffer, 3, file);
	header->filename_len = hex_to_decimal(tmp_buffer) + 1; 
	read(tmp_buffer, 3, file);
	header->extra_field_len = hex_to_decimal(tmp_buffer) + 1;

	//using parsed field lengths to read variable length fields such as "Filename" and "Extra Field"
	header->filename = (byte*)malloc(sizeof(byte)*header->filename_len);
	fgets(header->filename, header->filename_len, file);
	header->extra_field = (byte*)malloc(sizeof(byte)*header->extra_field_len);
	fgets(header->extra_field, header->extra_field_len, file);
	free(tmp_buffer);
	return 0;
}

/**
	<b>"The data descriptor is only present if bit 3 of the bit flag field is set."</b>
*/
void parse_data_descriptor(data_descriptor* descriptor, FILE* file) {
	read(descriptor->CRC, 5, file);
	read(descriptor->compressed_size, 5, file);
	read(descriptor->uncompressed_size, 5, file);
}

int main(int argc, char* argv[]) {
	int					status = 0;
	local_file_header	header;
	data_descriptor		descriptor;

	if (argc == 2) {
		FILE* file;
		printf("%s", argv[1]);
		status = fopen_s(&file, argv[1], "rb");
		if (!status) {
			status = parse_header(&header, file);

			//The data descriptor is only present if bit 3 of the bit flag field is set.
			if ((header.flags[0] & 0x04 == 0x04) && !header.CRC && !header.compressed_size && !header.uncompressed_size) {
				parse_data_descriptor(&descriptor, file);
			}
			fclose(file);
		}
		else {
			printf("Problems with opening your file.");
		}
	}
	else {
		printf("Incorrect number of arguments! Program should be run only with one argument- path to (PK)ZIP file to be parsed.");
		status = -1;
	}

	getchar();
	return status;
}