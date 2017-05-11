#pragma once
#include <thread>
#include <future>
#include <iostream>

#define EVENT_BUF_LEN   4096  

class FileWatcher {
    private:
	int _fd;
	int _wd;
	bool _still;
	std::thread _t;
	void fileChecker();
    public:
	std::function<void(void)> callBack;
	//void(*_callBack)(void);
	FileWatcher(std::string&, bool = true);
	FileWatcher(const char*, bool = true);
	//bool isModified();
	~FileWatcher();
};
