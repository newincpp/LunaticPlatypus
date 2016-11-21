#define TINYOBJLOADER_IMPLEMENTATION
#include <iostream>
#include <map>
#if defined(ALEMBIC)
#include "Alembic/AbcGeom/All.h"
#include "Alembic/AbcCoreAbstract/All.h"
#include "Alembic/AbcCoreFactory/All.h"
#include "Alembic/Util/All.h"
#include "Alembic/Abc/TypedPropertyTraits.h"
#include "Alembic/AbcMaterial/IMaterial.h"
#include "Alembic/AbcMaterial/MaterialAssignment.h"
#include "Alembic/Util/PlainOldDataType.h"
#endif
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
	std::cout << "the file is fucked: " << file << '\n';
    }
    std::cout << "loaded with warning:\n" << err << '\n';

    // Loop over shapes
    //for (size_t s = 0; s < shapes.size(); s++) {
    s_._meshes.reserve(shapes.size());
    for (tinyobj::shape_t object: shapes) {
	std::cout << "loading: " << object.name << '\n';
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

void printVertex(std::vector<float> const &vertex) {
	std::cout << vertex[0] << ' ' <<
		vertex[1] << ' ' <<
		vertex[2] << ' ' <<
		vertex[3] << ' ' <<
		vertex[4] << ' ' <<
		vertex[5] << ' ' <<
		vertex[6] << ' ' <<
		vertex[7] << ' ' << '\n';
}

void Importer::genMesh(const tinyobj::shape_t& object, const tinyobj::attrib_t& attrib, Scene& s_) {
	// Loop over faces(polygon)
	size_t index_offset = 0;
	std::vector<GLfloat> vertexBuffer;
	std::vector<GLuint> indiceBuffer;

	vertexBuffer.reserve(object.mesh.num_face_vertices.size() * 8);
	indiceBuffer.reserve(object.mesh.indices.size() * 3);
	std::map<std::vector<float>, int> uniqueVertices; //to easily compare vertices (the vector inside is always 8 values: vertex, normals, uvs) and access their indices (should become unordered but problem of hashes)
	for (size_t f = 0; f < object.mesh.num_face_vertices.size(); f++) {
	    size_t fv = object.mesh.num_face_vertices[f];

	    // Loop over vertices in the face.
	    for (size_t v = 0; v < fv; v++) {
		// access to vertex
		if (object.mesh.indices.size() > index_offset + v) {
			tinyobj::index_t idx = object.mesh.indices[index_offset + v];
			std::vector<float> vertex = { attrib.vertices[3*idx.vertex_index+0], attrib.vertices[3*idx.vertex_index+1], attrib.vertices[3*idx.vertex_index+2],
				attrib.normals[3*idx.normal_index+0], attrib.normals[3*idx.normal_index+1], attrib.normals[3*idx.normal_index+2] };
			if (idx.texcoord_index < 0) {
				//push useless uv because we always upload 8 values and if there's no uv it does shitty things
				vertex.push_back(0.0f);
				vertex.push_back(0.0f);
			} else {
				vertex.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
				vertex.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
			}
			if (uniqueVertices.count(vertex) <= 0) {
				//push a new vertex if we never encountered it before (to the unordered map AND the buffer)
				uniqueVertices[vertex] = vertexBuffer.size() / 8.0f;
				vertexBuffer.push_back(vertex[0]);
				vertexBuffer.push_back(vertex[1]);
				vertexBuffer.push_back(vertex[2]);
				vertexBuffer.push_back(vertex[3]);
				vertexBuffer.push_back(vertex[4]);
				vertexBuffer.push_back(vertex[5]);
				vertexBuffer.push_back(vertex[6]);
				vertexBuffer.push_back(vertex[7]);
			}
			//push indice of vertex
			indiceBuffer.push_back(uniqueVertices[vertex]);
		}
	    }
	    index_offset += fv;

	    // per-face material
	    object.mesh.material_ids[f];
	}
	s_._meshes.emplace_back();
	s_._meshes[s_._meshes.size() - 1].uploadToGPU(vertexBuffer, indiceBuffer);
	s_._meshes[s_._meshes.size() - 1]._name = object.name;
}

#elif defined(ALEMBIC)

void Importer::visitObject(Alembic::Abc::IObject iObj, std::string const &iIndent, Scene& s_) {
	// Object has a name, a full name, some meta data,
	// and then it has a compound property full of properties.

	std::vector<GLfloat> vertexBuffer;
	std::vector<GLuint> indiceBuffer;
	std::string path = iObj.getFullName();

	if (path != "/") {
		std::cout << "Object name=" << path << '\n';
	}
	const Alembic::Abc::MetaData &md = iObj.getMetaData();

	if (Alembic::AbcGeom::ICurves::matches(md)) {
		std::cout << "Curves not implemented yet\n";
		//Alembic::AbcGeom::ICurves curves(iObj);
		//Alembic::AbcGeom::ICurvesSchema cs = curves.getSchema();
		//cs.getPositionsProperty(); //IP3fArrayProperty
		//cs.getNumVerticesProperty(); //IInt32ArrayProperty
		//cs.getUVsParam(); //IV2fGeomParam
		//cs.getNormalsParam(); //IN3fGeomParam ==IV3f
	} else if (Alembic::AbcGeom::INuPatch::matches(md)) {
		std::cout << "NuPatch not implemented yet\n";
	} else if (Alembic::AbcGeom::IPoints::matches(md)) {
		std::cout << "Points not implemented yet\n";
		//Alembic::AbcGeom::IPoints points(iObj);
		//Alembic::AbcGeom::IPointsSchema ps = points.getSchema();
		//for (size_t i = 0; i < ps.getNumSamples(); i++) {
		//	Alembic::AbcGeom::IPointsSchema::Sample s;
		//	ps.get(s, i);
		//	s.getPositions(); //P3fArraySamplePtr
		//	s.getIds(); //UInt64ArraySamplePtr
		//}
	} else if (Alembic::AbcGeom::IPolyMesh::matches(md)) {
		std::cout << "PolyMesh\n";
		Alembic::AbcGeom::IPolyMesh mesh(iObj);
		Alembic::AbcGeom::IPolyMeshSchema ms = mesh.getSchema();
		Alembic::AbcGeom::IV2fGeomParam uvParam = ms.getUVsParam();
		Alembic::AbcGeom::IN3fGeomParam normalsParam = ms.getNormalsParam();
		Imath::Vec3<glm::float32> const *normals = NULL;
		Imath::Vec2<glm::float32> const *uv = NULL;
		for (size_t i = 0; i < ms.getNumSamples(); i++) {
			Alembic::AbcGeom::IPolyMeshSchema::Sample s;
			ms.get(s, i);
			Alembic::Abc::P3fArraySamplePtr positionsPtr = s.getPositions(); //P3fArraySamplePtr == 3 float32
			Imath::Vec3<glm::float32> const *vertex = positionsPtr->get();
			if (normalsParam.valid()) {
			    normals = normalsParam.getIndexedValue(i).getVals()->get();
			} else {
			    normals = NULL;
			}
			if (uvParam.valid()) {
			    uv = uvParam.getIndexedValue(i).getVals()->get();
			} else {
			    uv = NULL;
			}
			//std::cout << "Positions: ";
			for (size_t j = 0; j < positionsPtr->size(); j++) {
			    vertexBuffer.push_back((*vertex).x);
			    //std:: cout << "\nv= " << vertexBuffer.back();
			    vertexBuffer.push_back((*vertex).y);
			    //std:: cout << ' ' << vertexBuffer.back();
			    vertexBuffer.push_back((*vertex).z);
			    //std:: cout << ' ' << vertexBuffer.back();
			    vertex++;
			    if (normals != NULL) {
				//std::cout << " n= " << normals[j].x << ' ' << normals[j].y << ' ' << normals[j].z;
				vertexBuffer.push_back(normals[j].x);
				vertexBuffer.push_back(normals[j].y);
				vertexBuffer.push_back(normals[j].z);
			    } else {
				//std::cout << " n= 1 1 1";
				vertexBuffer.push_back(1.0f);
				vertexBuffer.push_back(1.0f);
				vertexBuffer.push_back(1.0f);
			    }
			    if (uv != NULL) { //TODO: check
				//std::cout << " uv= " << uv[j].x << ' ' << uv[j].y;
				vertexBuffer.push_back(uv[j].x);
				vertexBuffer.push_back(uv[j].y);
			    } else {
				//std::cout << " uv= 0 0";
				vertexBuffer.push_back(0.0f);
				vertexBuffer.push_back(0.0f);
			    }
			}
			//std::cout << '\n';
			Alembic::Abc::Int32ArraySamplePtr indicesPtr = s.getFaceIndices(); //Int32ArraySamplePtr
			glm::int32 const *indice = indicesPtr->get();
			Alembic::Abc::Int32ArraySamplePtr countPtr = s.getFaceCounts(); //Int32ArraySamplePtr
			glm::int32 const *nbIndicesInFace = countPtr->get();
			if (*nbIndicesInFace > 3) {
				std::cout << "WARNING: Triangulate your mesh if you want to avoid problems, quads are trianglulated naively and ngons are not implemented.\n";
				for (size_t j = 0; j < indicesPtr->size(); j += *nbIndicesInFace) {
					for (int k = *nbIndicesInFace - 3; k >= 0; k--) {
						indiceBuffer.push_back(*indice);
						indiceBuffer.push_back(*(indice + 1 + k));
						indiceBuffer.push_back(*(indice + 2 + k));
						//std::cout << *indice << ' ' << *(indice + 1) << ' ' << *(indice + 2 + k) << ' ';
					}
					indice += *nbIndicesInFace;
				}
			} else {
				for (size_t j = 0; j < indicesPtr->size(); j++) {
					//std::cout << *indice << ' ';
					indiceBuffer.push_back(*indice);
					indice++;
				}
			}
			//std::cout << '\n';
		}
		s_._meshes.emplace_back();
		std::cout << "\nUPLOAD " << path << '\n';
		for (size_t k = 0; k < vertexBuffer.size(); k += 8) {
		    std::cout << "v= " << vertexBuffer[k] << ' '
				       << vertexBuffer[k + 1] << ' '
				       << vertexBuffer[k + 2] << ' ' <<
				 "n= " << vertexBuffer[k + 3] << ' '
				       << vertexBuffer[k + 4] << ' '
				       << vertexBuffer[k + 5] << ' ' <<
				"uv= " << vertexBuffer[k + 6] << ' '
				       << vertexBuffer[k + 7] << '\n';
		}
		for (size_t k = 0; k < indiceBuffer.size(); k += 3) {
		    std::cout << "f= " << indiceBuffer[k] << ' '
				       << indiceBuffer[k + 1] << ' '
				       << indiceBuffer[k + 2] << '\n';
		}
		s_._meshes[s_._meshes.size() - 1].uploadToGPU(vertexBuffer, indiceBuffer);
		s_._meshes[s_._meshes.size() - 1]._name = path;
	} else if (Alembic::AbcGeom::ISubDSchema::matches(md)) {
		std::cout << "ISubDSchema not implemented yet\n";
		//Alembic::AbcGeom::ISubD mesh(iObj);
		//Alembic::AbcGeom::ISubDSchema ms = mesh.getSchema();
		//for (size_t i = 0; i < ms.getNumSamples(); i++) {
		//	Alembic::AbcGeom::ISubDSchema::Sample s;
		//	ms.get(s, i);
		//	s.getPositions(); //P3fArraySamplePtr
		//	s.getFaceIndices(); //Int32ArraySamplePtr
		//	s.getFaceCounts(); //Int32ArraySamplePtr
		//}
		//ms.getUVsParam(); //IV2fGeomParam
		//Box3d bnds = getBounds( iObj, seconds );
		//std::cout << path << " " << bnds.min << " " << bnds.max << std::endl;
	}

	// now the child objects
	for (size_t i = 0 ; i < iObj.getNumChildren() ; i++) {
		visitObject(Alembic::Abc::IObject(iObj, iObj.getChildHeader(i).getName()), iIndent, s_);
	}
}

void Importer::load(std::string& file, Scene& s_) {
    std::cout << "Import using Alembic\n";

    // Create an instance of the Importer class
    Alembic::AbcCoreFactory::IFactory factory;
    factory.setPolicy(Alembic::Abc::ErrorHandler::kQuietNoopPolicy);
    Alembic::AbcCoreFactory::IFactory::CoreType coreType;
    Alembic::Abc::IArchive archive = factory.getArchive(file, coreType);

    if (archive) {
	    std::cout  << "AbcEcho for "
		    << Alembic::AbcCoreAbstract::GetLibraryVersion ()
		    << '\n';

	    std::string appName;
	    std::string libraryVersionString;
	    Alembic::Util::uint32_t libraryVersion;
	    std::string whenWritten;
	    std::string userDescription;
	    GetArchiveInfo (archive,
			    appName,
			    libraryVersionString,
			    libraryVersion,
			    whenWritten,
			    userDescription);

	    if (appName != "") {
		    std::cout << "  file written by: " << appName << '\n';
		    std::cout << "  using Alembic : " << libraryVersionString << '\n';
		    std::cout << "  written on : " << whenWritten << '\n';
		    std::cout << "  user description : " << userDescription << '\n';
		    std::cout << '\n';
	    } else {
		    std::cout << file << '\n';
		    std::cout << "  (file doesn't have any ArchiveInfo)"
			    << '\n';
		    std::cout << '\n';
	    }
	    visitObject(archive.getTop(), "", s_);
    } else {
	    std::cout << "archive is null :C " << file << '\n';
    }
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
