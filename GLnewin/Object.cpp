#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "glm/gtc/matrix_transform.hpp"
#include "Object.hh"


GLnewin::Object::Object(const std::string& file) : _mesh(_genMesh(file)), _rotPosScale(NULL) {
    _shader.setVertex(GLnewin::Shader::fileToString("shaders/vert.glsl"));
    _shader.setFragment(GLnewin::Shader::fileToString("shaders/frag.glsl"));
    _shader.link();
    _mesh.setShader(&_shader);
    _rotPosScale = new Uniform<glm::mat4>(_shader.genUniform(glm::mat4(), "object"));
}

void GLnewin::Object::draw() noexcept {
    _mesh.draw();
}

void GLnewin::Object::setPos(const glm::vec3& v) noexcept {
    _pos = v;
    *_rotPosScale = glm::translate(glm::mat4(), _pos);
    _rotPosScale->upload();
}

void GLnewin::Object::setRot(const glm::vec3& v) noexcept {
    _rot = v;
}

void GLnewin::Object::setScale(const glm::vec3& v) noexcept {
    _scale = v;
}

std::vector<GLfloat> GLnewin::Object::_genMesh(const std::string& file) {
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

	//Vertex v(Vector3f(pPos->x, pPos->y, pPos->z), Vector2f(pTexCoord->x, pTexCoord->y), Vector3f(pNormal->x, pNormal->y, pNormal->z));

	//Vertices.push_back(v);
	verts.push_back(pPos->x);
	verts.push_back(pPos->y);
	verts.push_back(pPos->z);
    }
    for (GLfloat x : verts) {
	std::cout << "value: " << x << std::endl;
    }
    return verts;
}
