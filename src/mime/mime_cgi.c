
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

#include <libgen.h>
#include "crotalus.h"
#include "php_crotalus.h" /* php/sapi/crotalu */

static char *env_gen_extra(const char *key, const char *value,
                           unsigned int extra);
static void create_argv(request * req, char **aargv);
static int complete_env(request * req);

int verbose_cgi_logs = 0;
/* The +1 is for the the NULL in complete_env */
static char **common_cgi_env = NULL;
short common_cgi_env_count = 0;

/*
 * Name: create_common_env
 *
 * Description: Set up the environment variables that are common to
 * all CGI scripts
 */

void create_common_env(void)
{
    int i;
    common_cgi_env = calloc((COMMON_CGI_COUNT + 1),sizeof(char *));
    common_cgi_env_count = 0;

    if (common_cgi_env == NULL) {
        DIE("unable to allocate memory for common_cgi_env");
    }

    /* NOTE NOTE NOTE:
       If you (the reader) someday modify this chunk of code to
       handle more "common" CGI environment variables, then bump the
       value COMMON_CGI_COUNT in defines.h UP

       Also, in the case of document_root and server_admin, two variables
       that may or may not be defined depending on the way the server
       is configured, we check for null values and use an empty
       string to denote a NULL value to the environment, as per the
       specification. The quote for which follows:

       "In all cases, a missing environment variable is
       equivalent to a zero-length (NULL) value, and vice versa."
     */
    common_cgi_env[common_cgi_env_count++] = env_gen_extra("PATH",
                                         ((cgi_path !=
                                           NULL) ? cgi_path :
                                          DEFAULT_PATH), 0);
    common_cgi_env[common_cgi_env_count++] =
        env_gen_extra("SERVER_SOFTWARE", SERVER_VERSION, 0);
    common_cgi_env[common_cgi_env_count++] = env_gen_extra("SERVER_NAME", server_name, 0);
    common_cgi_env[common_cgi_env_count++] =
        env_gen_extra("GATEWAY_INTERFACE", CGI_VERSION, 0);

    common_cgi_env[common_cgi_env_count++] =
        env_gen_extra("SERVER_PORT", simple_itoa(server_port), 0);

    /* NCSA and APACHE added -- not in CGI spec */
#ifdef USE_NCSA_CGI_ENV
    common_cgi_env[common_cgi_env_count++] =
        env_gen_extra("DOCUMENT_ROOT", crpref_docroot(), 0);

    /* NCSA added */
    common_cgi_env[common_cgi_env_count++] = env_gen_extra("SERVER_ROOT", server_root, 0);
#endif

    /* APACHE added */
    common_cgi_env[common_cgi_env_count++] = env_gen_extra("SERVER_ADMIN", server_admin, 0);
    common_cgi_env[common_cgi_env_count] = NULL;

    /* Sanity checking -- make *sure* the memory got allocated */
    if (common_cgi_env_count != COMMON_CGI_COUNT) {
        log_error_time();
        fprintf(stderr, "COMMON_CGI_COUNT not high enough.\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < common_cgi_env_count; ++i) {
        if (common_cgi_env[i] == NULL) {
            log_error_time();
            fprintf(stderr,
                    "Unable to allocate a component of common_cgi_env - out of memory.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void add_to_common_env(char *key, char *value)
{
    common_cgi_env = realloc(common_cgi_env, (common_cgi_env_count + 2) * (sizeof(char *)));
    if (common_cgi_env== NULL) {
        DIE("Unable to allocate memory for common CGI environment variable.");
    }
    common_cgi_env[common_cgi_env_count] = env_gen_extra(key,value, 0);
    if (common_cgi_env[common_cgi_env_count] == NULL) {
        /* errors already reported */
        DIE("memory allocation failure in add_to_common_env");
    }
    common_cgi_env[++common_cgi_env_count] = NULL;
    /* I find it hard to believe that somebody would actually
     * make 90+ *common* CGI variables, but it's always better
     * to be safe.
     */
    if (common_cgi_env_count > CGI_ENV_MAX) {
        DIE("far too many common CGI environment variables added.");
    }
}

void clear_common_env(void)
{
    int i;

    for (i = 0; i <= COMMON_CGI_COUNT; ++i) {
        if (common_cgi_env[i] != NULL) {
            free(common_cgi_env[i]);
            common_cgi_env[i] = NULL;
        }
    }
}

/*
 * Name: env_gen_extra
 *       (and via a not-so-tricky #define, env_gen)
 * This routine calls malloc: please free the memory when you are done
 */
static char *env_gen_extra(const char *key, const char *value,
                           unsigned int extra)
{
  char *result;
  unsigned int key_len, value_len;

  if (value == NULL)          /* ServerAdmin may not be defined, eg */
    value = "";
  key_len = strlen(key);
  value_len = strlen(value);
  /* leave room for '=' sign and null terminator */
  result = malloc(extra + key_len + value_len + 2);
  if (result) {
    memcpy(result + extra, key, key_len);
    *(result + extra + key_len) = '=';
    memcpy(result + extra + key_len + 1, value, value_len);
    *(result + extra + key_len + value_len + 1) = '\0';
  } else {
    log_error_time();
    perror("malloc");
    log_error_time();
    fprintf(stderr, "tried to allocate (key=value) extra=%u: %s=%s\n",
        extra, key, value);
  }
  return result;
}

/*
 * Name: add_cgi_env
 *
 * Description: adds a variable to CGI's environment
 * Used for HTTP_ headers
 */

int add_cgi_env(request * req, const char *key, const char *value,
                int http_prefix)
{
    char *p;
    unsigned int prefix_len;

    if (http_prefix) {
        prefix_len = 5;
    } else {
        prefix_len = 0;
    }

    if (req->cgi_env_index < CGI_ENV_MAX) {
        p = env_gen_extra(key, value, prefix_len);
        if (!p) {
            log_error_doc(req);
            fprintf(stderr,
                    "Unable to generate additional CGI environment "
                    "variable -- ran out of memory!\n");
            return 0;
        }
        if (prefix_len)
            memcpy(p, "HTTP_", 5);
        req->cgi_env[req->cgi_env_index++] = p;
        return 1;
    }
    log_error_doc(req);
    fprintf(stderr, "Unable to generate additional CGI Environment "
            "variable \"%s%s=%s\" -- not enough space!\n",
            (prefix_len ? "HTTP_" : ""), key, value);
    return 0;
}

#define my_add_cgi_env(req, key, value) { \
    int ok = add_cgi_env(req, key, value, 0); \
    if (!ok) return 0; \
    }

/*
 * Name: complete_env
 *
 * Description: adds the known client header env variables
 * and terminates the environment array
 */

static int complete_env(request * req)
{
    int i;

    for (i = 0; common_cgi_env[i]; i++)
        req->cgi_env[i] = common_cgi_env[i];

    {
        const char *w;
        switch (req->method) {
        case M_POST:
            w = "POST";
            break;
        case M_HEAD:
            w = "HEAD";
            break;
        case M_GET:
            w = "GET";
            break;
        default:
            w = "UNKNOWN";
            break;
        }
        my_add_cgi_env(req, "REQUEST_METHOD", w);
    }

    if (req->header_host)
        my_add_cgi_env(req, "HTTP_HOST", req->header_host);
    my_add_cgi_env(req, "SERVER_ADDR", req->local_ip_addr);
    my_add_cgi_env(req, "SERVER_PROTOCOL",
                   http_ver_string(req->http_version));
    my_add_cgi_env(req, "REQUEST_URI", req->request_uri);

    if (req->path_info)
        my_add_cgi_env(req, "PATH_INFO", req->path_info);

    if (req->path_translated)
        /* while path_translated depends on path_info,
         * there are cases when path_translated might
         * not exist when path_info does
         */
        my_add_cgi_env(req, "PATH_TRANSLATED", req->path_translated);

    my_add_cgi_env(req, "SCRIPT_NAME", req->script_name);

    if (req->query_string)
        my_add_cgi_env(req, "QUERY_STRING", req->query_string);
    my_add_cgi_env(req, "REMOTE_ADDR", req->remote_ip_addr);
    my_add_cgi_env(req, "REMOTE_PORT", simple_itoa(req->remote_port));

    if (req->method == M_POST) {
        if (req->content_type) {
            my_add_cgi_env(req, "CONTENT_TYPE", req->content_type);
        } else {
            my_add_cgi_env(req, "CONTENT_TYPE", default_type);
        }
        if (req->content_length) {
            my_add_cgi_env(req, "CONTENT_LENGTH", req->content_length);
        }
    }
#ifdef ACCEPT_ON
    if (req->accept[0])
        my_add_cgi_env(req, "HTTP_ACCEPT", req->accept);
#endif

    if (req->cgi_env_index < CGI_ENV_MAX + 1) {
        req->cgi_env[req->cgi_env_index] = NULL; /* terminate */
        return 1;
    }
    log_error_doc(req);
    fprintf(stderr, "Not enough space in CGI environment for remainder"
            " of variables.\n");
    return 0;
}

/*
 * Name: make_args_cgi
 *
 * Build argv list for a CGI script according to spec
 *
 */

static void create_argv(request * req, char **aargv)
{
    char *p, *q, *r;
    int aargc;

    q = req->query_string;
    aargv[0] = req->pathname;

    /* here, we handle a special "indexed" query string.
     * Taken from the CGI/1.1 SPEC:
     * This is identified by a GET or HEAD request with a query string
     * with no *unencoded* '=' in it.
     * For such a request, I'm supposed to parse the search string
     * into words, according to the following rules:

     search-string = search-word *( "+" search-word )
     search-word   = 1*schar
     schar         = xunreserved | escaped | xreserved
     xunreserved   = alpha | digit | xsafe | extra
     xsafe         = "$" | "-" | "_" | "."
     xreserved     = ";" | "/" | "?" | ":" | "@" | "&"

     After parsing, each word is URL-decoded, optionally encoded in a system
     defined manner, and then the argument list
     is set to the list of words.


     Thus, schar is alpha|digit|"$"|"-"|"_"|"."|";"|"/"|"?"|":"|"@"|"&"

     As of this writing, escape.pl escapes the following chars:

     "-", "_", ".", "!", "~", "*", "'", "(", ")",
     "0".."9", "A".."Z", "a".."z",
     ";", "/", "?", ":", "@", "&", "=", "+", "\$", ","

     Which therefore means
     "=", "+", "~", "!", "*", "'", "(", ")", ","
     are *not* escaped and should be?
     Wait, we don't do any escaping, and nor should we.
     According to the RFC draft, we unescape and then re-escape
     in a "system defined manner" (here: none).

     The CGI/1.1 draft (03, latest is 1999???) is very unclear here.

     I am using the latest published RFC, 2396, for what does and does
     not need escaping.

     Since boa builds the argument list and does not call /bin/sh,
     (boa uses execve for CGI)
     */

    if (q && !strchr(q, '=')) {
        /* we have an 'index' style */
        q = strdup(q);
        if (!q) {
            log_error_doc(req);
            fputs("unable to strdup 'q' in create_argv!\n", stderr);
            _exit(EXIT_FAILURE);
        }
        for (aargc = 1; q && (aargc < CGI_ARGC_MAX);) {
            r = q;
            /* for an index-style CGI, + is used to separate arguments
             * an escaped '+' is of no concern to us
             */
            if ((p = strchr(q, '+'))) {
                *p = '\0';
                q = p + 1;
            } else {
                q = NULL;
            }
            if (unescape_uri(r, NULL)) {
                /* printf("parameter %d: %s\n",aargc,r); */
                aargv[aargc++] = r;
            }
        }
        aargv[aargc] = NULL;
    } else {
        aargv[1] = NULL;
    }
}

#ifdef HAVE_LIBSHARE
static int php_work_main(work_t *work, int post_data_fd, shbuf_t *resp_buff)
{
  httpd_conn hc;
  struct stat st;
  char path[PATH_MAX+1];
  char buf[256];
  char curpath[PATH_MAX+1];
  char *str;
  off_t of;
  int err;

#ifdef PHP_RUNTIME
  crotalus_php_init();
#endif

memset(&hc, 0, sizeof(hc));

memset(curpath, 0, sizeof(curpath));
getcwd(curpath, sizeof(curpath)-1);

  memset(path, 0, sizeof(path));
  strncpy(path, work->pathname, sizeof(path)-1);
  hc.script_name = basename(path);

  memset(path, 0, sizeof(path));
  strncpy(path, work->pathname, sizeof(path)-1);
  hc.pathname = strdup(work->pathname);//strdup(dirname(path));
//  hc.buff = shbuf_init();
  hc.buff = resp_buff;
  hc.method = work->method;

#if 0
  hc.fd = work->fd;
#endif
  hc.fd = post_data_fd;


//open(work->pathname, O_RDWR);

#if 0
  shbuf_catstr(hc.buff, 
      "<?php\n"
      "echo \"hi\n\";\n"
      "?>\n");
#endif

  err = stat(work->pathname, &st); 
  sprintf(buf, "%u", st.st_size);
  hc.content_length = strdup(buf);

  of = crotalus_php_request(&hc, FALSE);

  /* prepend content-length */
  str = strstr(shbuf_data(hc.buff) + sizeof(uint32_t), "Content-Length:");
  if (str) {
    sprintf(buf, "Content-Length: %-8.8u\r\n", hc.bytes_written);
    strncpy(str, buf, strlen(buf));
  }

// /* DEBUG: */ close(hc.fd);

#ifdef PHP_RUNTIME
  crotalus_php_shutdown();
#endif

  return (0);
}
static int cgi_work_main(int post_data_fd, shbuf_t *buff) /* child */
{
  work_t *work;
  char readbuf[4096];
  char pathname[PATH_MAX-1];
  FILE *pfl;
  uint32_t fd;

  work = (work_t *)shbuf_data(buff);
  fd = (uint32_t)work->fd;

  if (work->cgi_type == PHP) {
    work_t t_work;
    memcpy(&t_work, work, sizeof(t_work));

    /* clear request data */
    shbuf_clear(buff);

    /* return work request fd */
    shbuf_cat(buff, &fd, sizeof(uint32_t));

    return (php_work_main(&t_work, post_data_fd, buff));
  }

  memset(pathname, 0, sizeof(pathname));
  strncpy(pathname, work->pathname, sizeof(pathname)-1);


  if (work->cgi_type == EXEC || work->cgi_type == NPH) {
    char *c;
    unsigned int l;
    char *newpath, *oldpath;
    char *ptr;

    c = strrchr(pathname, '/');
    if (!c) {
      /* there will always be a '.' */
      fprintf(stderr,
          "unable to find '/' in req->pathname: \"%s\"\n",
          pathname);
      return -1;
    }

    *c = '\0';

    if (chdir(pathname) != 0) {
      int saved_errno = errno;
      fprintf(stderr, "Could not chdir to \"%s\":",
          pathname);
      errno = saved_errno;
      perror("chdir");
      return -1;
    }

    ptr = ++c;
    l = strlen(ptr) + 3;
    /* prefix './' */
    newpath = malloc(sizeof (char) * l);
    if (!newpath) {
      /* there will always be a '.' */
      perror("unable to malloc for newpath");
      return -1;
    }
    newpath[0] = '.';
    newpath[1] = '/';
    memcpy(&newpath[2], ptr, l - 2); /* includes the trailing '\0' */
    strncpy(pathname, newpath, sizeof(pathname)-1);
  }

  /* tie post_data_fd to POST stdin */
  if (work->method == M_POST) { /* tie stdin to file */
    lseek(post_data_fd, SEEK_SET, 0);
    dup2(post_data_fd, STDIN_FILENO);
    close(post_data_fd);
  }

  /* pipe contents to return buffer */
  pfl = popen(pathname, "r");
  if (!pfl)
    return (-errno);

  /* clear request data */
  shbuf_clear(buff);

  /* return work request fd */
  shbuf_cat(buff, &fd, sizeof(uint32_t));

  /* return work response */
  memset(readbuf, 0, sizeof(readbuf));
  while (fgets(readbuf, sizeof(readbuf) - 1, pfl)) {
    shbuf_cat(buff, readbuf, strlen(readbuf));
    memset(readbuf, 0, sizeof(readbuf));
  }
  pclose(pfl);

  return (0);
}

static int cgi_work_resp(int err_code, shbuf_t *buff)
{
  struct request *req;
  uint32_t fd;

  if (err_code)
    return 0; /* done */

  memcpy(&fd, shbuf_data(buff), sizeof(uint32_t));
  req = find_request_by_fd(fd);
  shbuf_trim(buff, sizeof(uint32_t));
  if (!req) {
    return 0; /* error */
  }

  if (req->method == M_POST) {
    close(req->post_data_fd); /* child closed it too */
    req->post_data_fd = 0;
  }

  if (!req->buff)
    req->buff = shbuf_init();
  shbuf_append(buff, req->buff);
  req->status = BUFF_READ;

 if (req->cgi_type == EXEC) {
    req->cgi_status = CGI_PARSE; /* got to parse cgi header */
    /* for cgi_header... I get half the buffer! */
    req->header_line = req->header_end =
      (req->buffer + BUFFER_SIZE / 2);
  } else {
    req->cgi_status = CGI_BUFFER;
    /* I get all the buffer! */
    req->header_line = req->header_end = req->buffer;
  }     
  req->filepos = 0;

  return 1;
}

static int init_work_cgi(request * req)
{
  shproc_pool_t *pool;
  shproc_t *proc;
  work_t work;
  char buf[8192];
  int err;

  pool = shproc_pool();
  if (!pool)
    pool = shproc_init(cgi_work_main, cgi_work_resp);

  complete_env(req);

  memset(&work, 0, sizeof(work));
  strncpy(work.pathname, req->pathname, sizeof(work.pathname));
  work.fd = req->fd;
  work.method = req->method;
  work.cgi_status = req->cgi_status;
  work.cgi_type = req->cgi_type;

  if (req->method == M_POST) {
    err = shproc_push(pool, req->post_data_fd, 
        (unsigned char *)&work, sizeof(work)); 
  } else {
    err = shproc_push(pool, 0, (unsigned char *)&work, sizeof(work));
  }
  if (err)
    return (err);

  req->status = BUFF_POLL;
  return (1);
}
#endif

/*
 * Name: init_cgi
 *
 * Description: Called for GET/POST requests that refer to ScriptAlias
 * directories or application/x-httpd-cgi files.  Ties stdout to socket,
 * stdin to data if POST, and execs CGI.
 * stderr remains tied to our log file; is this good?
 *
 * Returns:
 * 0 - error or NPH, either way the socket is closed
 * 1 - success
 */

int init_cgi(request * req)
{
    int child_pid;
    int pipes[2];
    int use_pipes = 0;

    SQUASH_KA(req);

#ifdef HAVE_LIBSHARE
    if (req->cgi_type == WORK ||
        req->cgi_type == PHP) {
      return (init_work_cgi(req)); 
    }
#endif

    if (req->cgi_type) {
        if (complete_env(req) == 0) {
            return 0;
        }
    }
    DEBUG(DEBUG_CGI_ENV) {
        int i;
        for (i = 0; i < req->cgi_env_index; ++i)
            log_error_time();
            fprintf(stderr, "%s - environment variable for cgi: \"%s\"\n",
                    __FILE__, req->cgi_env[i]);
    }

    /* we want to use pipes whenever it's a CGI or directory */
    /* otherwise (NPH, gunzip) we want no pipes */
    if (req->cgi_type == EXEC ||
        (!req->cgi_type &&
         (req->pathname[strlen(req->pathname) - 1] == '/'))) {
        use_pipes = 1;
        if (pipe(pipes) == -1) {
            log_error_doc(req);
            perror("pipe");
            return 0;
        }

        /* set the read end of the socket to non-blocking */
        if (set_nonblock_fd(pipes[0]) == -1) {
            log_error_doc(req);
            perror("cgi-fcntl");
            close(pipes[0]);
            close(pipes[1]);
            return 0;
        }
    }

    child_pid = fork();
    switch (child_pid) {
    case -1:
        /* fork unsuccessful */
        /* FIXME: There is a problem here. send_r_error (called by
         * crotalus_perror) would work for NPH and CGI, but not for GUNZIP.  
         * Fix that. 
         */
        crotalus_perror(req, "fork failed");
        if (use_pipes) {
            close(pipes[0]);
            close(pipes[1]);
        }
        return 0;
        break;
    case 0:
        /* child */
        reset_signals();

        if (req->cgi_type == EXEC || req->cgi_type == NPH) {
            char *c;
            unsigned int l;
            char *newpath, *oldpath;

            c = strrchr(req->pathname, '/');
            if (!c) {
                /* there will always be a '.' */
                log_error_doc(req);
                fprintf(stderr,
                        "unable to find '/' in req->pathname: \"%s\"\n",
                        req->pathname);
                if (use_pipes)
                    close(pipes[1]);
                _exit(EXIT_FAILURE);
            }

            *c = '\0';

            if (chdir(req->pathname) != 0) {
                int saved_errno = errno;
                log_error_doc(req);
                fprintf(stderr, "Could not chdir to \"%s\":",
                        req->pathname);
                errno = saved_errno;
                perror("chdir");
                if (use_pipes)
                    close(pipes[1]);
                _exit(EXIT_FAILURE);
            }

            oldpath = req->pathname;
            req->pathname = ++c;
            l = strlen(req->pathname) + 3;
            /* prefix './' */
            newpath = malloc(sizeof (char) * l);
            if (!newpath) {
                /* there will always be a '.' */
                log_error_doc(req);
                perror("unable to malloc for newpath");
                if (use_pipes)
                    close(pipes[1]);
                _exit(EXIT_FAILURE);
            }
            newpath[0] = '.';
            newpath[1] = '/';
            memcpy(&newpath[2], req->pathname, l - 2); /* includes the trailing '\0' */
            free(oldpath);
            req->pathname = newpath;
        }
        if (use_pipes) {
            /* close the 'read' end of the pipes[] */
            close(pipes[0]);
            /* tie CGI's STDOUT to our write end of pipe */
            if (dup2(pipes[1], STDOUT_FILENO) == -1) {
                log_error_doc(req);
                perror("dup2 - pipes");
                _exit(EXIT_FAILURE);
            }
            close(pipes[1]);
        } else {
            /* tie stdout to socket */
            if (dup2(req->fd, STDOUT_FILENO) == -1) {
                log_error_doc(req);
                perror("dup2 - fd");
                _exit(EXIT_FAILURE);
            }
            close(req->fd);
        }
        /* Switch socket flags back to blocking */
        if (set_block_fd(STDOUT_FILENO) == -1) {
            log_error_doc(req);
            perror("cgi-fcntl");
            _exit(EXIT_FAILURE);
        }
        /* tie post_data_fd to POST stdin */
        if (req->method == M_POST) { /* tie stdin to file */
            lseek(req->post_data_fd, SEEK_SET, 0);
            dup2(req->post_data_fd, STDIN_FILENO);
            close(req->post_data_fd);
        }

#ifdef USE_SETRLIMIT
        /* setrlimit stuff.
         * This is neat!
         * RLIMIT_STACK    max stack size
         * RLIMIT_CORE     max core file size
         * RLIMIT_RSS      max resident set size
         * RLIMIT_NPROC    max number of processes
         * RLIMIT_NOFILE   max number of open files
         * RLIMIT_MEMLOCK  max locked-in-memory address space
         * RLIMIT_AS       address space (virtual memory) limit
         *
         * RLIMIT_CPU      CPU time in seconds
         * RLIMIT_DATA     max data size
         *
         * Currently, we only limit the CPU time and the DATA segment
         * We also "nice" the process.
         *
         * This section of code adapted from patches sent in by Steve Thompson
         * (no email available)
         */

        {
            struct rlimit rl;
            int retval;

            if (cgi_rlimit_cpu) {
                rl.rlim_cur = rl.rlim_max = cgi_rlimit_cpu;
                retval = setrlimit(RLIMIT_CPU, &rl);
                if (retval == -1) {
                    log_error_time();
                    fprintf(stderr,
                            "setrlimit(RLIMIT_CPU,%d): %s\n",
                            rlimit_cpu, strerror(errno));
                    _exit(EXIT_FAILURE);
                }
            }

            if (cgi_limit_data) {
                rl.rlim_cur = rl.rlim_max = cgi_rlimit_data;
                retval = setrlimit(RLIMIT_DATA, &rl);
                if (retval == -1) {
                    log_error_time();
                    fprintf(stderr,
                            "setrlimit(RLIMIT_DATA,%d): %s\n",
                            rlimit_data, strerror(errno));
                    _exit(EXIT_FAILURE);
                }
            }

            if (cgi_nice) {
                retval = nice(cgi_nice);
                if (retval == -1) {
                    log_error_time();
                    perror("nice");
                    _exit(EXIT_FAILURE);
                }
            }
        }
#endif

        umask(cgi_umask);       /* change umask *again* u=rwx,g=rxw,o= */

        /*
         * tie STDERR to cgi_log_fd
         * cgi_log_fd will automatically close, close-on-exec rocks!
         * if we don't tie STDERR (current log_error) to cgi_log_fd,
         *  then we ought to tie it to /dev/null
         *  FIXME: we currently don't tie it to /dev/null, we leave it
         *  tied to whatever 'error_log' points to.  This means CGIs can
         *  scribble on the error_log, probably a bad thing.
         */
        if (cgi_log_fd) {
            dup2(cgi_log_fd, STDERR_FILENO);
        }

        if (req->cgi_type) {
            char *aargv[CGI_ARGC_MAX + 1];
            create_argv(req, aargv);
            execve(req->pathname, aargv, req->cgi_env);
        } else {
            if (req->pathname[strlen(req->pathname) - 1] == '/')
                execl(dirmaker, dirmaker, req->pathname, req->request_uri,
                      (void *) NULL);
#ifdef GUNZIP
            else
                execl(GUNZIP, GUNZIP, "--stdout", "--decompress",
                      req->pathname, (void *) NULL);
#endif
        }
        /* execve failed */
        log_error_doc(req);
        fprintf(stderr, "Unable to execve/execl pathname: \"%s\"",
                req->pathname);
        perror("");
        _exit(EXIT_FAILURE);
        break;

    default:
        /* parent */
        /* if here, fork was successful */
        if (verbose_cgi_logs) {
            log_error_time();
            fprintf(stderr, "Forked child \"%s\" pid %d\n",
                    req->pathname, child_pid);
        }

        if (req->method == M_POST) {
            close(req->post_data_fd); /* child closed it too */
            req->post_data_fd = 0;
        }

        /* NPH, GUNZIP, etc... all go straight to the fd */
        if (!use_pipes)
            return 0;

        close(pipes[1]);
        req->data_fd = pipes[0];

        req->status = PIPE_READ;
        if (req->cgi_type == EXEC) {
            req->cgi_status = CGI_PARSE; /* got to parse cgi header */
            /* for cgi_header... I get half the buffer! */
            req->header_line = req->header_end =
                (req->buffer + BUFFER_SIZE / 2);
        } else {
            req->cgi_status = CGI_BUFFER;
            /* I get all the buffer! */
            req->header_line = req->header_end = req->buffer;
        }

        /* reset req->filepos for logging (it's used in pipe.c) */
        /* still don't know why req->filesize might be reset though */
        req->filepos = 0;
        break;
    }

    return 1;
}

