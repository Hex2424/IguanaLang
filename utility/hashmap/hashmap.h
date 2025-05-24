#ifndef HASHDICTC
#define HASHDICTC
#include <stdlib.h> /* malloc/calloc */
#include <stdint.h> /* uint32_t */
#include <stdbool.h>
#include <string.h> /* memcpy/memcmp */
#include <xmmintrin.h>

typedef int (*enumFunc)(void *key, int count, void *value, void *user);

#define HASHDICT_VALUE_TYPE void*
#define KEY_LENGTH_TYPE uint8_t

struct keynode {
	struct keynode *next;
	char *key;
	KEY_LENGTH_TYPE len;
	HASHDICT_VALUE_TYPE value;
};
		
struct dictionary {
	struct keynode **table;
	int length, count;
	double growth_treshold;
	double growth_factor;
	HASHDICT_VALUE_TYPE *value;
};

typedef struct dictionary Hashmap_t;
typedef Hashmap_t* HashmapHandle_t;

/* See README.md */

bool Hashmap_new(HashmapHandle_t dic, int initial_size);
void Hashmap_delete(HashmapHandle_t dic);
int Hashmap_set(HashmapHandle_t dic, const void *key, void* valueObject);
int Hashmap_add(HashmapHandle_t dic, const void *key, const int keyn);
int Hashmap_find(const HashmapHandle_t dic, const void *key, const int keyn);
bool Hashmap_forEach(const HashmapHandle_t dic, enumFunc f, const void *user);
uint64_t Hashmap_size(const HashmapHandle_t dic);
#endif
