
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

/* set requet's mime type by web file extension based on mime hash-map. */
mime_t *mime_interp_extension(request *req)
{
  struct stat st;
  mime_t *m;
  char path[PATH_MAX+1];
  char *mime_str;
  int i;

  mime_str = get_mime_type(req->pathname);
  if (0 != strcmp(mime_str, default_type)) {
    for (i = 0; i < MAX_MIME_DEFINITION; i++) {
      m = mime_info(i);
      if ((m->flags & MIMEF_INLINE))
        continue;
      if ((m->flags & MIMEF_ENCODE))
        continue;
      if (0 == strcasecmp(mime_str, m->type)) {
        return (m);
      }
    }
  } else {
    for (i = 0; i < MAX_MIME_DEFINITION; i++) {
      m = mime_info(i);
      if ((m->flags & MIMEF_INLINE))
        continue;
      if (!(m->flags & MIMEF_ENCODE))
        continue;
      sprintf(path, "%s.%s", req->pathname, m->ext);
      if (0 == stat(path, &st)) {
        return (m);
      }
    }
  }

  return (NULL);
}

mime_filter_t *mime_interp_filter(request *req)
{
  char *mime_type;
  int i;

  for (i = 0; i < MAX_MIME_FILTERS; i++) {
    if (!strstr(req->encoding, mime_filter[i].type))
      continue; /* not support by client. */

    mime_type = get_mime_type(req->request_uri);
    if (access_filter_allow(mime_filter[i].type, mime_type)) {
      return (&mime_filter[i]);
    }
  }

  return (NULL);
}

/* process mime-type file handling or indicate a normal HTML file is present. */
int mime_interp_request(request *req)
{
  mime_t *m;
  int i;

  if (req->method == M_HEAD) {
    //    return 0;
  }

  if (!req->mime ||
      0 == strcasecmp(req->mime->type, default_type)) {
    /* process normally. */
    return (0);
  }

  m = req->mime;
  switch (req->method) {
    case M_HEAD:
    case M_GET:
      if (m->get) {
//        mime_interp_head(m, req);
        return (m->get(req));
      }
      break;

    case M_POST:
      if (m->post) {
//        mime_interp_head(m, req);
        return (m->post(req));
      }
      break;

    case M_PUT:
      if (m->put) {
//        mime_interp_head(m, req);
        return (m->put(req));
      }
      break;

    default:
      break;
  }

  return (0); /* not handled */
}

void mime_interp_head(mime_t *mime, request *req)
{

  req->response_status = R_REQUEST_OK;
  if (req->http_version == HTTP09) 
    return;

  if (mime->head) {
    mime->head(req);
  } else {
    req_write(req, http_ver_string(req->http_version));
    req_write(req, " 200 OK" CRLF);
    print_http_headers(req);
    print_content_length(req);
    print_last_modified(req);
    print_content_type(req);
    req_write(req, CRLF);
    req_flush(req);
  }

}

int mime_head_cgi(request *req)
{

    req_write(req, http_ver_string(req->http_version));
    req_write(req, " 200 OK" CRLF);
    print_http_headers(req);
  /* let script write rest of header */
  return (0);
}

int mime_get_cgi(request *req)
{
    req->script_name = strdup(req->request_uri);
    req->cgi_type = EXEC;
    return (init_cgi(req));
} 




