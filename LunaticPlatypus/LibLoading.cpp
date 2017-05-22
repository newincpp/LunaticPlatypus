#include "LibLoading.hh"

void *
LoadLib(const char *filename_)
{
    void *retVal;

#ifdef __linux__
    retVal = dlopen(filename_, RTLD_NOW);
#elif _WIN32
    retVal = LoadLibraryA(filename_);
#endif
    return retVal;
}

char *
LoadErr()
{
    char *retVal;

#ifdef __linux__
    retVal = dlerror();
#elif _WIN32
    DWORD errVal = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errVal,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&retVal,
        0, NULL);
#endif
    return retVal;
}

void *
LoadSym(void *handle_, const char *symbol_)
{
    void *retVal;

#ifdef __linux__
#elif _WIN32
    retVal = GetProcAddress((HMODULE)handle_, symbol_);
#endif
    return retVal;
}

int
UnloadLib(void *handle_)
{
#ifdef __linux__
    dlclose(handle_);
#elif _WIN32
    FreeLibrary((HMODULE)handle_);
#endif
    return 0;
}
