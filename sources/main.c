
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
#include <stdarg.h>

#include "headers.h"
#include "error_handling.h"
#include "printing.h"
#include "parser.h"



void open_and_parse(char* path_to_file) {
	FILE* file = fopen(path_to_file, "rb");
	if (!file) die_with_error("Could not open file");
	// TODO check that file is a normal file, not a directory or socket or pipe or ...

	pk_zip* zip = parse_zip(file);

	// TODO printing
	// TODO free(zip);
	fclose(file);

}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		// get executable name from argv[0] or use default if absent
		char* self_name = "zipinfo";
		if (argc > 0) self_name = argv[0];

		printf("Usage: %s path_to_file\n", self_name);
		exit(0);
	}

	open_and_parse(argv[1]);
	return 0;
}