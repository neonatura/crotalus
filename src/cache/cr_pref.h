
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

#ifndef __CACHE__CR_PREF_H__
#define __CACHE__CR_PREF_H__

/**
 * Data cacheing routines.
 * @ingroup crotalus
 * @defgroup crotalus_cache
 * @{
 */

#define CRPREF_ROOT_DIR "ServerRoot"
#define CRPREF_WEB_PORT "Port"
#define CRPREF_BIND_ADDR "Listen"
#define CRPREF_PROC_USER "User"
#define CRPREF_PROC_GROUP "Group"
#define CRPREF_ENV_ADMIN "ServerAdmin"
#define CRPREF_PROC_PID "PidFile"
#define CRPREF_ERROR_LOG "ErrorLog"
#define CRPREF_ACCESS_LOG "AccessLog"
#define CRPREF_CGI_LOG "CGILog"
#define CRPREF_CGI_UMASK "CGIumask"
#define CRPREF_TIME_LOCAL "UseLocaltime"
#define CRPREF_CGI_VERBOSE "VerboseCGILogs"
#define CRPREF_VHOST "VirtualHost"
#define CRPREF_VHOST_DIR "VHostRoot"
#define CRPREF_VHOST_NAME "DefaultVHost"
#define CRPREF_WEB_DIR "DocumentRoot"
#define CRPREF_USER_DIR "UserDir"
#define CRPREF_PATH_INDEX "DirectoryIndex"
#define CRPREF_CGI_INDEX "DirectoryMaker"
#define CRPREF_PROC_INDEX "DirectoryCache"
#define CRPREF_KEEPALIVE_MAX "KeepAliveMax"
#define CRPREF_KEEPALIVE_SPAN "KeepAliveTimeout"
#define CRPREF_MIME_PATH "MimeTypes"
#define CRPREF_MIME_DEFAULT "DefaultType"
#define CRPREF_ENV_PATH "CGIPath"
#define CRPREF_POST_LIMIT "SinglePostLimit"
#define CRPREF_CONF_DIR "ServerRoot"
#define CRPREF_WEB_NAME "ServerName"
#define CRPREF_PARENT_INDEX "ParentIndex"

/** The number of Crotalus preference options. */
#define CR_PREF_MAX 29

#define CRLOG_ERROR "Error"
#define CRLOG_ACCESS "Access"
#define CRLOG_CGI "CGI"

/** Set a global Crotalus preference value. */
int cr_pref_set(const char *token, const char *value);

/** Obtain a global Crotalus preference value. */
char *cr_pref_get(const char *token);

/** Obtain the default value for a preference. */
char *cr_pref_default(const char *token);

char *crpref_docroot(void);

/**
 * @}
 */

#endif /* ndef __CACHE__CR_PREF_H__ */
