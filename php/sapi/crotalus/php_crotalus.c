
/*
 * @copyright
 *
 *  Copyright 2016 Neo Natura
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

#include "php.h"
#include "SAPI.h"
#include "php_main.h"
#include "php_crotalus.h"
#include "php_variables.h"
#include "php_version.h"
#include "php_ini.h"
#include "zend_highlight.h"

#include "ext/standard/php_smart_str.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_GETNAMEINFO
#include <sys/socket.h>
#include <netdb.h>
#endif

typedef struct {
	httpd_conn *hc;
	void (*on_close)(int);

	size_t unconsumed_length;
	smart_str sbuf;
	int seen_cl;
	int seen_cn;
} php_crotalus_globals;

#define PHP_SYS_CALL(x) do { x } while (n == -1 && errno == EINTR)

#ifdef ZTS
static int crotalus_globals_id;
#define TG(v) TSRMG(crotalus_globals_id, php_crotalus_globals *, v)
#else
static php_crotalus_globals crotalus_globals;
#define TG(v) (crotalus_globals.v)
#endif

static int sapi_crotalus_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	int n;
	uint sent = 0;

fprintf(stderr, "DEBUG: sapi_crotalus_ub_write(): %s\n", str);
	
	if (TG(sbuf).c != 0) {
		smart_str_appendl_ex(&TG(sbuf), str, str_length, 1);
		return str_length;
	}
	
	while (str_length > 0) {
		PHP_SYS_CALL(n = send(TG(hc)->fd, str, str_length, 0););

		if (n == -1) {
			if (errno == EAGAIN) {
				smart_str_appendl_ex(&TG(sbuf), str, str_length, 1);

				return sent + str_length;
			} else
				php_handle_aborted_connection();
		}

		TG(hc)->bytes_written += n;
		str += n;
		sent += n;
		str_length -= n;
	}

	return sent;
}

#define COMBINE_HEADERS 64

#if defined(IOV_MAX)
# if IOV_MAX - 64 <= 0
#  define SERIALIZE_HEADERS
# endif
#endif

static int do_writev(struct iovec *vec, int nvec, int len TSRMLS_DC)
{
	int n;

	assert(nvec <= IOV_MAX);

	if (TG(sbuf).c == 0) {
		PHP_SYS_CALL(n = writev(TG(hc)->fd, vec, nvec););

		if (n == -1) {
			if (errno == EAGAIN) {
				n = 0;
			} else {
				php_handle_aborted_connection();
			}
		}


		TG(hc)->bytes_written += n;
	} else {
		n = 0;
	}

	if (n < len) {
		int i;

		/* merge all unwritten data into sbuf */
		for (i = 0; i < nvec; vec++, i++) {
			/* has this vector been written completely? */
			if (n >= vec->iov_len) {
				/* yes, proceed */
				n -= vec->iov_len;
				continue;
			}

			if (n > 0) {
				/* adjust vector */
				vec->iov_base = (char *) vec->iov_base + n;
				vec->iov_len -= n;
				n = 0;
			}

			smart_str_appendl_ex(&TG(sbuf), vec->iov_base, vec->iov_len, 1);
		}
	}
	
	return 0;
}

#ifdef SERIALIZE_HEADERS
# define ADD_VEC(str,l) smart_str_appendl(&vec_str, (str), (l))
# define VEC_BASE() smart_str vec_str = {0}
# define VEC_FREE() smart_str_free(&vec_str)
#else
# define ADD_VEC(str,l) vec[n].iov_base=str;len += (vec[n].iov_len=l); n++
# define VEC_BASE() struct iovec vec[COMBINE_HEADERS]
# define VEC_FREE() do {} while (0)
#endif

#define ADD_VEC_S(str) ADD_VEC((str), sizeof(str)-1)

#define CL_TOKEN "Content-length: "
#define CN_TOKEN "Connection: "
#define KA_DO "Connection: keep-alive\r\n"
#define KA_NO "Connection: close\r\n"
#define DEF_CT "Content-Type: text/html\r\n"

