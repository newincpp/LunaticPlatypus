#ifndef SCENEMANAGER_H_
# define SCENEMANAGER_H_

#include "RenderSystem.hh"
#include "Camera.hh"
#include "Object.hh"

namespace GLnewin {
    class Scene {
	private:
	    GLnewin::Shader _shader;
	    GLnewin::Camera _cam;
	public:
	    Scene();
	    std::vector<GLfloat> genMesh(const std::string& file);
	    void setShader(const Shader&);
	    const Shader& getShader() const { return _shader; }
	    inline void setTrivialShader() {
		std::string trivVertex = "#version 330 \n layout(location = 0) in vec3 vertex; \n uniform mat4 projection; \n uniform mat4 modelview; \n uniform mat4 object; \n void main(void) { \n gl_Position = projection * modelview * object * vec4(vertex, 1.0); \n }";
		std::string trivFrag = "#version 330 \n uniform float color; \n void main(void) { \n gl_FragColor = vec4(0.0,0.3,color, 1.0); \n }";

		_shader.setVertex(trivVertex);
		_shader.setFragment(trivFrag);
		_shader.link();
	    }
	    inline Camera& getCamera() noexcept { return _cam; }
	    inline Camera genCamera() noexcept { return Camera(_shader); }
	    void setActive();
    };
}

#endif /* !SCENEMANAGER_H_ */
