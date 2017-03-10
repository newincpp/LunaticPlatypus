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
    //s_._drawList.emplace_back();
    //std::pair<Shader, std::vector<Mesh>>& o =  s_._drawList.back();

    //o.first.add(shader.name + ext + "fragment.glsl", GL_FRAGMENT_SHADER);
    //o.first.add(shader.name + ext + "vertex.glsl", GL_VERTEX_SHADER);
    //o.first.link({"gPosition", "gNormal", "gAlbedoSpec"});

    //o.second.reserve(shapes.size());

    //s_._cameras.emplace_back(s_._fb[0]);




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
    const Alembic::Abc::MetaData &md = iobj.getMetaData();
    std::string path = iobj.getName();

    if (Alembic::AbcGeom::ICurves::matches(md)) {
	std::cout << "Curves not implemented yet\n";
    } else if (Alembic::AbcGeom::INuPatch::matches(md)) {
	std::cout << "NuPatch not implemented yet\n";
    } else if (Alembic::AbcGeom::IPoints::matches(md)) {
	std::cout << "Points not implemented yet\n";
    } else if (Alembic::AbcGeom::ISubDSchema::matches(md)) {
	std::cout << "ISubDSchema not implemented yet\n";
    } else if (Alembic::AbcGeom::ILight::matches(md)) {
	std::cout << "Lights not implemented yet\n";
    } else if (Alembic::AbcGeom::IXform::matches(md)) {
	std::cout << "==== XFORM here ====\n";
    } else if (Alembic::AbcGeom::ICamera::matches(md)) {
	genCamera();
    } else if (Alembic::AbcGeom::IPolyMesh::matches(md)) {
	genMesh();
    }
}

void Importer::genMesh() {
    std::cout << "generating Mesh" << std::endl;
}

void Importer::genCamera() {
    std::cout << "generating Camera" << std::endl;
}
