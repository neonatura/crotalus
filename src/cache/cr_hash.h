
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

#ifndef __CACHE__HASH_H__
#define __CACHE__HASH_H__

struct hash_struct {
    char *key;
    char *value;
    struct hash_struct *next;
};
typedef struct hash_struct hash_struct;

/** Generate a hash index from a string value. */
unsigned cr_hash(const char *str);

/**
 * Adds a key/value pair to the provided hashtable
 */
hash_struct *hash_insert(hash_struct * table[], const unsigned int hash, const char *key, const char *value);

/**
 * Obtain a hashed value from a hash-table.
 */
hash_struct *hash_find(hash_struct * table[], const char *key, const unsigned int hash);



#endif /* ndef __CACHE__HASH_H__ */


