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
        static std::thread::id _threadId;
	std::future<void> _renderThread;
        float _budget;
    public:
	RenderThread();
	~RenderThread();
	std::list<std::function<void(void)>> uniqueTasks;
	// inline T unsafeGetSubsystem() { return _subsystem }
	inline OglCore& unsafeGetRenderer() { return _renderer; }
        inline std::thread::id getThreadId() const { return _threadId; }

        void setKeepAlive(bool);
	void run();
};
