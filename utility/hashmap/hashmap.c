#include "hashmap.h"
#include "../misc/safety_macros.h"
#define hash_func meiyan

static inline uint32_t meiyan(const char *key, int count) {
	typedef uint32_t* P;
	uint32_t h = 0x811c9dc5;
	while (count >= 8) {
		h = (h ^ ((((*(P)key) << 5) | ((*(P)key) >> 27)) ^ *(P)(key + 4))) * 0xad3e7;
		count -= 8;
		key += 8;
	}
	#define tmp h = (h ^ *(uint16_t*)key) * 0xad3e7; key += 2;
	if (count & 4) { tmp tmp }
	if (count & 2) { tmp }
	if (count & 1) { h = (h ^ *key) * 0xad3e7; }
	#undef tmp
	return h ^ (h >> 16);
}

struct keynode *keynode_new(char*k, int l) {
	struct keynode *node = malloc(sizeof(struct keynode));
	node->len = l;
	node->key = malloc(l);
	memcpy(node->key, k, l);
	node->next = 0;
	node->value = -1;
	return node;
}

void keynode_delete(struct keynode *node) {
	free(node->key);
	if (node->next) keynode_delete(node->next);
	free(node);
}

bool Hashmap_new(HashmapHandle_t dic, int initial_size) {
	if (initial_size == 0) initial_size = 1024;
	dic->length = initial_size;
	dic->count = 0;
	dic->table = calloc(sizeof(struct keynode*), initial_size);
	if(dic->table == NULL)
	{
		return ERROR;
	}
	dic->growth_treshold = 2.0;
	dic->growth_factor = 10;
	
	return SUCCESS;
}

void Hashmap_delete(HashmapHandle_t dic) {
	for (int i = 0; i < dic->length; i++) {
		if (dic->table[i])
			keynode_delete(dic->table[i]);
	}
	free(dic->table);
	dic->table = 0;
	free(dic);
}

void Hashmap_reinsert_when_resizing(HashmapHandle_t dic, struct keynode *k2) {
	int n = hash_func(k2->key, k2->len) % dic->length;
	if (dic->table[n] == 0) {
		dic->table[n] = k2;
		dic->value = &dic->table[n]->value;
		return;
	}
	struct keynode *k = dic->table[n];
	k2->next = k;
	dic->table[n] = k2;
	dic->value = &k2->value;
}

void Hashmap_resize(HashmapHandle_t dic, int newsize) {
	int o = dic->length;
	struct keynode **old = dic->table;
	dic->table = calloc(sizeof(struct keynode*), newsize);
	dic->length = newsize;
	for (int i = 0; i < o; i++) {
		struct keynode *k = old[i];
		while (k) {
			struct keynode *next = k->next;
			k->next = 0;
			Hashmap_reinsert_when_resizing(dic, k);
			k = next;
		}
	}
	free(old);
}

int Hashmap_set(HashmapHandle_t dic, void *key, void* valueObject)
{
	int keyLength = strlen(key);
	int result = Hashmap_add(dic, key, keyLength);

	*(dic->value) = valueObject;

	return result;
}


int Hashmap_add(HashmapHandle_t dic, void *key, int keyn) {
	int n = hash_func((const char*)key, keyn) % dic->length;
	if (dic->table[n] == 0) {
		double f = (double)dic->count / (double)dic->length;
		if (f > dic->growth_treshold) {
			Hashmap_resize(dic, dic->length * dic->growth_factor);
			return Hashmap_add(dic, key, keyn);
		}
		dic->table[n] = keynode_new((char*)key, keyn);
		dic->value = &dic->table[n]->value;
		dic->count++;
		return 0;
	}
	struct keynode *k = dic->table[n];
	while (k) {
		if (k->len == keyn && memcmp(k->key, key, keyn) == 0) {
			dic->value = &k->value;
			return 1;
		}
		k = k->next;
	}
	dic->count++;
	struct keynode *k2 = keynode_new((char*)key, keyn);
	k2->next = dic->table[n];
	dic->table[n] = k2;
	dic->value = &k2->value;
	return 0;
}

int Hashmap_find(HashmapHandle_t dic, void *key, int keyn) {
	int n = hash_func((const char*)key, keyn) % dic->length;
    #if defined(__MINGW32__) || defined(__MINGW64__)
	__builtin_prefetch(gc->table[n]);
    #endif
    
    #if defined(_WIN32) || defined(_WIN64)
    _mm_prefetch((char*)gc->table[n], _MM_HINT_T0);
    #endif
	struct keynode *k = dic->table[n];
	if (!k) return 0;
	while (k) {
		if (k->len == keyn && !memcmp(k->key, key, keyn)) {
			dic->value = &k->value;
			return 1;
		}
		k = k->next;
	}
	return 0;
}

void Hashmap_forEach(HashmapHandle_t dic, enumFunc f, void *user) {
	for (int i = 0; i < dic->length; i++) {
		if (dic->table[i] != 0) {
			struct keynode *k = dic->table[i];
			while (k) {
				
				if (!f(k->key, k->len, k->value, user)) return;
				k = k->next;
			}
		}
	}
}

inline uint64_t Hashmap_size(HashmapHandle_t dic)
{
	return dic->count;
}

#undef hash_func
