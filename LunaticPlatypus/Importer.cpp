#define TINYOBJLOADER_IMPLEMENTATION		
#include <iostream>
#include <map>
#include "Importer.hh"

Importer::Importer(std::string file, DrawBuffer& s_, Heart::IGamelogic* g_) {
    load(file, s_, g_);
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
