//#warning "using GLTF importer"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void Importer::load(std::string& file, RenderThread& d_, Heart::IGamelogic* g_, Graph& scene_) {
    tinygltf::Model* model = new tinygltf::Model();
    std::string err;
    tinygltf::TinyGLTF loader;
    if (file.substr(file.size() - 4) == ".glb") { // assuming this is a binary
        loader.LoadBinaryFromFile(model, &err, file.c_str());
    } else {
        loader.LoadASCIIFromFile(model, &err, file.c_str());
    }
    if (err.size()) {
	std::cout << "gltf2: failed to open or parce " << file << "\n" << "with error:\n" << err << std::endl;
        return;
    }
    std::cout << "asset.copyright          : " << model->asset.copyright << '\n';
    std::cout << "asset.generator          : " << model->asset.generator << '\n';
    std::cout << "asset.version            : " << model->asset.version << '\n';
    std::cout << "asset.minVersion         : " << model->asset.minVersion << '\n';
    std::cout << "defaultScene: " << model->defaultScene << '\n';
    std::cout << std::endl;

    glm::mat4 root(1.0f);
    const tinygltf::Scene &scene = model->scenes[model->defaultScene];
    std::cout << "available scene in file:\n";
    for (size_t i = 0; i < model->scenes.size(); i++) {
        std::cout << model->scenes[i].name << '\n';
    }
    std::cout << "loading the default one: " << scene.name << '\n';
    for (unsigned int node : scene.nodes) {
        std::cout << "node id: " << node << '\n';
        visitor(model, model->nodes[node], 0, d_, g_, root, scene_.root);
    }
    d_.uniqueTasks.push_back([&d_, model](){
        d_.unsafeGetRenderer().getDrawBuffer().addAllUniformsToShaders();
        delete model;
    });
    std::cout << "load finished" << std::endl;

}

glm::mat4 convertTo(const decltype(tinygltf::Node::matrix)& from) {
    if (from.size() == 16) {
        return glm::make_mat4(from.data());
    } else {
        return glm::mat4(1.0f);
    }
}

void Importer::visitor(const tinygltf::Model* model_, const tinygltf::Node& nod_, unsigned int depth, RenderThread& renderThread_, Heart::IGamelogic* glog_, glm::mat4 transf_, Node& sceneGraph_) {
    std::cout << "node name: \"" << nod_.name << "\"\n";
    transf_ *= convertTo(nod_.matrix);
    if (nod_.mesh >= 0) {
        std::cout << "mesh id: " << nod_.mesh << '\n';
        renderThread_.uniqueTasks.push_back(std::bind(&Importer::genMesh, this, model_, model_->meshes[nod_.mesh], &renderThread_, transf_, sceneGraph_));
    }
    for (unsigned int child: nod_.children) {
        std::cout << "node id: " << child << '\n';
        visitor(model_, model_->nodes[child], depth + 1, renderThread_, glog_, transf_, sceneGraph_);
    }
}
void Importer::genMesh(const tinygltf::Model* model_, const tinygltf::Mesh& mesh_, RenderThread* renderThread_, glm::mat4, Node& sceneGraph_) {
    std::cout << "----> generating mesh" << std::endl;
    Node& n = sceneGraph_.push();

    std::map<std::string, GLuint> attributeLocation = {{"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2} };

    std::list<std::pair<Shader, std::vector<Mesh>>>& dlist = renderThread_->unsafeGetRenderer().getDrawBuffer()._drawList;
    if (dlist.empty()) {
        dlist.emplace_back();
        dlist.back().first.add("default.material/vertex.glsl", GL_VERTEX_SHADER);
        dlist.back().first.add("default.material/fragment.glsl", GL_FRAGMENT_SHADER);
        dlist.back().first.link({"gPosition", "gNormal", "gAlbedoSpec"});
    }
    dlist.back().second.emplace_back();
    Mesh& m = dlist.back().second.back();

    glGenVertexArrays(1, &m._vao);
    glBindVertexArray(m._vao);

    for (size_t i = 0; i < model_->bufferViews.size(); i++) {
        const tinygltf::BufferView &bufferView = model_->bufferViews[i];
        if (bufferView.target == 0) {
            std::cout << "WARN: bufferView.target is zero" << std::endl;
            continue;  // Unsupported bufferView.
        } else if (bufferView.target == GL_ELEMENT_ARRAY_BUFFER){
            glGenBuffers(1, &m._ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m._ebo);
            std::cout << "generating GL_ELEMENT_ARRAY_BUFFER\n";
        } else if (bufferView.target == GL_ARRAY_BUFFER){
            glGenBuffers(1, &m._vbo);
            glBindBuffer(bufferView.target, m._vbo);
            std::cout << "generating GL_ARRAY_BUFFER\n";
        }
        const tinygltf::Buffer &buffer = model_->buffers[bufferView.buffer];
        std::cout << "buffer.size= " << buffer.data.size() << ", byteOffset = " << bufferView.byteOffset << std::endl;
#ifdef BUFFER_STORAGE
        glBufferStorage(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset, STORAGE_FLAGS);
#elif BUFFER_DATA
        glBufferData(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
#endif
    }

    glBindBuffer(GL_ARRAY_BUFFER, m._vbo);
    std::cout << "primitive size: " << mesh_.primitives.size() << '\n';
    for (const tinygltf::Primitive &primitive : mesh_.primitives) {
        const tinygltf::Accessor &indexAccessor = model_->accessors[primitive.indices];
        m._size = indexAccessor.count;
        if (primitive.indices >= 0) {
            for (const std::pair<const std::string, int>& attribute : primitive.attributes) {
                const tinygltf::Accessor &accessor = model_->accessors[attribute.second];
                int count = 1;
                if (accessor.type == TINYGLTF_TYPE_SCALAR) {
                    count = 1;
                } else if (accessor.type == TINYGLTF_TYPE_VEC2) {
                    count = 2;
                } else if (accessor.type == TINYGLTF_TYPE_VEC3) {
                    count = 3;
                } else if (accessor.type == TINYGLTF_TYPE_VEC4) {
                    count = 4;
                } else {
                    assert(0);
                }
                // it->first would be "POSITION", "NORMAL", "TEXCOORD_0", ...
                std::cout << "set attribute: " << attribute.first;
                if ((attribute.first.compare("POSITION") == 0) || (attribute.first.compare("NORMAL") == 0) || (attribute.first.compare("TEXCOORD_0") == 0)) {
                    if (attributeLocation.find(attribute.first) != attributeLocation.end()) {
                        std::cout << " at location: " << attributeLocation[attribute.first] << '\n';
                        glEnableVertexAttribArray(attributeLocation[attribute.first]);
                        glVertexAttribPointer(attributeLocation[attribute.first], count, accessor.componentType, GL_FALSE, 0, BUFFER_OFFSET(accessor.byteOffset));
                        std::cout << "--------->> offset inplace: " << accessor.byteOffset << '\n';
                    } else {
                        std::cout << attribute.first << "will be in the vertex buffer object but not enabled\n";
                    }
                }
            }
        } else {
            std::cout << "primitive indices wrong \n";
        }
    }
}

void Importer::genCamera(const tinygltf::Node&, RenderThread&, glm::mat4&, Node&) {
}
void Importer::genGameClass(const std::string&, Heart::IGamelogic*, glm::mat4&, Node&) {
}