static int sapi_crotalus_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char buf[1024], *p;
	VEC_BASE();
	int n = 0;
	zend_llist_position pos;
	sapi_header_struct *h;
	size_t len = 0;
	
	if (!SG(sapi_headers).http_status_line) {
		ADD_VEC_S("HTTP/1.1 ");
		p = smart_str_print_long(buf+sizeof(buf)-1, 
				SG(sapi_headers).http_response_code);
		ADD_VEC(p, strlen(p));
		ADD_VEC_S(" HTTP\r\n");
	} else {
		ADD_VEC(SG(sapi_headers).http_status_line, 
				strlen(SG(sapi_headers).http_status_line));
		ADD_VEC("\r\n", 2);
	}
	TG(hc)->status = SG(sapi_headers).http_response_code;

	if (SG(sapi_headers).send_default_content_type) {
		ADD_VEC(DEF_CT, strlen(DEF_CT));
	}

	h = zend_llist_get_first_ex(&sapi_headers->headers, &pos);
	while (h) {
		
		switch (h->header[0]) {
			case 'c': case 'C':
				if (!TG(seen_cl) && strncasecmp(h->header, CL_TOKEN, sizeof(CL_TOKEN)-1) == 0) {
					TG(seen_cl) = 1;
				} else if (!TG(seen_cn) && strncasecmp(h->header, CN_TOKEN, sizeof(CN_TOKEN)-1) == 0) {
					TG(seen_cn) = 1;
				}
		}

		ADD_VEC(h->header, h->header_len);
#ifndef SERIALIZE_HEADERS
		if (n >= COMBINE_HEADERS - 1) {
			len = do_writev(vec, n, len TSRMLS_CC);
			n = 0;
		}
#endif
		ADD_VEC("\r\n", 2);
		
		h = zend_llist_get_next_ex(&sapi_headers->headers, &pos);
	}

	if (TG(seen_cl) && !TG(seen_cn) && TG(hc)->keepalive == KA_ACTIVE) {
		ADD_VEC(KA_DO, sizeof(KA_DO)-1);
	} else {
		TG(hc)->keepalive = KA_INACTIVE;
		ADD_VEC(KA_NO, sizeof(KA_NO)-1);
	}
		
	ADD_VEC("\r\n", 2);

#ifdef SERIALIZE_HEADERS
	sapi_crotalus_ub_write(vec_str.c, vec_str.len TSRMLS_CC);
#else			
	do_writev(vec, n, len TSRMLS_CC);
#endif

	VEC_FREE();

	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

/* to understand this, read cgi_interpose_input() in libhttpd.c */
#define SIZEOF_UNCONSUMED_BYTES() (TG(hc)->read_idx - TG(hc)->checked_idx)
#define CONSUME_BYTES(n) do { TG(hc)->checked_idx += (n); } while (0)


static int sapi_crotalus_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	size_t read_bytes = 0;

fprintf(stderr, "DEBUG: sapi_crotalus_read_post(<max %u bytes>)\n", count_bytes);

  read_bytes = MIN(shbuf_size(TG(hc)->buff), count_bytes);
  memcpy(buffer, shbuf_data(TG(hc)->buff), read_bytes);
  shbuf_trim(TG(hc)->buff, read_bytes);
 
#if 0
	if (TG(unconsumed_length) > 0) {
		read_bytes = MIN(TG(unconsumed_length), count_bytes);
		memcpy(buffer, TG(hc)->read_buf + TG(hc)->checked_idx, read_bytes);
		TG(unconsumed_length) -= read_bytes;
		CONSUME_BYTES(read_bytes);
	}
#endif
	
	return read_bytes;
}

static char *sapi_crotalus_read_cookies(TSRMLS_D)
{
	return TG(hc)->header_cookie ?  TG(hc)->header_cookie : "";
}

#define BUF_SIZE 512
#define ADD_STRING_EX(name,buf)									\
	php_register_variable(name, buf, track_vars_array TSRMLS_CC)
#define ADD_STRING(name) ADD_STRING_EX((name), buf)

