
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

#ifndef PHP_CROTALUS_H
#define PHP_CROTALUS_H

#include <sys/types.h>
#include <sys/stat.h>
#include "crotalus.h"

typedef struct request httpd_conn;

void	 crotalus_php_shutdown(void);
void	 crotalus_php_init(void);
off_t	 crotalus_php_request(httpd_conn *hc, int show_source);

void	 crotalus_register_on_close(void (*)(int));
void	 crotalus_closed_conn(int fd);
int		 crotalus_get_fd(void);
void	 crotalus_set_dont_close(void);

const char *http_ver_string(int ver);
const char *http_method_string(int method);


#endif
