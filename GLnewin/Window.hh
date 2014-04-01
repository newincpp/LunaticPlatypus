#ifndef WINDOW_H_
# define WINDOW_H_

//#include "IRendertarget.hh"

#include <string>
#include <vector>
#include <array>
#include <thread>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "GL.h"

namespace GLnewin {
    int ctxErrorHandler(Display *dpy, XErrorEvent *ev);

    class Window : public IGLContext {
	private:
#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
	    typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, bool, const int*);

	    Display* _display;
	    ::Window _win;
	    GLXContext _ctx;
	    Colormap _cmap;
	    std::string _name;
	    bool _fullscreen;
	public:
	    explicit Window(unsigned long, unsigned long, bool, const std::string&);
	    explicit Window(const Window*) = delete;
	    explicit Window(Window&&) = delete;
	    ~Window();

	    virtual void initalize(unsigned long, unsigned long);
	    virtual void getSize() const noexcept;

	    void flush() const noexcept;
	    void clear() const noexcept;

	    bool isExtensionSupported(const std::string& extList, const std::string& extension);
	    void setName(const std::string&) noexcept;
    };
}

#endif /* !WINDOW_H_*/
