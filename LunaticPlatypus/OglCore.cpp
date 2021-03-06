#include <ctime>
#include "OglCore.hh"

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

OglCore::OglCore() : uTime(0.0f, "uTime") { 
}

void OglCore::init() {
    _beginTime = std::chrono::high_resolution_clock::now();

    checkGlError glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    std::vector<GLfloat> vertices = {
	//vertexPos		//normal		//uvCoord
	-1.0f,  1.0f,5.5f, 	0.0f,0.0f,1.0f, 	0.0f,1.0f, // Top-left
	1.0f,  1.0f, 5.5f, 	0.0f,0.0f,1.0f, 	1.0f,1.0f, // Top-right
	1.0f, -1.0f, 5.5f, 	0.0f,0.0f,1.0f, 	1.0f,0.0f, // Bottom-right
	-1.0f, -1.0f, 5.5f, 	0.0f,0.0f,1.0f, 	0.0f,0.0f  // Bottom-left
    };

    std::vector<GLuint> elements = {
	0, 2, 1,
	0, 3, 2
    };

    _compositor.add("./compositor.glsl",GL_FRAGMENT_SHADER);
    _compositor.add("./compositorVert.glsl",GL_VERTEX_SHADER);
    _compositor.link({"outColour"});

    _s._dynamicShadowCaster.add("./dynamicShadows.glsl",GL_FRAGMENT_SHADER);
    _s._dynamicShadowCaster.add("./dynamicShadowsVert.glsl",GL_VERTEX_SHADER);
    _s._dynamicShadowCaster.link({"outColour"});

    _s._fb.emplace_back();
    _s._fb[0].addBuffer("gPosition");
    _s._fb[0].addBuffer("gNormalRough");
    _s._fb[0].addBuffer("gAlbedoMetallic");
    _s._fb[0].addDepthBuffer("gDepth");
    _s._fb[0].enable();
    //Mesh m;
    //m.uploadToGPU(vertices, elements);
    _s._cameras.emplace_back(_s._fb[0]);

    _renderTarget.uploadToGPU(vertices, elements);
    _compositor.containUniform(_renderTarget.uMeshTransform);
    _compositor.containUniform(_s._cameras[0].uView);
    _compositor.containUniform(_s._cameras[0].uProjection);
    _compositor.containUniform(uTime);

    _s._lights.emplace_back();
    _s._dynamicShadowCaster.containUniform(uTime);

    //_sgBuffer->first.use();
    uTime.addItselfToShaders(_s._drawList);
    //_uPostPRocessTexture.addItselfToShaders(_s._drawList);
    _s.addCameraUniformsToShaders();
    std::cout << "OpenGL renderer initialized" << std::endl;
}

void OglCore::render() {
    std::chrono::time_point<std::chrono::high_resolution_clock> beginFrame = std::chrono::high_resolution_clock::now();
    GLfloat time = std::chrono::duration_cast<std::chrono::milliseconds>(beginFrame - _beginTime).count();
    uTime.updateValue(time, _currentFrame);

    _s.update(_currentFrame);


    int i = 2; // 1 is reserved to illumination buffer
    for (ImageRAM& buffer : _randomAccessBuffers) {
        buffer.useRW(i);
        ++i;
    }
    checkGlError;
    _s.render();
    checkGlError;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _s.castDynamicShadows();
    checkGlError;

    //glDisable(GL_CULL_FACE);
    _compositor.use();
    checkGlError;
    _s.bindGBuffer(0);
    Light::_illuminationBuffer->useR(1);
    i = 2; // 1 is reserved to illumination buffer
    for (ImageRAM& buffer : _randomAccessBuffers) {
        buffer.useR(i);
        ++i;
    }
    //uTime.upload();
    _renderTarget.render();
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    checkGlError;
    ++_currentFrame;
    swap();
}
