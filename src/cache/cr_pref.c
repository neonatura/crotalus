
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

static hash_struct *pref_hashtable[MIME_HASHTABLE_SIZE];
static time_t pref_stamp;

void reset_pref_stamp(void)
{
  pref_stamp = time(NULL);
}

static unsigned _cr_pref_hash(const char *extension)
{
    return cr_hash(extension) % MIME_HASHTABLE_SIZE;
}

int cr_pref_set(const char *token, const char *value)
{
  unsigned int hash;

  hash = _cr_pref_hash(token);
  if (!value || !*value) {
    // DEBUG: hash_remove() .. TODO 
  } else {
    if (hash_insert(pref_hashtable, hash, token, value) == NULL) {
      return (-EINVAL);
}
  }

  pref_stamp = time(NULL);

  return (0);
}


/**
 * Obtain value for preference option.
 * @returns blank string when token not found
 */
char *cr_pref_get(const char *token)
{
    hash_struct *current;
    unsigned int hash;

    /* token[0] *can't* be NIL */
    if (!token || !*token)
      return "";

    hash = _cr_pref_hash(token);
    current = hash_find(pref_hashtable, token, hash);
    if (current) {
      return (current->value);
    }

    return (cr_pref_default(token));
}


typedef struct cr_pref_def_t
{
  char *token;
  char *value;
  char *desc;
} cr_pref_def_t;
static struct cr_pref_def_t _pref_def[CR_PREF_MAX] = {
	{ CRPREF_WEB_PORT, "80", "The port Crotalus runs on.  The default port for http servers is 80. If it is less than 1024, the server must be started as root." },
	{ CRPREF_BIND_ADDR, "", "The Internet address to bind to.  If you leave it out, it takes the behavior before 0.93.17.2, which is to bind to all addresses (INADDR_ANY)." },
	{ CRPREF_PROC_USER, "nobody", "The name or UID the server should run as." },
	{ CRPREF_PROC_GROUP, "nobody", "The group name or GID the server should run as." },
	{ CRPREF_ENV_ADMIN, "", "The email address where server problems should be sent." },
	{ CRPREF_PROC_PID, "/var/run/crotalus.pid", "Where to put the pid of the process." },
	{ CRPREF_ERROR_LOG, "/var/log/crotalus/error_log", "The location of the error log file. If this does not start with /, it is considered relative to the server root." },
	{ CRPREF_ACCESS_LOG, "/var/log/crotalus/access_log", "The location of the access log file. If this does not start with /, it is considered relative to the server root." },
	{ CRPREF_CGI_LOG, "/var/log/crotalus/cgi_log", "The location of the CGI stderr log file. If this does not start with /, it is considered relative to the server root." },
	{ CRPREF_CGI_UMASK, "022", "The CGIumask is set immediately before execution of the CGI." },
	{ CRPREF_TIME_LOCAL, "Off", "Set to \"On\" to use localtime instead of UTC time." },
	{ CRPREF_CGI_VERBOSE, "Off", "It simply notes the start and stop times of cgis in the error log Comment out to disable." },
	{ CRPREF_VHOST, "Off", "Set to \"On\" to enable. Given DocumentRoot /var/www, requests on interface 'A' or IP 'IP-A' # become /var/www/IP-A." },
	{ CRPREF_VHOST_DIR, "", "The root location for all virtually hosted data. Given VHostRoot /var/www, requests to host foo.bar.com, where foo.bar.com is ip a.b.c.d, become /var/www/a.b.c.d/foo.bar.com" },
	{ CRPREF_VHOST_NAME, "default", "Define this in order to have a default hostname when the client does not specify one, if using VirtualHostName. Specify \"default\" in order to use the OS assigned hostname." },
	{ CRPREF_WEB_DIR, "/var/www", "The root directory of the HTML documents." },
	{ CRPREF_USER_DIR, "public_html", "The name of the directory which is appended onto a user's home directory if a ~user request is received." },
	{ CRPREF_PATH_INDEX, "index.html", "Name of the file to use as a pre-written HTML directory index." },
	{ CRPREF_CGI_INDEX, "", "Name of program used to create a directory listing." },
	{ CRPREF_PROC_INDEX, "/var/spool/crotalus/dir", "The on-the-fly indexing of Boa can be used to generate indexes of directories." },
	{ CRPREF_KEEPALIVE_MAX, "1000", "Number of KeepAlive requests to allow per connection. Set to 0 to disable keepalive processing. " },
	{ CRPREF_KEEPALIVE_SPAN, "10", "The seconds to wait before keepalive connection times out." },
	//{ CRPREF_MIME_PATH, "/etc/mime.types", "The file that is used to generate mime type pairs and Content-Type fields." },
  { CRPREF_MIME_DEFAULT, "text/plain", "MIME type used if the file extension is unknown, or there is no file extension." },
	{ CRPREF_ENV_PATH, "/bin:/usr/bin:/usr/local/bin", "The value of the $PATH environment variable given to CGI progs." },
	{ CRPREF_POST_LIMIT, "16000000", "The value of the $PATH environment variable given to CGI progs." },
  { CRPREF_CONF_DIR, "/etc/crotalus", "The directory where Crotalus configuration files reside." },
  { CRPREF_WEB_NAME, "", "The name of this server that should be sent back to clients." },
  { CRPREF_PARENT_INDEX, "Off", "Search recursively for a parent directory when a requested web path is not found. Comment out to disable." }
};


