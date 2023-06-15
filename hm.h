/* Defining hash map in C

   Using several Internet resources, here is my implementation
   of a hash map in C. It declares the hm struct and related
   functions, and uses open addressing and linear probing.

   Because of the nature of this hash map, there is no remove
   function for entries, as well as no delete map function.
   The map is created upon startup based on the .dfa file
   provided by the user.

   This header file declares a struct, hm, that represents
   the hash map created at the beginning of the program. It
   then declares a create, get, set, and length method for
   the map.
  
 */

typedef struct hm hm;

hm *hm_create(void);

void *hm_get(hm *map, const char *key);

const chat *hm_set(hm *map, const char *key, void *value);

size_t hm_length(hm *map);
