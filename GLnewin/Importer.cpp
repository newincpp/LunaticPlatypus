#include <iostream>
#include <map>
#ifdef TINYOBJLOADER
#define TINYOBJLOADER_IMPLEMENTATION
#endif
#include "Importer.hh"

Importer::Importer(std::string file, DrawBuffer& s_) {
    load(file, s_);
}

#ifdef TINYOBJLOADER
#include "ImporterTinyobj.cpp"
#elif defined(ALEMBIC)
#include "ImporterAlembic.cpp"
#else
#include "ImporterAssimp.cpp"
#endif
