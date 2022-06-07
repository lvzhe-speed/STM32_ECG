#ifndef __BSP_LOOP_BUFFER_H
#define __BSP_LOOP_BUFFER_H
#include "sys.h"

typedef int data_type;

typedef struct _loop_buf {
	data_type *buf;
	unsigned int head;
	unsigned int tail;
	unsigned int size;   /**< 4 */
	unsigned int remain; /**< Ê£Óà¿Õ¼ä */
} LOOP_BUFFER;


int loop_buffer_create(LOOP_BUFFER *lbuf,data_type *buf, unsigned int size);
int loop_buffer_write(LOOP_BUFFER *lbuf, data_type *wbuf, unsigned int wlen);
int loop_buffer_read(LOOP_BUFFER *lbuf, data_type *rbuf, unsigned int rlen);
int loop_buffer_free(LOOP_BUFFER *lbuf);
void my_memcpy(data_type *dest,data_type *src,unsigned int len);
#endif

