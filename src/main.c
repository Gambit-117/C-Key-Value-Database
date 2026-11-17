#include <stdio.h>
#include <kv.h>

kv_t *kv_test(void *p_kv_table) {
  kv_t *db = kv_init(16);
  printf("%p %p\n", db->capacity, db->entry_count);
  return db;
}

int main() {
  kv_t *table = kv_init(3);
  kv_test(table);
  
}
