#warning "using Alembic Importer Version 2"
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

void Importer::load(std::string& file, DrawBuffer& s_) {
    s_._drawList.emplace_back();
    std::pair<Shader, std::vector<Mesh>>& o =  s_._drawList.back();

    o.first.add("gold.material/fragment.glsl", GL_FRAGMENT_SHADER);
    o.first.add("gold.material/vertex.glsl", GL_VERTEX_SHADER);
    o.first.link({"gPosition", "gNormal", "gAlbedoSpec"});

    //o.second.reserve(shapes.size());



    // Create an instance of the Importer class
    Alembic::AbcCoreFactory::IFactory factory;
    factory.setPolicy(Alembic::Abc::ErrorHandler::kQuietNoopPolicy);
    Alembic::AbcCoreFactory::IFactory::CoreType coreType;
    Alembic::Abc::IArchive archive = factory.getArchive(file, coreType);

    std::cout  << "Alembic library version: " << Alembic::AbcCoreAbstract::GetLibraryVersion() << '\n';
    if (!archive || !archive.valid()) {
	std::cout << "Alembic: failed to open archive\n" << std::endl;
    }

    std::string appName;
    std::string libraryVersionString;
    Alembic::Util::uint32_t libraryVersion;
    std::string whenWritten;
    std::string userDescription;
    GetArchiveInfo (archive, appName, libraryVersionString, libraryVersion, whenWritten, userDescription);

    std::cout << "file written by: " << appName << '\n' << "using Alembic : " << libraryVersionString << '\n' << "user description : " << userDescription << '\n';

    Alembic::Abc::IObject iobj = archive.getTop();
    glm::mat4 root(1.0f);
    visitor(iobj, 0, s_, root);
}

void Importer::visitor(const Alembic::Abc::IObject& iobj, unsigned int it, DrawBuffer& s_, glm::mat4 transform_) {
    const Alembic::Abc::MetaData &md = iobj.getMetaData();
    std::string path = iobj.getName();
    for (unsigned int i =0 ; i < it; ++i) {
	std::cout << " ";
    }

    if (Alembic::AbcGeom::ICurves::matches(md)) {
	//std::cout << "Curves not implemented yet\n";
    } else if (Alembic::AbcGeom::INuPatch::matches(md)) {
	//std::cout << "NuPatch not implemented yet\n";
    } else if (Alembic::AbcGeom::IPoints::matches(md)) {
	//std::cout << "Points not implemented yet\n";
    } else if (Alembic::AbcGeom::ISubDSchema::matches(md)) {
	//std::cout << "ISubDSchema not implemented yet\n";
    } else if (Alembic::AbcGeom::IFaceSet::matches(md)) {
	//std::cout << "IFaceset not implemented yet\n";
    } else if (Alembic::AbcGeom::ILight::matches(md)) {
	//std::cout << "Lights not implemented yet\n";
    } else if (Alembic::AbcGeom::IXform::matches(md)) {
	transformUpdate(iobj, transform_);
    } else if (Alembic::AbcGeom::ICamera::matches(md)) {
	genCamera(iobj , s_, transform_);
    } else if (Alembic::AbcGeom::IPolyMesh::matches(md)) {
	genMesh(iobj, s_, transform_);
    } else {
	std::cout << "unknown type: " << md.serialize() << '\n';
    }

    for (size_t i = 0 ; i < iobj.getNumChildren() ; i++) {
	visitor(iobj.getChild(i), it + 1, s_, transform_);
    }
}

void Importer::transformUpdate(const Alembic::Abc::IObject& iobj, glm::mat4& transform_) {
    std::cout << "updating transform from xform\n";
    Alembic::AbcGeom::IXform matrix(iobj);
    Alembic::AbcGeom::IXformSchema ms = matrix.getSchema();
    Alembic::AbcGeom::XformSample s;
    ms.get(s, 0);

    glm::mat4 to;
    Alembic::Abc::M44d from(s.getMatrix());
    to[0][0] = (GLfloat)from[0][0]; to[0][1] = (GLfloat)from[0][1];  to[0][2] = (GLfloat)from[0][2]; to[0][3] = (GLfloat)from[0][3];
    to[1][0] = (GLfloat)from[1][0]; to[1][1] = (GLfloat)from[1][1];  to[1][2] = (GLfloat)from[1][2]; to[1][3] = (GLfloat)from[1][3];
    to[2][0] = (GLfloat)from[2][0]; to[2][1] = (GLfloat)from[2][1];  to[2][2] = (GLfloat)from[2][2]; to[2][3] = (GLfloat)from[2][3];
    to[3][0] = (GLfloat)from[3][0]; to[3][1] = (GLfloat)from[3][1];  to[3][2] = (GLfloat)from[3][2]; to[3][3] = (GLfloat)from[3][3];
    if (!s.getInheritsXforms()) {
	transform_ = to;
    } else {
	transform_ *= to;
    }
}

void Importer::genMesh(const Alembic::Abc::IObject& iobj, DrawBuffer& s_, glm::mat4& transform_) {
    std::cout << "generating Mesh: " << iobj.getName() << std::endl;

    Alembic::AbcGeom::IPolyMesh mesh(iobj);
    Alembic::AbcGeom::IPolyMeshSchema ms = mesh.getSchema();
    Alembic::AbcGeom::IV2fGeomParam uvParam = ms.getUVsParam();
    Alembic::AbcGeom::IN3fGeomParam normalsParam = ms.getNormalsParam();

    std::vector<GLfloat> vertexBuffer;
    std::vector<GLuint> indiceBuffer;
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

    std::pair<Shader, std::vector<Mesh>>& o =  s_._drawList.back();
    o.second.emplace_back();
    o.second[o.second.size() - 1].uploadToGPU(vertexBuffer, indiceBuffer);
    o.second[o.second.size() - 1].uMeshTransform = transform_;
    o.second[o.second.size() - 1]._name = iobj.getName();
}

void Importer::genCamera(const Alembic::Abc::IObject& iobj, DrawBuffer& s_, glm::mat4& transform_) {
    std::cout << "generating Camera" << std::endl;
    Alembic::AbcGeom::ICamera cam(iobj);
    Alembic::AbcGeom::ICameraSchema ms = cam.getSchema();
    Alembic::AbcGeom::CameraSample s;

    ms.get(s, 0);
    s_._cameras.emplace_back(s_._fb[0]);
    Camera& mainCamera = s_._cameras[s_._cameras.size() - 1];

    mainCamera.lookAt(glm::vec3(.0f, 4.5f, 8.4f));
    mainCamera.setPos(glm::vec3(-9.3, 8.4f, 15.9));
    mainCamera.fieldOfview(s.getFieldOfView());
    mainCamera.clipPlane(glm::vec2(s.getNearClippingPlane(), s.getFarClippingPlane()));
    mainCamera.upVector(glm::vec3(0.0f,-1.0f,0.0f));
}
