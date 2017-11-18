#include "Importer.cpp"

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

void Importer::visitObject(Alembic::Abc::IObject iObj, std::string const &iIndent, DrawBuffer& s_) {
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
		    s_._cameras.emplace_back(s_._fb[0]);
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

void Importer::load(std::string& file, DrawBuffer& s_) {
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
    s_._cameras.emplace_back(s_._fb[0]);
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

