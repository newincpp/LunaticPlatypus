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
#include "DynamicGameClass.hh"

void Importer::load(std::string& file, DrawBuffer& s_) {
    // Create an instance of the Importer class
    Alembic::AbcCoreFactory::IFactory factory;
    factory.setPolicy(Alembic::Abc::ErrorHandler::kQuietNoopPolicy);
    Alembic::AbcCoreFactory::IFactory::CoreType coreType;
    Alembic::Abc::IArchive archive = factory.getArchive(file, coreType);

    std::cout  << "Alembic library version: " << Alembic::AbcCoreAbstract::GetLibraryVersion() << '\n';
    if (!archive || !archive.valid()) {
	std::cout << "Alembic: failed to open " << file << "\n" << std::endl;
	return;
    }

    std::string appName;
    std::string libraryVersionString;
    Alembic::Util::uint32_t libraryVersion;
    std::string whenWritten;
    std::string userDescription;
    GetArchiveInfo (archive, appName, libraryVersionString, libraryVersion, whenWritten, userDescription);

    std::cout << "file written by: " << appName << ' ' << "using: " << libraryVersionString << '\n' << "user description : " << userDescription << '\n';

    Alembic::Abc::IObject iobj = archive.getTop();
    glm::mat4 root(1.0f);
    visitor(iobj, 0, s_, root);
}

