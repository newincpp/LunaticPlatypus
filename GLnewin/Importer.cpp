#define TINYOBJLOADER_IMPLEMENTATION
#include <iostream>
#include "Importer.hh"

Importer::Importer(std::string file, Scene& s_) {
    load(file, s_);
}

inline glm::mat4 Importer::aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;

    to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

    return to;
}

#ifdef TINYOBJLOADER
void Importer::load(std::string& file, Scene& s_) {
    std::cout << "Import using tinyObjLoader\n";
    /*
       std::vector<Mesh> Scene::_meshes;
       std::vector<Camera> Scene::_cameras;
       */
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file.c_str())) {
	std::cout << "the file is fucked: " << file << std::endl;
    }
    std::cout << "loaded with warning:\n" << err << std::endl;

    // Loop over shapes
    //for (size_t s = 0; s < shapes.size(); s++) {
    s_._meshes.reserve(shapes.size());
    for (tinyobj::shape_t object: shapes) {
	std::cout << "loading: " << object.name << "\n";
	genMesh(object, attrib, s_);
	std::cout << "finished...\n";
    }

    std::cout << "generating cameras\n";
    s_._cameras.emplace_back();
    Camera& mainCamera = s_._cameras[0];
    
    mainCamera.lookAt(glm::vec3(.0f, 3.7f, 8.4f)); // Nelo.obj
    mainCamera.setPos(glm::vec3(-9.3, 4.4f, 15.9)); // Nelo.obj
    //mainCamera.lookAt(glm::vec3(59.0f, 131.0f, 582.0f)); // DemoCity.obj
    //mainCamera.setPos(glm::vec3(136.0f, 231.0f, 218.0f)); // DemoCity.obj
    //mainCamera.setPos(glm::vec3(-136.0f, 231.0f, 18.0f));
    mainCamera.fieldOfview(1.623f);
    mainCamera.clipPlane(glm::vec2(0.1f, 10000.0f));
    mainCamera.upVector(glm::vec3(0.0f,1.0f,0.0f));
    s_._cameras.emplace_back(s_._cameras[0]);
    for(unsigned int i = 0; i < 250; ++i) {
	s_._cameras.emplace_back(s_._cameras[0]);
    }
}

void Importer::genMesh(const tinyobj::shape_t& object, const tinyobj::attrib_t& attrib, Scene& s_) {
	// Loop over faces(polygon)
	size_t index_offset = 0;
	std::vector<GLfloat> vertexBuffer;
	std::vector<GLuint> indiceBuffer;

	vertexBuffer.reserve(object.mesh.num_face_vertices.size() * 8);
	indiceBuffer.reserve(object.mesh.indices.size() * 3);
	std::cout << "mesh size: " << object.mesh.num_face_vertices.size() * 8 << '\n';
	for (size_t f = 0; f < object.mesh.num_face_vertices.size(); f++) {
	    size_t fv = object.mesh.num_face_vertices[f];

	    // Loop over vertices in the face.
	    for (size_t v = 0; v < fv; v++) {
		// access to vertex
		tinyobj::index_t idx = object.mesh.indices[index_offset + v];

		vertexBuffer.push_back(attrib.vertices[3*idx.vertex_index+0]);
		vertexBuffer.push_back(attrib.vertices[3*idx.vertex_index+1]);
		vertexBuffer.push_back(attrib.vertices[3*idx.vertex_index+2]);

		vertexBuffer.push_back(attrib.normals[3*idx.normal_index+0]);
		vertexBuffer.push_back(attrib.normals[3*idx.normal_index+1]);
		vertexBuffer.push_back(attrib.normals[3*idx.normal_index+2]);

		vertexBuffer.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
		vertexBuffer.push_back(attrib.texcoords[2*idx.texcoord_index+1]);

		indiceBuffer.push_back((index_offset + v) * 8);
	    }
	    index_offset += fv;

	    // per-face material
	    object.mesh.material_ids[f];
	}
	s_._meshes.emplace_back();
	s_._meshes[s_._meshes.size() - 1].uploadToGPU(vertexBuffer, indiceBuffer);
	s_._meshes[s_._meshes.size() - 1]._name = object.name;
}

#else

