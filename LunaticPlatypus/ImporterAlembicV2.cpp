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

#include "StaticGameClass.hh"

void Importer::load(std::string& file, RenderThread& d_, Heart::IGamelogic* g_, Graph& scene_) {
    std::chrono::time_point<std::chrono::high_resolution_clock> beginLoad = std::chrono::high_resolution_clock::now();
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

    if (std::this_thread::get_id() != d_.getThreadId()) {
        std::cout << "\033[31mloading from a separated thread (this is unstable for now so expect glitches)\n\033[0m";
    }
    Alembic::Abc::IObject iobj = archive.getTop();
    glm::mat4 root(1.0f);
    scene_._valid = false;
    visitor(iobj, 0, d_, g_, root, scene_.root);
    scene_._valid = true;
    scene_.update(true); //readjusting local matricies


    std::function<void(void)> f = [&d_](){ d_.unsafeGetRenderer().getDrawBuffer().addAllUniformsToShaders(); };
    d_.uniqueTasks.push_back(f);

    std::chrono::time_point<std::chrono::high_resolution_clock> endLoad = std::chrono::high_resolution_clock::now();
    std::cout << "load finished: " << std::chrono::duration_cast<std::chrono::milliseconds>(endLoad - beginLoad).count() << " ms" << std::endl;
}

glm::mat4 convertTo(Alembic::Abc::M44d&& from) {
    glm::mat4 to;
    to[0][0] = (GLfloat)from[0][0]; to[0][1] = (GLfloat)from[0][1];  to[0][2] = (GLfloat)from[0][2]; to[0][3] = (GLfloat)from[0][3];
    to[1][0] = (GLfloat)from[1][0]; to[1][1] = (GLfloat)from[1][1];  to[1][2] = (GLfloat)from[1][2]; to[1][3] = (GLfloat)from[1][3];
    to[2][0] = (GLfloat)from[2][0]; to[2][1] = (GLfloat)from[2][1];  to[2][2] = (GLfloat)from[2][2]; to[2][3] = (GLfloat)from[2][3];
    to[3][0] = (GLfloat)from[3][0]; to[3][1] = (GLfloat)from[3][1];  to[3][2] = (GLfloat)from[3][2]; to[3][3] = (GLfloat)from[3][3];
    return to;
}

void Importer::visitor(const Alembic::Abc::IObject& iobj, unsigned int it, RenderThread& s_, Heart::IGamelogic* g_, glm::mat4 transform_, Node& node_) {
    const Alembic::Abc::MetaData &md = iobj.getMetaData();

    Node& x = node_.push(std::string(iobj.getName()));
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
	x.setLocalTransform(transformUpdate(iobj, transform_));
	std::string gameClassType("_GameClass");
	if ((gameClassType.size() < iobj.getName().size()) && (gameClassType == iobj.getName().substr(iobj.getName().size() - gameClassType.size()))) {
	    std::string name = iobj.getName().substr(0, iobj.getName().size() - gameClassType.size());
	    genGameClass(name, g_, transform_, x);
	}
    } else if (Alembic::AbcGeom::ICamera::matches(md)) {
	genCamera(iobj, s_, transform_, x);
    } else if (Alembic::AbcGeom::IPolyMesh::matches(md)) {
	genMesh(iobj, s_, transform_, x);
    } else {
	std::cout << "unknown type: " << md.serialize() << '\n';
    }

    for (size_t i = 0 ; i < iobj.getNumChildren() ; i++) {
	visitor(iobj.getChild(i), it + 1, s_, g_, transform_, x);
    }
}

glm::mat4 Importer::transformUpdate(const Alembic::Abc::IObject& iobj, glm::mat4& transform_) {
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
    return to;
}

