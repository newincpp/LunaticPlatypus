#pragma once
#include <stack>
#include "Mesh.hh"
#include "Camera.hh"
#include "glew.h"
#include "DrawBuffer.hh"

#ifdef TINYOBJLOADER
#include "tiny_obj_loader.h"
#elif defined(ALEMBIC) || defined(ALEMBICV2)
#define register // deleting some warnings in alembic
#include <map>
#include "Alembic/Abc/IObject.h"
#elif defined(ASSIMP)
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#else
#error "You have to define which importer implementation you want"
#endif

class Importer {
    private:
#ifdef TINYOBJLOADER
	void genMesh(const tinyobj::shape_t&, const tinyobj::attrib_t&, int , std::vector<Mesh>&);
#elif defined(ALEMBICV2)
	std::map<std::string, std::list<std::pair<Shader, std::vector<Mesh>>>::iterator> _shaderList;
	void genMesh(const Alembic::Abc::IObject&, DrawBuffer&, glm::mat4&);
	void genCamera(const Alembic::Abc::IObject&, DrawBuffer&, glm::mat4&);
	void genGameClass(const std::string&, glm::mat4&);
	void transformUpdate(const Alembic::Abc::IObject&, glm::mat4&);
	void visitor(const Alembic::Abc::IObject&, unsigned int, DrawBuffer&, glm::mat4);
#elif defined(ALEMBIC)
	inline glm::mat4 createTransformMatrix(const glm::vec3 &, const glm::vec3 &, const glm::vec3 &);
	inline glm::vec3 AlembicVec3toGlmVec3(Alembic::Abc::V3d const &);
	void getFullPositionScale(Alembic::Abc::IObject const &, glm::vec3 &, glm::vec3 &, glm::vec3 &);
	inline glm::mat4 M44d4x4ToGlm(const Alembic::Abc::M44d&);
	void visitObject(Alembic::Abc::IObject, std::string const &, DrawBuffer&);
#else
	inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
	void genMesh(const aiScene* scene_, DrawBuffer&);
#endif
    public:
	Importer(std::string file, DrawBuffer&);
	void load(std::string& file, DrawBuffer&);
};
