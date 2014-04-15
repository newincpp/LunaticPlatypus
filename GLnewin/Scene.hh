#ifndef SCENEMANAGER_H_
# define SCENEMANAGER_H_

#include <utility>
#include <future>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "RenderSystem.hh"
#include "Camera.hh"
#include "Mesh.hh"

namespace GLnewin {
    class Scene : public IRenderable {
	private:
	    template <typename T>
		struct __InternalFuture {
		    private:
#define __INT_SAFE_FUTURE_CALL(X) if(!_worker){return;}if(_worker->valid()){ X }
			std::future<T>* _worker;
			inline void _intPush(std::future<T>&& n) noexcept {
			    _worker = new std::future<T>(std::move(n));
			}
		    public:
			__InternalFuture(){}
			__InternalFuture(std::future<T>&& n) : _worker(NULL) {
			    _intPush(std::move(n));
			}
			~__InternalFuture() {
			    if (_worker) {
				delete _worker;
			    }
			}
			void push(std::future<T>&& n) {
			    _intPush(std::move(n));
			}
			T get() {
			    __INT_SAFE_FUTURE_CALL(_worker->get());
			    delete _worker;
			}
			void wait() const {
			    if (!_worker) {
				return;
			    }
			    if (_worker->valid()) {
				_worker->get();
			    }
			    _worker->wait();
			}
		};
	    __InternalFuture<void> _worker;
	    std::vector<const IRenderable*> _objects;
	    GLnewin::Shader _shader;
	    GLnewin::Camera* _cam;
	    inline void _setTrivialShader() noexcept {
		std::string trivVertex = "#version 330 \n layout(location = 0) in vec3 vertex; \n uniform mat4 projection; \n uniform mat4 view; \n uniform mat4 model; \n void main(void) { \n gl_Position = projection * view * model * vec4(vertex, 1.0); \n }";
		std::string trivFrag = "#version 330 \n out vec4 outColor; \n uniform float color; \n void main(void) { \n outColor = vec4(0.0,0.3, color, 1.0); \n }";
		_shader.setVertex(trivVertex);
		_shader.setFragment(trivFrag);
		_shader.link();
	    }
	    void _internalRender() const;
	public:
	    Scene();
	    Scene(const Shader&);
	    ~Scene();
	    template <unsigned int RENDERTYPE = GL_TRIANGLES, bool DEBUG = false>
		Mesh<RENDERTYPE>* genMesh(const std::string& file);
	    void setShader(const Shader&);
	    const Shader& getShader() const { return _shader; }
	    inline Camera& getCamera() noexcept { return *_cam; }
	    inline Camera genCamera() noexcept { return Camera(_shader); }
	    void render();
	    virtual void draw() const noexcept;
	    virtual void draw() noexcept;
	    void pushRenderCandidate(const IRenderable*) noexcept;
	    template<unsigned int RENDERTYPE = GL_TRIANGLES>
		void bindMatrix(Mesh<RENDERTYPE>&) const noexcept;
    };
}

template <unsigned int RENDERTYPE, bool DEBUG>
GLnewin::Mesh<RENDERTYPE>* GLnewin::Scene::genMesh(const std::string& file) {
    std::vector<GLfloat> verts;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_CalcTangentSpace|aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_SortByPType);
    if( !scene) {
	std::cout << "error: " << importer.GetErrorString() << std::endl;
    }
    aiMesh* paiMesh = scene->mMeshes[0];

    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
	const aiVector3D& pPos = paiMesh->mVertices[i];
	//const aiVector3D* pNormal = &(paiMesh->mNormals[i]) : &Zero3D;
	//const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

	verts.push_back(pPos.x);
	verts.push_back(pPos.y);
	verts.push_back(pPos.z);
    }
    if (DEBUG) { //TODO delete this code at compiling
	for (GLfloat x : verts) {
	    std::cout << "value: " << x << std::endl;
	}
    }
    Mesh<RENDERTYPE>* m = new Mesh<RENDERTYPE>(verts);
    m->bindMatrixModifier(_shader.genUniform(glm::mat4(), "model"));
    return m;
}

template<unsigned int RENDERTYPE>
void GLnewin::Scene::bindMatrix(Mesh<RENDERTYPE>& m) const noexcept {
    m.bindMatrixModifier(_shader.genUniform(glm::mat4(), "model"));
}

#endif /* !SCENEMANAGER_H_ */
