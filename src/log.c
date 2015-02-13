
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura
 *  Copyright (C) 1995 Paul Phillips <paulp@go2net.com>
 *  Copyright (C) 1996-1999 Larry Doolittle <ldoolitt@crotalus.org>
 *  Copyright (C) 1999-2004 Jon Nelson <jnelson@crotalus.org>
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

int cgi_log_fd;

/*
 * Prep log directory for writing.
 */
int open_logs(void)
{

  mkdir("/var/log/crotalus/", 0777);
  chown("/var/log/crotalus", server_uid, server_gid);
  cgi_log_fd = -1;

  return (0);
}

/*
 * Name: log_access
 *
 * Description: Writes log data to access_log.
 */

/* NOTES on the commonlog format:
 * Taken from notes on the NetBuddy program
 *  http://www.computer-dynamics.com/commonlog.html

 remotehost

 remotehost rfc931 authuser [date] "request" status bytes

 remotehost - IP of the client
 rfc931 - remote name of the user (always '-')
 authuser - username entered for authentication - almost always '-'
 [date] - the date in [08/Nov/1997:01:05:03 -0600] (with brackets) format
 "request" - literal request from the client (crotalus may clean this up,
   replacing control-characters with '_' perhaps - NOTE: not done)
 status - http status code
 bytes - number of bytes transferred

 crotalus appends:
   referer
   user-agent

 and may prepend (depending on configuration):
 virtualhost - the name or IP (depending on whether name-based
   virtualhosting is enabled) of the host the client accessed
*/

void log_access(request * req)
{
  static FILE *log_fl;
  struct stat st;

  if (!req) {
    if (log_fl ) {
      fclose(log_fl);
      log_fl = NULL;
    }
    return;
  }

  if (!log_fl) {
    char *path = crpref_log_path(CRLOG_ACCESS);
    if (!*path)
      return;

    if (0 == stat(path, &st)) 
      log_fl = fopen(path, "ab");
    else
      log_fl = fopen(path, "wb");
    if (!log_fl)
      return;
  }

  if (virtualhost) {
    fprintf(log_fl, "%s ", req->local_ip_addr);
  } else if (vhost_root) {
    fprintf(log_fl, "%s ", (req->host ? req->host : "(null)"));
  }
  fprintf(log_fl, "%s - - %s\"%s\" %d %ld \"%s\" \"%s\"\n",
      req->remote_ip_addr,
      get_commonlog_time(),
      req->logline ? req->logline : "-",
      req->response_status,
      req->bytes_written,
      (req->header_referer ? req->header_referer : "-"),
      (req->header_user_agent ? req->header_user_agent : "-"));
}

/*
 * Name: log_error_doc
 *
 * Description: Logs the current time and transaction identification
 * to the stderr (the error log):
 * should always be followed by an fprintf to stderr
 *
 * Example output:
 [08/Nov/1997:01:05:03 -0600] request from 192.228.331.232 "GET /~joeblow/dir/ HTTP/1.0" ("/usr/user1/joeblow/public_html/dir/"): write: Broken pipe

 Apache uses:
 [Wed Oct 11 14:32:52 2000] [error] [client 127.0.0.1] client denied by server configuration: /export/home/live/ap/htdocs/test
 */

void log_error_doc(request * req)
{
  char buf[2048];

  memset(buf, 0, sizeof(buf));

  if (virtualhost) {
    sprintf(buf+strlen(buf), "%s ", req->local_ip_addr);
  } else if (vhost_root) {
    sprintf(buf+strlen(buf), "%s ", (req->host ? req->host : "(null)"));
  }
  if (vhost_root) {
    sprintf(buf+strlen(buf), "%s - - %srequest [%s] \"%s\" (\"%s\"): ",
        req->remote_ip_addr,
        get_commonlog_time(),
        (req->header_host ? req->header_host : "(null)"),
        (req->logline ? req->logline : "(null)"),
        (req->pathname ? req->pathname : "(null)"));
  } else {
    sprintf(buf+strlen(buf), "%s - - %srequest \"%s\" (\"%s\"): ",
        req->remote_ip_addr,
        get_commonlog_time(),
        (req->logline ? req->logline : "(null)"),
        (req->pathname ? req->pathname : "(null)"));
  }

  sprintf(buf+strlen(buf), " (%s)\n", strerror(errno));

  log_error(buf);
}

/*
 * Name: crotalus_perror
 *
 * Description: logs an error to user and error file both
 *
 */
void crotalus_perror(request * req, const char *message)
{
    log_error_doc(req);
 //   perror(message);            /* don't need to save errno because log_error_doc does */
    send_r_error(req);
}



/*
 * Name: log_error
 *
 * Description: performs a log_error_time and writes a message to stderr
 *
 */

void log_error(const char *mesg)
{
  static FILE *log_fl;
  struct stat st;

  if (!mesg) {
    if (log_fl ) {
      fclose(log_fl);
      log_fl = NULL;
    }
    return;
  }

  if (!log_fl) {
    char *path = crpref_log_path(CRLOG_ERROR);
    if (!*path)
      return;

    if (0 == stat(path, &st)) 
      log_fl = fopen(path, "ab");
    else
      log_fl = fopen(path, "wb");
    if (!log_fl)
      return;
  }

  fprintf(log_fl, "%s%s\n", get_commonlog_time(), mesg);
}

/*
 * Name: log_error_mesg
 *
 * Description: performs a log_error_time, writes the file and lineno
 * to stderr (saving errno), and then a perror with message
 *
 */
void log_error_mesg(const char *file, int line, const char *func, const char *mesg)
{
  char buf[1024];

  sprintf(buf, "%s:%d (%s) - %s", file, line, func, strerror(errno));
  log_error(buf);
  if (mesg)
    log_error(mesg);
}

