
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura
 *  Copyright (C) 1999-2005 Larry Doolittle <ldoolitt@boa.org>
 *  Copyright (C) 2000-2005 Jon Nelson <jnelson@boa.org>
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

static char *_default_mime_type_file;

const char *config_file_name;

unsigned int server_port;
uid_t server_uid;
gid_t server_gid;
char *server_root;
char *server_name;
char *server_admin;
char *server_ip;
int virtualhost;
char *vhost_root;
const char *default_vhost;
unsigned max_connections;

char *default_type;
char *default_charset;
char *dirmaker;

const char *tempdir;

unsigned int cgi_umask = 027;

char *pid_file;
char *cgi_path;
int single_post_limit;
int conceal_server_identity = 0;

int ka_timeout;
unsigned int default_timeout;
unsigned int ka_max;

int use_localtime;
int use_parentindex;

#ifdef USE_SETRLIMIT
extern int cgi_rlimit_cpu;      /* boa.c */
extern int cgi_rlimit_data;     /* boa.c */
extern int cgi_nice;            /* boa.c */
#endif

char *access_log_name;
char *error_log_name;
char *cgi_log_name;

/* These are new */
static void c_add_cgi_env(char *v1, char *v2, void *table_ptr);
static void c_set_user(char *v1, char *v2, void *t);
static void c_set_group(char *v1, char *v2, void *t);
static void c_set_string(char *v1, char *v2, void *t);
static void c_set_int(char *v1, char *v2, void *t);
static void c_set_unity(char *v1, char *v2, void *t);
static void c_add_mime_types_file(char *v1, char *v2, void *t);
static void c_add_mime_type(char *v1, char *v2, void *t);
static void c_add_alias(char *v1, char *v2, void *t);
static void c_add_access(char *v1, char *v2, void *t);

struct ccommand {
    const char *name;
    const int type;
    void (*action) (char *, char *, void *);
    void *object;
};

typedef struct ccommand Command;

static void apply_command(Command * p, char *args);
static void trim(char *s);
static void parse(FILE * f);

/* Fakery to keep the value passed to action() a void *,
   see usage in table and c_add_alias() below */
static enum ALIAS script_number = SCRIPTALIAS;
static enum ALIAS redirect_number = REDIRECT;
static enum ALIAS alias_number = ALIAS;
static int access_allow_number = ACCESS_ALLOW;
static int access_deny_number = ACCESS_DENY;
static uid_t current_uid = 0;
static char *def_mime_types_path = "/etc/mime.types";

/* Help keep the table below compact */
#define STMT_NO_ARGS 1
#define STMT_ONE_ARG 2
#define STMT_TWO_ARGS 3

#define S0A STMT_NO_ARGS
#define S1A STMT_ONE_ARG
#define S2A STMT_TWO_ARGS

/* function prototype */
Command *lookup_keyword(char *c);

static void c_set_preference(char *v1, char *v2, void *t)
{
  cr_pref_set(t, v1); 
fprintf(stderr, "DEBUG: c_set_preference[%s] = '%s'\n", t, v1);
}

static void apply_inline_preferences(void)
{
  int i;

reset_pref_stamp();


  server_port = atoi(cr_pref_get(CRPREF_WEB_PORT));
  server_ip = strdup(cr_pref_get(CRPREF_BIND_ADDR));
  c_set_user(cr_pref_get(CRPREF_PROC_USER), NULL, NULL);
  c_set_group(cr_pref_get(CRPREF_PROC_GROUP), NULL, NULL);
  server_admin = strdup(cr_pref_get(CRPREF_ENV_ADMIN));
  if (0 != strcmp(cr_pref_get(CRPREF_TIME_LOCAL), "Off"))
    use_localtime = 1;
  server_name = strdup(cr_pref_get(CRPREF_WEB_NAME));
  if (0 != strcmp(cr_pref_get(CRPREF_CGI_VERBOSE), "Off"))
    verbose_cgi_logs = 1;
  if (0 != strcmp(cr_pref_get(CRPREF_VHOST), "Off"))
    virtualhost = 1;
  

  /*
   * * No 'pre-defined' support is provided for "VHostRoot". * *
   vhost_root = strdup(cr_pref_get(CRPREF_VHOST_DIR));
   */

  default_vhost = strdup(cr_pref_get(CRPREF_VHOST_NAME));

  /*
   * * No 'pre-defined' support is provided for 'DirectoryMaker'. *
   dirmaker = strdup(cr_pref_get(CRPREF_CGI_INDEX));
   */

  pid_file = strdup(cr_pref_get(CRPREF_PROC_PID));
  ka_max = atoi(cr_pref_get(CRPREF_KEEPALIVE_MAX));
  ka_timeout = atoi(cr_pref_get(CRPREF_KEEPALIVE_SPAN));
  c_add_mime_types_file(def_mime_types_path, NULL, NULL);
  default_type = strdup(cr_pref_get(CRPREF_MIME_DEFAULT));
  //default_charset = strdup(cr_pref_get(CRPREF_CHARSET));
  single_post_limit = atoi(cr_pref_get(CRPREF_POST_LIMIT));
  cgi_umask = atoi(cr_pref_get(CRPREF_CGI_UMASK));
  //max_connections = atoi(cr_pref_get(CRPREF_PROC_LIMIT)); 
  add_to_common_env(CRPREF_ENV_PATH, (char *)cr_pref_get(CRPREF_ENV_PATH));

  if (0 != strcmp(cr_pref_get(CRPREF_PARENT_INDEX), "Off"))
    use_parentindex = 1;

  access_log_name = strdup(crpref_log_path(CRLOG_ACCESS));
  error_log_name = strdup(crpref_log_path(CRLOG_ERROR));
  cgi_log_name = strdup(crpref_log_path(CRLOG_CGI));
}

struct ccommand clist[] = {
    {"Port", S1A, c_set_int, &server_port},
    {"Listen", S1A, c_set_string, &server_ip},
    {"BackLog", S1A, c_set_int, &backlog},
    {"User", S1A, c_set_user, NULL},
    {"Group", S1A, c_set_group, NULL},
    {"ServerAdmin", S1A, c_set_string, &server_admin},
    {"ServerRoot", S1A, c_set_string, &server_root},
    {"UseLocaltime", S0A, c_set_unity, &use_localtime},
    {"ErrorLog", S1A, c_set_preference, CRPREF_ERROR_LOG},
    {"AccessLog", S1A, c_set_preference, CRPREF_ACCESS_LOG},
//    {"CgiLog", S1A, c_set_string, &cgi_log_name}, /* compatibility with CGILog */
    {"CGILog", S1A, c_set_preference, CRPREF_CGI_LOG},
    {"VerboseCGILogs", S0A, c_set_unity, &verbose_cgi_logs},
    {"ServerName", S1A, c_set_string, &server_name},
    {"VirtualHost", S0A, c_set_unity, &virtualhost},
    {"VHostRoot", S1A, c_set_string, &vhost_root},
    {"DefaultVHost", S1A, c_set_string, &default_vhost},
    {"DocumentRoot", S1A, c_set_preference, CRPREF_WEB_DIR},
    {"UserDir", S1A, c_set_preference, CRPREF_USER_DIR},
    {"DirectoryIndex", S1A, c_set_preference, CRPREF_PATH_INDEX},
    {"DirectoryMaker", S1A, c_set_string, &dirmaker},
    {"DirectoryCache", S1A, c_set_preference, CRPREF_PROC_INDEX},
    {"PidFile", S1A, c_set_string, &pid_file},
    {"KeepAliveMax", S1A, c_set_int, &ka_max},
    {"KeepAliveTimeout", S1A, c_set_int, &ka_timeout},
    {"DefaultType", S1A, c_set_string, &default_type},
    {"DefaultCharset", S1A, c_set_string, &default_charset},
    {"AddType", S2A, c_add_mime_type, NULL},
    {"ScriptAlias", S2A, c_add_alias, &script_number},
    {"Redirect", S2A, c_add_alias, &redirect_number},
    {"Alias", S2A, c_add_alias, &alias_number},
    {"SinglePostLimit", S1A, c_set_int, &single_post_limit},
    {"CGIPath", S1A, c_set_string, &cgi_path},
    {"CGIumask", S1A, c_set_int, &cgi_umask},
    {"MaxConnections", S1A, c_set_int, &max_connections},
    {"ConcealServerIdentity", S0A, c_set_unity, &conceal_server_identity},
    {"Allow", S1A, c_add_access, &access_allow_number},
    {"Deny", S1A, c_add_access, &access_deny_number},
#ifdef USE_SETRLIMIT
    {"CGIRlimitCpu", S2A, c_set_int, &cgi_rlimit_cpu},
    {"CGIRlimitData", S2A, c_set_int, &cgi_rlimit_data},
    {"CGINice", S2A, c_set_int, &cgi_nice},
#endif
    {"CGIEnv", S2A, c_add_cgi_env, NULL},
    {"ParentIndex", S0A, c_set_unity, &use_parentindex}
};

