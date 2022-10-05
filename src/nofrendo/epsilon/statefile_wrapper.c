#include "statefile_wrapper.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <extapp_api.h>
#include "lz4.h"

#define MAX_FILE_SIZE 8192
#define MAX_SCRIPTSTORE_SIZE 4096

extern bool file_exists(const char * filename);
extern bool erase_file(const char * filename);
extern const char * read_file(const char * filename);
extern bool write_file(const char * filename, const char * s, size_t len);

typedef struct {
	char *fd;
	size_t pos;
	int isWrite;
	char name[256];
} statefile_desc_t;

FILE *statefile_fopen(const char *pathname, const char *mode) {
	printf("Wrapper: open %s mode %s\n", pathname, mode);
	statefile_desc_t *s=calloc(sizeof(statefile_desc_t), 1);
	if (!s) goto err;
	strncpy(s->name, pathname, 256);
	if (mode[0]=='r') {
		if (!extapp_fileExists(pathname, EXTAPP_RAM_FILE_SYSTEM)) goto err;
		s->fd = malloc((size_t)MAX_FILE_SIZE);
		size_t len = 0;
		const char *compressed_data = extapp_fileRead(pathname, &len, EXTAPP_RAM_FILE_SYSTEM);
		const int decompressed_size = LZ4_decompress_safe(compressed_data, s->fd,
			len, MAX_FILE_SIZE);
		if(decompressed_size <= 0) {
			free(s->fd);
			goto err;
		}
	} else if (mode[0]=='w') {
		extapp_fileErase(pathname, EXTAPP_RAM_FILE_SYSTEM);
		s->fd=calloc(MAX_FILE_SIZE, 1);
		s->isWrite=1;
	} else {
		goto err;
	}
	return (FILE*)s;
err:
	printf("Wrapper: open failed\n");
	free(s);
	return NULL;
}

int statefile_fclose(FILE *stream) {
	statefile_desc_t *s=(statefile_desc_t*)stream;
	if (s->isWrite) {
		char* compressed_data = malloc((size_t) MAX_SCRIPTSTORE_SIZE);
		const int compressed_data_size = LZ4_compress_default(s->fd, compressed_data,
			MAX_FILE_SIZE, MAX_SCRIPTSTORE_SIZE);
		if(compressed_data_size > 0) {
			printf("We successfully compressed data: %d\n", compressed_data_size);
			extapp_fileWrite(s->name, compressed_data, compressed_data_size, EXTAPP_RAM_FILE_SYSTEM);
		}
		free(compressed_data);
	}
	free(s->fd);
	free(s);
	return 0;
}

size_t statefile_fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	statefile_desc_t *s=(statefile_desc_t*)stream;
	printf("Wrapper: pos %zd reading %zd\n", s->pos, size*nmemb);
	if (size*nmemb==0) return nmemb;
	memcpy(ptr, s->fd + s->pos, size*nmemb);
	s->pos+=(size*nmemb);
	return nmemb;
}

size_t statefile_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	statefile_desc_t *s=(statefile_desc_t*)stream;
	printf("Wrapper: pos %zd writing %zd\n", s->pos, size*nmemb);
	if (size*nmemb==0) return nmemb;
	if (s->pos + size*nmemb > MAX_FILE_SIZE) return 0;
	memcpy(s->fd + s->pos, ptr, size*nmemb);
	s->pos+=(size*nmemb);
	return nmemb;
}

int statefile_fseek(FILE *stream, long offset, int whence) {
	statefile_desc_t *s=(statefile_desc_t*)stream;
	int r=s->pos;
	if (whence==SEEK_SET) {
		s->pos=offset;
	} else if (whence==SEEK_CUR) {
		s->pos+=offset;
	} else if (whence==SEEK_END) {
		abort(); //not implemented
	}
	printf("Wrapper: seek from %d to %zd\n", r, s->pos);
	return 0;
}