char *cr_pref_default(const char *token)
{
  int i;

  for (i = 0; i < CR_PREF_MAX; i++) {
    if (0 == strcasecmp(_pref_def[i].token, token))
      return (_pref_def[i].value);
  }
 
  return ("");
}

char *cr_pref_token(int idx)
{
  if (idx < 0 || idx >= CR_PREF_MAX)
    return ("Unknown");
  return (_pref_def[idx].token);
}

char *cr_pref_desc(const char *token)
{
  int i;

  for (i = 0; i < CR_PREF_MAX; i++) {
    if (0 == strcasecmp(_pref_def[i].token, token))
      return (_pref_def[i].desc);
  }
 
  return ("");
}

/** The absoluate path to the web's "root" directory used to dispense files. */
char *crpref_docroot(void)
{
  static time_t ret_stamp;
  static char *ret_str;

  if (ret_stamp != pref_stamp || !ret_str) {
    if (ret_str) free(ret_str);
    ret_str = strdup(cr_pref_get(CRPREF_WEB_DIR));
    pref_stamp = ret_stamp;
  }
  if (!*ret_str)
    return (NULL); /* not set */

  return (ret_str);
}

char *crpref_userdir(void)
{
  static time_t ret_stamp;
  static char *ret_str;

  if (ret_stamp != pref_stamp || !ret_str) {
    if (ret_str) free(ret_str);
    ret_str = strdup(cr_pref_get(CRPREF_USER_DIR));
    ret_stamp = pref_stamp;
  }
  if (!*ret_str)
    return (NULL); /* not set */

  return (ret_str);
}

char *crpref_dirindex(void)
{
  static time_t ret_stamp;
  static char *ret_str;

  if (ret_stamp != pref_stamp || !ret_str) {
    if (ret_str) free(ret_str);
    ret_str = strdup(cr_pref_get(CRPREF_PATH_INDEX));
    ret_stamp = pref_stamp;
  }
  if (!*ret_str)
    return (NULL); /* not set */

  return (ret_str);
}

char *crpref_cachedir(void)
{
  static time_t ret_stamp;
  static char *ret_str;

  if (ret_stamp != pref_stamp) {
    if (ret_str) free(ret_str);
    ret_str = strdup(cr_pref_get(CRPREF_PROC_INDEX));
    ret_stamp = pref_stamp;
  }
  if (!*ret_str)
    return (NULL); /* not set */

  return (ret_str);
}

char *crpref_log_path(char *type)
{
  char buf[256];
  sprintf(buf, "%sLog", type);
  return (cr_pref_get(buf));
}

