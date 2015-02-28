
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
  { CGI_MIME_TYPE, "cgi", mime_head_cgi, mime_get_cgi, NULL, NULL, 0 },
  { SH_MIME_TYPE, "sh", mime_head_cgi, mime_get_cgi, NULL, NULL, 0 },
  { GZIP_MIME_TYPE, "gz", mime_head_gzip, mime_get_gzip, NULL, NULL, MIMEF_ENCODE },
  //{ PHP_MIME_TYPE, mime_interp_php, 0 }
};  


mime_t *mime_info(int index)
{

  if (index < 0 || index >= MAX_MIME_DEFINITION)
    return (NULL);
    
  return (&mime_definition[index]);
}


