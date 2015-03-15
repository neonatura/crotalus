
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
#include "zlib.h"

#define DEFAULT_COMPRESS_LEVEL 3

#ifndef MIN
#define MIN(a,b) (a<b?a:b)
#endif

#define GZIP_SHM_NAME "/Crotalus_filter_zlib"

static char *get_zlib_shm_name(void)
{
  static char ret_buf[256];

  sprintf(ret_buf, "%s.%u", GZIP_SHM_NAME, (unsigned int)getpid());
#ifdef HAVE_LIBPTHREAD
  sprintf(ret_buf+strlen(ret_buf), ".%u", (unsigned int)pthread_self());
#endif
  return (ret_buf);
}

int mime_head_gzip(request *req)
{

  if (req->http_version == HTTP09)
    return;

  req_write(req, http_ver_string(req->http_version));
  req_write(req, " 200 OK-GUNZIP" CRLF);
  print_http_headers(req);

  print_content_type(req);
  print_last_modified(req);

  req_write(req, CRLF);
  req_flush(req);
 
  return (0);
}

int mime_get_gzip(request *req)
{
  char path[PATH_MAX];
  char buff[2048];
  gzFile fl;
  int r_len;

  sprintf(path, "%s.%s", req->pathname, req->mime->ext);
  fl = gzopen(path, "rb");
  if (!fl) {
    send_r_bad_gateway(req);
    return (-1);
  }

  /* http header */
  send_r_request_ok(req);

  while (!gzeof(fl)) {
    memset(buff, 0, sizeof(buff));
    r_len = gzread(fl, buff, sizeof(buff));
    if (r_len < 1)
      break;

    /* buffer is flushed by send_r_request_ok() call above. */
    memcpy(req->buffer + req->buffer_end, buff, r_len);
    req->buffer_end += r_len;

    req_flush(req);
  }

  gzclose(fl);
  return (1);
}

int mime_filter_write_chunk(request *req, char *w_buf, size_t buf_len)
{
  size_t w_len;
  size_t w_of;
  char ebuf[8192];
  char buf[256];

  sprintf(buf, "%x%s", (unsigned int)buf_len, CRLF);
  if (write(req->fd, buf, strlen(buf)) < strlen(buf))
    goto error;

  if (buf_len) {
    w_of = 0;
    while (w_of < buf_len) {
      w_len = write(req->fd, w_buf + w_of, buf_len - w_of);
      if (w_len < 0)
        goto error;

      w_of += w_len;
    }
  }

  sprintf(buf, "%s", CRLF);
  if (write(req->fd, buf, strlen(buf)) < strlen(buf))
    return (-1);

  return (0);

error:
  sprintf(ebuf, "mime_filter_write_sock: write '%s': %s [errno %d].", req->pathname, strerror(errno), errno);
  WARN(ebuf);

  return (-2);
}

int mime_filter_write_mem(request *req, int fd, int *of_p)
{
  char buf[BUFFER_SIZE];
  int of = *of_p;
  int len;
  int w_len;

  len = lseek(fd, 0L, SEEK_CUR);
  if (len == of)
    return (0);

  /* read from memory and write to socket */
  lseek(fd, of, SEEK_SET);
  for (of = 0; of < len; of += BUFFER_SIZE) {
    w_len = read(fd, buf, MIN(BUFFER_SIZE, len - of)); 
    if (w_len < 0)
      return (-1);
    mime_filter_write_chunk(req, buf, w_len);
  } 

  *of_p = len;
  return (0);
}
  

int mime_filter_init_deflate(request *req)
{
  int err;

  memset(&req->zlib, 0, sizeof(req->zlib));
  err = deflateInit(&req->zlib, DEFAULT_COMPRESS_LEVEL);
  if (err != Z_OK)
    return (-1);

  return (0);
}

int mime_filter_write_deflate(request *req)
{
  char out_buf[BUFFER_SIZE*2];
  char hex_buf[256];
  size_t w_len;
  unsigned int len;
  int ret;

  if (req->buffer_start == req->buffer_end)
    return (0);

  w_len = (req->buffer_end - req->buffer_start);
  req->zlib.avail_in = w_len;
  req->zlib.next_in = req->buffer + req->buffer_start;

  do {
    memset(out_buf, 0, sizeof(out_buf));
    req->zlib.avail_out = sizeof(out_buf);
    req->zlib.next_out = out_buf;
    ret = deflate(&req->zlib, 0);
    if (ret == Z_STREAM_ERROR) {
      char ebuf[8192];

      sprintf(ebuf, "mime_filter_write_deflate: deflate '%s': %s [zerr %d, errno %d].", req->pathname, strerror(errno), ret, errno);
      WARN(ebuf);

      return (-2);
    }

    len = sizeof(out_buf) - req->zlib.avail_out;
    mime_filter_write_chunk(req, out_buf, len);
  } while (req->zlib.avail_out == 0);
  /* req->zlib.avail_in == 0 */

  return (w_len);
}

