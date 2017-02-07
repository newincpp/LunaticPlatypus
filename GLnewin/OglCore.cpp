#include <ctime>
#include <stack>
#include "OglCore.hh"

#include <imgui.h>

void getGlError(const char* file_, unsigned long line_) {
    GLenum e = glGetError();
    if (e != GL_NO_ERROR) {
	std::cout << "in file: \033[0;1m" << file_ << "\033[0m line: \033[1m" << line_ << "\033[0m: "; 
	if (e == GL_INVALID_ENUM) 				{ std::cout << "\033[31mGL_INVALID_ENUM\033[0m: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.\n"; }
	else if (e == GL_INVALID_VALUE) 			{ std::cout << "\033[31mGL_INVAGL_INVALID_VALUE\033[0m: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.\n"; }
	else if (e == GL_INVALID_OPERATION) 			{ std::cout << "\033[31mGL_INVAGL_INVALID_OPERATION\033[0m: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.\n"; }
	else if (e == GL_INVALID_FRAMEBUFFER_OPERATION) 	{ std::cout << "\033[31mGL_INVAGL_INVALID_FRAMEBUFFER_OPERATION\033[0m: The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.\n"; }
	else if (e == GL_OUT_OF_MEMORY) 			{ std::cout << "\033[31mGL_INVAGL_OUT_OF_MEMORY\033[0m: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n"; }
	else if (e == GL_STACK_UNDERFLOW) 			{ std::cout << "\033[31mGL_INVAGL_STACK_UNDERFLOW\033[0m: An attempt has been made to perform an operation that would cause an internal stack to underflow.\n"; }
	else if (e == GL_STACK_OVERFLOW) 			{ std::cout << "\033[31mGL_INVAGL_STACK_OVERFLOW\033[0m: An attempt has been made to perform an operation that would cause an internal stack to overflow.\n"; }
    }
}

OglCore::OglCore() : uTime(1, 0.0f), _uPostPRocessTexture(2) { 
    init();
}

void OglCore::init() {
    _beginTime = std::chrono::high_resolution_clock::now();

    checkGlError glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    std::vector<GLfloat> vertices = {
	//vertexPos		//normal		//uvCoord
	-1.0f,  1.0f,5.5f, 	0.0f,0.0f,1.0f, 	0.0f,1.0f, // Top-left
	1.0f,  1.0f, 5.5f, 	0.0f,0.0f,1.0f, 	1.0f,1.0f, // Top-right
	1.0f, -1.0f, 5.5f, 	0.0f,0.0f,1.0f, 	1.0f,0.0f, // Bottom-right
	-1.0f, -1.0f, 5.5f, 	0.0f,0.0f,1.0f, 	0.0f,0.0f  // Bottom-left
    };

    std::vector<GLuint> elements = {
	0, 1, 2,
	2, 3, 0
    };

    _sgBuffer.add("./fragGBuffer.glsl", GL_FRAGMENT_SHADER);
    _sgBuffer.add("./vertGBuffer.glsl", GL_VERTEX_SHADER);
    _sgBuffer.link({"gPosition", "gNormal", "gAlbedoSpec"});

    _sPostProc.add("./postProcess.glsl",GL_FRAGMENT_SHADER);
    _sPostProc.add("./postProcessVert.glsl",GL_VERTEX_SHADER);
    _sPostProc.link({"outColour"});

    Mesh m;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fractalTex);
    glBindTexture(GL_TEXTURE_2D, fractalTex);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16UI, 512, 512); // int
    glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA16F, 512, 512); // float
    checkGlError;
    glBindTexture(GL_TEXTURE_2D, 0);
    _s._fb.emplace_back();
    _s._fb[0].addBuffer("gPosition");
    _s._fb[0].addBuffer("gNormal");
    _s._fb[0].addBuffer("gAlbedoSpec");
    _s._fb[0].addDepthBuffer("gDepth");
    _s._fb[0].enable();
    m.uploadToGPU(vertices, elements);
    _s._meshes.push_back(m);
    _s._cameras.emplace_back(_s._fb[0]);

    _renderTarget.uploadToGPU(vertices, elements);

    _sgBuffer.use();
    autoRelocate(uTime);
    std::cout << "OpenGL renderer initialized" << std::endl;
}

void OglCore::render() {
    std::chrono::time_point<std::chrono::high_resolution_clock> beginFrame = std::chrono::high_resolution_clock::now();
    GLfloat time = std::chrono::duration_cast<std::chrono::milliseconds>(beginFrame - _beginTime).count();
    uTime = time;

    _s.update();

    //glBindImageTexture(1, fractalTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16UI); // int
    glBindImageTexture(1, fractalTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F); // float
    checkGlError;
    _sgBuffer.use();
    checkGlError;
    //autoRelocate(uTime);
    uTime.upload();
    checkGlError;
    _s.render();
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    checkGlError;


    _sPostProc.use();
    _s.bindGBuffer(0);
    //autoRelocate(uTime);
    //uTime.upload();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _renderTarget.render();
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    checkGlError;
}

