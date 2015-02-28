
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

/**
 * @{
 */

#define MIMEF_ENCODE (1 << 0)

#define MAX_MIME_DEFINITION 3

#if 0
struct mime_t;

typedef int mime_f(struct mime_t *mime, request *req);
   
/** A mime type definition. */
typedef struct mime_t 
{
  /* the mime type iso name */
  char *type;
  /* the filename extension referencing this mime definition. */
  char *ext;
  /* the "HEAD" http function. */
  mime_f *head;
  /* the "GET" http function. */
  mime_f *get;
  /* the "POST" http function. */
  mime_f *post;
  /* the "PUT" http function. */
  mime_f *put;
  /* method to decode stored content. */
  mime_f *decode;
  /* flags indicating the features of the mime definition (MIMEF_XXX). */
  int flags;
} mime_t;
#endif

extern mime_t mime_definition[MAX_MIME_DEFINITION];


mime_t *mime_info(int index);

/**
 * @}
 */


