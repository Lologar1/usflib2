#include "usfhashmap.h"

uint64_t usf_strhash(const char *str) {
    uint64_t hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

uint64_t usf_hash(uint64_t val) {
	val ^= val >> 33;
	val *= 0xFF51AFD7ED558CCD; //Prime
	val ^= val >> 33;
	val *= 0xA635194A4D16E3CB; //Prime
	val ^= val >> 33;
	return val;
}

usf_hashmap *usf_strhmput(usf_hashmap *hashmap, char *key, usf_data value) {
	uint64_t i, hash;
	usf_data *entry;

	if (hashmap == NULL) { //Initialize if non-existent
		hashmap = malloc(sizeof(usf_hashmap));
		hashmap -> size = 0; //Empty at start
		hashmap -> capacity = USF_HASHMAP_DEFAULTSIZE;
		hashmap -> array = calloc(sizeof(usf_data **), USF_HASHMAP_DEFAULTSIZE);
	}

	if (key == NULL) return hashmap; //Cannot put at null

	if (hashmap -> size + 1 > hashmap -> capacity / 2) //3/4 value hard-coded for now
		usf_resizehm(hashmap, hashmap -> capacity * USF_HASHMAP_RESIZE_MULTIPLIER);

	i = usf_strhash(key);

	for (;; i = usf_hash(i)) {
		hash = i % hashmap -> capacity;

		entry = hashmap -> array[hash]; //Element already present
		if (entry == NULL || entry[0].p == NULL || !strcmp((char *) entry[0].p, key)) {
			//Empty node, can put here (key : value)
			if (entry == NULL) {
				entry = hashmap -> array[hash] = calloc(sizeof(usf_data *), 2);
			}

			if (entry[0].p == NULL) {
				//Key (+1 for terminating \0)
				entry[0] = (usf_data) { .p = malloc(sizeof(char) * (strlen(key) + 1)) };
				strcpy(entry[0].p, key);
				hashmap -> size++;
			}

			entry[1] = value;
			break; //Successfully put
		}
	}

	return hashmap;
}

usf_data usf_strhmget(usf_hashmap *hashmap, char *key) {
	uint64_t i, hash;
	usf_data *entry;

	if (hashmap == NULL) return (usf_data) { .p = NULL };

	i = usf_strhash(key);

	for (;; i = usf_hash(i)) {
		hash = i % hashmap -> capacity;

		entry = hashmap -> array[hash];

		if (entry == NULL) //Not present
			return (usf_data) { .p = NULL };

		if (entry[0].p == NULL || strcmp(key, (char *) entry[0].p))
			continue; //Collision case

		return entry[1];
	}
}

usf_data usf_strhmdel(usf_hashmap *hashmap, char *key) {
	uint64_t i, hash;
	usf_data *entry, v;

	if (hashmap == NULL) return USFNULL;

	i = usf_strhash(key);

	for (;; i = usf_hash(i)) {
		hash = i % hashmap -> capacity;

		entry = hashmap -> array[hash];

		if (entry == NULL)
			return USFNULL;

		if (entry[0].p == NULL || strcmp(key, (char *) entry[0].p))
			continue;

		hashmap -> size--; //Decrement count
		v = entry[1]; //Get data
		free(entry[0].p);
		entry[0] = USFNULL; //Destroy key
		return v;
	}
}

void usf_resizehm(usf_hashmap *hashmap, uint64_t size) {
	uint64_t i, j, hash;
	usf_data **oldarray, **newarray;

	if (hashmap == NULL || hashmap -> capacity >= size) return;

	oldarray = hashmap -> array;
	newarray = calloc(sizeof(usf_data **), size);

	for (j = 0; j < hashmap -> capacity; j++) {
		if (oldarray[j] == NULL || oldarray[j][0].p == NULL)
			continue; //Dead or uninitialized

		i = usf_strhash(oldarray[j][0].p); //Key

		for (;; i = usf_hash(i)) {
			hash = i % size;

			if (newarray[hash] == NULL) {
				newarray[hash] = oldarray[j];
				break;
			}
		}
	}

	hashmap -> array = newarray;
	hashmap -> capacity = size;
	free(oldarray); //Contents copied
}

void usf_freestrhm(usf_hashmap *hashmap) {
	uint64_t i;
	usf_data **array = hashmap -> array;
	usf_data *entry;

	for (i = 0; i < hashmap -> capacity; i++) {
		entry = array[i];
		if (entry == NULL) continue; //Uninitialized

		free(entry[0].p); //Free key pointer
		free(entry); //Free key:val pair
	}

	free(array); //Final free of entire array
	free(hashmap);
}

void usf_freehm(usf_hashmap *hashmap) {
	uint64_t i;
	usf_data **array = hashmap -> array;
	usf_data *entry;

	for (i = 0; i < hashmap -> capacity; i++) {
		entry = array[i];
		if (entry == NULL) continue; //Uninitialized

		free(entry); //Free key:val pair
	}

	free(array); //Final free of entire array
	free(hashmap);
}
