#define TINYOBJLOADER_IMPLEMENTATION		
#include <iostream>
#include <map>
#include "Importer.hh"

Importer::Importer(std::string file, DrawBuffer& s_) {
    load(file, s_);
}

#ifdef TINYOBJLOADER
#include "ImporterTinyobj.cpp"
#elif defined(ALEMBIC)
#include "ImporterAlembic.cpp"
#elif defined(ASSIMP)
#include "ImporterAssimp.cpp"
#else
#error "You have to define which importer implementation you want"
#endif
