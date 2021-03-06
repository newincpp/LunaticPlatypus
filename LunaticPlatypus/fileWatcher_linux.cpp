#include <poll.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <stdio.h>

#include <unistd.h>

FileWatcher::FileWatcher(const char* f_) : _still(true), callBack([](){}) {
    //_fd = inotify_init();
    _fd = inotify_init1(IN_NONBLOCK);

    if (_fd < 0 ) {
	std::cout << "file \"" << f_ << "\" is not valid\n";
	return;
    }
    _wd = inotify_add_watch(_fd, f_, IN_CLOSE_WRITE);

    _t = std::thread(std::bind(&FileWatcher::fileChecker, this));
}

FileWatcher::FileWatcher(std::string& f_)  : FileWatcher(f_.c_str()) {
}

void FileWatcher::fileChecker() {
    struct pollfd ufds = {_fd, POLLIN, 0};
    struct inotify_event e;
    while (_still) {
	int r = poll(&ufds, 1, 200); // 500ms timeout
	if (r == -1) { // error
	    std::cout << "error occured\n";
	} else if (r == 0) { // timeout
	    //std::cout << "timeout\n";
	} else {
	    int len = 0;
	    len = read(_fd, &e, sizeof(struct inotify_event));
	    if (len != -1) {
		callBack();
	    } else {
                //std::cout << "nothing wrote\n" << std::endl;
            }
	    while (-1 != (len = read(_fd, &e, sizeof(struct inotify_event)))) {
                //std::cout << "empty events :" << len << "\n";
	    }
	}
    }
    std::cout << "end\n";
}

//bool FileWatcher::isModified() {
    //if (_length != 0) {
    //    _length = 0;
    //    return true;
    //}
//    return false;
//}

FileWatcher::~FileWatcher() {
    _still = false;
    inotify_rm_watch(_fd, _wd);
    close(_fd);
}