void Importer::genMesh(const Alembic::Abc::IObject& iobj, RenderThread& s_, glm::mat4& transform_, Node& n_) {
    Alembic::AbcGeom::IPolyMesh mesh(iobj);
    Alembic::AbcGeom::IPolyMeshSchema schema = mesh.getSchema();

    std::vector<GLfloat>* vertexBuffer = nullptr;
    std::vector<GLuint>* indiceBuffer = nullptr;

    Alembic::AbcCoreAbstract::index_t index = 0;
    Alembic::Abc::P3fArraySamplePtr points = schema.getPositionsProperty().getValue(Alembic::Abc::ISampleSelector(index));
    Alembic::AbcGeom::IN3fGeomParam::Sample sampN;
    schema.getNormalsParam().getExpanded(sampN, Alembic::Abc::ISampleSelector(index));
    Alembic::Abc::N3fArraySamplePtr normals = sampN.getVals();

    Alembic::Abc::V2fArraySamplePtr uvs;
    if (schema.getUVsParam().valid()) {
	Alembic::AbcGeom::IV2fGeomParam::Sample sampUV;
	schema.getUVsParam().getExpanded(sampUV, Alembic::Abc::ISampleSelector(index));
	uvs = sampUV.getVals();
    }

    unsigned int numPoints = points->size();
    vertexBuffer = new std::vector<GLfloat>();
    vertexBuffer->reserve(numPoints);
    for (unsigned int i = 0; i < numPoints; ++i) {
	vertexBuffer->push_back((*points)[i].x);
	vertexBuffer->push_back((*points)[i].y);
	vertexBuffer->push_back((*points)[i].z);

	if (schema.getNormalsParam().valid()) {
	    vertexBuffer->push_back((*normals)[i].x);
	    vertexBuffer->push_back((*normals)[i].y);
	    vertexBuffer->push_back((*normals)[i].z);
	} else {
	    std::cout << "normal data absent will result in incorrect shading\n";
	    vertexBuffer->push_back(0.0f);
	    vertexBuffer->push_back(0.0f);
	    vertexBuffer->push_back(0.0f);
	} 
	
	if (schema.getUVsParam().valid()) {
	    vertexBuffer->push_back((*uvs)[i].x);
	    vertexBuffer->push_back((*uvs)[i].y);
	} else {
	    vertexBuffer->push_back(0.0f);
	    vertexBuffer->push_back(0.0f);
	}
    }

    // Get face count info
    Alembic::AbcGeom::IPolyMeshSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));
    Alembic::Abc::Int32ArraySamplePtr iCounts = samp.getFaceCounts();
    Alembic::Abc::Int32ArraySamplePtr iIndices = samp.getFaceIndices();
    unsigned int numConnects = iIndices->size();

    std::vector<unsigned long> faceBaseOffset;
    faceBaseOffset.reserve(iCounts->size());
    unsigned int base = 0;
    for (unsigned int i = 0; i < iCounts->size(); ++i) {
	faceBaseOffset.push_back(base);
        base += (*iCounts)[i];
    }

    std::vector<std::string> oFaceSetNames;
    schema.getFaceSetNames(oFaceSetNames);
    //decltype(_shaderList)* shaderListLambdaHelper = &_shaderList;
    DrawBuffer& d = s_.unsafeGetRenderer().getDrawBuffer();

    Mesh* meshReferance = nullptr;
    std::list<std::pair<Shader, std::vector<Mesh>>>::iterator selectedShader;
    for (std::string faceSetName : oFaceSetNames) {
        // -------- extracting data ----------
        Alembic::AbcGeom::IFaceSetSchema fSetSamp = schema.getFaceSet(faceSetName).getSchema();
        Alembic::AbcGeom::IFaceSetSchema::Sample faceSet;
        fSetSamp.get(faceSet, 0);
        indiceBuffer = new std::vector<GLuint>();
        indiceBuffer->reserve(numConnects);
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
                indiceBuffer->push_back((*iIndices)[base+0]);
                indiceBuffer->push_back((*iIndices)[base+2]);
                indiceBuffer->push_back((*iIndices)[base+1]);
            } else if (size == 4) {
                indiceBuffer->push_back((*iIndices)[base+0]);
                indiceBuffer->push_back((*iIndices)[base+3]);
                indiceBuffer->push_back((*iIndices)[base+1]);

                indiceBuffer->push_back((*iIndices)[base+1]);
                indiceBuffer->push_back((*iIndices)[base+3]);
                indiceBuffer->push_back((*iIndices)[base+2]);
            } 
        }

        // --------- pushing it to the renderThread -----------
        std::map<std::string, std::list<std::pair<Shader, std::vector<Mesh>>>::iterator>::iterator shaderN = _shaderList.find(faceSetName);
        if(shaderN != _shaderList.end()) {
            selectedShader = shaderN->second;
        } else {
            d._drawList.emplace_back();
            selectedShader = std::prev(d._drawList.end());
            _shaderList[faceSetName] = selectedShader;


            std::function<void(void)> createShader = [selectedShader, faceSetName]() {
                        selectedShader->first.add(faceSetName + ".material/vertex.glsl", GL_VERTEX_SHADER);
                        selectedShader->first.add(faceSetName + ".material/fragment.glsl", GL_FRAGMENT_SHADER);
                        selectedShader->first.link({"gPosition", "gNormal", "gAlbedoSpec"});
                        selectedShader->second.reserve(1024);
                };
            s_.uniqueTasks.push_back(createShader);
        }

        if (!meshReferance && selectedShader->second.size()) {
            meshReferance = &(selectedShader->second.back());
            vertexBuffer = nullptr;
        }


        std::function<void(void)> meshUpload = [selectedShader, meshReferance, vertexBuffer, indiceBuffer, &n_, transform_]() {
            // upload the mesh
            Mesh* internalMeshRef = meshReferance; // avoiding "cannot assign to a variable captured by copy in a non-mutable lambda" error
            selectedShader->second.emplace_back();
            if (vertexBuffer && !internalMeshRef) { // internalMeshRef will be null for the first mesh (because it's the reference)
                internalMeshRef = &(selectedShader->second.back()); // in the first mesh case the mesh reference will still be nullptr
                // it is still beter to do it that way because it avoir passing meshReference bu reference and set it in the lambda
                // passing meshReference by reference mean adding conditionnal mutex (and therefore breaking multithreading advantages)
                // mutex will be mandatory because genMesh function can finish before this lambda is executed.
                // (this case can happen if there is a lot of task pushed to the renderthread or when the frametime budget will be added)
                internalMeshRef->uploadVertexOnly(*vertexBuffer);
            }
            if (indiceBuffer) {
                Mesh& meshRef = selectedShader->second.back();
                meshRef.uploadElementOnly(*indiceBuffer, internalMeshRef->_vbo, internalMeshRef->_vao); // TODO use a iterator instead of meshBaseVBO
                meshRef.uMeshTransform = transform_;
                meshRef.linkNode(n_.push());
            }
        };

        std::function<void(void)> delayedIndexFree = [indiceBuffer]() {
            delete indiceBuffer;
        };
        s_.uniqueTasks.push_back(meshUpload);
        s_.uniqueTasks.push_back(delayedIndexFree);
    }

    std::function<void(void)> delayedVertexFree = [vertexBuffer]() {
        delete vertexBuffer;
    };
    s_.uniqueTasks.push_back(delayedVertexFree);
}

