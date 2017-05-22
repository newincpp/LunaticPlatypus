#include "fileWatcher.hh"

FileWatcher::FileWatcher(const char* f_, bool watchStyle) : _still(true), callBack([]() {}) {
}

FileWatcher::FileWatcher(std::string& f_, bool watchStyle) : FileWatcher(f_.c_str(), watchStyle) {
}

void FileWatcher::fileChecker() {
}

//bool FileWatcher::isModified() {
//if (_length != 0) {
//    _length = 0;
//    return true;
//}
//    return false;
//}

FileWatcher::~FileWatcher() {
}
