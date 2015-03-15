
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

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

static hash_struct *access_hashtable[MIME_HASHTABLE_SIZE];

static unsigned _cr_access_hash(const char *token)
{
  return cr_hash(token) % MIME_HASHTABLE_SIZE;
}

int cr_access_set(const char *token, const char *value)
{
  unsigned int hash;

  hash = _cr_access_hash(token);
  if (!value || !*value) {
    // DEBUG: hash_remove() .. TODO 
  } else {
    if (hash_insert(access_hashtable, hash, token, value) == NULL) {
      return (-EINVAL);
    }
  }

  return (0);
}


/**
 * @returns blank string when token not found
 */
char *cr_access_get(const char *token)
{
  hash_struct *current;
  unsigned int hash;

  /* token[0] *can't* be NIL */
  if (!token || !*token)
    return "";

  hash = _cr_access_hash(token);
  current = hash_find(access_hashtable, token, hash);
  if (current) {
    return (current->value);
  }

  return ("");
}

/**
 * Specifies a rule-set for access controls.
 */
typedef struct access_node {
  char *host_pattern;
  char *file_pattern; /* not imp'd */
  enum access_type type;
} access_node;

/**
 * Specifies a list of rule-sets for individual hosts.
 */
typedef struct access_list {
  char hostname[MAXHOSTNAMELEN+1];
  access_node *nodes;
  int n_access; 

  struct access_list *next;
} access_list; 

static access_list *_access_host_list;

static void access_shutdown(void)
{
  access_list *l;
  access_list *l_next;
  int i;

  for (l = _access_host_list; l; l = l->next) {
    if (l->nodes) {
      for (i = 0; i < l->n_access; i++) {
        if (l->nodes[i].host_pattern) {
          free(l->nodes[i].host_pattern);
        }
      }
      free(l->nodes);
    }

    l->nodes = NULL;
    l->n_access = 0;
  }

  for (l = _access_host_list; l; l = l_next) {
    l_next = l->next;
    free(l);
  }

  _access_host_list = NULL;

}


void access_init(void)
{

  if (_access_host_list) {
    access_shutdown();
  }

}

static void _strtolower(char *str)
{
  int i;
  for (i = 0; i < strlen(str); i++)
    str[i] = tolower(str[i]);
}

static void _access_host_init(access_list *l)
{
  FILE *fl;
  char path[PATH_MAX+1];
  char tok[4096];
  char *val;

  if (virtualhost)
    sprintf(path, "%s/%s/.htaccess", crpref_docroot(), l->hostname);
  else
    sprintf(path, "%s/.htaccess", crpref_docroot());
  fl = fopen(path, "rb");
  if (!fl)
    return;

  while (!feof(fl)) {
    memset(tok, 0, sizeof(tok));
    fgets(tok, sizeof(tok) - 1, fl); 
    strtok(tok, "\r\n");
    if (!*tok || *tok == '#')
      continue;

    val = strchr(tok, ' ');
    if (!val)
      continue;

    _strtolower(tok);
    *val++ = '\0';

    if (0 == strcmp(tok, "allow")) {
      access_node_add(l, val, ACCESS_ALLOW);  
    } else if (0 == strcmp(tok, "deny")) {
      access_node_add(l, val, ACCESS_DENY);
    } else if (0 == strcmp(tok, ADD_OUTPUT_FILTER_LABEL)) {
      char *str = strchr(val, ' ');
      if (!str)
        continue;

      *str++ = '\0';
      if (0 == strcmp(val, OUTPUT_FILTER_DEFLATE)) {
        access_filter_add(OUTPUT_FILTER_DEFLATE, str);
      } else if (0 == strcmp(val, OUTPUT_FILTER_GZIP)) {
        access_filter_add(OUTPUT_FILTER_GZIP, str);
      }
    }
  }
  
  fclose(fl);
}

/**
 * @param mime_type The filter to apply for output contents.
 * @param mime_type The mime type to filter output.
 */
void access_filter_add(char *filter, char *mime_type)
{
  cr_access_set(filter, mime_type);
}

int access_filter_allow(char *filter_type, char *mime_type)
{
  char *str;

  str = cr_access_get(filter_type);
  if (0 == strcasecmp(str, mime_type))
    return (1);

  return (0);
}

access_list *access_list_get(char *hostname)
{
  access_list *l;

  if (!virtualhost)
    hostname = server_name; /* limit to a single access list */


  for (l = _access_host_list; l; l = l->next) {
    if (0 == strcasecmp(l->hostname, hostname))
      return (l);
  }

  l = (access_list *)calloc(1, sizeof(access_list));
  strncpy(l->hostname, hostname, sizeof(l->hostname)-1);
  l->next = _access_host_list;
  _access_host_list = l;

  /* read in '.htaccess' file for host */
  _access_host_init(l);

  return (l);
}

int access_node_add(access_list *l, const char *pattern, enum access_type type)
{
  l->nodes = realloc(l->nodes, (l->n_access + 1) * sizeof (struct access_node));
  if (!l->nodes)
    return (-ENOMEM);

  l->nodes[l->n_access].type = type;
  l->nodes[l->n_access].host_pattern = strdup(pattern);
  if (!l->nodes[l->n_access].host_pattern)
    return (-ENOMEM);

  ++l->n_access;

  return (0);
}                               /* access_add */
int access_add(char *hostname, const char *pattern, enum access_type type)
{
  access_list *l; 

  l = access_list_get(hostname);
  if (!l)
    return (-ENOMEM);

  return (access_node_add(l, pattern, type));
}

enum access_type access_allow(char *hostname, char *origin, char *filename)
{
  access_list *l;
  int i;

  l = access_list_get(hostname);
  if (!l)
    return (-ENOMEM);

  /* find first match in allow / deny rules */
  for (i = 0; i < l->n_access; i++) {
    if (fnmatch(l->nodes[i].host_pattern, origin, 0) == 0) {
      return l->nodes[i].type;
    }
  }

  /* default to allow */
  return ACCESS_ALLOW;
}