static void c_add_cgi_env(char *v1, char *v2, void *t)
{
    add_to_common_env(v1, v2);
}

static void c_set_user(char *v1, char *v2, void *t)
{
    struct passwd *passwdbuf;
    char *endptr;
    int i;

    DEBUG(DEBUG_CONFIG) {
        log_error_time();
        printf("User %s = ", v1);
    }
    i = strtol(v1, &endptr, 0);
    if (*v1 != '\0' && *endptr == '\0') {
        server_uid = i;
    } else {
        passwdbuf = getpwnam(v1);
        if (!passwdbuf) {
            log_error_time();
            fprintf(stderr, "No such user: %s\n", v1);
            if (current_uid)
                return;
            exit(EXIT_FAILURE);
        }
        server_uid = passwdbuf->pw_uid;
    }
    DEBUG(DEBUG_CONFIG) {
        printf("%d\n", server_uid);
    }
}

static void c_set_group(char *v1, char *v2, void *t)
{
    struct group *groupbuf;
    char *endptr;
    int i;
    DEBUG(DEBUG_CONFIG) {
        log_error_time();
        printf("Group %s = ", v1);
    }
    i = strtol(v1, &endptr, 0);
    if (*v1 != '\0' && *endptr == '\0') {
        server_gid = i;
    } else {
        groupbuf = getgrnam(v1);
        if (!groupbuf) {
            log_error_time();
            fprintf(stderr, "No such group: %s\n", v1);
            if (current_uid)
                return;
            exit(EXIT_FAILURE);
        }
        server_gid = groupbuf->gr_gid;
    }
    DEBUG(DEBUG_CONFIG) {
        printf("%d\n", server_gid);
    }
}

static void c_set_string(char *v1, char *v2, void *t)
{
    DEBUG(DEBUG_CONFIG) {
        log_error_time();
        printf("Setting pointer %p to string %s ..", t, v1);
    }
    if (t) {
        if (*(char **) t != NULL)
            free(*(char **) t);
        *(char **) t = strdup(v1);
        if (!*(char **) t) {
            DIE("Unable to strdup in c_set_string");
        }
        DEBUG(DEBUG_CONFIG) {
            printf("done.\n");
        }
    } else {
        DEBUG(DEBUG_CONFIG) {
            printf("skipped.\n");
        }
    }
}

static void c_set_int(char *v1, char *v2, void *t)
{
    char *endptr;
    int i;
    DEBUG(DEBUG_CONFIG) {
        log_error_time();
        printf("Setting pointer %p to integer string %s ..", t, v1);
    }
    if (t) {
        i = strtol(v1, &endptr, 0); /* Automatic base 10/16/8 switching */
        if (*v1 != '\0' && *endptr == '\0') {
            *(int *) t = i;
            DEBUG(DEBUG_CONFIG) {
                printf(" Integer converted as %d, done\n", i);
            }
        } else {
            /* XXX should tell line number to user */
            fprintf(stderr, "Error: %s found where integer expected\n",
                    v1);
        }
    } else {
        DEBUG(DEBUG_CONFIG) {
            printf("skipped.\n");
        }
    }
}

static void c_set_unity(char *v1, char *v2, void *t)
{
    DEBUG(DEBUG_CONFIG) {
        log_error_time();
        printf("Setting pointer %p to unity\n", t);
    }
    if (!t)
      return;
    if (v1 && 0 == strcasecmp(v1, "off")) {
      *(int *) t = 0;
    } else {
      *(int *) t = 1;
    }
}

static void c_add_mime_type(char *v1, char *v2, void *t)
{
    add_mime_type(v2, v1);
}

static void _mime_types_file_init(char *path, char *v1)
{
  struct stat st;
  FILE *f;
  int err;

  err = stat(path, &st);
  if (err) {
    /* no configuration file exists */
    err = stat(v1, &st);
    if (!err) {
      char *data = NULL;

      /* copy pre-configured location. */
      f = fopen(v1, "rb");
      if (f) {
        data = (char *)calloc(st.st_size, sizeof(char));
        fread(data, sizeof(char), st.st_size, f);
        fclose(f);
      }

      if (data) {
        f = fopen(path, "wb");
        if (f) {
          fwrite(data, sizeof(char), st.st_size, f);
          fclose(f);
        }
        free(data);
      }
    } else {
      /* use default */
      /* generate default */
      f = fopen(v1, "wb");
      if (f) {
        fwrite(_default_mime_type_file, sizeof(char), strlen(_default_mime_type_file), f); 
        fclose(f);
      }
    }
  }
}

static void c_add_mime_types_file(char *v1, char *v2, void *t)
{
  struct stat st;
  FILE *f;
  char path[PATH_MAX+1];
  char buf[256], *p;
  char *type, *extension, *c2;
  int len;
  int err;

  sprintf(path, "%s/mime.types", server_root);
  _mime_types_file_init(path, v1);

  f = fopen(path, "rb");
  if (!f)
    return;

  while (fgets(buf, 255, f) != NULL) {
    if (buf[0] == '\0' || buf[0] == '#' || buf[0] == '\n')
      continue;

    c2 = strchr(buf, '\n');
    if (c2)
      *c2 = '\0';

    len = strcspn(buf, "\t ");
    if (!len)
      continue;

    buf[len] = '\0';
    type = buf;
    for (p = buf + len + 1; *p; ++p) {
      if (isalnum(*p))
        break;
    }

    for (len = strcspn(p, "\t "); len; len = strcspn(p, "\t ")) {
      p[len] = '\0';
      extension = p;
      add_mime_type(extension, type);
      /* blah blah */
      for (p = p + len + 1; *p; ++p) {
        if (isalnum(*p))
          break;
      }
    }
  }

  fclose(f);
}



