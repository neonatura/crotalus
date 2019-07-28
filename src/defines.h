
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura
 *  Copyright (C) 1995 Paul Phillips <paulp@go2net.com>
 *  Copyright (C) 1996-1999 Larry Doolittle <ldoolitt@boa.org>
 *  Copyright (C) 1997-2004 Jon Nelson <jnelson@boa.org>
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

#ifndef __DEFINES_H__
#define __DEFINES_H__


/** Primary configuration filename. */
#define DEFAULT_CONFIG_FILE "crotalus.conf"

/** Use -c on the command line to specify alternate configuration directory. */
#ifndef SERVER_ROOT
#define SERVER_ROOT cr_pref_get(CRPREF_CONF_DIR)
#endif

/**
 * suppress filled with messages about client disconnects, etc...
 */
#define QUIET_DISCONNECT 1

/** "CGIPath" */
#define DEFAULT_PATH (cr_pref_get(CRPREF_ENV_PATH))

/** "DefaultVHost" */
#define DEFAULT_VHOST cr_pref_get(CRPREF_VHOST_NAME)

/** kernel socket buffer size */
#define SOCKETBUF_SIZE 49152

/** internal buffer for client */
#define CLIENT_STREAM_SIZE 8192

/** generic buffer size */
#define BUFFER_SIZE 4096
#define MAX_HEADER_LENGTH			1024

#define MIME_HASHTABLE_SIZE			47
#define ALIAS_HASHTABLE_SIZE                    17
#define PASSWD_HASHTABLE_SIZE		        47

#define REQUEST_TIMEOUT				60

#define MIME_TYPES_DEFAULT cr_pref_get(CRPREF_MIME_PATH)

#define CGI_MIME_TYPE "application/x-httpd-cgi"
#define PHP_MIME_TYPE "application/x-httpd-php"
#define GZIP_MIME_TYPE "application/x-gzip"
#define SH_MIME_TYPE "application/x-sh"

#define MAX_FILE_LENGTH				NAME_MAX
#define MAX_PATH_LENGTH				PATH_MAX

#ifdef ACCEPT_ON
#define MAX_ACCEPT_LENGTH MAX_HEADER_LENGTH
#else
#define MAX_ACCEPT_LENGTH 0
#endif

#ifndef SERVER_VERSION
#define SERVER_VERSION 				"Crotalus/4.0"
#endif

#define CGI_VERSION				"CGI/1.1"

#ifdef USE_NCSA_CGI_ENV
#define COMMON_CGI_COUNT 8
#else
#define COMMON_CGI_COUNT 6
#endif

#define CGI_ENV_MAX     100
#define CGI_ARGC_MAX 128

#define SERVER_METHOD "http"

/*********** MMAP_LIST CONSTANTS ************************/
#define MMAP_LIST_SIZE 256
#define MMAP_LIST_MASK 255
#define MMAP_LIST_USE_MAX 128

#define MAX_FILE_MMAP 100 * 1024 /* 100K */

/*************** POLL / SELECT MACROS *******************/
#ifdef HAVE_POLL
#define BOA_READ (POLLIN|POLLPRI|POLLHUP)
#define BOA_WRITE (POLLOUT|POLLHUP)
#define BOA_FD_SET(req, thefd,where) { struct pollfd *my_pfd = &pfds[pfd_len]; req->pollfd_id = pfd_len++; my_pfd->fd = thefd; my_pfd->events = where; }
#define BOA_FD_CLR(req, fd, where) /* this doesn't do anything? */
#else                           /* SELECT */
#define BOA_READ (&block_read_fdset)
#define BOA_WRITE (&block_write_fdset)
#define BOA_FD_SET(req, fd, where) { FD_SET(fd, where); if (fd > max_fd) max_fd = fd; }
#define BOA_FD_CLR(req, fd, where) { FD_CLR(fd, where); }
#endif

/******** MACROS TO CHANGE BLOCK/NON-BLOCK **************/
/* If and when everyone has a modern gcc or other near-C99 compiler,
 * change these to static inline functions. Also note that since
 * we never fuss with O_APPEND append or O_ASYNC, we shouldn't have
 * to perform an extra system call to F_GETFL first.
 */
#ifdef BOA_USE_GETFL
#define set_block_fd(fd)    real_set_block_fd(fd)
#define set_nonblock_fd(fd) real_set_nonblock_fd(fd)
#else
#define set_block_fd(fd)    fcntl(fd, F_SETFL, 0)
#define set_nonblock_fd(fd) fcntl(fd, F_SETFL, NOBLOCK)
#endif

/********************* DEBUG STUFF ***********************/
extern int debug_level;

#ifdef DISABLE_DEBUG
#define real_debug_level 0
#else
#define real_debug_level debug_level
#endif

#define DEBUG(foo)          if (real_debug_level & foo)

#define DEBUG_ALIAS         (1<<0)
#define DEBUG_CGI_OUTPUT    (1<<1)
#define DEBUG_CGI_INPUT     (1<<2)
#define DEBUG_CGI_ENV       (1<<3)
#define DEBUG_HEADER_READ   (1<<4)
#define DEBUG_PIPELINE      (1<<5)
#define DEBUG_PLUGIN_ERRORS (1<<6)
#define DEBUG_RANGE         (1<<7)
#define DEBUG_CONFIG        (1<<8)
#define DEBUG_BUFFER_IO     (1<<9)
#define DEBUG_BODY_READ     (1<<10)
#define DEBUG_MMAP_CACHE    (1<<11)
#define DEBUG_REQUEST       (1<<12)
#define DEBUG_HASH          (1<<13)

/***************** USEFUL MACROS ************************/

#define CRLF "\r\n"
#define SQUASH_KA(req)	(req->keepalive=KA_STOPPED)

#ifdef HAVE_FUNC
#define WARN(_str) log_error_mesg(__FILE__, __LINE__, __func__, (_str))
#else
#define WARN(_str) log_error_mesg(__FILE__, __LINE__, "<n/a>", (_str))
#endif
#define DIE(_str) WARN(_str)
#define log_error_time() WARN(NULL)

#define INT_TO_HEX(x) (((x)>9)?(('a'-10)+(x)):('0'+(x)))
#define HEX_TO_DECIMAL(char1, char2)    \
    (((char1 >= 'A') ? (((char1 & 0xdf) - 'A') + 10) : (char1 - '0')) * 16) + \
    (((char2 >= 'A') ? (((char2 & 0xdf) - 'A') + 10) : (char2 - '0')))

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif


#endif /* ndef __DEFINES_H__ */
