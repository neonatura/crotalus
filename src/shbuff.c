
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura
 *
 *  This file is part of the Crotalus Web Daemon.
 *        
 *  Crotalus is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version. 
 *
 *  Crotalus is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  @endcopyright
 */

#include "crotalus.h"


/*
 * Name: write_from_buff
 * Description: Writes data previously read from a libshare buffer
 *
 * Return values:
 *  -1: request blocked, move to blocked queue
 *   0: EOF or error, close it down
 *   1: successful write, recycle in ready queue
 */

int write_from_shbuff(request * req)
{
#ifdef HAVE_LIBSHARE
  int bytes_written;
  size_t bytes_to_write = req->header_end - req->header_line;


  if (bytes_to_write == 0) {
    if (req->cgi_status == CGI_DONE)
      return 0;

    req->status = BUFF_READ;
    req->header_end = req->header_line = req->buffer;
    return 1;
  }

  bytes_written = write(req->fd, req->header_line, bytes_to_write);

  if (bytes_written == -1) {
    if (errno == EWOULDBLOCK || errno == EAGAIN)
      return -1;          /* request blocked at the buff level, but keep going */
    else if (errno == EINTR)
      return 1;
    else {
      req->status = DEAD;
      log_error_doc(req);
      return 0;
    }
  }

  req->header_line += bytes_written;
  req->bytes_written += bytes_written;

  /* if there won't be anything to write next time, switch state */
  if ((unsigned) bytes_written == bytes_to_write) {
    req->status = BUFF_READ;
    req->header_end = req->header_line = req->buffer;
  }

  return 1;
#else
  return 0;
#endif
}

/*
 * Name: read_from_buff
 * Description: Reads data from a libshare memory buffer
 *
 * Return values:
 *  -1: request blocked, move to blocked queue
 *   0: EOF or error, close it down
 *   1: successful read, recycle in ready queue
 */
int read_from_shbuff(request * req)
{
#ifdef HAVE_LIBSHARE
  int bytes_read; /* signed */
  unsigned int bytes_to_read; /* unsigned */

  if (!req->buff) {
    return -1; /* keep going */
  }

  bytes_to_read = BUFFER_SIZE - (req->header_end - req->buffer - 1);
  if (bytes_to_read == 0) {   /* buffer full */
    if (req->cgi_status == CGI_PARSE) { /* got+parsed header */
      req->cgi_status = CGI_BUFFER;
      *req->header_end = '\0'; /* points to end of read data */
      return process_cgi_header(req); /* cgi_status will change */
    }
    req->status = BUFF_WRITE;
    return 1;
  }

  bytes_read = MIN(shbuf_size(req->buff), bytes_to_read);
  memcpy(req->header_end, shbuf_data(req->buff), bytes_read);
  shbuf_trim(req->buff, bytes_read);
  *(req->header_end + bytes_read) = '\0';

  if (bytes_read == 0) {      /* eof, write rest of buffer */
    req->status = BUFF_WRITE;
    if (req->cgi_status == CGI_PARSE) { /* hasn't processed header yet */
      req->cgi_status = CGI_DONE;
      *req->header_end = '\0'; /* points to end of read data */
      return process_cgi_header(req); /* cgi_status will change */
    }
    req->cgi_status = CGI_DONE;
    return 1;
  }

  req->header_end += bytes_read;

  if (req->cgi_status != CGI_PARSE)
    return write_from_shbuff(req); /* why not try and flush the buffer now? */
  else {
    char *c, *buf;

    buf = req->header_line;

    c = strstr(buf, "\n\r\n");
    if (c == NULL) {
      c = strstr(buf, "\n\n");
      if (c == NULL) {
        return 1;
      }
    }
    req->cgi_status = CGI_DONE;
    *req->header_end = '\0'; /* points to end of read data */
    return process_cgi_header(req); /* cgi_status will change */
  }

  return 1;
#else
  return 0;
#endif
}

int poll_from_shbuff(struct request *req)
{
#ifdef HAVE_LIBSHARE
  shproc_pool_t *pool;
  
  pool = shproc_pool();
  if (!pool) {
    return 0; /* invalid */
  }

  shproc_poll(pool);
  if (req->buff && shbuf_size(req->buff) != 0) {
    /* data collected -- move to BUFF_READ state */
    req->status = BUFF_READ;
  }

  return 1; /* to continue.. */
#else
  return 0;
#endif
}

