
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
      if ((m->flags & MIMEF_ENCODE))
        continue;
      if (0 == strcasecmp(mime_str, m->type)) {
        return (m);
      }
    }
  } else {
    for (i = 0; i < MAX_MIME_DEFINITION; i++) {
      m = mime_info(i);
      if (!(m->flags & MIMEF_ENCODE))
        continue;
      sprintf(path, "%s.%s", req->pathname, m->ext);
  fprintf(stderr, "DEBUG: seeking encode path '%s'\n", path);
      if (0 == stat(path, &st)) {
  fprintf(stderr, "DEBUG: found encode path '%s'\n", path);
        return (m);
      }
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
    fprintf(stderr, "DEBUG: mime_interp_req: M_HEAD\n");
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
        return (m->get(m, req));
      }
      break;

    case M_POST:
      if (m->post) {
//        mime_interp_head(m, req);
        return (m->post(m, req));
      }
      break;

    case M_PUT:
      if (m->put) {
//        mime_interp_head(m, req);
        return (m->put(m, req));
      }
      break;

    default:
      fprintf(stderr, "DEBUG: mime_interp_req: unknown request method %d\n", req->method);
      break;
  }

  return (0); /* not handled */
}

void mime_interp_head(mime_t *mime, request *req)
{

  req->response_status = R_REQUEST_OK;
  if (req->http_version == HTTP09) 
    return;

fprintf(stderr, "DEBUG: mime_interp_head: type '%s'\n", mime->type);


  if (mime->head) {
    mime->head(mime, req);
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

int mime_head_cgi(mime_t *mime, request *req)
{

    req_write(req, http_ver_string(req->http_version));
    req_write(req, " 200 OK" CRLF);
    print_http_headers(req);
  /* let script write rest of header */
  return (0);
}

int mime_get_cgi(mime_t *mime, request *req)
{
    req->script_name = strdup(req->request_uri);
    req->cgi_type = EXEC;
fprintf(stderr, "DEBUG: mime_interp_request: script_name '%s'\n", req->script_name);
    return (init_cgi(req));
} 




