
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

mime_t mime_definition[MAX_MIME_DEFINITION] = {
#ifdef CGI
  { CGI_MIME_TYPE, "cgi", mime_head_cgi, mime_get_cgi, NULL, NULL, 0 },
  { SH_MIME_TYPE, "sh", mime_head_cgi, mime_get_cgi, NULL, NULL, 0 },
#else
  { CGI_MIME_TYPE, "cgi", NULL, NULL, NULL, NULL, MIMEF_INLINE },
  { SH_MIME_TYPE, "sh", NULL, NULL, NULL, NULL, MIMEF_INLINE },
#endif
#ifdef GUNZIP
  { GZIP_MIME_TYPE, "gz", mime_head_gzip, mime_get_gzip, NULL, NULL, MIMEF_ENCODE },
#else
  { GZIP_MIME_TYPE, "gz", NULL, NULL, NULL, NULL, MIMEF_INLINE },
#endif
#ifdef PHP_RUNTIME
  { PHP_MIME_TYPE, "php", mime_head_php, mime_get_php, NULL, NULL, 0 },
#else
  { PHP_MIME_TYPE, "php", NULL, NULL, NULL, NULL, MIMEF_INLINE }
#endif
};  

mime_filter_t mime_filter[MAX_MIME_FILTERS] = {
  { OUTPUT_FILTER_DEFLATE, mime_filter_init_deflate, mime_filter_write_deflate, mime_filter_flush_deflate },
  { OUTPUT_FILTER_GZIP, mime_filter_init_gzip, mime_filter_write_gzip, mime_filter_flush_gzip },
};


mime_t *mime_info(int index)
{

  if (index < 0 || index >= MAX_MIME_DEFINITION)
    return (NULL);
    
  return (&mime_definition[index]);
}


