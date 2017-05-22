#include "fileWatcher.hh"

#ifdef __linux__
#include "fileWatcher_linux.cpp"
#elif __APPLE__
#include "fileWatcher_osx.cpp"
#elif _Win32
#include "fileWatcher_win32.cpp"
#endif
