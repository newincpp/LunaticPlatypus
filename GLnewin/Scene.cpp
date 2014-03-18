#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Scene.hh"

GLnewin::Scene::Scene() : _cam(_shader) {
    setTrivialShader();
    _cam.reGenUniform(_shader);
}

void GLnewin::Scene::setShader(const Shader& n) {
    _shader = n;
}

std::vector<GLfloat> GLnewin::Scene::genMesh(const std::string& file) {
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

void GLnewin::Scene::setActive() {
    _shader.use();
    _cam.setActive();
}