static void c_add_alias(char *v1, char *v2, void *t)
{
    DEBUG(DEBUG_CONFIG) {
        log_error_time();
        printf("Calling add_alias with args \"%s\", \"%s\", and %d\n",
               v1, v2, *(int *) t);
    }
    add_alias(v1, v2, *(enum ALIAS *) t);
}

static void c_add_access(char *v1, char *v2, void *t)
{
#ifdef ACCESS_CONTROL
    access_add(server_name, v1, *(int *) t);
#else
    log_error("This version of Boa doesn't support access controls.\n"
        "Please recompile with --enable-access-control.\n");
#endif                          /* ACCESS_CONTROL */
}

struct ccommand *lookup_keyword(char *c)
{
  struct ccommand *p;
    DEBUG(DEBUG_CONFIG) {
        log_error_time();
        printf("Checking string '%s' against keyword list\n", c);
    }
    for (p = clist;
         p < clist + (sizeof (clist) / sizeof (struct ccommand)); p++) {
        if (strcasecmp(c, p->name) == 0)
            return p;
    }
    return NULL;
}

static void apply_command(Command * p, char *args)
{
    char *second;

    switch (p->type) {
    case STMT_NO_ARGS:
        (p->action) (NULL, NULL, p->object);
        break;
    case STMT_ONE_ARG:
        (p->action) (args, NULL, p->object);
        break;
    case STMT_TWO_ARGS:
        /* FIXME: if no 2nd arg exists, we use NULL. Desirable? */
        while (isspace(*args))
            ++args;
        if (*args == '\0') {
            log_error_time();
            fprintf(stderr, "expected at least 1 arg! (%s)\n", p->name);
            exit(EXIT_FAILURE);
        }

        second = args;
        while (!isspace(*second))
            ++second;
        if (*second == '\0') {
            /* nuthin but spaces */
            second = NULL;
        } else {
            *second = '\0';
            ++second;
            while (isspace(*second))
                ++second;
            if (*second == '\0') {
                second = NULL;
            }
        }

        (p->action) (args, second, p->object);
        break;
    default:
        exit(EXIT_FAILURE);
    }
}

static void trim(char *s)
{
    char *c = s + strlen(s) - 1;

    while (isspace(*c) && c > s) {
        *c = '\0';
        --c;
    }
}

static void parse(FILE * f)
{
    char buf[1025], *c;
    Command *p;
    int line = 0;

    current_uid = getuid();
    while (fgets(buf, 1024, f) != NULL) {
        ++line;
        if (buf[0] == '\0' || buf[0] == '#' || buf[0] == '\n')
            continue;

        /* kill the linefeed and any trailing whitespace */
        trim(buf);
        if (buf[0] == '\0')
            continue;

        /* look for multiple arguments */
        c = buf;
        while (!isspace(*c))
            ++c;

        if (*c == '\0') {
            /* no args */
            c = NULL;
        } else {
            /* one or more args */
            *c = '\0';
            ++c;
        }

        p = lookup_keyword(buf);

        if (!p) {
            log_error_time();
            fprintf(stderr, "Line %d: Did not find keyword \"%s\"\n", line,
                    buf);
            exit(EXIT_FAILURE);
        } else {
          DEBUG(DEBUG_CONFIG) {
            log_error_time();
            fprintf(stderr,
                "Found keyword %s in \"%s\" (%s)!\n",
                p->name, buf, c);
          }
          apply_command(p, c);
        }
    }
}

/** Reads config files, then makes sure that all required variables were set properly.  */
void read_config_files(void)
{
  FILE *config;
  char **mime_types;
  char path[PATH_MAX+1];
  int i;

  current_uid = getuid();

  apply_inline_preferences();

  if (!config_file_name) {
    config_file_name = DEFAULT_CONFIG_FILE;
  }

#ifdef ACCESS_CONTROL
  access_init();
#endif                          /* ACCESS_CONTROL */

  sprintf(path, "%s/%s", server_root, config_file_name);
  config = fopen(path, "r");
  if (!config) {
    generate_default_config_file(path);
    config = fopen(path, "r");
    if (!config) {
      fprintf(stderr, "Could not open the Crotalus configuration file '%s'.\n", config_file_name);
      exit(EXIT_FAILURE);
    }
  }
  parse(config);
  fclose(config);

  if (!server_name) {
    struct hostent *he;
    char temp_name[100];

    if (gethostname(temp_name, 100) == -1) {
      perror("gethostname:");
      exit(EXIT_FAILURE);
    }

    he = gethostbyname(temp_name);
    if (he == NULL) {
      perror("gethostbyname:");
      exit(EXIT_FAILURE);
    }

    server_name = strdup(he->h_name);
    if (server_name == NULL) {
      perror("strdup:");
      exit(EXIT_FAILURE);
    }
  }
  tempdir = getenv("TMP");
  if (tempdir == NULL)
    tempdir = "/tmp";

  if (single_post_limit < 0) {
    fprintf(stderr, "Invalid value for single_post_limit: %d\n",
        single_post_limit);
    exit(EXIT_FAILURE);
  }

  if (vhost_root && virtualhost) {
    fprintf(stderr, "Both VHostRoot and VirtualHost were enabled, and "
        "they are mutually exclusive.\n");
    exit(EXIT_FAILURE);
  }

  if (vhost_root && crpref_docroot()) {
    fprintf(stderr,
        "Both VHostRoot and DocumentRoot were enabled, and "
        "they are mutually exclusive.\n");
    exit(EXIT_FAILURE);
  }

  if (!default_vhost) {
    default_vhost = DEFAULT_VHOST;
  }

#ifdef USE_SETRLIMIT
  if (cgi_rlimit_cpu < 0)
    cgi_rlimit_cpu = 0;

  if (cgi_rlimit_data < 0)
    cgi_rlimit_data = 0;

  if (cgi_nice < 0)
    cgi_nice = 0;
#endif

  if (max_connections < 1) {
    struct rlimit rl;
    int c;

    /* has not been set explicitly */
    c = getrlimit(RLIMIT_NOFILE, &rl);
    if (c < 0) {
      DIE("getrlimit");
    }
    max_connections = rl.rlim_cur;
  }
  if (max_connections > FD_SETSIZE - 20)
    max_connections = FD_SETSIZE - 20;

  if (ka_timeout < 0) ka_timeout=0;  /* not worth a message */
  /* save some time */
  default_timeout = (ka_timeout ? ka_timeout : REQUEST_TIMEOUT);
#ifdef HAVE_POLL
  default_timeout *= 1000;
#endif

  if (default_type == NULL) {
    DIE("DefaultType *must* be set!");
  }

#ifdef PHP
  add_mime_type("php", PHP_MIME_TYPE);
#endif
#ifdef CGI
  add_mime_type("cgi", CGI_MIME_TYPE);
#endif

}

void generate_default_config_file(char *path)
{
  FILE *fl;
  char *tok;
  int i;

  fl = fopen(path, "wb");
  if (!fl)
    return;

  fprintf(fl, "## Crotalus Web Daemon v%s\n", PACKAGE_VERSION); 
  fprintf(fl, "\n");
  fprintf(fl, "\n");

  for (i = 0; i < CR_PREF_MAX; i++) {
    tok = cr_pref_token(i);
    fprintf(fl, "## Name: %s\n", tok);
    fprintf(fl, "## %s\n", cr_pref_desc(tok));
    fprintf(fl, "## Default: %s\n", cr_pref_default(tok));
    fprintf(fl, "#%s %s\n", tok, cr_pref_default(tok));
    fprintf(fl, "\n");
    fprintf(fl, "\n");
  }

  fclose(fl);
}

