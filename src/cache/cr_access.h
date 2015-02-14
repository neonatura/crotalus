
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


#ifndef __CACHE__ACCESS_H__
#define __CACHE__ACCESS_H__

/**
 * Origin host web access control.
 * @ingroup crotalus
 * @defgroup crotalus_access
 * @{
 */


enum access_type { ACCESS_DENY, ACCESS_ALLOW };

/**
 * Initialize the 'access control' sub-system.
 */
void access_init(void);

/**
 * Add a new 'access control rule' for a host.
 * @param pattern An IP address with filename-matching criteria (i.e. "127.*.*.1")
 */
int access_add(char *hostname, const char *pattern, enum access_type type);

/**
 * Determine whether any rules are present for a given web host and remote origin IP address.
 */
enum access_type access_allow(char *hostname, char *origin, char *filename);

/**
 * @}
 */

#endif /* ndef __CACHE__ACCESS_H__ */
