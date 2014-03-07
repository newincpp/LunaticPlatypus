#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "glm/gtc/matrix_transform.hpp"
#include "Object.hh"


GLnewin::Object::Object() : _mesh({-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f}), _rotPosScale(NULL) {
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

bool GLnewin::Object::DoTheImportThing(const std::string& pFile) {
    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    //const aiScene* scene = importer.ReadFile(pFile, aiProcess_CalcTangentSpace|aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_SortByPType);

    // If the import failed, report it
//    if( !scene)
    {
//	std::cout << "error: " << importer.GetErrorString() << std::endl;
	return false;
    }
    // Now we can access the file's contents.
    //DoTheSceneProcessing( scene);
    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}