int mime_filter_flush_deflate(request *req)
{
  char out_buf[BUFFER_SIZE*2];
  char hex_buf[256];
  size_t out_len;
  unsigned int len;
  int ret;

  /* obtain deflate suffix */
  req->zlib.avail_in = 0; 
  req->zlib.next_in = req->buffer;

  do {
    memset(out_buf, 0, sizeof(out_buf));
    req->zlib.avail_out = sizeof(out_buf);
    req->zlib.next_out = out_buf;
    ret = deflate(&req->zlib, Z_FINISH);
    if (ret == Z_STREAM_ERROR) {
      char ebuf[8192];

      sprintf(ebuf, "mime_filter_flush_deflate: deflate '%s': %s [zerr %d, errno %d].", req->pathname, strerror(errno), ret, errno);
      WARN(ebuf);

      return (-2);
    }

    len = sizeof(out_buf) - req->zlib.avail_out;
    mime_filter_write_chunk(req, out_buf, len);
  } while (req->zlib.avail_out == 0);

  /* terminating sequence */
  mime_filter_write_chunk(req, NULL, 0);

  deflateEnd(&req->zlib);

  return (0);
}


int mime_filter_init_gzip(request *req)
{
  char ebuf[8192];
  int fd;

  req->gzip_of = 0;
  req->gzip_fl = NULL;

  req->gzip_fd = shm_open(get_zlib_shm_name(), O_RDWR | O_CREAT | O_TRUNC, 0777);
  if (req->gzip_fd == -1) {
    sprintf(ebuf, "mime_filter_init_gzip: shm_open: %s [errno %d].", strerror(errno), errno);
    WARN(ebuf);
    return (-1);
  }

  req->gzip_fl = gzdopen(req->gzip_fd, "w+");
  if (!req->gzip_fl) {
    sprintf(ebuf, "mime_filter_init_gzip: gzdopen '%s': %s [errno %d].", req->pathname, strerror(errno), errno);
    WARN(ebuf);
    close(req->gzip_fd);
    req->gzip_fd = -1;
    shm_unlink(get_zlib_shm_name());
    return (-1);
  }

  return (0);
}

int mime_filter_write_gzip(request *req)
{
  char ebuf[8192];
  int err;
  size_t len;

  if (!req->gzip_fl)
    return (-2); /* invalid */

  len = req->buffer_end - req->buffer_start;
  err = gzwrite(req->gzip_fl, req->buffer + req->buffer_start, len);
  if (err < 0) {
    char ebuf[8192];

    sprintf(ebuf, "mime_filter_write_gzip: gzwrite '%s': %s [zerr %d, errno %d].", req->pathname, gzerror(req->gzip_fl, err), err, errno); 
    WARN(ebuf);

    return (-2);
  }

  if (err > 0)
    mime_filter_write_mem(req, req->gzip_fd, &req->gzip_of);

  return (err);
}

int mime_filter_flush_gzip(request *req)
{
  char ebuf[8192];
  size_t of;
  int err;

  if (!req->gzip_fl)
    return (-2); /* invalid */

  err = gzflush(req->gzip_fl, Z_FINISH);
  if (err) {
    sprintf(ebuf, "mime_filter_flush_gzip: gzflush: %s [zerr %d, errno %d].", gzerror(req->gzip_fl, err), err, errno);
    WARN(ebuf);
    gzclose(req->gzip_fl);
    req->gzip_fl = NULL;
    shm_unlink(get_zlib_shm_name());
    return (-1);
  }

  /* write trailing content to socket */
  mime_filter_write_mem(req, req->gzip_fd, &req->gzip_of);
  mime_filter_write_chunk(req, NULL, 0);

  /* free resources */
  gzclose(req->gzip_fl);
  shm_unlink(get_zlib_shm_name());

  return (0);
}