static void sapi_crotalus_register_variables(zval *track_vars_array TSRMLS_DC)
{
	char buf[BUF_SIZE + 1];
	char *p;

	php_register_variable("PHP_SELF", SG(request_info).request_uri, track_vars_array TSRMLS_CC);
	php_register_variable("SERVER_SOFTWARE", PACKAGE_STRING, track_vars_array TSRMLS_CC);
	php_register_variable("GATEWAY_INTERFACE", "CGI/1.1", track_vars_array TSRMLS_CC);
	php_register_variable("REQUEST_METHOD", (char *) SG(request_info).request_method, track_vars_array TSRMLS_CC);
	php_register_variable("REQUEST_URI", SG(request_info).request_uri, track_vars_array TSRMLS_CC);
	php_register_variable("PATH_TRANSLATED", SG(request_info).path_translated, track_vars_array TSRMLS_CC);

  php_register_variable("SERVER_PROTOCOL", (char *)http_ver_string(TG(hc)->http_version), track_vars_array TSRMLS_CC);
#if 0
	if (TG(hc)->one_one) {
		php_register_variable("SERVER_PROTOCOL", "HTTP/1.1", track_vars_array TSRMLS_CC);
	} else {
		php_register_variable("SERVER_PROTOCOL", "HTTP/1.0", track_vars_array TSRMLS_CC);
	}
#endif

#if 0
	p = httpd_ntoa(&TG(hc)->client_addr);	
	ADD_STRING_EX("REMOTE_ADDR", p);
	ADD_STRING_EX("REMOTE_HOST", p);
#endif
	ADD_STRING_EX("REMOTE_ADDR", TG(hc)->remote_ip_addr);
	ADD_STRING_EX("REMOTE_HOST", TG(hc)->remote_ip_addr);

	ADD_STRING_EX("SERVER_PORT",
			smart_str_print_long(buf + sizeof(buf) - 1, server_port));

	buf[0] = '/';
	memcpy(buf + 1, TG(hc)->path_info, strlen(TG(hc)->path_info) + 1);
	ADD_STRING("PATH_INFO");

	buf[0] = '/';
	memcpy(buf + 1, TG(hc)->script_name, strlen(TG(hc)->script_name) + 1);
	ADD_STRING("SCRIPT_NAME");

#define CONDADD(name, field) 							\
	if (TG(hc)->field[0]) {								\
		php_register_variable(#name, TG(hc)->field, track_vars_array TSRMLS_CC); \
	}

	CONDADD(QUERY_STRING, query_string);
	CONDADD(HTTP_HOST, header_host);
	CONDADD(HTTP_REFERER, header_referer);
	CONDADD(HTTP_USER_AGENT, header_user_agent);
#ifdef ACCEPT_ON
	CONDADD(HTTP_ACCEPT, accept);
#endif
	CONDADD(HTTP_ACCEPT_LANGUAGE, language);
	CONDADD(HTTP_ACCEPT_ENCODING, encoding);
	CONDADD(HTTP_COOKIE, header_cookie);
	CONDADD(CONTENT_TYPE, content_type);
#ifdef UNIMPLEM_REMOTEUSER
	CONDADD(REMOTE_USER, remoteuser);
#endif

#if 0
	CONDADD(SERVER_PROTOCOL, http_ver_string(http_version));
#endif
  php_register_variable("SERVER_PROTOCOL", 
      (char *)http_ver_string(TG(hc)->http_version), 
      track_vars_array TSRMLS_CC);

	if (TG(hc)->content_length) {
    ADD_STRING_EX("CONTENT_LENGTH", TG(hc)->content_length);
  }
#if 0
  if (TG(hc)->contentlength != -1) {
    ADD_STRING_EX("CONTENT_LENGTH",
        smart_str_print_long(buf + sizeof(buf) - 1, 
          TG(hc)->contentlength));
  }
#endif

  if (TG(hc)->header_authorization && 
      TG(hc)->header_authorization[0])
		php_register_variable("AUTH_TYPE", "Basic", track_vars_array TSRMLS_CC);
}

static PHP_MINIT_FUNCTION(crotalus)
{
	return SUCCESS;
}

static zend_module_entry php_crotalus_module = {
	STANDARD_MODULE_HEADER,
	"crotalus",
	NULL,
	PHP_MINIT(crotalus),
	NULL,
	NULL,
	NULL,
	NULL, /* info */
	NULL,
	STANDARD_MODULE_PROPERTIES
};

static int php_crotalus_startup(sapi_module_struct *sapi_module)
{
#if PHP_API_VERSION >= 20020918
	if (php_module_startup(sapi_module, &php_crotalus_module, 1) == FAILURE) {
#else
	if (php_module_startup(sapi_module) == FAILURE
			|| zend_startup_module(&php_crotalus_module) == FAILURE) {
#endif
fprintf(stderr, "DEBUG: php_crotalus_startup(): failure\n");
		return FAILURE;
	}
fprintf(stderr, "DEBUG: php_crotalus_startup(): success\n");
	return SUCCESS;
}

static int sapi_crotalus_get_fd(int *nfd TSRMLS_DC)
{
	if (nfd) *nfd = TG(hc)->fd;
	return SUCCESS;
}

static sapi_module_struct crotalus_sapi_module =
{
	"crotalus",
	"crotalus",
	
	php_crotalus_startup,
	php_module_shutdown_wrapper,
	
	NULL,									/* activate */
	NULL,									/* deactivate */

	sapi_crotalus_ub_write,
	NULL,
	NULL,									/* get uid */
	NULL,									/* getenv */

	php_error,
	
	NULL,
	sapi_crotalus_send_headers,
	NULL,
	sapi_crotalus_read_post,
	sapi_crotalus_read_cookies,

	sapi_crotalus_register_variables,
	NULL,									/* Log message */
	NULL,									/* Get request time */
	NULL,									/* Child terminate */

	NULL,									/* php.ini path override */
	NULL,									/* Block interruptions */
	NULL,									/* Unblock interruptions */

	NULL,
	NULL,
	NULL,
	0,
	0,
	sapi_crotalus_get_fd
};

static void crotalus_module_main(int show_source TSRMLS_DC)
{
	zend_file_handle file_handle;

	if (php_request_startup(TSRMLS_C) == FAILURE) {
		return;
	}
	
	if (show_source) {
		zend_syntax_highlighter_ini syntax_highlighter_ini;

		php_get_highlight_struct(&syntax_highlighter_ini);
		highlight_file(SG(request_info).path_translated, &syntax_highlighter_ini TSRMLS_CC);
	} else {
		file_handle.type = ZEND_HANDLE_FILENAME;
		file_handle.filename = SG(request_info).path_translated;
		file_handle.free_filename = 0;
		file_handle.opened_path = NULL;

		php_execute_script(&file_handle TSRMLS_CC);
	}
	
	php_request_shutdown(NULL);
}

static void crotalus_request_ctor(TSRMLS_D)
{
	smart_str s = {0};

	TG(seen_cl) = 0;
	TG(seen_cn) = 0;
	TG(sbuf).c = 0;
	SG(request_info).query_string = TG(hc)->query_string?strdup(TG(hc)->query_string):NULL;

#if 0
	smart_str_appends_ex(&s, TG(hc)->hs->cwd, 1);
	smart_str_appends_ex(&s, TG(hc)->expnfilename, 1);
#endif
	smart_str_appends_ex(&s, TG(hc)->pathname, 1);
	smart_str_0(&s);
	SG(request_info).path_translated = s.c;
	
	s.c = NULL;
	smart_str_appendc_ex(&s, '/', 1);
	smart_str_appends_ex(&s, TG(hc)->script_name, 1);
	smart_str_0(&s);
	SG(request_info).request_uri = s.c;
	SG(request_info).request_method = http_method_string(TG(hc)->method);

#if 0
	if (TG(hc)->one_one) SG(request_info).proto_num = 1001;
#endif
  if (TG(hc)->http_version == HTTP11) 
    SG(request_info).proto_num = 1001;

	else SG(request_info).proto_num = 1000;
	SG(sapi_headers).http_response_code = 200;
	if (TG(hc)->content_type)
		SG(request_info).content_type = strdup(TG(hc)->content_type);
	SG(request_info).content_length = atoi(TG(hc)->content_length);

	TG(unconsumed_length) = SG(request_info).content_length;
	
	php_handle_auth_data(TG(hc)->header_authorization TSRMLS_CC);
}

static void crotalus_request_dtor(TSRMLS_D)
{
	smart_str_free_ex(&TG(sbuf), 1);
	if (SG(request_info).query_string)
		free(SG(request_info).query_string);
	free(SG(request_info).request_uri);
	free(SG(request_info).path_translated);
	if (SG(request_info).content_type)
		free(SG(request_info).content_type);
}

#ifdef ZTS

#ifdef TSRM_ST
#define thread_create_simple_detached(n) st_thread_create(n, NULL, 0, 0)
#define thread_usleep(n) st_usleep(n)
#define thread_exit() st_thread_exit(NULL)
/* No preemption, simple operations are safe */
#define thread_atomic_inc(n) (++n)
#define thread_atomic_dec(n) (--n)
#else
#error No thread primitives available
#endif

/* We might want to replace this with a STAILQ */
typedef struct qreq {
	httpd_conn *hc;
	struct qreq *next;
} qreq_t;

static MUTEX_T qr_lock;
static qreq_t *queued_requests;
static qreq_t *last_qr;
static int nr_free_threads;
static int nr_threads;
static int max_threads = 50;

#define HANDLE_STRINGS() { \
	HANDLE_STR(encodedurl); \
	HANDLE_STR(decodedurl); \
	HANDLE_STR(script_name); \
	HANDLE_STR(pathname); \
	HANDLE_STR(path_info); \
	HANDLE_STR(query_string); \
	HANDLE_STR(header_referer); \
	HANDLE_STR(header_user_agent); \
	HANDLE_STR(encoding); \
	HANDLE_STR(language); \
	HANDLE_STR(header_cookie); \
	HANDLE_STR(content_type); \
	HANDLE_STR(header_authorization); \
	}

static httpd_conn *duplicate_conn(httpd_conn *hc, httpd_conn *nhc)
{
	memcpy(nhc, hc, sizeof(*nhc));

#define HANDLE_STR(m) nhc->m = nhc->m ? strdup(nhc->m) : NULL
	HANDLE_STRINGS();
#ifdef ACCEPT_ON
	HANDLE_STR(accept);
#endif
#ifdef UNIMPLEM_REMOTEUSER
	HANDLE_STR(remoteuser);
#endif
#undef HANDLE_STR
	
	return nhc;
}

static void destroy_conn(httpd_conn *hc)
{
#define HANDLE_STR(m) if (hc->m) free(hc->m)
	HANDLE_STRINGS();
#ifdef ACCEPT_ON
	HANDLE_STR(accept);
#endif
#ifdef UNIMPLEM_REMOTEUSER
	HANDLE_STR(remoteuser);
#endif
#undef HANDLE_STR
}

static httpd_conn *dequeue_request(void)
{
	httpd_conn *ret = NULL;
	qreq_t *m;
	
	tsrm_mutex_lock(qr_lock);
	if (queued_requests) {
		m = queued_requests;
		ret = m->hc;
		if (!(queued_requests = m->next))
			last_qr = NULL;
		free(m);
	}
	tsrm_mutex_unlock(qr_lock);
	
	return ret;
}

static void *worker_thread(void *);

static void queue_request(httpd_conn *hc)
{
	qreq_t *m;
	httpd_conn *nhc;
	
	/* Mark as long-running request */
	hc->file_address = (char *) 1;

	/*
     * We cannot synchronously revoke accesses to hc in the worker
	 * thread, so we need to pass a copy of hc to the worker thread.
	 */
	nhc = malloc(sizeof *nhc);
	duplicate_conn(hc, nhc);
	
	/* Allocate request queue container */
	m = malloc(sizeof *m);
	m->hc = nhc;
	m->next = NULL;
	
	tsrm_mutex_lock(qr_lock);
	/* Create new threads when reaching a certain threshold */
	if (nr_threads < max_threads && nr_free_threads < 2) {
		nr_threads++; /* protected by qr_lock */
		
		thread_atomic_inc(nr_free_threads);
		thread_create_simple_detached(worker_thread);
	}
	/* Insert container into request queue */
	if (queued_requests)
		last_qr->next = m;
	else
		queued_requests = m;
	last_qr = m;
	tsrm_mutex_unlock(qr_lock);
}

static off_t crotalus_real_php_request(httpd_conn *hc, int TSRMLS_DC);

static void *worker_thread(void *dummy)
{
	int do_work = 50;
	httpd_conn *hc;

	while (do_work) {
		hc = dequeue_request();

		if (!hc) {
/*			do_work--; */
			thread_usleep(500000);
			continue;
		}
/*		do_work = 50; */

		thread_atomic_dec(nr_free_threads);

		crotalus_real_php_request(hc, 0 TSRMLS_CC);
		shutdown(hc->fd, 0);
		destroy_conn(hc);
		free(hc);

		thread_atomic_inc(nr_free_threads);
	}
	thread_atomic_dec(nr_free_threads);
	thread_atomic_dec(nr_threads);
	thread_exit();
}

static void remove_dead_conn(int fd)
{
	qreq_t *m, *prev = NULL;

	tsrm_mutex_lock(qr_lock);
	m = queued_requests;
	while (m) {
		if (m->hc->fd == fd) {
			if (prev)
				if (!(prev->next = m->next))
					last_qr = prev;
			else
				if (!(queued_requests = m->next))
					last_qr = NULL;
			destroy_conn(m->hc);
			free(m->hc);
			free(m);
			break;
		}
		prev = m;
		m = m->next;
	}
	tsrm_mutex_unlock(qr_lock);
}

#endif

static off_t crotalus_real_php_request(httpd_conn *hc, int show_source TSRMLS_DC)
{
	TG(hc) = hc;
	hc->bytes_written = 0;

fprintf(stderr, "DEBUG: crotalus_real_php_request()\n");

	if (hc->content_length) {
#ifdef UNIMPLEMTED_LINGER
		hc->should_linger = 1;
#endif
		hc->keepalive = KA_INACTIVE;
	}
	
#if 0 /* DEBUG: */
  if (hc->content_length &&
      SIZEOF_UNCONSUMED_BYTES() < atoi(hc->content_length)) {
#ifdef UNIMPLEM_READ_BODY_INTO_MEM
		hc->read_body_into_mem = 1;
#endif
		return 0;
	}
#endif
	
	crotalus_request_ctor(TSRMLS_C);

	crotalus_module_main(show_source TSRMLS_CC);

	/* disable kl, if no content-length was seen or Connection: was set */
	if (TG(seen_cl) == 0 || TG(seen_cn) == 1) {
		TG(hc)->keepalive = KA_INACTIVE;
	}
	
#if 0 /* DEBUG: */
	if (TG(sbuf).c != 0) {
		if (TG(hc)->response)
			free(TG(hc)->response);
		
		TG(hc)->response = TG(sbuf).c;
		TG(hc)->responselen = TG(sbuf).len;
		TG(hc)->maxresponse = TG(sbuf).a;

		TG(sbuf).c = 0;
		TG(sbuf).len = 0;
		TG(sbuf).a = 0;
	}
#endif

	crotalus_request_dtor(TSRMLS_C);

	return 0;
}

off_t crotalus_php_request(httpd_conn *hc, int show_source)
{
#ifdef ZTS
	queue_request(hc);
#else
	TSRMLS_FETCH();
	return crotalus_real_php_request(hc, show_source TSRMLS_CC);
#endif
}

void crotalus_register_on_close(void (*arg)(int)) 
{
	TSRMLS_FETCH();
	TG(on_close) = arg;
}

void crotalus_closed_conn(int fd)
{
	TSRMLS_FETCH();
	if (TG(on_close)) TG(on_close)(fd);
}

int crotalus_get_fd(void)
{
fprintf(stderr, "DEBUG: crotalus_get_fd: fd %d\n", TG(hc)->fd);
	TSRMLS_FETCH();
	return TG(hc)->fd;
}

void crotalus_set_dont_close(void)
{
	TSRMLS_FETCH();
#ifdef UNIMPLEM_FILE_ADDRESS
	TG(hc)->file_address = (char *) 1;
#endif
}


void crotalus_php_init(void)
{
	char *ini;

#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
	ts_allocate_id(&crotalus_globals_id, sizeof(php_crotalus_globals), NULL, NULL);
	qr_lock = tsrm_mutex_alloc();
	crotalus_register_on_close(remove_dead_conn);
#endif

	if ((ini = getenv("PHP_INI_PATH"))) {
		crotalus_sapi_module.php_ini_path_override = ini;
	}

	sapi_startup(&crotalus_sapi_module);
	crotalus_sapi_module.startup(&crotalus_sapi_module);
	
	{
		TSRMLS_FETCH();

		SG(server_context) = (void *) 1;
	}
}

void crotalus_php_shutdown(void)
{
	TSRMLS_FETCH();

	if (SG(server_context) != NULL) {
		crotalus_sapi_module.shutdown(&crotalus_sapi_module);
		sapi_shutdown();
#ifdef ZTS
		tsrm_shutdown();
#endif
	}
}
