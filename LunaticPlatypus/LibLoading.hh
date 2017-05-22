#pragma once

#ifdef __linux__
#include <dlfcn.h>
#elif _WIN32
#include <Windows.h>
#endif

void *LoadLib(const char *filename_);

char *LoadErr();

void *LoadSym(void *handle_, const char *symbol_);

int UnloadLib(void *handle_);


