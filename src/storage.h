#ifndef STORAGE_H
#define STORAGE_H

#include <stdlib.h>
#include <stdint.h>

typedef struct StorageSlot StorageSlot;

void storage_init();

StorageSlot* storage_get_slot(uint32_t index);
StorageSlot* storage_add_slot();
void storage_remove_slot(StorageSlot* slot);
void* storage_get(StorageSlot* storage, const char* name, size_t size);
uint32_t storage_num_slots();

#endif