static char *_default_mime_type_file = 
  "# RFC 2045, 2046, 2047, 2048, and 2077.  The Internet media type\n"
  "# registry is at <http://www.iana.org/assignments/media-types/>.\n"
  "\n"
  "# MIME type					Extensions\n"
  "application/andrew-inset			ez\n"
  "application/atom+xml				atom\n"
  "application/atomcat+xml				atomcat\n"
  "application/atomsvc+xml				atomsvc\n"
  "application/auth-policy+xml			apxml\n"
  "application/ccxml+xml				ccxml\n"
  "application/cellml+xml				cellml cml\n"
  "application/cpl+xml				cpl\n"
  "application/davmount+xml			davmount\n"
  "application/dicom				dcm\n"
  "application/dssc+der				dssc\n"
  "application/dssc+xml				xdssc\n"
  "application/dvcs				dvc\n"
  "application/emma+xml				emma\n"
  "application/fastinfoset				finf\n"
  "application/font-tdpfr				pfr\n"
  "application/hyperstudio				stk\n"
  "application/ipfix				ipfix\n"
  "application/json				json\n"
  "application/lost+xml				lostxml\n"
  "application/mac-binhex40			hqx\n"
  "application/marc				mrc\n"
  "application/mathematica				nb ma mb\n"
  "application/mbox				mbox\n"
  "application/mp21				m21 mp21\n"
  "application/msword				doc\n"
  "application/mxf					mxf\n"
  "application/ocsp-request			orq\n"
  "application/ocsp-response			ors\n"
  "application/octet-stream		bin lha lzh exe class so dll img iso\n"
  "application/oda					oda\n"
  "application/oebps-package+xml			opf\n"
  "application/ogg					ogx\n"
  "application/pdf					pdf\n"
  "application/pgp-signature			sig\n"
  "application/pkcs10				p10\n"
  "application/pkcs7-mime				p7m p7c\n"
  "application/pkcs7-signature			p7s\n"
  "application/pkix-cert				cer\n"
  "application/pkix-crl				crl\n"
  "application/pkix-pkipath			pkipath\n"
  "application/pls+xml				pls\n"
  "application/postscript				ai eps ps\n"
  "application/prs.alvestrand.titrax-sheet\n"
  "application/prs.cww				cw cww\n"
  "application/prs.nprend				rnd rct\n"
  "application/rdf+xml				rdf\n"
  "application/reginfo+xml				rif\n"
  "application/relax-ng-compact-syntax		rnc\n"
  "application/resource-lists-diff+xml		rld\n"
  "application/resource-lists+xml			rl\n"
  "application/rls-services+xml			rs\n"
  "application/rtf					rtf\n"
  "application/scvp-cv-request			scq\n"
  "application/scvp-cv-response			scs\n"
  "application/scvp-vp-request			spq\n"
  "application/scvp-vp-response			spp\n"
  "application/sdp					sdp\n"
  "application/sgml-open-catalog			soc\n"
  "application/shf+xml				shf\n"
  "application/sieve				siv sieve\n"
  "application/simple-filter+xml			cl\n"
  "application/smil				smil smi sml\n"
  "application/sparql-query			rq\n"
  "application/sparql-results+xml			srx\n"
  "application/srgs				gram\n"
  "application/srgs+xml				grxml\n"
  "application/ssml+xml				ssml\n"
  "application/timestamp-query			tsq\n"
  "application/timestamp-reply			tsr\n"
  "application/vnd.3gpp.pic-bw-large		plb\n"
  "application/vnd.3gpp.pic-bw-small		psb\n"
  "application/vnd.3gpp.pic-bw-var			pvb\n"
  "application/vnd.3gpp2.sms			sms\n"
  "application/vnd.3gpp2.tcap			tcap\n"
  "application/vnd.3M.Post-it-Notes		pwn\n"
  "application/vnd.accpac.simply.aso		aso\n"
  "application/vnd.accpac.simply.imp		imp\n"
  "application/vnd.acucobol			acu\n"
  "application/vnd.acucorp				atc acutc\n"
  "application/vnd.adobe.partial-upload\n"
  "application/vnd.adobe.xdp+xml			xdp\n"
  "application/vnd.adobe.xfdf			xfdf\n"
  "application/vnd.airzip.filesecure.azf		azf\n"
  "application/vnd.airzip.filesecure.azs		azs\n"
  "application/vnd.americandynamics.acc		acc\n"
  "application/vnd.amiga.ami			ami\n"
  "application/vnd.anser-web-certificate-issue-initiation	cii\n"
  "application/vnd.anser-web-funds-transfer-initiation	fti\n"
  "application/vnd.apple.installer+xml		dist distz pkg mpkg\n"
  "application/vnd.apple.mpegurl			m3u8\n"
  "application/vnd.aristanetworks.swi		swi\n"
  "application/vnd.audiograph			aep\n"
  "application/vnd.autopackage			package\n"
  "application/vnd.blueice.multipass		mpm\n"
  "application/vnd.bluetooth.ep.oob		ep\n"
  "application/vnd.bmi				bmi\n"
  "application/vnd.businessobjects			rep\n"
  "application/vnd.cendio.thinlinc.clientconf	tlclient\n"
  "application/vnd.chemdraw+xml			cdxml\n"
  "application/vnd.chipnuts.karaoke-mmd		mmd\n"
  "application/vnd.cinderella			cdy\n"
  "application/vnd.claymore			cla\n"
  "application/vnd.cloanto.rp9			rp9\n"
  "application/vnd.clonk.c4group			c4g c4d c4f c4p c4u\n"
  "application/vnd.commerce-battelle	ica icf icd ic0 ic1 ic2 ic3 ic4 ic5 ic6 ic7 ic8\n"
  "application/vnd.commonspace			csp cst\n"
  "application/vnd.contact.cmsg			cdbcmsg\n"
  "application/vnd.cosmocaller			cmc\n"
  "application/vnd.crick.clicker			clkx\n"
  "application/vnd.crick.clicker.keyboard		clkk\n"
  "application/vnd.crick.clicker.palette		clkp\n"
  "application/vnd.crick.clicker.template		clkt\n"
  "application/vnd.crick.clicker.wordbank		clkw\n"
  "application/vnd.criticaltools.wbs+xml		wbs\n"
  "application/vnd.ctc-posml			pml\n"
  "application/vnd.cups-ppd			ppd\n"
  "application/vnd.curl				curl\n"
  "application/vnd.data-vision.rdz			rdz\n"
  "application/vnd.denovo.fcselayout-link		fe_launch\n"
  "application/vnd.dir-bi.plate-dl-nosuffix\n"
  "application/vnd.dna				dna\n"
  "application/vnd.dpgraph				dpg mwc dpgraph\n"
  "application/vnd.dreamfactory			dfac\n"
  "application/vnd.dynageo				geo\n"
  "application/vnd.ecowin.chart			mag\n"
  "application/vnd.enliven				nml\n"
  "application/vnd.epson.esf			esf\n"
  "application/vnd.epson.msf			msf\n"
  "application/vnd.epson.quickanime		qam\n"
  "application/vnd.epson.salt			slt\n"
  "application/vnd.epson.ssf			ssf\n"
  "application/vnd.ericsson.quickcall		qcall qca\n"
  "application/vnd.eszigno3+xml			es3 et3\n"
  "application/vnd.ezpix-album			ez2\n"
  "application/vnd.ezpix-package			ez3\n"
  "application/vnd.fdf				fdf\n"
  "application/vnd.fdsn.mseed			msd mseed\n"
  "application/vnd.fsdn.seed			seed dataless\n"
  "application/vnd.FloGraphIt			gph\n"
  "application/vnd.fluxtime.clip			ftc\n"
  "application/vnd.font-fontforge-sfd		sfd\n"
  "application/vnd.framemaker			fm\n"
  "application/vnd.frogans.fnc			fnc\n"
  "application/vnd.frogans.ltf			ltf\n"
  "application/vnd.fsc.weblaunch			fsc\n"
  "application/vnd.fujitsu.oasys			oas\n"
  "application/vnd.fujitsu.oasys2			oa2\n"
  "application/vnd.fujitsu.oasys3			oa3\n"
  "application/vnd.fujitsu.oasysgp			fg5\n"
  "application/vnd.fujitsu.oasysprs		bh2\n"
  "application/vnd.fujixerox.ddd			ddd\n"
  "application/vnd.fujixerox.docuworks		xdw\n"
  "application/vnd.fujixerox.docuworks.binder	xbd\n"
  "application/vnd.fuzzysheet			fzs\n"
  "application/vnd.genomatix.tuxedo		txd\n"
  "application/vnd.geogebra.file			ggb\n"
  "application/vnd.geogebra.tool			ggt\n"
  "application/vnd.geometry-explorer		gex gre\n"
  "application/vnd.geonext				gxt\n"
  "application/vnd.geoplan				g2w\n"
  "application/vnd.geospace			g3w\n"
  "application/vnd.google-earth.kml+xml		kml\n"
  "application/vnd.google-earth.kmz		kmz\n"
  "application/vnd.grafeq				gqf gqs\n"
  "application/vnd.groove-account			gac\n"
  "application/vnd.groove-help			ghf\n"
  "application/vnd.groove-identity-message		gim\n"
  "application/vnd.groove-injector			grv\n"
  "application/vnd.groove-tool-message		gtm\n"
  "application/vnd.groove-tool-template		tpl\n"
  "application/vnd.groove-vcard			vcg\n"
  "application/vnd.HandHeld-Entertainment+xml	zmm\n"
  "application/vnd.hbci				hbci hbc kom upa pkd bpd\n"
  "application/vnd.hhe.lesson-player		les\n"
  "application/vnd.hp-HPGL				hpgl\n"
  "application/vnd.hp-hpid				hpi hpid\n"
  "application/vnd.hp-hps				hps\n"
  "application/vnd.hp-jlyt				jlt\n"
  "application/vnd.hp-PCL				pcl\n"
  "application/vnd.hydrostatix.sof-data		sfd-hdstx\n"
  "application/vnd.hzn-3d-crossword		x3d\n"
  "application/vnd.ibm.electronic-media		emm\n"
  "application/vnd.ibm.MiniPay			mpy\n"
  "application/vnd.ibm.modcap			list3820 listafp afp pseg3820\n"
  "application/vnd.ibm.rights-management		irm\n"
  "application/vnd.ibm.secure-container		sc\n"
  "application/vnd.iccprofile			icc icm\n"
  "application/vnd.igloader			igl\n"
  "application/vnd.immervision-ivp			ivp\n"
  "application/vnd.immervision-ivu			ivu\n"
  "application/vnd.informedcontrol.rms+xml\n"
  "# application/vnd.informix-visionary obsoleted by application/vnd.visionary\n"
  "application/vnd.intercon.formnet		xpw xpx\n"
  "application/vnd.intu.qbo			qbo\n"
  "application/vnd.intu.qfx			qfx\n"
  "application/vnd.ipunplugged.rcprofile		rcprofile\n"
  "application/vnd.irepository.package+xml		irp\n"
  "application/vnd.is-xpr				xpr\n"
  "application/vnd.jam				jam\n"
  "application/vnd.jcp.javame.midlet-rms		rms\n"
  "application/vnd.jisp				jisp\n"
  "application/vnd.joost.joda-archive		joda\n"
  "application/vnd.kahootz				ktz ktr\n"
  "application/vnd.kde.karbon			karbon\n"
  "application/vnd.kde.kchart			chrt\n"
  "application/vnd.kde.kformula			kfo\n"
  "application/vnd.kde.kivio			flw\n"
  "application/vnd.kde.kontour			kon\n"
  "application/vnd.kde.kpresenter			kpr kpt\n"
  "application/vnd.kde.kspread			ksp\n"
  "application/vnd.kde.kword			kwd kwt\n"
  "application/vnd.kenameaapp			htke\n"
  "application/vnd.kidspiration			kia\n"
  "application/vnd.Kinar				kne knp sdf\n"
  "application/vnd.koan				skp skd skm skt\n"
  "application/vnd.kodak-descriptor		sse\n"
  "application/vnd.llamagraphics.life-balance.desktop	lbd\n"
  "application/vnd.llamagraphics.life-balance.exchange+xml	lbe\n"
  "application/vnd.lotus-1-2-3			123 wk4 wk3 wk1\n"
  "application/vnd.lotus-approach			apr vew\n"
  "application/vnd.lotus-freelance			prz pre\n"
  "application/vnd.lotus-notes			nsf ntf ndl ns4 ns3 ns2 nsh nsg\n"
  "application/vnd.lotus-organizer			or3 or2 org\n"
  "application/vnd.lotus-screencam			scm\n"
  "application/vnd.lotus-wordpro			lwp sam\n"
  "application/vnd.macports.portpkg		portpkg\n"
  "application/vnd.marlin.drm.mdcf			mdc\n"
  "application/vnd.mcd				mcd\n"
  "application/vnd.medcalcdata			mc1\n"
  "application/vnd.mediastation.cdkey		cdkey\n"
  "application/vnd.MFER				mwf\n"
  "application/vnd.mfmp				mfm\n"
  "application/vnd.micrografx.flo			flo\n"
  "application/vnd.micrografx.igx			igx\n"
  "application/vnd.mif				mif\n"
  "application/vnd.Mobius.DAF			daf\n"
  "application/vnd.Mobius.DIS			dis\n"
  "application/vnd.Mobius.MBK			mbk\n"
  "application/vnd.Mobius.MQY			mqy\n"
  "application/vnd.Mobius.MSL			msl\n"
  "application/vnd.Mobius.PLC			plc\n"
  "application/vnd.Mobius.TXF			txf\n"
  "application/vnd.mophun.application		mpn\n"
  "application/vnd.mophun.certificate		mpc\n"
  "application/vnd.mozilla.xul+xml			xul\n"
  "application/vnd.ms-artgalry			cil\n"
  "application/vnd.ms-asf				asf\n"
  "application/vnd.ms-cab-compressed		cab\n"
  "application/vnd.ms-excel			xls\n"
  "application/vnd.ms-fontobject			eot\n"
  "application/vnd.ms-htmlhelp			chm\n"
  "application/vnd.ms-ims				ims\n"
  "application/vnd.ms-lrm				lrm\n"
  "application/vnd.ms-powerpoint			ppt\n"
  "application/vnd.ms-project			mpp\n"
  "application/vnd.ms-tnef				tnef tnf\n"
  "application/vnd.ms-works			wcm wdb wks wps\n"
  "application/vnd.ms-wpl				wpl\n"
  "application/vnd.ms-xpsdocument			xps\n"
  "application/vnd.mseq				mseq\n"
  "application/vnd.multiad.creator			crtr\n"
  "application/vnd.multiad.creator.cif		cif\n"
  "application/vnd.musician			mus\n"
  "application/vnd.muvee.style			msty\n"
  "application/vnd.nervana				entity request bkm kcm\n"
  "application/vnd.neurolanguage.nlu		nlu\n"
  "application/vnd.noblenet-directory		nnd\n"
  "application/vnd.noblenet-sealer			nns\n"
  "application/vnd.noblenet-web			nnw\n"
  "application/vnd.nokia.n-gage.ac+xml		ac\n"
  "application/vnd.nokia.n-gage.data		ngdat\n"
  "application/vnd.nokia.n-gage.symbian.install	n-gage\n"
  "application/vnd.nokia.radio-preset		rpst\n"
  "application/vnd.nokia.radio-presets		rpss\n"
  "application/vnd.novadigm.EDM			edm\n"
  "application/vnd.novadigm.EDX			edx\n"
  "application/vnd.novadigm.EXT			ext\n"
  "application/vnd.oasis.opendocument.chart			odc\n"
  "application/vnd.oasis.opendocument.chart-template		otc\n"
  "application/vnd.oasis.opendocument.database			odb\n"
  "application/vnd.oasis.opendocument.formula			odf\n"
  "application/vnd.oasis.opendocument.formula-template		otf\n"
  "application/vnd.oasis.opendocument.graphics			odg\n"
  "application/vnd.oasis.opendocument.graphics-template		otg\n"
  "application/vnd.oasis.opendocument.image			odi\n"
  "application/vnd.oasis.opendocument.image-template		oti\n"
  "application/vnd.oasis.opendocument.presentation			odp\n"
  "application/vnd.oasis.opendocument.presentation-template	otp\n"
  "application/vnd.oasis.opendocument.spreadsheet			ods\n"
  "application/vnd.oasis.opendocument.spreadsheet-template		ots\n"
  "application/vnd.oasis.opendocument.text				odt\n"
  "application/vnd.oasis.opendocument.text-master			odm\n"
  "application/vnd.oasis.opendocument.text-template		ott\n"
  "application/vnd.oasis.opendocument.text-web			oth\n"
  "application/vnd.olpc-sugar			xo\n"
  "application/vnd.oma.dd2+xml			dd2\n"
  "application/vnd.openofficeorg.extension		oxt\n"
  "application/vnd.osa.netdeploy			ndc\n"
  "application/vnd.osgi.dp				dp\n"
  "application/vnd.palm				prc pdb pqa oprc\n"
  "application/vnd.pg.format		    	str\n"
  "application/vnd.pg.osasli			ei6\n"
  "application/vnd.piaccess.application-license	pil\n"
  "application/vnd.picsel				efif\n"
  "application/vnd.pocketlearn			plf\n"
  "application/vnd.powerbuilder6			pbd\n"
  "application/vnd.preminet			preminet\n"
  "application/vnd.previewsystems.box		box vbox\n"
  "application/vnd.proteus.magazine		mgz\n"
  "application/vnd.publishare-delta-tree		qps\n"
  "application/vnd.pvi.ptid1			ptid\n"
  "application/vnd.qualcomm.brew-app-res		bar\n"
  "application/vnd.Quark.QuarkXPress		qxd qxt qwd qwt qxl qxb\n"
  "application/vnd.realvnc.bed			bed\n"
  "application/vnd.recordare.musicxml		mxl\n"
  "application/vnd.route66.link66+xml		link66\n"
  "application/vnd.sailingtracker.track		st\n"
  "application/vnd.scribus				scd sla slaz\n"
  "application/vnd.sealed.3df			s3df\n"
  "application/vnd.sealed.csf			scsf\n"
  "application/vnd.sealed.doc			sdoc sdo s1w\n"
  "application/vnd.sealed.eml			seml sem\n"
  "application/vnd.sealed.mht			smht smh\n"
  "application/vnd.sealed.ppt			sppt s1p\n"
  "application/vnd.sealed.tiff			stif\n"
  "application/vnd.sealed.xls			sxls sxl s1e\n"
  "application/vnd.sealedmedia.softseal.html	stml s1h\n"
  "application/vnd.sealedmedia.softseal.pdf	spdf spd s1a\n"
  "application/vnd.seemail				see\n"
  "application/vnd.sema				sema\n"
  "application/vnd.semd				semd\n"
  "application/vnd.semf				semf\n"
  "application/vnd.shana.informed.formdata		ifm\n"
  "application/vnd.shana.informed.formtemplate	itp\n"
  "application/vnd.shana.informed.interchange	iif\n"
  "application/vnd.shana.informed.package		ipk\n"
  "application/vnd.SimTech-MindMapper		twd twds\n"
  "application/vnd.smaf				mmf\n"
  "application/vnd.smart.teacher			teacher\n"
  "application/vnd.software602.filler.form+xml	fo\n"
  "application/vnd.software602.filler.form-xml-zip	zfo\n"
  "application/vnd.solent.sdkm+xml			sdkm sdkd\n"
  "application/vnd.spotfire.dxp			dxp\n"
  "application/vnd.spotfire.sfs			sfs\n"
  "application/vnd.sun.wadl+xml			wadl\n"
  "application/vnd.sus-calendar			sus susp\n"
  "application/vnd.syncml.dm+wbxml			bdm\n"
  "application/vnd.syncml.dm+xml			xdm\n"
  "application/vnd.syncml+xml			xsm\n"
  "application/vnd.tao.intent-module-archive	tao\n"
  "application/vnd.tmobile-livetv			tmo\n"
  "application/vnd.trid.tpt			tpt\n"
  "application/vnd.triscape.mxs			mxs\n"
  "application/vnd.trueapp				tra\n"
  "application/vnd.ufdl				ufdl ufd frm\n"
  "application/vnd.uiq.theme			utz\n"
  "application/vnd.umajin				umj\n"
  "application/vnd.unity				unityweb\n"
  "application/vnd.uoml+xml			uoml uo\n"
  "application/vnd.vcx				vcx\n"
  "application/vnd.vd-study			mxi study-inter model-inter\n"
  "application/vnd.vectorworks			vwx\n"
  "application/vnd.vidsoft.vidconference		vsc\n"
  "application/vnd.visio				vsd vst vsw vss\n"
  "application/vnd.visionary			vis\n"
  "application/vnd.vsf				vsf\n"
  "application/vnd.wap.sic				sic\n"
  "application/vnd.wap.slc				slc\n"
  "application/vnd.wap.wbxml			wbxml\n"
  "application/vnd.wap.wmlc			wmlc\n"
  "application/vnd.wap.wmlscriptc			wmlsc\n"
  "application/vnd.webturbo			wtb\n"
  "application/vnd.wfa.wsc				wsc\n"
  "application/vnd.wmc				wmc\n"
  "application/vnd.wolfram.player			nbp\n"
  "application/vnd.wordperfect			wpd\n"
  "application/vnd.wqd				wqd\n"
  "application/vnd.wt.stf				stf\n"
  "application/vnd.wv.csp+wbxml			wv\n"
  "application/vnd.xara				xar\n"
  "application/vnd.xfdl				xfdl xfd\n"
  "application/vnd.xmpie.cpkg			cpkg\n"
  "application/vnd.xmpie.dpkg			dpkg\n"
  "application/vnd.xmpie.ppkg			ppkg\n"
  "application/vnd.xmpie.xlim			xlim\n"
  "application/vnd.yamaha.hv-dic			hvd\n"
  "application/vnd.yamaha.hv-script		hvs\n"
  "application/vnd.yamaha.hv-voice			hvp\n"
  "application/vnd.yamaha.openscoreformat		osf\n"
  "application/vnd.yamaha.smaf-audio		saf\n"
  "application/vnd.yamaha.smaf-phrase		spf\n"
  "application/vnd.yellowriver-custom-menu		cmp\n"
  "application/vnd.zul				zir zirz\n"
  "application/vnd.zzazz.deck+xml			zaz\n"
  "application/voicexml+xml			vxml\n"
  "application/watcherinfo+xml			wif\n"
  "application/wsdl+xml				wsdl\n"
  "application/wspolicy+xml			wspolicy\n"
  "application/xcap-att+xml			xav\n"
  "application/xcap-caps+xml			xca\n"
  "application/xcap-el+xml				xel\n"
  "application/xcap-error+xml			xer\n"
  "application/xcap-ns+xml				xns\n"
  "application/xcon-conference-info-diff+xml\n"
  "application/xcon-conference-info+xml\n"
  "application/xhtml+xml				xhtml xhtm xht\n"
  "application/xml-dtd				dtd\n"
  "application/xop+xml				xop\n"
  "application/xslt+xml				xsl xslt\n"
  "application/xv+xml				mxml xhvml xvml xvm\n"
  "application/zip					zip\n"
  "audio/32kadpcm					726\n"
  "audio/ac3					ac3\n"
  "audio/AMR					amr\n"
  "audio/AMR-WB					awb\n"
  "audio/ATRAC-ADVANCED-LOSSLESS			aal\n"
  "audio/ATRAC-X					atx\n"
  "audio/ATRAC3					at3 aa3 omg\n"
  "audio/basic					au snd\n"
  "audio/dls					dls\n"
  "audio/EVRC					evc\n"
  "audio/EVRCB					evb\n"
  "audio/EVRCWB					evw\n"
  "audio/iLBC					lbc\n"
  "audio/L16					l16\n"
  "audio/mobile-xmf				mxmf\n"
  "audio/mpeg					mpga mp1 mp2 mp3\n"
  "audio/ogg					oga ogg spx\n"
  "audio/prs.sid					sid psid\n"
  "audio/qcelp					qcp\n"
  "audio/SMV					smv\n"
  "audio/vnd.audikoz				koz\n"
  "audio/vnd.digital-winds				eol\n"
  "audio/vnd.dolby.mlp				mlp\n"
  "audio/vnd.dts					dts\n"
  "audio/vnd.dts.hd				dtshd\n"
  "audio/vnd.everad.plj				plj\n"
  "audio/vnd.lucent.voice				lvp\n"
  "audio/vnd.ms-playready.media.pya		pya\n"
  "audio/vnd.nortel.vbk				vbk\n"
  "audio/vnd.nuera.ecelp4800			ecelp4800\n"
  "audio/vnd.nuera.ecelp7470			ecelp7470\n"
  "audio/vnd.nuera.ecelp9600			ecelp9600\n"
  "audio/vnd.sealedmedia.softseal.mpeg		smp3 smp s1m\n"
  "image/fits					fits fit fts\n"
  "image/gif					gif\n"
  "image/ief					ief\n"
  "image/jp2					jp2 jpg2\n"
  "image/jpeg					jpeg jpg jpe jfif\n"
  "image/jpm					jpm jpgm\n"
  "image/jpx					jpx jpf\n"
  "image/png					png\n"
  "image/prs.btif					btif btf\n"
  "image/prs.pti					pti\n"
  "image/t38					t38\n"
  "image/tiff					tiff tif\n"
  "image/tiff-fx					tfx\n"
  "image/vnd.adobe.photoshop			psd\n"
  "image/vnd.djvu					djvu djv\n"
  "image/vnd.dxf					dxf\n"
  "image/vnd.fastbidsheet				fbs\n"
  "image/vnd.fpx					fpx\n"
  "image/vnd.fst					fst\n"
  "image/vnd.fujixerox.edmics-mmr			mmr\n"
  "image/vnd.fujixerox.edmics-rlc			rlc\n"
  "image/vnd.globalgraphics.pgb			pgb\n"
  "image/vnd.microsoft.icon			ico\n"
  "image/vnd.ms-modi				mdi\n"
  "image/vnd.radiance				hdr rgbe xyze\n"
  "image/vnd.sealed.png				spng spn s1n\n"
  "image/vnd.sealedmedia.softseal.gif		sgif sgi s1g\n"
  "image/vnd.sealedmedia.softseal.jpg		sjpg sjp s1j\n"
  "image/vnd.wap.wbmp				wbmp\n"
  "image/vnd.xiff					xif\n"
  "message/global					u8msg\n"
  "message/global-delivery-status			u8dsn\n"
  "message/global-disposition-notification		u8mdn\n"
  "message/global-headers				u8hdr\n"
  "message/rfc822					eml mail art\n"
  "model/iges					igs iges\n"
  "model/mesh					msh mesh silo\n"
  "model/vnd.dwf					dwf\n"
  "model/vnd.gdl					gdl gsm win dor lmp rsm msm ism\n"
  "model/vnd.gtw					gtw\n"
  "model/vnd.moml+xml				moml\n"
  "model/vnd.mts					mts\n"
  "model/vnd.parasolid.transmit.binary		x_b xmt_bin\n"
  "model/vnd.parasolid.transmit.text		x_t xmt_txt\n"
  "model/vnd.vtu					vtu\n"
  "model/vrml					wrl vrml\n"
  "multipart/voice-message				vpm\n"
  "text/calendar					ics ifb\n"
  "text/css					css\n"
  "text/csv					csv\n"
  "text/dns					soa zone\n"
  "# text/ecmascript obsoleted by application/ecmascript\n"
  "text/html					html htm\n"
  "# obsoleted by application/javascript\n"
  "text/javascript					js\n"
  "text/plain				asc txt text pm el c h cc hh cxx hxx f90\n"
  "text/prs.fallenstein.rst			rst\n"
  "text/prs.lines.tag				tag dsc\n"
  "text/richtext					rtx\n"
  "# rtf: application/rtf\n"
  "text/sgml					sgml sgm\n"
  "text/tab-separated-values			tsv\n"
  "text/uri-list					uris uri\n"
  "text/vnd.abc					abc\n"
  "text/vnd.DMClientScript				dms\n"
  "text/vnd.esmertec.theme-descriptor		jtd\n"
  "text/vnd.fly					fly\n"
  "text/vnd.fmi.flexstor				flx\n"
  "text/vnd.graphviz				gv dot\n"
  "text/vnd.in3d.3dml				3dml 3dm\n"
  "text/vnd.in3d.spot				spot spo\n"
  "text/vnd.ms-mediapackage			mpf\n"
  "text/vnd.net2phone.commcenter.command		ccc\n"
  "text/vnd.si.uricatalogue			uric\n"
  "text/vnd.sun.j2me.app-descriptor		jad\n"
  "text/vnd.trolltech.linguist			ts\n"
  "text/vnd.wap.si					si\n"
  "text/vnd.wap.sl					sl\n"
  "text/vnd.wap.wml				wml\n"
  "text/vnd.wap.wmlscript				wmls\n"
  "text/xml					xml\n"
  "text/xml-external-parsed-entity			ent\n"
  "video/3gpp					3gp 3gpp\n"
  "video/3gpp2					3g2 3gpp2\n"
  "video/mj2					mj2 mjp2\n"
  "video/mp4					mp4 mpg4\n"
  "video/mpeg					mpeg mpg mpe\n"
  "video/ogg					ogv\n"
  "video/quicktime					qt mov\n"
  "video/vnd.fvt					fvt\n"
  "video/vnd.mpegurl				mxu m4u\n"
  "video/vnd.ms-playready.media.pyv		pyv\n"
  "video/vnd.nokia.interleaved-multimedia		nim\n"
  "video/vnd.sealed.mpeg1				smpg s11\n"
  "video/vnd.sealed.mpeg4				s14\n"
  "video/vnd.sealed.swf				sswf ssw\n"
  "video/vnd.sealedmedia.softseal.mov		smov smo s1q\n"
  "application/mac-compactpro			cpt\n"
  "application/mathml+xml				mml\n"
  "application/metalink+xml			metalink\n"
  "application/rss+xml				rss\n"
  "application/vnd.ms-excel.addin.macroEnabled.12		xlam\n"
  "application/vnd.ms-excel.sheet.binary.macroEnabled.12	xlsb\n"
  "application/vnd.ms-excel.sheet.macroEnabled.12		xlsm\n"
  "application/vnd.ms-excel.template.macroEnabled.12	xltm\n"
  "application/vnd.ms-powerpoint.addin.macroEnabled.12		ppam\n"
  "application/vnd.ms-powerpoint.presentation.macroEnabled.12	pptm\n"
  "application/vnd.ms-powerpoint.slide.macroEnabled.12		sldm\n"
  "application/vnd.ms-powerpoint.slideshow.macroEnabled.12		ppsm\n"
  "application/vnd.ms-powerpoint.template.macroEnabled.12		potm\n"
  "application/vnd.ms-word.document.macroEnabled.12	docm\n"
  "application/vnd.ms-word.template.macroEnabled.12	dotm\n"
  "application/vnd.oma.dd+xml			dd\n"
  "application/vnd.oma.drm.content			dcf\n"
  "application/vnd.oma.drm.dcf			o4a o4v\n"
  "application/vnd.oma.drm.message			dm\n"
  "application/vnd.oma.drm.rights+wbxml		drc\n"
  "application/vnd.oma.drm.rights+xml		dr\n"
  "application/vnd.openxmlformats-officedocument.presentationml.presentation pptx\n"
  "application/vnd.openxmlformats-officedocument.presentationml.slide	sldx\n"
  "application/vnd.openxmlformats-officedocument.presentationml.slideshow	ppsx\n"
  "application/vnd.openxmlformats-officedocument.presentationml.template	potx\n"
  "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet	xlsx\n"
  "application/vnd.openxmlformats-officedocument.spreadsheetml.template	xltx\n"
  "application/vnd.openxmlformats-officedocument.wordprocessingml.document	docx\n"
  "application/vnd.openxmlformats-officedocument.wordprocessingml.template	dotx\n"
  "application/vnd.sun.xml.calc			sxc\n"
  "application/vnd.sun.xml.calc.template		stc\n"
  "application/vnd.sun.xml.draw			sxd\n"
  "application/vnd.sun.xml.draw.template		std\n"
  "application/vnd.sun.xml.impress			sxi\n"
  "application/vnd.sun.xml.impress.template	sti\n"
  "application/vnd.sun.xml.math			sxm\n"
  "application/vnd.sun.xml.writer			sxw\n"
  "application/vnd.sun.xml.writer.global		sxg\n"
  "application/vnd.sun.xml.writer.template		stw\n"
  "application/vnd.symbian.install			sis\n"
  "application/vnd.wap.mms-message			mms\n"
  "application/x-bcpio				bcpio\n"
  "application/x-bittorrent			torrent\n"
  "application/x-bzip2				bz2\n"
  "application/x-cdlink				vcd\n"
  "application/x-chess-pgn				pgn\n"
  "application/x-cpio				cpio\n"
  "application/x-csh				csh\n"
  "application/x-director				dcr dir dxr\n"
  "application/x-dvi				dvi\n"
  "application/x-futuresplash			spl\n"
  "application/x-gtar				gtar\n"
  "application/x-gzip				gz tgz\n"
  "application/x-hdf				hdf\n"
  "application/x-java-archive			jar\n"
  "application/x-java-jnlp-file			jnlp\n"
  "application/x-java-pack200			pack\n"
  "application/x-killustrator			kil\n"
  "application/x-latex				latex\n"
  "application/x-netcdf				nc cdf\n"
  "application/x-perl				pl\n"
  "application/x-rpm				rpm\n"
  "application/x-sh				sh\n"
  "application/x-shar				shar\n"
  "application/x-shockwave-flash			swf\n"
  "application/x-stuffit				sit\n"
  "application/x-sv4cpio				sv4cpio\n"
  "application/x-sv4crc				sv4crc\n"
  "application/x-tar				tar\n"
  "application/x-tcl				tcl\n"
  "application/x-tex				tex\n"
  "application/x-texinfo				texinfo texi\n"
  "application/x-troff				t tr roff\n"
  "application/x-troff-man				man 1 2 3 4 5 6 7 8\n"
  "application/x-troff-me				me\n"
  "application/x-troff-ms				ms\n"
  "application/x-ustar				ustar\n"
  "application/x-wais-source			src\n"
  "application/x-xz				xz\n"
  "audio/midi					mid midi kar\n"
  "audio/x-aiff					aif aiff aifc\n"
  "audio/x-mod					mod ult uni m15 mtm 669 med\n"
  "audio/x-mpegurl					m3u\n"
  "audio/x-ms-wax					wax\n"
  "audio/x-ms-wma					wma\n"
  "audio/x-pn-realaudio				ram rm\n"
  "audio/x-realaudio				ra\n"
  "audio/x-s3m					s3m\n"
  "audio/x-stm					stm\n"
  "audio/x-wav					wav\n"
  "chemical/x-xyz					xyz\n"
  "image/bmp					bmp\n"
  "image/svg+xml					svg svgz\n"
  "image/x-cmu-raster				ras\n"
  "image/x-portable-anymap				pnm\n"
  "image/x-portable-bitmap				pbm\n"
  "image/x-portable-graymap			pgm\n"
  "image/x-portable-pixmap				ppm\n"
  "image/x-rgb					rgb\n"
  "image/x-targa					tga\n"
  "image/x-xbitmap					xbm\n"
  "image/x-xpixmap					xpm\n"
  "image/x-xwindowdump				xwd\n"
  "text/cache-manifest				manifest\n"
  "text/x-pod					pod\n"
  "text/x-setext					etx\n"
  "text/x-vcard					vcf\n"
  "video/webm					webm\n"
  "video/x-flv					flv\n"
  "video/x-ms-asf					asx\n"
  "video/x-ms-wm					wm\n"
  "video/x-ms-wmv					wmv\n"
  "video/x-ms-wmx					wmx\n"
  "video/x-ms-wvx					wvx\n"
  "video/x-msvideo					avi\n"
  "video/x-sgi-movie				movie\n"
  "x-conference/x-cooltalk				ice\n"
  "x-epoc/x-sisx-app				sisx\n";
