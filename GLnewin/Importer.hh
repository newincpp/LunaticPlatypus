#include <stack>
#include "Mesh.hh"
#include "Camera.hh"
#include "glew.h"
#include "Scene.hh"
#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing fla

#ifdef TINYOBJLOADER
#include "tiny_obj_loader.h"
#elif defined(ALEMBIC)
#include "Alembic/Abc/IObject.h"
#endif


class Material {
};

class Importer {
    public:
	//std::stack<Mesh> _meshBuffer;
	//std::stack<Material> _materialBuffer;
	//Camera _mainCamera;
	Importer(std::string file, Scene&);
	inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);

#ifdef TINYOBJLOADER
	void genMesh(const tinyobj::shape_t&, const tinyobj::attrib_t&, Scene&);
#elif defined(ALEMBIC)
	void visitObject(Alembic::Abc::IObject iObj, std::string const &iIndent, Scene& s_);
#else
	void genMesh(const aiScene* scene_, Scene&);
#endif

	void load(std::string& file, Scene&);
};
