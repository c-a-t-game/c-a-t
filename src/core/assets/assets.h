#ifndef ASSETS_H
#define ASSETS_H

#define get_asset(T, name) ((typeof(T)*)_get_asset(name))

void load_assets();
void* _get_asset(const char* name);

#endif