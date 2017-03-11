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
    Alembic::AbcGeom::IPolyMeshSchema schema = mesh.getSchema();

    if (schema.getTopologyVariance() == Alembic::AbcGeom::kHeterogenousTopology) {
	std::cout << "mesh is has heterogenous topology\n";
	return;
    }


    std::vector<GLfloat> vertexBuffer;
    std::vector<GLuint> indiceBuffer;


    std::vector<std::string> oFaceSetNames;
    schema.getFaceSetNames(oFaceSetNames);
    for (std::string x : oFaceSetNames) {
	std::cout << "faceSet Name: " << x << '\n';

	Alembic::AbcGeom::IFaceSetSchema msp = schema.getFaceSet(x).getSchema();
	Alembic::AbcGeom::IFaceSetSchema::Sample p;
	msp.get(p, 0);

	std::cout << "faceset = " << p.getFaces()->size() << '\n';
	for (unsigned int i = 0; p.getFaces()->size() > i; ++i) {
	    std::cout << "f " << (*p.getFaces())[i] << '\n';
	}
    }



    Alembic::AbcCoreAbstract::index_t index;
    Alembic::Abc::P3fArraySamplePtr points = schema.getPositionsProperty().getValue(Alembic::Abc::ISampleSelector(index));
    Alembic::AbcGeom::IN3fGeomParam::Sample sampN;
    schema.getNormalsParam().getExpanded(sampN, Alembic::Abc::ISampleSelector(index));
    Alembic::Abc::N3fArraySamplePtr sampVal = sampN.getVals();
    size_t sampSize = sampVal->size();

    unsigned int numPoints = points->size();
    vertexBuffer.reserve(numPoints);
    for (unsigned int i = 0; i < numPoints; ++i) {
	vertexBuffer.push_back((*points)[i].x);
	vertexBuffer.push_back((*points)[i].y);
	vertexBuffer.push_back((*points)[i].z);

	vertexBuffer.push_back((*sampVal)[i].x);
	vertexBuffer.push_back((*sampVal)[i].y);
	vertexBuffer.push_back((*sampVal)[i].z);

	vertexBuffer.push_back(0.0f);
	vertexBuffer.push_back(0.0f);
    }

    // Get face count info
    Alembic::AbcGeom::IPolyMeshSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));
    Alembic::Abc::Int32ArraySamplePtr iCounts = samp.getFaceCounts();
    Alembic::Abc::Int32ArraySamplePtr iIndices = samp.getFaceIndices();
    unsigned int numPolys = iCounts->size();
    unsigned int numConnects = iIndices->size();
    indiceBuffer.reserve(numConnects);


    Alembic::AbcGeom::IFaceSetSchema msp = schema.getFaceSet(oFaceSetNames[0]).getSchema();
    Alembic::AbcGeom::IFaceSetSchema::Sample faceSet;
    msp.get(faceSet, 0);
    unsigned int facePointIndex = 0;
    unsigned int base = 0;
    for (unsigned int i = 0; i < numPolys; ++i) {
	//int curNum = (*iCounts)[(*faceSet.getFaces())[i]];
	int curNum = (*iCounts)[i];
	if (curNum == 3) {
	    indiceBuffer.push_back((*iIndices)[base+curNum-0-1]);
	    indiceBuffer.push_back((*iIndices)[base+curNum-1-1]);
	    indiceBuffer.push_back((*iIndices)[base+curNum-2-1]);
	    facePointIndex += 3;
	} else if (curNum == 4) {
	    indiceBuffer.push_back((*iIndices)[base+curNum-0-1]);
	    indiceBuffer.push_back((*iIndices)[base+curNum-1-1]);
	    indiceBuffer.push_back((*iIndices)[base+curNum-2-1]);

	    indiceBuffer.push_back((*iIndices)[base+curNum-2-1]);
	    indiceBuffer.push_back((*iIndices)[base+curNum-3-1]);
	    indiceBuffer.push_back((*iIndices)[base+curNum-0-1]);
	    facePointIndex += 4;
	} else {
	    //TODO ngon not supported yet
	}

	base += curNum;
    }
    std::pair<Shader, std::vector<Mesh>>& o =  s_._drawList.back();
    o.second.emplace_back();
    o.second[o.second.size() - 1].uploadToGPU(vertexBuffer, indiceBuffer);
    o.second[o.second.size() - 1].uMeshTransform = transform_;
    o.second[o.second.size() - 1]._name = iobj.getName();
}

void Importer::genCamera(const Alembic::Abc::IObject& iobj, DrawBuffer& s_, glm::mat4& transform_) {
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
    mainCamera.upVector(glm::vec3(0.0f, 1.0f, 0.0f));
}
