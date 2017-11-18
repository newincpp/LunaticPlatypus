#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "Importer.cpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#include "StaticGameClass.hh"

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
    
    std::vector<GLuint>* GLBufferPool = new std::vector<GLuint>();
    d_.uniqueTasks.push_back(std::bind(&Importer::preload, this, *model, GLBufferPool));
    for (unsigned int node : scene.nodes) {
        visitor(model, model->nodes[node], 0, d_, g_, root, scene_.root, GLBufferPool);
    }
    d_.uniqueTasks.push_back([&d_, model, GLBufferPool](){
        DrawBuffer& d = d_.unsafeGetRenderer().getDrawBuffer();
        d.addAllUniformsToShaders();
        if (d._cameras.empty()) { // add a default camera if empty
            std::cout << "no camera was detected in the file: generating a default one\n";
	    d._cameras.emplace_back(d._fb[0]);
	    Camera& mainCamera = d._cameras[d._cameras.size() - 1];
	    mainCamera.lookAt(glm::vec3(.0f, 4.5f, 8.4f));
	    mainCamera.setPos(glm::vec3(-9.3, 8.4f, 15.9));
	    mainCamera.fieldOfview(90.0f);
        }
        delete model;
        delete GLBufferPool;
	std::cout << "d.size(): " << d._drawList.begin()->second.size() << std::endl;
    });
    std::cout << "load finished" << std::endl;
}

