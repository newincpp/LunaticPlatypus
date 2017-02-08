// only work on linux
#include <sys/types.h>
#include <unistd.h>
#include <sys/inotify.h>

#include <poll.h>

#include "fileWatcher.hh"


FileWatcher::FileWatcher(const char* f_) : _still(true) {
    _fd = inotify_init();

    if (_fd < 0 ) {
	std::cout << "file \"" << f_ << "\" is not valid\n";
	return;
    }
    _wd = inotify_add_watch(_fd, f_, IN_MODIFY);

    t = std::thread([this]() {
	    struct pollfd pfd = { _fd, POLLIN, 0 };
	    while (_still) {
	    	int ret = poll(&pfd, 1, 100);
		if (ret > 0) {
			_length = read(_fd, _buffer, 4096);
			std::cout << "change detected\n";
		}
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
    t.join();
    if (_fd < 0) {
	inotify_rm_watch(_fd, _wd);
	close(_fd);
    }
}
