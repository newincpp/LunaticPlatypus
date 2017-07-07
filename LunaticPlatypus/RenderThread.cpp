#include <iostream>
#ifdef IMGUIENABLED
#include "imgui/imgui.h"
#endif
#include "RenderThread.hh"

RenderThread::RenderThread() : keepAlive(true) {
}

RenderThread::~RenderThread() {
    //if (_subsystem.valid()) {
    //    _subsystem.wait();
    //}

    if (_renderThread.valid()) {
	_renderThread.wait();
    }
}

// void ParallelSubsystem::_subsystemLogic(ParallelSubsystem*);
void RenderThread::_renderLogic(RenderThread* t_) {
    std::cout << "RenderThread id: " << std::this_thread::get_id() << '\n';
    while (t_->keepAlive) {
#ifdef IMGUIENABLED
	ImGui::NewFrame();
#endif

        std::list<std::function<void(void)>>::iterator endStamp = t_->uniqueTasks.end(); // to avoid looping if permanently adding tasks
        std::list<std::function<void(void)>>::iterator lastTask;
        for (std::list<std::function<void(void)>>::iterator task = t_->uniqueTasks.begin(); task != endStamp;) {
            (*task)();
            lastTask = task;
            task++;
            t_->uniqueTasks.erase(lastTask); // according to the cppref iterators aren't invalidated after an erase
        }
	t_->_renderer.render();

#ifdef IMGUIENABLED
	//ImGui::Text("\nApplication average %f ms/frame (%.1f FPS)", deltaTime, 1000.0/double(std::chrono::duration_cast<std::chrono::milliseconds>(endFrame-beginFrame).count()));
	ImGui::Render();
#endif
    }
}

void RenderThread::run() {
    //_subsystem = std::async(std::launch::async, ParallelSubsystem::_subsystemLogic, this);
    _renderThread = std::async(std::launch::async, RenderThread::_renderLogic, this);
}

void RenderThread::setKeepAlive(bool alive) {
    keepAlive = alive;
}
