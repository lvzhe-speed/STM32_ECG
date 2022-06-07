/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2019
#       All rights reserved
#
#       @author       :lvzhe
#       @mail         :1750895316@qq.com
#       @file         :loop_buffer.c
#       @date         :2020-07-03 20:36
#       @algorithm    :
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bsp_loop_buffer.h"

void my_memcpy(data_type *dest,data_type *src,unsigned int len)
{
	char *d=(char*)dest;
	char *s=(char*)src;
	while(len-->0)
	{
		*d++=*s++;
	}
}


int loop_buffer_create(LOOP_BUFFER *lbuf,data_type *buf, unsigned int size)
{
	lbuf->buf = buf;
	lbuf->head = lbuf->tail = 0;
	lbuf->size = size;
	lbuf->remain = size;

	return 0;
}

int loop_buffer_write(LOOP_BUFFER *lbuf, data_type *wbuf, unsigned int wlen)
{
	int overflow;

	if (wlen > lbuf->remain){
		return 0;
	}
	overflow = lbuf->head + wlen - lbuf->size;
	if (overflow > 0) {
		my_memcpy(lbuf->buf + lbuf->head, wbuf, wlen - overflow);
		my_memcpy(lbuf->buf, wbuf + (wlen - overflow), overflow);
		//TODO: 如果会同时写，则lock
		lbuf->head = overflow;
	} else {
		my_memcpy(lbuf->buf + lbuf->head, wbuf, wlen);
		lbuf->head += wlen;
	}

	//TODO: lock
	lbuf->remain -= wlen;

	return wlen;
}

int loop_buffer_read(LOOP_BUFFER *lbuf, data_type *rbuf, unsigned int rlen)
{
	int overflow;

	if (rlen > (lbuf->size - lbuf->remain)){
		return 0;

	}
	
	overflow = lbuf->tail + rlen - lbuf->size;
	if (overflow > 0) {
		my_memcpy(rbuf, lbuf->buf + lbuf->tail, rlen - overflow);
		my_memcpy(rbuf + (rlen - overflow), lbuf->buf, overflow);
		//TODO: 如果会同时读，则lock
		lbuf->tail = overflow;
	} else {
		my_memcpy(rbuf, lbuf->buf + lbuf->tail, rlen);
		lbuf->tail += rlen;
	}
	//TODO: lock
	lbuf->remain += rlen;

	return rlen;
}

int loop_buffer_free(LOOP_BUFFER *lbuf)
{
	free(lbuf->buf);
	lbuf->buf=NULL;
	lbuf->head = lbuf->tail = 0;

	return 0;
}

