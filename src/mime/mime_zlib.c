
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

int mime_head_gzip(mime_t *mime, request *req)
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

int mime_get_gzip(mime_t *mime, request *req)
{
  char path[PATH_MAX];
  char buff[2048];
  gzFile fl;
  int r_len;


  sprintf(path, "%s.%s", req->pathname, mime->ext);
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
