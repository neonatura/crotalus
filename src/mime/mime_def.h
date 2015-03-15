
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

#define MAX_MIME_DEFINITION 3
#define MAX_MIME_FILTERS 2 

#define MIMEF_ENCODE (1 << 0)
#define MIMEF_INLINE (1 << 1)

#define ADD_OUTPUT_FILTER_LABEL "addoutputfilterbytype"
#define OUTPUT_FILTER_DEFLATE "deflate"
#define OUTPUT_FILTER_GZIP "gzip"


extern mime_t mime_definition[MAX_MIME_DEFINITION];

extern mime_filter_t mime_filter[MAX_MIME_FILTERS];

mime_t *mime_info(int index);


/**
 * @}
 */


