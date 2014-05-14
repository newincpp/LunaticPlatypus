#include <iostream>
#include <unistd.h>
#include <stack>
#include "IRender.hh"
#include "Window.hh"

GLnewin::Window::Window(unsigned long sizeX, unsigned long sizeY, bool fullscreen, const std::string& name) :  _win(), _ctx(0), _name(name), _fullscreen(fullscreen) {
    initalize(sizeX, sizeY);
}

void GLnewin::Window::initalize(unsigned long sizeX, unsigned long sizeY) {
    _display = XOpenDisplay(NULL);

    if (!_display) {
	std::cout << "Failed to open X display" << std::endl;
	return;
    }

    // Get a matching FB config
    static int visual_attribs[] = {
	GLX_X_RENDERABLE    , True,
	GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
	GLX_RENDER_TYPE     , GLX_RGBA_BIT,
	GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
	GLX_RED_SIZE        , 8,
	GLX_GREEN_SIZE      , 8,
	GLX_BLUE_SIZE       , 8,
	GLX_ALPHA_SIZE      , 8,
	GLX_DEPTH_SIZE      , 24,
	GLX_STENCIL_SIZE    , 8,
	GLX_DOUBLEBUFFER    , True,
	//GLX_SAMPLE_BUFFERS  , 1,
	//GLX_SAMPLES         , 4,
	None
    };

    int glx_major, glx_minor;

    // FBConfigs were added in GLX version 1.3.
    if ( !glXQueryVersion(_display, &glx_major, &glx_minor ) || ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) ) {
	std::cout << "Invalid GLX version" << std::endl;
	return;
    }

    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(_display, DefaultScreen(_display), visual_attribs, &fbcount);
    if (!fbc) {
	std::cout <<  "Failed to retrieve a framebuffer config" << std::endl;
	return;
    }

    // Pick the FB config/visual with the most samples per pixel
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    for (int i = 0; i < fbcount; ++i) {
	XVisualInfo *vi = glXGetVisualFromFBConfig(_display, fbc[i] );
	if (vi) {
	    int samp_buf, samples;
	    glXGetFBConfigAttrib(_display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
	    glXGetFBConfigAttrib(_display, fbc[i], GLX_SAMPLES       , &samples  );
	    if ( best_fbc < 0 || (samp_buf && samples > best_num_samp))
		best_fbc = i, best_num_samp = samples;
	    if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
		worst_fbc = i, worst_num_samp = samples;
	}
	XFree(vi);
    }

    GLXFBConfig bestFbc = fbc[ best_fbc ];

    // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
    XFree(fbc);

    // Get a visual
    XVisualInfo *vi = glXGetVisualFromFBConfig(_display, bestFbc);

    XSetWindowAttributes swa;
    swa.colormap = _cmap = XCreateColormap(_display, RootWindow(_display, vi->screen ), vi->visual, AllocNone);
    swa.background_pixmap = None ;
    swa.border_pixel      = 0;
    swa.event_mask        = StructureNotifyMask;
    swa.override_redirect = _fullscreen;

    _win = XCreateWindow(_display, RootWindow(_display, vi->screen ), 0, 0, sizeX, sizeY, 0, vi->depth, InputOutput, vi->visual, (CWOverrideRedirect * _fullscreen)|CWBorderPixel|CWColormap|CWEventMask, &swa );
    if (!_win) {
	std::cout << "Failed to create window." << std::endl;
	exit(1);
    }

    // Done with the visual info data
    XFree(vi);

    XStoreName(_display, _win, _name.c_str());

    XMapWindow(_display, _win);

    // Get the default screen's GLX extension list
    const char *glxExts = glXQueryExtensionsString(_display, DefaultScreen(_display));

    // NOTE: It is not necessary to create or make current to a context before
    // calling glXGetProcAddressARB
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");

    // Check for the GLX_ARB_create_context extension string and the function.
    // If either is not present, use GLX 1.3 context creation method.
    if(!isExtensionSupported(glxExts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) {
	std::cout << "glXCreateContextAttribsARB() not found... using old-style GLX context" << std::endl;
	_ctx = glXCreateNewContext(_display, bestFbc, GLX_RGBA_TYPE, 0, True);
    }

    // If it does, try to get a GL 3.0 context!
    else {
	int context_attribs[] = {
	    0x2091, 3, // 0x2091 = GLX_CONTEXT_MAJOR_VERSION_ARB
	    0x2092, 0, // 0x2091 = GLX_CONTEXT_MINOR_VERSION_ARB
	    //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
	    None
	};

	_ctx = glXCreateContextAttribsARB(_display, bestFbc, 0, True, context_attribs);

	// Sync to ensure any errors generated are processed.
	XSync(_display, False);
    }
    // Sync to ensure any errors generated are processed.
    XSync(_display, False);
    // Restore the original error handler
    // Verifying that context is a direct context
    if (!glXIsDirect (_display, _ctx)) {
	std::cout << "Indirect GLX rendering context obtained" << std::endl;
    } else {
	std::cout << "Direct GLX rendering context obtained" << std::endl;
    }
    std::cout << "Making context current" << std::endl;
    glXMakeCurrent(_display, _win, _ctx );
    glewInit();

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "runing on: " << glGetString(GL_VENDOR) << std::endl;
    //std::cout << "render type: " << glGetString(GL_RENDER) << std::endl;
    std::cout << "Shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

}

GLnewin::Window::~Window() {
    glXMakeCurrent(_display, 0, 0 );
    glXDestroyContext(_display, _ctx );

    XDestroyWindow(_display, _win);
    XFreeColormap(_display, _cmap);
    XCloseDisplay(_display);
}

GLnewin::IGLContext::Size GLnewin::Window::getSize() const noexcept {
    return Size{1920, 1080};
}

void GLnewin::Window::flush() const noexcept {
    glXSwapBuffers(_display, _win);
}

void GLnewin::Window::clear() const noexcept {
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
}


bool GLnewin::Window::isExtensionSupported(const std::string& extList, const std::string& extension) {
    if (extList.find(extension) != std::string::npos) {
	return true;
    } else {
	return false;
    }
}

void GLnewin::Window::setName(const std::string& nnew) noexcept {
    XStoreName(_display, _win, nnew.c_str());
    _name = nnew;
}
