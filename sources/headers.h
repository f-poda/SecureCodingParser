
#include <stdint.h>


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
