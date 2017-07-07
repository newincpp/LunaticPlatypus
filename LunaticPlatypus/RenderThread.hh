#include <future>
#include <functional>
#include <list>
#include "OglCore.hh"
#include "WindowHandle.hh"

// will maybe become template<typename T> ParallelSubsystem
class RenderThread {
    private:
	std::mutex m;
	bool keepAlive;
	// T _subsystem
	OglCore _renderer;
	// static void _subsystemLogic(ParallelSubsystem*);
	static void _renderLogic(RenderThread*);
	std::future<void> _renderThread;
    public:
	RenderThread();
	~RenderThread();
	std::list<std::function<void(void)>> uniqueTasks;
	// inline T unsafeGetSubsystem() { return _subsystem }
	inline OglCore& unsafeGetRenderer() { return _renderer; }
        void setKeepAlive(bool);
	void run();
};
