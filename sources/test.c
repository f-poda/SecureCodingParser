
/*
   4.3.1 A ZIP file MUST contain an "end of central directory record". A ZIP
   file containing only an "end of central directory record" is considered an
   empty ZIP file.  Files may be added or replaced within a ZIP file, or deleted.
   A ZIP file MUST have only one "end of central directory record".  Other
   records defined in this specification MAY be used as needed to support
   storage requirements for individual ZIP files.

   4.3.2 Each file placed into a ZIP file MUST be preceeded by  a "local
   file header" record for that file.  Each "local file header" MUST be
   accompanied by a corresponding "central directory header" record within
   the central directory section of the ZIP file.

   4.3.3 Files MAY be stored in arbitrary order within a ZIP file.  A ZIP
   file MAY span multiple volumes or it MAY be split into user-defined
   segment sizes. All values MUST be stored in little-endian byte order unless
   otherwise specified in this document for a specific data element.

   4.3.4 Compression MUST NOT be applied to a "local file header", an "encryption
   header", or an "end of central directory record".  Individual "central
   directory records" must not be compressed, but the aggregate of all central
   directory records MAY be compressed.

   4.3.5 File data MAY be followed by a "data descriptor" for the file.  Data
   descriptors are used to facilitate ZIP file streaming.


   4.3.6 Overall .ZIP file format:

      [local file header 1]
      [encryption header 1]
      [file data 1]
      [data descriptor 1]
      .
      .
      .
      [local file header n]
      [encryption header n]
      [file data n]
      [data descriptor n]
      [archive decryption header]
      [archive extra data record]
      [central directory header 1]
      .
      .
      .
      [central directory header n]
      [zip64 end of central directory record]
      [zip64 end of central directory locator]
      [end of central directory record]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <endian.h>
#include <stdarg.h>



// local file header
#define LFH_SIGNATURE 0x04034b50
typedef struct {
    uint32_t signature;
    uint16_t version;
    uint16_t flags;
    uint16_t compression;
    uint16_t mod_time;
    uint16_t mod_date;
    uint32_t CRC;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_len;
    uint16_t extra_field_len;

    //variable length
    uint8_t* filename;
    uint8_t* extra_field;
    uint8_t* compressed_data;

} LFH;

// central directory header
#define CDH_SIGNATURE 0x02014b50
typedef struct {
    uint32_t signature;
    uint16_t version_made_by;
    uint16_t version_extract;
    uint16_t flags;
    uint16_t compression;
    uint16_t mod_time;
    uint16_t mod_date;
    uint32_t CRC;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_len;
    uint16_t extra_field_len;
    uint16_t comment_len;
    uint16_t disk_num;
    uint16_t internal_attr;
    uint32_t external_attr;
    uint32_t relative_offset;

    //variable length
    uint8_t* filename;
    uint8_t* extra_field;
    uint8_t* comment;
} CDH;



// node for doubly linked list of local file headers
typedef struct LFH_n  {
    LFH*          val;
    struct LFH_n* next;
    struct LFH_n* prev;
} LFH_node;




// end of central directory record
typedef struct {

} CDR_end;


// whole pk zip file
typedef struct {
    LFH_node* LFH_list;
    CDR_end CDR_end;
} pk_zip;

////////////////////////////////////////////////////////////////////////////////
// Error handling functions
////////////////////////////////////////////////////////////////////////////////

void die_with_message(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);

    va_end(args);
}

void die_with_error(char* str) {
    perror(str);
    exit(-1);
}

void sure_fread(void* target, size_t size, FILE *stream) {
    size_t read = fread(target, 1, size, stream);
    if (read != size) {
        die_with_message("Error reading file, read %u of %u bytes\n", read, size);
    }
}

void* sure_malloc(size_t size) {
    void* ris = malloc(size);
    if (ris == NULL && size > 0){
        die_with_error("Cound not allocate buffer");
    }
    return ris;
}

////////////////////////////////////////////////////////////////////////////////
// Printing functions
////////////////////////////////////////////////////////////////////////////////

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

/*
try to parse a block and add it to the current pk_zip structure.

If EOF is encountered unexpectedly, exit with error.
return 1 when EOF is encountered at the beginning, else return 0
*/
int parse_block(FILE* file, pk_zip* zip) {

    // read 4 bytes for the first signature and recognize the block typez
    uint32_t signature;
    fread(&signature, sizeof(signature), 1, file);
    // TODO check the amount read

    switch(signature) {
        case LFH_SIGNATURE:
            parse_LFH(file);
            return 0;
        break;
        case CDH_SIGNATURE:
            parse_CDH(file);
            return 0;

        default:
            die_with_message("Found unknown signature 0x%08x at offset %08x\n", signature, ftell(file)-4);
    }

    return 1;
}

void parse_file(char* path_to_file) {
    FILE*  file;
    pk_zip* zip = (pk_zip*) sure_malloc(sizeof(pk_zip));
    // TODO initialize main structure

    file = fopen(path_to_file, "rb");
    if (!file) die_with_error("Could not open file");
    // TODO check that file is a normal file, not a directory or socket or pipe or ...

    int end = 0;
    while(!end) {
        end = parse_block(file, zip);
    }

    fclose(file);

    // TODO free(pk_zip);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        // get executable name from argv[0] or use default if absent
        char* self_name = "zipinfo";
        if (argc > 0) self_name = argv[0];

        printf("Usage: %s path_to_file\n", self_name);
        exit(0);
    }

    parse_file(argv[1]);
    return 0;
}