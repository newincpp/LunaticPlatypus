#pragma once
#include <thread>
#include <future>
#include <iostream>

#define EVENT_BUF_LEN   4096  

class FileWatcher {
    private:
	int _length;
	int _fd;
	int _wd;
	bool _still;
	char _buffer[4096];
	std::thread t;
    public:
	//void(*_callBack)(void);
	FileWatcher(std::string&);
	FileWatcher(const char*);
	bool isModified();
	~FileWatcher();
};
