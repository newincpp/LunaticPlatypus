#include "fileWatcher.hh"

#ifdef __linux__
#include "fileWatcher_linux.cpp"
#elif __APPLE__
#include "fileWatcher_osx.cpp"
#endif
