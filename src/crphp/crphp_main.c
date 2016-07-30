
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

#include "crphp.h"

unsigned int server_port;


int main(int argc, char *argv[])
{
  httpd_conn hc;
  char path[PATH_MAX+1];
  off_t of;
  char buf[256];
const char *php_data = "<php>\n"
  "echo 'hi';\n"
  "<?php>\n";


  memset(path, 0, sizeof(path));
  getcwd(path, sizeof(path));
  hc.pathname = strdup(path);
  hc.script_name = strdup("test.php");
  //hc.script_name = strdup(argv[0]);
  hc.buff = shbuf_init();
  hc.method = M_GET;
  hc.fd = fileno(stdout);
  shbuf_catstr(hc.buff, php_data);
  sprintf(buf, "%u", shbuf_size(hc.buff));
  hc.content_length = strdup(buf);

  crotalus_php_init();

  of = crotalus_php_request(&hc, FALSE);

  crotalus_php_shutdown();
}

const char *http_ver_string(int ver)
{
  switch(ver) {
    case HTTP09:
      return "HTTP/0.9";
      break;
    case HTTP10:
      return "HTTP/1.0";
      break;
    case HTTP11:
      return "HTTP/1.1";
      break;
    default:
      return "HTTP/1.0";
  }
  return NULL;
}

const char *http_method_string(int method)
{
  switch(method) {
    case M_GET:
      return "GET";
    case M_HEAD:
      return "HEAD";
    case M_PUT:
      return "PUT";
    case M_POST:
      return "POST";
    default:
      return "MISC";
  }
}
