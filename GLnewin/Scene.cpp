#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Scene.hh"

GLnewin::Scene::Scene() : _cam(NULL) {
    _setTrivialShader();
    _cam = new Camera(_shader);
}

GLnewin::Scene::Scene(const Shader& s) : _shader(s), _cam(new Camera(_shader)) {
}

GLnewin::Scene::~Scene() {
    delete _cam;
}

void GLnewin::Scene::setShader(const Shader& n) {
    _shader = n;
    _cam->reGenUniform(_shader);
}

GLnewin::Mesh* GLnewin::Scene::genMesh(const std::string& file) {
    std::vector<GLfloat> verts;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_CalcTangentSpace|aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_SortByPType);
    if( !scene) {
	std::cout << "error: " << importer.GetErrorString() << std::endl;
    }
    aiMesh* paiMesh = scene->mMeshes[0];

    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
	const aiVector3D* pPos = &(paiMesh->mVertices[i]);
	//const aiVector3D* pNormal = &(paiMesh->mNormals[i]) : &Zero3D;
	//const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

	verts.push_back(pPos->x);
	verts.push_back(pPos->y);
	verts.push_back(pPos->z);
    }
    for (GLfloat x : verts) {
	std::cout << "value: " << x << std::endl;
    }
    return new Mesh(verts);
}

void GLnewin::Scene::render() {
    _shader.use();
    _cam->setActive();
    for (const IRenderable* ent : _objects) {
	ent->draw();
    }
}

void GLnewin::Scene::pushRenderCandidate(const IRenderable* a) noexcept {
    _objects.push_back(a);
}
