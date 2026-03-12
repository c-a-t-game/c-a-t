#include "storage.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#ifdef _WIN32
#define BINARY "b"
#else
#define BINARY
#endif

#define FILENAME "saves.bin"

typedef struct {
    char* name;
    void* data;
    size_t data_size;
} StorageSlotEntry;

struct StorageSlot {
    uint32_t size, capacity;
    StorageSlotEntry* entries;
};

static uint32_t num_storages, storages_capacity;
static StorageSlot** storages;

static int compare_string(const void* a, const void* b) {
    return strcmp(*(char**)a, *(char**)b);
}

static FILE* open_savefile(const char* mode) {
    FILE* f = fopen(FILENAME, mode);
    if (!f) fprintf(stderr, "Error opening '%s': %s\n", FILENAME, strerror(errno));
    return f;
}

void storage_save() {
    char name[256];
    FILE* f = open_savefile("w" BINARY);
    if (!f) return;
    fwrite(&num_storages, sizeof(uint32_t), 1, f);
    for (uint32_t i = 0; i < storages_capacity; i++) {
        StorageSlot* slot = storages[i];
        if (!slot) continue;
        fwrite(&slot->size, sizeof(uint32_t), 1, f);
        for (uint32_t j = 0; j < slot->size; j++) {
            StorageSlotEntry* entry = &slot->entries[j];
            fwrite(entry->name, strlen(entry->name) + 1, 1, f);
            fwrite(&entry->data_size, sizeof(uint32_t), 1, f);
            fwrite(entry->data, entry->data_size, 1, f);
        }
    }
}

void storage_init() {
    static bool inited;
    if (inited) return;
    inited = true;

    atexit(storage_save);

    char name[256];
    FILE* f = open_savefile("r" BINARY);
    if (!f) return;
    uint32_t storage_count;
    if (fread(&storage_count, sizeof(uint32_t), 1, f) < 1) goto error;
    for (uint32_t i = 0; i < storage_count; i++) {
        StorageSlot* slot = storage_add_slot();
        uint32_t num_entries;
        if (fread(&num_entries, sizeof(uint32_t), 1, f) < 1) goto error;
        for (uint32_t i = 0; i < num_entries; i++) {
            int c;
            int ptr = 0;
            while ((c = fgetc(f)) >= 1) if (ptr < 255) name[ptr++] = c;
            name[ptr] = 0;
            uint32_t data_size;
            if (fread(&data_size, sizeof(uint32_t), 1, f) < 1) goto error;
            void* data = storage_get(slot, name, data_size);
            if (fread(data, data_size, 1, f) < 1) goto error;
        }
    }
    goto success;
    error:
    for (uint32_t i = 0; i < storages_capacity; i++) {
        if (storages[i]) storage_remove_slot(storages[i]);
    }
    free(storages);
    storages = NULL;
    success:
    fclose(f);
    if (num_storages == 0) storage_add_slot();
}

StorageSlot* storage_get_slot(uint32_t index) {
    if (index >= num_storages) return NULL;
    return storages[index];
}

StorageSlot* storage_add_slot() {
    if (num_storages >= storages_capacity) {
        uint32_t old_capacity = storages_capacity;
        if (storages_capacity == 0) storages_capacity = 4;
        else storages_capacity *= 2;
        storages = realloc(storages, sizeof(StorageSlot*) * storages_capacity);
        memset(storages + old_capacity, 0, (storages_capacity - old_capacity) * sizeof(StorageSlot*));
    }
    num_storages++;
    for (uint32_t i = 0; i < storages_capacity; i++) {
        if (storages[i] != NULL) continue;
        StorageSlot* slot = malloc(sizeof(StorageSlot));
        slot->size = 0;
        slot->capacity = 4;
        slot->entries = malloc(sizeof(StorageSlotEntry) * slot->capacity);
        return storages[i] = slot;
    }
    __builtin_unreachable();
}

void storage_remove_slot(StorageSlot* slot) {
    for (uint32_t j = 0; j < slot->size; j++) {
        free(slot->entries[j].name);
        free(slot->entries[j].data);
    }
    free(slot->entries);
    free(slot);

    uint32_t index = 0;
    for (; index < storages_capacity; index++) {
        if (storages[index] == slot) break;
    }
    if (index == storages_capacity) return;
    num_storages--;
    storages[index] = NULL;
}

void* storage_get(StorageSlot* storage, const char* name, size_t size) {
    StorageSlotEntry* entry = bsearch(&name, storage->entries, storage->size, sizeof(StorageSlotEntry), compare_string);
    if (!entry) {
        if (storage->size >= storage->capacity) {
            storage->capacity *= 2;
            storage->entries = realloc(storage->entries, sizeof(StorageSlotEntry) * storage->size);
        }
        entry = &storage->entries[storage->size++];
        entry->name = strdup(name);
        entry->data = calloc(size, 1);
        entry->data_size = size;
        qsort(storage->entries, storage->size, sizeof(StorageSlotEntry), compare_string);
    }
    return entry->data;
}

uint32_t storage_num_slots() {
    return num_storages;
}
