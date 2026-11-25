#include <stddef.h>
#include <stdio.h>
#include <kv.h>
#include <stdlib.h>
#include <string.h>

#define TOMBSTONE 0x1

size_t hash(char *val, int capacity) {
  size_t hash = 0x13371337deadbeef;

  while(*val) {
    hash ^= *val;
    hash = hash << 8;
    hash += *val;

    val++;

  }

  return hash % capacity;
}



// fn kv_free 
// params: 
// - db: a pointer to the db 
// returns: 0 if the db is deleted successfully, otherwise 
// on error, returns -1
int kv_free(kv_t *db) {
  if (!db) return -1;

  // free keys, entries, and db memory 
  for (int i = 0; i < db->capacity - 1; i++) {

    kv_entry_t *entry = &db->entries[i];

    free (entry->key);
    free (entry->value);
  }

  free(db->entries);
  free(db);

  return 0;
}


// fn kv_delete
// params:
// - db: a pointer to the db
// - key: a pointer to the key value
// returns: the index of the key deleted, otherwise on 
// error, returns -1, on not found return -2
int kv_delete(kv_t *db, char *key) {
  if (!db || !key) return -1;

  size_t idx = hash(key, db->capacity);

  for (int i = 0; i < db->capacity -1; i++) {
    size_t real_idx = (idx + 1) % db->capacity;

    kv_entry_t *entry = &db->entries[real_idx];

    // If key is set, set to TOMBSTONE and return
    if (entry->key && 
      entry->key != (void*)TOMBSTONE &&
      !strcmp(entry->key, key)) {
        free(entry->key);
        free(entry->value);
        db->entry_count--;
        entry->key = (void*)TOMBSTONE;
        entry->value = NULL;

        return real_idx;
    }
      
    // If key is not set, or if it is already TOMBSTONE, fail
    if (entry->key == NULL) {
      return -1;
    }
  }
  return -1;
}

// fn kv_put
// params:
// - db: a pointer to the db
// - key: a pointer to the key value
// - value: a pointer to the value itself
// returns: the index of the key, otherwise on
// error, returns -1, on not found return -2
int kv_put(kv_t *db, char *key, char *value) {
  if (!db || !key || !value) return -1;

  size_t idx = hash(key, db->capacity);

  for (int i = 0; i < db->capacity - 1; i++) {

    size_t real_idx = (idx + i) % db->capacity;

    kv_entry_t *entry = &db->entries[real_idx];

    // the key is already set, updating
    if (entry->key && 
        entry->key != (void*)TOMBSTONE &&
        !strcmp(entry->key, key)) {
          char *newval = strdup(value);
          if (!newval) return -1;
          entry->value = newval;
          return real_idx;
        }
    
    // land in a slot that is "empty"
    if (!entry->key || entry->key == (void*)TOMBSTONE) {
      char *newval = strdup(value);
      char *newkey = strdup(key);
      if (!newval || !newkey) {
        free(newkey);
        free(newval);
        return -1;
      }
      entry->value = newval;
      entry->key = newkey;
      db->entry_count++;
      return real_idx;
    }
  }

  // the db is occpuied
  return -2;
}


// fn kv_get
// params:
// - db: a pointer to the db  
// - key: a pointer to the key value 
// returns: the index of the key, otherwise on 
// error, return -1, on not found return NULL
char * kv_get(kv_t *db, char *key) {
  if (!db || !key) return NULL;

  size_t idx = hash(key, db->capacity);

  for (int i = 0; i < db->capacity - 1; i++) {

    size_t real_idx = (idx + i) % db->capacity;

    kv_entry_t *entry = &db->entries[real_idx];

    // If entry is not found
    if (entry->key == NULL) return NULL;

    // the key matches
    if (entry->key && 
      entry->key != (void*)TOMBSTONE &&
      !strcmp(entry->key, key)) {
      return entry->value;
    }
  }
 return NULL;
} 


kv_t *kv_init(size_t capacity) {
  if (capacity == 0) return NULL;

  kv_t *table = malloc(sizeof(kv_t));
  if (table == NULL) {
    return NULL;
  }
    
  table->capacity = capacity;
  table->entry_count = 0;

  table->entries = calloc(sizeof(kv_entry_t), capacity);
  if (table->entries == NULL) {
    return NULL;
  }

  return table;
}

