#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void die_with_message(const char *format, ...) {
	va_list args;
	va_start(args, format);

	vfprintf(stderr, format, args);

	va_end(args);
	exit(-1);
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