void Importer::genCamera(const Alembic::Abc::IObject& iobj, RenderThread& s_, glm::mat4& transform_, Node& n_) {
    Alembic::AbcGeom::ICamera cam(iobj);
    Alembic::AbcGeom::ICameraSchema ms = cam.getSchema();
    Alembic::AbcGeom::CameraSample s;

    ms.get(s, 0);
    DrawBuffer& d = s_.unsafeGetRenderer().getDrawBuffer();
    s_.uniqueTasks.push_back([&d, s, transform_](){
	    d._cameras.emplace_back(d._fb[0]);
	    Camera& mainCamera = d._cameras[d._cameras.size() - 1];

	    //mainCamera.lookAt(glm::vec3(.0f, 4.5f, 8.4f));
	    //mainCamera.setPos(glm::vec3(-9.3, 8.4f, 15.9));
	    //mainCamera.upVector(glm::vec3(0.0f, 1.0f, 0.0f));
            
            mainCamera.setViewMatrix(glm::mat4(transform_));

	    //mainCamera.fieldOfview(90.0f);
	    mainCamera.fieldOfview(s.getFieldOfView()); //valdrind said it make a "Conditional jump or move depends on uninitialised value(s)"
	    mainCamera.clipPlane(glm::vec2(s.getNearClippingPlane(), s.getFarClippingPlane())); //valdrind said it make a "Conditional jump or move depends on uninitialised value(s)"
    });
    //n_.linkWorldTransform(&(mainCamera.uMeshTransform._value.m4));
}

void Importer::genGameClass(const std::string& name_, Heart::IGamelogic* g_, glm::mat4&, Node& n_) {
    std::cout << "gameClass detected: " << name_ << '\n';
    GameClass* gc = nullptr;
    gc = StaticGameClassGenerator::gen(name_, n_);
    if (!gc) {
        std::cout << name_ << " is loaded as Dynamic\n";
        gc = new DynamicGameClass(name_, n_);
    } else {
        std::cout << name_ << " is loaded as static\n";
    }
    g_->_gameClasses.push_back(gc);
}
