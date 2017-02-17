#include <sys/types.h>
#include <unistd.h>
#include <sys/inotify.h>

FileWatcher::FileWatcher(const char* f_) : _still(true) {
    _fd = inotify_init();

    if (_fd < 0 ) {
	std::cout << "file \"" << f_ << "\" is not valid\n";
    }
    _wd = inotify_add_watch(_fd, f_, IN_MODIFY);

    t = std::thread([this]() {
	    while (_still) {
	    _length = read(_fd, _buffer, 4096);
	    std::cout << "change detected\n";
	    }
	    std::cout << "end\n";
	    });
}

FileWatcher::FileWatcher(std::string& f_)  : FileWatcher(f_.c_str()) {
}

bool FileWatcher::isModified() {
    if (_length != 0) {
	_length = 0;
	return true;
    }
    return false;
}

FileWatcher::~FileWatcher() {
    _still = false;
    inotify_rm_watch(_fd, _wd);
    close(_fd);
}
