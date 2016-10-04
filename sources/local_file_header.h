#ifndef TYPES
#define TYPES

typedef unsigned char byte;

typedef struct {
	//added one byte to each field for terminating zero
	byte signature[5];			//always is \x50\x4b\x03\x04
	byte version[3];
	byte flags[3];
	byte compression[3];
	byte mod_time[3];
	byte mod_date[3];
	byte CRC[5];
	long compressed_size;
	long uncompressed_size;
	long filename_len;
	long extra_field_len;
	byte* filename;				//variable length
	byte* extra_field;			//variable length
}local_file_header;


typedef struct {
	byte CRC[5];
	byte compressed_size[5];
	byte uncompressed_size[5];
}data_descriptor;
#endif