void Importer::load(std::string& file, Scene& s_) {
    std::cout << "Import using Assimp\n";

    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll 
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(file, 
	    aiProcess_CalcTangentSpace       | 
	    aiProcess_Triangulate            |
	    aiProcess_JoinIdenticalVertices  |
	    aiProcess_SortByPType);

    // If the import failed, report it
    if( !scene) {
	std::cout << "fuu: " << importer.GetErrorString() << '\n';
    }
    genMesh(scene, s_);
    //scene->mCameras[0]->GetCameraMatrix(m);
    s_._cameras.emplace_back();
    Camera& mainCamera = s_._cameras[0];
    if (scene->mNumCameras) {
	aiCamera* c = scene->mCameras[0];
	std::cout << "position: " << scene->mCameras[0]->mPosition[0] << ' ' << scene->mCameras[0]->mPosition[1] << ' ' << scene->mCameras[0]->mPosition[2] << '\n';
	//c->GetCameraMatrix(m);
	//_mainCamera.setMatrix(aiMatrix4x4ToGlm(m));

	mainCamera.lookAt(glm::vec3(c->mLookAt[0], c->mLookAt[1], c->mLookAt[2]));
	mainCamera.setPos(glm::vec3(c->mPosition[0] / 1.0f, c->mPosition[1] / 1.0f, c->mPosition[2] / 1.0f));
	//_mainCamera.fieldOfview(c->mHorizontalFOV);
	mainCamera.fieldOfview(1.571f);
	mainCamera.clipPlane(glm::vec2(c->mClipPlaneNear, c->mClipPlaneFar));
	mainCamera.upVector(glm::vec3(c->mUp[0], c->mUp[1], c->mUp[2]));
    } else {
	std::cout << "no Camera Detected\n";
	mainCamera.lookAt(glm::vec3(.0f, 3.7f, 8.4f)); // Nelo.obj
	mainCamera.setPos(glm::vec3(-9.3, 4.4f, 15.9)); // Nelo.obj

	//mainCamera.lookAt(glm::vec3(59.0f, 131.0f, 582.0f)); // DemoCity.obj
	//mainCamera.setPos(glm::vec3(136.0f, 231.0f, 218.0f)); //DemoCity.obj
	mainCamera.fieldOfview(1.623f);
	mainCamera.clipPlane(glm::vec2(0.1f, 10000.0f));
	mainCamera.upVector(glm::vec3(0.0f,1.0f,0.0f));
	s_._cameras.emplace_back(s_._cameras[0]);
	for(unsigned int i = 0; i < 250; ++i) {
	    s_._cameras.emplace_back(s_._cameras[0]);
	}
    }
}

void Importer::genMesh(const aiScene* scene_, Scene& s_) {
    s_._meshes.reserve(scene_->mNumMeshes);
    for(unsigned int m = 0 ; m < scene_->mNumMeshes ; ++m) { 
	std::vector<GLfloat> vertexBuffer;
	std::vector<GLuint> indiceBuffer;
	aiMesh* mesh = scene_->mMeshes[m]; 
	vertexBuffer.reserve(mesh->mNumVertices * 8);
	indiceBuffer.reserve(mesh->mNumFaces * 3);


	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
	    vertexBuffer.push_back(mesh->mVertices[i].x);
	    vertexBuffer.push_back(mesh->mVertices[i].y);
	    vertexBuffer.push_back(mesh->mVertices[i].z);
	    vertexBuffer.push_back(mesh->mNormals[i].x);
	    vertexBuffer.push_back(mesh->mNormals[i].y);
	    vertexBuffer.push_back(mesh->mNormals[i].z);


	    if (mesh->HasTextureCoords(0)) {
		vertexBuffer.push_back(mesh->mTextureCoords[0][i].x);
		vertexBuffer.push_back(mesh->mTextureCoords[0][i].y);
	    } else {
		vertexBuffer.push_back(0.0f);
		vertexBuffer.push_back(0.0f);
	    }

	    //vertexBuffer.push_back(mesh->mBitangents[i].x);
	    //vertexBuffer.push_back(mesh->mBitangents[i].y);
	    //vertexBuffer.push_back(mesh->mBitangents[i].z);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[0]);
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[1]);
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[2]);
	}

	s_._meshes.emplace_back();
	s_._meshes[s_._meshes.size() - 1].uploadToGPU(vertexBuffer, indiceBuffer);
	s_._meshes[s_._meshes.size() - 1]._name = mesh->mName.C_Str();

	//_meshBuffer.emplace();
	//_meshBuffer.top().uploadToGPU(vertexBuffer, indiceBuffer);
	//_meshBuffer.top()._name = mesh->mName.C_Str();

	aiNode* n = scene_->mRootNode->FindNode(mesh->mName);
	if (n) {
	s_._meshes[s_._meshes.size() - 1].uMeshTransform = aiMatrix4x4ToGlm(n->mTransformation);
	}
    } 
}
#endif
