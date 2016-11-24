#define TINYOBJLOADER_IMPLEMENTATION
#include <iostream>
#include <map>
#if defined(ALEMBIC)
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/Util/All.h>
#include <Alembic/Abc/TypedPropertyTraits.h>
#include <Alembic/AbcMaterial/IMaterial.h>
#include <Alembic/AbcMaterial/MaterialAssignment.h>
#include <Alembic/Util/PlainOldDataType.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#endif
#include "Importer.hh"

Importer::Importer(std::string file, Scene& s_) {
    load(file, s_);
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
    mainCamera.fieldOfview(1.623f);
    mainCamera.clipPlane(glm::vec2(0.1f, 10000.0f));
    mainCamera.upVector(glm::vec3(0.0f,1.0f,0.0f));
    //for(unsigned int i = 0; i < 250; ++i) {
    //    s_._cameras.emplace_back(s_._cameras[0]);
    //}
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

inline glm::mat4 Importer::createTransformMatrix(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale) {
    glm::mat4 rotationMatrix = glm::rotate(rotation.x, glm::vec3(1.0, 0.0, 0.0)) *
	    glm::rotate(-rotation.z, glm::vec3(0.0, 1.0, 0.0)) *
	    glm::rotate(rotation.y, glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 matrix = rotationMatrix * glm::translate(position) * glm::scale(scale);
    return matrix;
    /*glm::mat4 to;

    to[0][0] = scale.x; to[0][1] = 0;  to[0][2] = 0; to[0][3] = position.x;
    to[1][0] = 0; to[1][1] = scale.y;  to[1][2] = 0; to[1][3] = position.y;
    to[2][0] = 0; to[2][1] = 0;  to[2][2] = scale.z; to[2][3] = position.z;
    to[3][0] = 0; to[3][1] = 0;  to[3][2] = 0; to[3][3] = 1;

    return to;*/
}

inline glm::vec3 Importer::AlembicVec3toGlmVec3(Alembic::Abc::V3d const &from) {
    return glm::vec3(from.x, from.y, from.z);
}

void Importer::getFullPositionScale(Alembic::Abc::IObject const &object, glm::vec3 &position, glm::vec3 &rotation, glm::vec3 &scale) {
    Alembic::Abc::IObject parent = object.getParent();
    while (parent != NULL) {
	    if (Alembic::AbcGeom::IXform::matches(parent.getMetaData())) {
		    Alembic::AbcGeom::IXform xForm(parent);
		    Alembic::AbcGeom::IXformSchema xFormSchema = xForm.getSchema();
		    Alembic::AbcGeom::XformSample xFormSample;
		    xFormSchema.get(xFormSample);
		    position += AlembicVec3toGlmVec3(xFormSample.getTranslation());
		    //        xFormSample.getXRotation() << ' ' <<
		    //        xFormSample.getYRotation() << ' ' <<
		    //        xFormSample.getZRotation() << ' '
		    //        << '\n';
		    rotation.x += xFormSample.getXRotation();
		    rotation.y += xFormSample.getYRotation();
		    rotation.z += xFormSample.getZRotation();
		    /*if ((float)xFormSample.getAngle() != 0) {
			std::cout << "add rotation of: " << parent.getFullName() << ' ' <<
			    xFormSample.getAngle() << " around " << xFormSample.getAxis().x << ' ' << xFormSample.getAxis().y << ' ' << xFormSample.getAxis().z << '\n';
			rotation *= glm::rotate((float)xFormSample.getAngle(), AlembicVec3toGlmVec3(xFormSample.getAxis()));
		    }*/
		    scale *= AlembicVec3toGlmVec3(xFormSample.getScale());
		    if (xFormSample.getInheritsXforms()) {
			return;
		    }
	    }
	    parent = parent.getParent();
    }
}

inline glm::mat4 Importer::M44d4x4ToGlm(const Alembic::Abc::M44d& from) {
    glm::mat4 to;

    to[0][0] = (GLfloat)from[0][0]; to[0][1] = (GLfloat)from[0][1];  to[0][2] = (GLfloat)from[0][2]; to[0][3] = (GLfloat)from[0][3];
    to[1][0] = (GLfloat)from[1][0]; to[1][1] = (GLfloat)from[1][1];  to[1][2] = (GLfloat)from[1][2]; to[1][3] = (GLfloat)from[1][3];
    to[2][0] = (GLfloat)from[2][0]; to[2][1] = (GLfloat)from[2][1];  to[2][2] = (GLfloat)from[2][2]; to[2][3] = (GLfloat)from[2][3];
    to[3][0] = (GLfloat)from[3][0]; to[3][1] = (GLfloat)from[3][1];  to[3][2] = (GLfloat)from[3][2]; to[3][3] = (GLfloat)from[3][3];

    return to;
}

void Importer::visitObject(Alembic::Abc::IObject iObj, std::string const &iIndent, Scene& s_) {
	// Object has a name, a full name, some meta data,
	// and then it has a compound property full of properties.

	std::vector<GLfloat> vertexBuffer;
	std::vector<GLuint> indiceBuffer;
	std::string path = iObj.getName();

	/*if (path != "/") {
		std::cout << "ObjName= " << path << " hierachy= " << iObj.getFullName() << '\n';
	}*/
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
		Alembic::AbcGeom::IPolyMesh mesh(iObj);
		Alembic::AbcGeom::IPolyMeshSchema ms = mesh.getSchema();
		Alembic::AbcGeom::IV2fGeomParam uvParam = ms.getUVsParam();
		Alembic::AbcGeom::IN3fGeomParam normalsParam = ms.getNormalsParam();
		for (size_t i = 0; i < ms.getNumSamples(); i++) {
			Alembic::AbcGeom::IPolyMeshSchema::Sample s;
			ms.get(s, i);
			Alembic::Abc::P3fArraySamplePtr positionsPtr = s.getPositions(); //P3fArraySamplePtr == 3 float32
			Imath::Vec3<glm::float32> const *vertex = positionsPtr->get();
			Alembic::Util::shared_ptr<Alembic::Abc::TypedArraySample<Alembic::Abc::N3fTPTraits>> normals;
			Alembic::Util::shared_ptr<Alembic::Abc::TypedArraySample<Alembic::Abc::V2fTPTraits>> uvs;
			if (normalsParam.valid()) {
			    normals = normalsParam.getIndexedValue(i).getVals();
			}
			if (uvParam.valid()) {
			    uvs = uvParam.getIndexedValue(i).getVals();
			}
			for (size_t j = 0; j < positionsPtr->size(); j++) {
			    vertexBuffer.push_back((*vertex).x);
			    vertexBuffer.push_back((*vertex).y);
			    vertexBuffer.push_back((*vertex).z);
			    vertex++;
			    if (normalsParam.valid()) {
				vertexBuffer.push_back((*normals)[j].x);
				vertexBuffer.push_back((*normals)[j].y);
				vertexBuffer.push_back((*normals)[j].z);
			    } else {
				std::cout << "caca ";
				vertexBuffer.push_back(1.0f);
				vertexBuffer.push_back(1.0f);
				vertexBuffer.push_back(1.0f);
			    }
			    if (uvParam.valid()) {
				vertexBuffer.push_back((*uvs)[j].x);
				vertexBuffer.push_back((*uvs)[j].y);
			    } else {
				vertexBuffer.push_back(0.0f);
				vertexBuffer.push_back(0.0f);
			    }
			}
			Alembic::Abc::Int32ArraySamplePtr indicesPtr = s.getFaceIndices(); //Int32ArraySamplePtr
			glm::int32 const *indice = indicesPtr->get();
			Alembic::Abc::Int32ArraySamplePtr countPtr = s.getFaceCounts(); //Int32ArraySamplePtr
			glm::int32 const *nbIndicesInFace = countPtr->get();
			if (*nbIndicesInFace > 3) {
				//std::cout << "WARNING: Triangulate your mesh if you want to avoid problems, quads are trianglulated naively and ngons are not implemented.\n";
				for (size_t j = 0; j < indicesPtr->size(); j += *nbIndicesInFace) {
					for (int k = *nbIndicesInFace - 3; k >= 0; k--) {
						indiceBuffer.push_back(*indice);
						indiceBuffer.push_back(*(indice + 1 + k));
						indiceBuffer.push_back(*(indice + 2 + k));
					}
					indice += *nbIndicesInFace;
				}
			} else {
			for (size_t j = 0; j < indicesPtr->size(); j++) {
					indiceBuffer.push_back(*indice);
					indice++;
				}
			}
		}
		if (iObj.getName() != "PlaneShape") {
		s_._meshes.emplace_back();
		s_._meshes[s_._meshes.size() - 1].uploadToGPU(vertexBuffer, indiceBuffer);
		s_._meshes[s_._meshes.size() - 1]._name = path;
		/*if (Alembic::AbcGeom::IXform::matches(iObj.getParent().getMetaData())) {
			Alembic::AbcGeom::IXform xForm(iObj.getParent());
			Alembic::AbcGeom::IXformSchema xFormSchema = xForm.getSchema();
			Alembic::AbcGeom::XformSample xFormSample;
			xFormSchema.get(xFormSample);*/
			glm::vec3 pos(0);
			glm::vec3 rotation(0);
			glm::vec3 scale(1);
			getFullPositionScale(iObj, pos, rotation, scale);
			pos = glm::vec3(pos.x, -pos.z, pos.y);
			std::cout << "Object " << iObj.getFullName() << " is in x:" << pos.x << " y:" << pos.y << " z:" << pos.z << ' ';
			//std::cout << " rotation:" << rotation.x << ' ' << rotation.y << ' ' << rotation.z << " scale:";
			std::cout << scale.x << ' ' << scale.y << ' ' << scale.z << '\n';
			s_._meshes[s_._meshes.size() - 1].uMeshTransform = createTransformMatrix(pos, rotation, scale);//M44d4x4ToGlm(xFormSample.getMatrix());
			/*std::cout << "parent with transform found\n";
		}*/
		}
	} else if (Alembic::AbcGeom::ISubDSchema::matches(md)) {
		std::cout << "ISubDSchema not implemented yet\n";
		//Alembic::AbcGeom::ISubD mesh(iObj);
		//Alembic::AbcGeom::ISubDSchema ms = mesh.getSchema();
		//for (size_t i = 0; i < ms.getNumSamples(); i++) {
		//	Alembic::AbcGeom::ISubDSchema::Sample s;
		//	ms.get(s, i);
		//	s.getPositions(); //P3fArraySamplePtr
		//	s.getFaceIndices(); //Int32ArraySamplePtr
		//	!s.getFaceCounts(); //Int32ArraySamplePtr
		//}
		//ms.getUVsParam(); //IV2fGeomParam
		//Box3d bnds = getBounds( iObj, seconds );
		//std::cout << path << " " << bnds.min << " " << bnds.max << std::endl;
	} else if (Alembic::AbcGeom::ILight::matches(md)) {
	    std::cout << "Light\n";
	} else if (Alembic::AbcGeom::IXform::matches(md)) {
	    //std::cout << "XFOOOORM\n\n\n";
	} else if (Alembic::AbcGeom::ICamera::matches(md)) {
	    std::cout << "Camera\n";
	    Alembic::AbcGeom::ICamera mesh(iObj);
	    Alembic::AbcGeom::ICameraSchema ms = mesh.getSchema();
	    //for (size_t i = 0; i < ms.getNumSamples(); i++) {
		    Alembic::AbcGeom::CameraSample s;
		    ms.get(s, 0);
		    s_._cameras.emplace_back();
		    Camera& mainCamera = s_._cameras[s_._cameras.size() - 1];

		    mainCamera.lookAt(glm::vec3(.0f, 5.0f, .0f)); // cube.abc
		    //mainCamera.lookAt(glm::vec3(-1.35f, 1.06f, 6.75f)); // cube.abc
		    //mainCamera.setPos(glm::vec3(-15.3, 5.0f, .0f)); // cube.abc
		    mainCamera.setPos(glm::vec3(.3, 5.0f, -5.0f)); // cube.abc
		    mainCamera.fieldOfview(s.getFieldOfView());
		    mainCamera.clipPlane(glm::vec2(s.getNearClippingPlane(), s.getFarClippingPlane()));
		    mainCamera.upVector(glm::vec3(0.0f,-1.0f,0.0f));
		    if (Alembic::AbcGeom::IXform::matches(iObj.getParent().getMetaData())) {
			    Alembic::AbcGeom::IXform xForm(iObj.getParent());
			    Alembic::AbcGeom::IXformSchema xFormSchema = xForm.getSchema();
			    Alembic::AbcGeom::XformSample xFormSample;
			    xFormSchema.get(xFormSample);
			    //glm::vec3 cameraPosition(xFormSample.getTranslation().x, -xFormSample.getTranslation().z, xFormSample.getTranslation().y);
			    //glm::vec3 cameraPosition(xFormSample.getTranslation().x, xFormSample.getTranslation().z, xFormSample.getTranslation().y);
			    //glm::vec3 cameraPosition(AlembicVec3toGlmVec3(xFormSample.getTranslation()));
			    //std::cout << "camera position from file = x:" << cameraPosition.x << " y:" << cameraPosition.y << " z:" << cameraPosition.z << '\n';
			    //mainCamera.setPos(glm::vec3(cameraPosition));
			    //Alembic::Abc::M44d matrix = xFormSample.getMatrix();
			    //mainCamera.uCamera = M44d4x4ToGlm(xFormSample.getMatrix());
		    } else {
			std::cout << "The position of the camera is not gotten in the file because lazyness, that's why :D\n";
		    }
	    //}
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
	    std::cout  << "Alembic library version: "
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
    //std::cout << "number of meshes:" << s_._meshes.size() << '\n';
    s_._cameras.emplace_back();
    Camera& mainCamera = s_._cameras[s_._cameras.size() - 1];

    mainCamera.lookAt(glm::vec3(.0f, .0f, .0f)); // cube.abc
    mainCamera.setPos(glm::vec3(-3.3, .0f, .0f)); // cube.abc
    //mainCamera.lookAt(glm::vec3(59.0f, 131.0f, 582.0f)); // DemoCity.obj
    //mainCamera.setPos(glm::vec3(136.0f, 231.0f, 218.0f)); // DemoCity.obj
    //mainCamera.setPos(glm::vec3(-136.0f, 231.0f, 18.0f));
    mainCamera.fieldOfview(1.623f);
    mainCamera.clipPlane(glm::vec2(0.1f, 10000.0f));
    mainCamera.upVector(glm::vec3(0.0f,1.0f,0.0f));
}

#else

inline glm::mat4 Importer::aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;

    to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

    return to;
}

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
    /*if (scene->mNumCameras) {
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
    } else {*/
	std::cout << "no Camera Detected\n";
	mainCamera.lookAt(glm::vec3(.0f, 3.7f, 8.4f)); // Nelo.obj
	//mainCamera.setPos(glm::vec3(-9.3, 4.4f, 15.9)); // Nelo.obj
	mainCamera.setPos(glm::vec3(-15.3, 4.4f, 0.0)); // NeloNOARM.obj

	//mainCamera.lookAt(glm::vec3(59.0f, 131.0f, 582.0f)); // DemoCity.obj
	//mainCamera.setPos(glm::vec3(136.0f, 231.0f, 218.0f)); //DemoCity.obj
	mainCamera.fieldOfview(1.623f);
	mainCamera.clipPlane(glm::vec2(0.1f, 10000.0f));
	mainCamera.upVector(glm::vec3(0.0f,1.0f,0.0f));
	s_._cameras.emplace_back(s_._cameras[0]);
	for(unsigned int i = 0; i < 250; ++i) {
	    s_._cameras.emplace_back(s_._cameras[0]);
	}
    //}
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