void Importer::visitor(const Alembic::Abc::IObject& iobj, unsigned int it, DrawBuffer& s_, glm::mat4 transform_) {
    const Alembic::Abc::MetaData &md = iobj.getMetaData();

    if (Alembic::AbcGeom::ICurves::matches(md)) {
	std::cout << "Curves not implemented yet\n";
    } else if (Alembic::AbcGeom::INuPatch::matches(md)) {
	std::cout << "NuPatch not implemented yet\n";
    } else if (Alembic::AbcGeom::IPoints::matches(md)) {
	std::cout << "Points not implemented yet\n";
    } else if (Alembic::AbcGeom::ISubDSchema::matches(md)) {
	std::cout << "ISubDSchema not implemented yet\n";
    } else if (Alembic::AbcGeom::IFaceSet::matches(md)) {
	//std::cout << "IFaceset not implemented yet\n";
    } else if (Alembic::AbcGeom::ILight::matches(md)) {
	std::cout << "Lights not implemented yet\n";
    } else if (Alembic::AbcGeom::IXform::matches(md)) {
	transformUpdate(iobj, transform_);
	std::string gameClassType("_GameClass");
	if ((gameClassType.size() < iobj.getName().size()) && (gameClassType == iobj.getName().substr(iobj.getName().size() - gameClassType.size()))) {
	    std::string name = iobj.getName().substr(0, iobj.getName().size() - gameClassType.size());
	    genGameClass(name, transform_);
	}
    } else if (Alembic::AbcGeom::ICamera::matches(md)) {
	genCamera(iobj, s_, transform_);
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
    Alembic::AbcGeom::IPolyMesh mesh(iobj);
    Alembic::AbcGeom::IPolyMeshSchema schema = mesh.getSchema();

    std::vector<GLfloat> vertexBuffer;
    std::vector<GLuint> indiceBuffer;

    Alembic::AbcCoreAbstract::index_t index = 0;
    Alembic::Abc::P3fArraySamplePtr points = schema.getPositionsProperty().getValue(Alembic::Abc::ISampleSelector(index));
    Alembic::AbcGeom::IN3fGeomParam::Sample sampN;
    schema.getNormalsParam().getExpanded(sampN, Alembic::Abc::ISampleSelector(index));
    Alembic::Abc::N3fArraySamplePtr normals = sampN.getVals();

    unsigned int numPoints = points->size();
    vertexBuffer.reserve(numPoints);
    for (unsigned int i = 0; i < numPoints; ++i) {
	vertexBuffer.push_back((*points)[i].x);
	vertexBuffer.push_back((*points)[i].y);
	vertexBuffer.push_back((*points)[i].z);

	vertexBuffer.push_back((*normals)[i].x);
	vertexBuffer.push_back((*normals)[i].y);
	vertexBuffer.push_back((*normals)[i].z);

	vertexBuffer.push_back(0.0f);
	vertexBuffer.push_back(0.0f);
    }

    // Get face count info
    Alembic::AbcGeom::IPolyMeshSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));
    Alembic::Abc::Int32ArraySamplePtr iCounts = samp.getFaceCounts();
    Alembic::Abc::Int32ArraySamplePtr iIndices = samp.getFaceIndices();
    unsigned int numConnects = iIndices->size();
    indiceBuffer.reserve(numConnects);


    std::vector<unsigned long> faceBaseOffset;
    faceBaseOffset.reserve(iCounts->size());
    unsigned int base = 0;
    for (unsigned int i = 0; i < iCounts->size(); ++i) {
	faceBaseOffset.push_back(base);
        base += (*iCounts)[i];
    }


    std::pair<Shader, std::vector<Mesh>>* o = nullptr;
    std::pair<Shader, std::vector<Mesh>>* meshReferance =  nullptr;
    std::vector<std::string> oFaceSetNames;
    schema.getFaceSetNames(oFaceSetNames);
    bool isFirst = true;
    unsigned long meshBaseVBO;
    for (std::string faceSetName : oFaceSetNames) {
	std::map<std::string, std::list<std::pair<Shader, std::vector<Mesh>>>::iterator>::iterator shaderN = _shaderList.find(faceSetName);

	if(shaderN != _shaderList.end()) {
	    o = &(*shaderN->second);
	} else {
	    s_._valid = false;
	    s_._drawList.emplace_back();
	    o = &(s_._drawList.back());
	    o->first.add(faceSetName + ".material/fragment.glsl", GL_FRAGMENT_SHADER);
	    o->first.add(faceSetName + ".material/vertex.glsl", GL_VERTEX_SHADER);
	    o->first.link({"gPosition", "gNormal", "gAlbedoSpec"});
	    s_._valid = true;
	    _shaderList[faceSetName] = --s_._drawList.end();
	}

	Alembic::AbcGeom::IFaceSetSchema fSetSamp = schema.getFaceSet(faceSetName).getSchema();
	Alembic::AbcGeom::IFaceSetSchema::Sample faceSet;
	fSetSamp.get(faceSet, 0);
	for (unsigned int i = 0; i < faceSet.getFaces()->size(); ++i) {
	    unsigned int faceNumber = (*faceSet.getFaces())[i];
	    unsigned long base = faceBaseOffset[faceNumber];
	    unsigned short size = 3;
	    if (faceNumber < (faceSet.getFaces()->size() - 1)) {
		size = faceBaseOffset[faceNumber+1] - base;
	    } else {
		size = (*iCounts)[iCounts->size() - 1];
	    }
	    if (size == 3) {
		indiceBuffer.push_back((*iIndices)[base+2]);
		indiceBuffer.push_back((*iIndices)[base+1]);
		indiceBuffer.push_back((*iIndices)[base+0]);
	    } else if (size == 4) {
		indiceBuffer.push_back((*iIndices)[base+2]);
		indiceBuffer.push_back((*iIndices)[base+1]);
		indiceBuffer.push_back((*iIndices)[base+0]);

		indiceBuffer.push_back((*iIndices)[base+0]);
		indiceBuffer.push_back((*iIndices)[base+3]);
		indiceBuffer.push_back((*iIndices)[base+2]);
	    } 
	}
	s_._valid = false;
	o->second.emplace_back();
	if (isFirst) {
	    meshBaseVBO = o->second.size() - 1;
	    meshReferance = o;
	    o->second[meshBaseVBO].uploadVertexOnly(vertexBuffer);
	    isFirst = false;
	}
	o->second[o->second.size() - 1].uploadElementOnly(indiceBuffer, meshReferance->second[meshBaseVBO]._vbo, meshReferance->second[meshBaseVBO]._vao); // TODO use a iterator instead of meshBaseVBO
	s_._valid = true;
	o->second[o->second.size() - 1].uMeshTransform = transform_;
	indiceBuffer.clear();
    }
}

void Importer::genCamera(const Alembic::Abc::IObject& iobj, DrawBuffer& s_, glm::mat4& transform_) {
    Alembic::AbcGeom::ICamera cam(iobj);
    Alembic::AbcGeom::ICameraSchema ms = cam.getSchema();
    Alembic::AbcGeom::CameraSample s;

    ms.get(s, 0);
    s_._valid = false;
    s_._cameras.emplace_back(s_._fb[0]);
    s_._valid = true;
    Camera& mainCamera = s_._cameras[s_._cameras.size() - 1];

    mainCamera.lookAt(glm::vec3(.0f, 4.5f, 8.4f));
    mainCamera.setPos(glm::vec3(-9.3, 8.4f, 15.9));
    mainCamera.fieldOfview(s.getFieldOfView());
    mainCamera.clipPlane(glm::vec2(s.getNearClippingPlane(), s.getFarClippingPlane()));
    mainCamera.upVector(glm::vec3(0.0f, -1.0f, 0.0f));
}

void Importer::genGameClass(const std::string& name_, glm::mat4&) {
    std::cout << "gameClass detected: " << name_ << '\n';
    DynamicGameClass c(name_);
}
