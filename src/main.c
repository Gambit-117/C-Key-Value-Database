#include <stdio.h>
#include <kv.h>

kv_t *kv_test(void *p_kv_table) {
  kv_t *db = kv_init(16);
  printf("%ld %ld\n", db->capacity, db->entry_count);
  return db;
}

int main() {
  kv_t *table = kv_init(3);
  kv_put(table, "hehe", "haha"); 

  for (int i = 0; i < table->capacity; i++){
    if (table->entries[i].key) {
      printf("[%d] %s: %s\n", 
          i,
          table->entries[i].key, 
          table->entries[i].value);
    }
  }

  char *val = kv_get(table, "hehe");
  char *val2 = kv_get(table, "haha");

  printf("%s %s\n", val, val2);

  kv_delete(table, "hehe");
  val = NULL;
  printf("%s %s\n", val, val2);
  val = kv_get(table, "hehe");

  printf("%s %s\n", val, val2);

  kv_free(table);
}