void Importer::preload(tinygltf::Model &model_, std::vector<GLuint>* GLBufferPool_) {
    std::cout << "preloading\n";
    GLBufferPool_->clear();
    GLBufferPool_->reserve(model_.bufferViews.size());
    GLBufferPool_->resize(model_.bufferViews.size());
    for (size_t i = 0; i < model_.bufferViews.size(); i++) { // TODO : batch into reasonably sized renderThread::uniqueTask to avoid stalling for a long time while uploading to GPU
      const tinygltf::BufferView &bufferView = model_.bufferViews[i];
      if (bufferView.target == 0) {
        std::cout << "WARN: bufferView.target is zero" << std::endl;
        continue;  // Unsupported bufferView.
      }
      const tinygltf::Buffer &buffer = model_.buffers[bufferView.buffer];
      glGenBuffers(1, &(*GLBufferPool_)[i]);
      glBindBuffer(bufferView.target, (*GLBufferPool_)[i]);
      glBufferData(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
      glBindBuffer(bufferView.target, 0);
    }
    std::cout << "------- preload done, " << GLBufferPool_->size() << " allocated \n";
}

glm::mat4 convertTo(const decltype(tinygltf::Node::matrix)& from) {
    if (from.size() == 16) {
        return glm::make_mat4(from.data());
    } else {
        return glm::mat4(1.0f);
    }
}

void Importer::visitor(const tinygltf::Model* model_, const tinygltf::Node& nod_, unsigned int depth, RenderThread& renderThread_, Heart::IGamelogic* glog_, glm::mat4 transf_, Node& sceneGraph_, std::vector<GLuint>* GLBufferPool_) {
    transf_ *= convertTo(nod_.matrix);
    if (nod_.mesh >= 0) {
        //std::cout << "mesh id: " << nod_.mesh << '\n';
        renderThread_.uniqueTasks.push_back(std::bind(&Importer::genMesh, this, model_, model_->meshes[nod_.mesh], &renderThread_, transf_, sceneGraph_, GLBufferPool_)); // TODO: this code will diplicate meshes if an index gets loaded twice, as a Mesh contain its transform matrix this is fine until glDrawElementsInstanced is implemented
    }
    if (nod_.camera >= 0) {
        std::cout << "camera id: " << nod_.camera << '\n';
        renderThread_.uniqueTasks.push_back(std::bind(&Importer::genCamera, this, model_->cameras[nod_.camera], &renderThread_, transf_, sceneGraph_));
    }
    if ((nod_.mesh == -1) && (nod_.camera == -1) && (nod_.name.size() > 10) && !(nod_.name.substr(nod_.name.size() - 10).compare("_GameClass"))) {
        std::cout << "gameclass detected: \"" << nod_.name << "\"\n";
        genGameClass(nod_.name.substr(0, nod_.name.size() - 10), glog_, transf_, sceneGraph_);
    }
    for (unsigned int child: nod_.children) {
        visitor(model_, model_->nodes[child], depth + 1, renderThread_, glog_, transf_, sceneGraph_, GLBufferPool_);
    }
}
void Importer::genMesh(const tinygltf::Model* model_, const tinygltf::Mesh& mesh_, RenderThread* renderThread_, glm::mat4, Node&, std::vector<GLuint>* GLBufferPool_) {
	//Node& n = sceneGraph_.push();

	std::map<std::string, GLuint> attributeLocation = {{"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2}};
	std::cout << "------- using pool of prealocated GLBuffer: " << GLBufferPool_->size() << " allocated \n";

	std::list<std::pair<Shader, std::vector<Mesh>>>& dlist = renderThread_->unsafeGetRenderer().getDrawBuffer()._drawList;
	if (dlist.empty()) {
		dlist.emplace_back();
		dlist.back().first.add("default.material/vertex.glsl", GL_VERTEX_SHADER);
		dlist.back().first.add("default.material/fragment.glsl", GL_FRAGMENT_SHADER);
		dlist.back().first.link({"gPosition", "gNormalRough", "gAlbedoMetallic"});
	}

	std::cout << "primitive size: " << mesh_.primitives.size() << '\n';

	// gltf documentation cf:
	// "Implementation note: Each primitive corresponds to one WebGL draw call"
	// a lunatic Platypus "Mesh" also represent a DrawCall
	for (const tinygltf::Primitive &primitive : mesh_.primitives) {
		if (primitive.indices >= 0) {
			std::cout << "----> generating mesh" << std::endl;
			dlist.back().second.emplace_back();
			Mesh& m = dlist.back().second.back();

			glGenVertexArrays(1, &m._vao);
			glBindVertexArray(m._vao);

			for (const std::pair<const std::string, int>& attribute : primitive.attributes) {
				const tinygltf::Accessor &accessor = model_->accessors[attribute.second];
				m._vbo = (*GLBufferPool_)[accessor.bufferView];
				glBindBuffer(GL_ARRAY_BUFFER, m._vbo);
				int size = 1;
				if (accessor.type == TINYGLTF_TYPE_SCALAR) {
					size = 1;
				} else if (accessor.type == TINYGLTF_TYPE_VEC2) {
					size = 2;
				} else if (accessor.type == TINYGLTF_TYPE_VEC3) {
					size = 3;
				} else if (accessor.type == TINYGLTF_TYPE_VEC4) {
					size = 4;
				} else {
					assert(0);
				}

				// it->first would be "POSITION", "NORMAL", "TEXCOORD_0", ...
				std::cout << "set attribute: " << attribute.first << " ";
				if (attributeLocation.find(attribute.first) != attributeLocation.end()) {
					std::cout << " at location: " << attributeLocation[attribute.first] << '\n';
					glEnableVertexAttribArray(attributeLocation[attribute.first]);
					glVertexAttribPointer(attributeLocation[attribute.first], size, accessor.componentType, GL_FALSE, model_->bufferViews[accessor.bufferView].byteStride, (void*)accessor.byteOffset);
					//if (accessor.componentType == GL_FLOAT) {
					//    std::cout << "type is glFloat\n";
					//}
					//std::cout << "offset inplace: " << accessor.byteOffset << " size is: " << size << '\n';
				} else {
					std::cout << attribute.first << "this attribute isn't a vertex buffer\n";
				}

			}

			const tinygltf::Accessor &indexAccessor = model_->accessors[primitive.indices];
			m._ebo = (*GLBufferPool_)[indexAccessor.bufferView];
			m._size = indexAccessor.count;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*GLBufferPool_)[indexAccessor.bufferView]);
			//CheckErrors("bind buffer");
			//int mode = -1;
			if (primitive.mode != TINYGLTF_MODE_TRIANGLES) {
				std::cout << "++++++++++ WARN: it's gonna display wierd !\n";
			}
		}
	}
}

void Importer::genCamera(const tinygltf::Camera& gltfCam_, RenderThread* rt_, glm::mat4 transform_, Node&) {
    DrawBuffer& d = rt_->unsafeGetRenderer().getDrawBuffer();
    std::cout << "camera Name: " << gltfCam_.name << '\n';

    d._cameras.emplace_back(d._fb[0]);
    Camera& cam = d._cameras.back();
    cam.lookAt(glm::vec3(.0f, 4.5f, 8.4f));
    glm::vec4 position = glm::vec4(0.0, 0.0, 0.0, 1.0) * transform_;
    cam.setPos(glm::vec3(position.x, position.y, position.z));
    //cam.upVector(glm::vec3(0.0f, -1.0f, 0.0f));
    if (gltfCam_.type.compare("perspective") == 0) {
        //std::cout << "aspect ratio: " << gltfCam_.perspective.aspectRatio << "\n";
        //std::cout << "fovy :" << gltfCam_.perspective.yfov << '\n';
        //std::cout << "calculated fovx rad:" << (gltfCam_.perspective.yfov * gltfCam_.perspective.aspectRatio) << '\n';
        //std::cout << "calculated fovx deg:" << ((gltfCam_.perspective.yfov * 180 / 3.14159)) * gltfCam_.perspective.aspectRatio << '\n';
        //cam.fieldOfview(90.0f);
        cam.fieldOfview(((gltfCam_.perspective.yfov * 180 / 3.14159)) * gltfCam_.perspective.aspectRatio);
        cam.clipPlane(glm::vec2(gltfCam_.perspective.znear, gltfCam_.perspective.zfar));
    }
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
