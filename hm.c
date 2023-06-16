/* hm.c
   A program defining a hash map in C

   This program defines a simple hash table (or hash map)
   in C that uses open addressing and linear probing.

 */

#include "hm.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


typedef struct {
  const char *key;
  void *value;
} hm_entry;

struct hm {
  hm_entry* entries;
  size_t capacity;
  size_t length;
};

#define INITIAL_CAPACITY 64;

hm *hm_create(void)
{
  /* First, we allocate space for our hash map, then
     allocate space for our entries
  */

  hm *map = malloc(sizeof(hm));

  if (map == NULL) {
    return NULL;
  }

  map->length = 0;
  map->capacity = INITIAL_CAPACITY;

  map->entries = calloc(map->capacity, sizeof(hm_entry));
  if (map->entries == NULL) {
    free(map);
    return NULL;
  }

  return map;
}

void *hm_destroy(hm *map) {
  for (size_t i = 0; i < map->capacity; i++) {
    free((void*)(map->entries[i].key));
  }

  free(map->entries);
  free(map);
}

/* Here, we define a hash function djb2. Original code can
   be found at http://www.cse.yorku.ca/~oz/hash.html and
   is originally written by dan bernstein. More info can
   be found at https://theartincode.stanis.me/008-djb2/
   
 */

unsigned long
djb2(const char *str)
{
  unsigned long hash = 5381;
  int c;

  while (c = *str++)
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

void *hm_get(hm *map, const char *key) {
  // Make sure that the hash fits in the table
  unsigned long hash = djb2(key);
  size_t index = (size_t)(hash & (unsigned long)(map->capacity - 1));

  // Begin linear probing
  while (map->entries[index].key != NULL) {
    if (strcmp(key, map->entries[index].key) == 0)
      return map->entries[index].value;

    index++;
    if (index >= map->capacity)
      index = 0;
  }

  return NULL;
}


// Function to set an entry (private scope)
static const char *
hm_set_entry(hm_entry *entries, size_t capacity,
	     const char *key, void *value, size_t *plength)
{
  unsigned long hash = djb2(key);
  size_t index = (hash & (uint64_t)(capacity - 1));

  // Begin linear probing
  while (entries[index].key != NULL) {
    if (strcmp(key, entries[index].key) == 0) {
      entries[index].value = value;
      return entries[index].key;
    }

    index++;
    if (index >= capacity)
      index = 0;
  }

  // Key not found, insert in next null space
  if (plength != NULL) {
    key = strdup(key);
    if (key == NULL) {
      return NULL;
    }
    (*plength)++;
  }

  // Update map
  entries[index].key = (char *)key;
  entries[index].value = value;
  return key;
}

// Setter function (public scope)
const char *
hm_set(hm *map, const char *key, void *value) {
  assert(value != NULL);
  if (value == NULL) {
    return NULL;
  }

  return hm_set_entry(map->entries, map->capacity, key, value,
		      &map->length);
}

size_t hm_length(hm *map) {
  return map->length;
}
