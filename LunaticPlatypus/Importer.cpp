#define TINYOBJLOADER_IMPLEMENTATION		
#include <iostream>
#include <map>
#include "Importer.hh"

Importer::Importer(std::string file, DrawBuffer& d_, Heart::IGamelogic* g_, Graph& s_) {
    load(file, d_, g_, s_);
}

#ifdef TINYOBJLOADER
#include "ImporterTinyobj.cpp"
#elif defined(ALEMBICV2)
#include "ImporterAlembicV2.cpp"
#elif defined(ALEMBIC)
#include "ImporterAlembic.cpp"
#elif defined(ASSIMP)
#include "ImporterAssimp.cpp"
#else
#error "You have to define which importer implementation you want"
#endif
