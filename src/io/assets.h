#ifndef ASSETS_H
#define ASSETS_H

void load_assets();
void* _get_asset(const char* name);

#define get_asset(type, name) (typeof(type)*)_get_asset(name)

#endif
