#warning "using tinyObjLoader"

void Importer::load(std::string& file, DrawBuffer& s_) {
    std::cout << "Import using tinyObjLoader\n";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file.c_str())) {
	std::cout << "the file is fucked: " << file << '\n';
    }
    if (!err.empty()) {
	std::cout << "loaded with warning:\n" << err << '\n';
    }

    // Loop over materials
    unsigned int material_id = 0;
    for (tinyobj::material_t& shader : materials) {
	std::cout << shader.name << '\n';
	s_._drawList.emplace_back();
	std::pair<Shader, std::vector<Mesh>>& o =  s_._drawList.back();
	//s_._drawList.first.add("./fragGBuffer.glsl", GL_FRAGMENT_SHADER);
	//add("./vertGBuffer.glsl", GL_VERTEX_SHADER);
	//link({"gPosition", "gNormal", "gAlbedoSpec"});
	// Loop over shapes
	o.second.reserve(shapes.size());
	for (tinyobj::shape_t object: shapes) {
	    genMesh(object, attrib, material_id, o.second);
	}
	++material_id;
    }

    std::cout << "generating defaultCamera\n";
    s_._cameras.emplace_back(s_._fb[0]);
    Camera& mainCamera = s_._cameras[0];

    mainCamera.lookAt(glm::vec3(.0f, 4.5f, 8.4f));
    mainCamera.setPos(glm::vec3(-9.3, 8.4f, 15.9));
    mainCamera.fieldOfview(1.823f);
    mainCamera.clipPlane(glm::vec2(0.1f, 10000.0f));
    mainCamera.upVector(glm::vec3(0.0f,1.0f,0.0f));
}

void Importer::genMesh(const tinyobj::shape_t& object_, const tinyobj::attrib_t& attrib_, int materialid_, std::vector<Mesh>& meshList_) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    std::vector<GLfloat> vertexBuffer;
    std::vector<GLuint> indiceBuffer;

    vertexBuffer.reserve(object_.mesh.num_face_vertices.size() * 8);
    indiceBuffer.reserve(object_.mesh.indices.size() * 3);
    std::map<std::vector<float>, int> uniqueVertices; //to easily compare vertices (the vector inside is always 8 values: vertex, normals, uvs) and access their indices (should become unordered but problem of hashes)
    for (size_t f = 0; f < object_.mesh.num_face_vertices.size(); f++) {
	size_t fv = object_.mesh.num_face_vertices[f];
	if (object_.mesh.material_ids[f] == materialid_) {
	    // Loop over vertices in the face.
	    for (size_t v = 0; v < fv; v++) {
		// access to vertex
		if (object_.mesh.indices.size() > index_offset + v) {
		    tinyobj::index_t idx = object_.mesh.indices[index_offset + v];
		    std::vector<float> vertex = { attrib_.vertices[3*idx.vertex_index+0], attrib_.vertices[3*idx.vertex_index+1], attrib_.vertices[3*idx.vertex_index+2],
			attrib_.normals[3*idx.normal_index+0], attrib_.normals[3*idx.normal_index+1], attrib_.normals[3*idx.normal_index+2] };
		    if (idx.texcoord_index < 0) {
			//push useless uv because we always upload 8 values and if there's no uv it does shitty things
			vertex.push_back(0.0f);
			vertex.push_back(0.0f);
		    } else {
			vertex.push_back(attrib_.texcoords[2*idx.texcoord_index+0]);
			vertex.push_back(attrib_.texcoords[2*idx.texcoord_index+1]);
		    }
		    if (uniqueVertices.count(vertex) <= 0) {
			//push a new vertex if we never encountered it before (to the unordered map AND the buffer)
			uniqueVertices[vertex] = vertexBuffer.size() / 8.0f;
			vertexBuffer.push_back(vertex[0]);
			vertexBuffer.push_back(vertex[1]);
			vertexBuffer.push_back(vertex[2]);
			vertexBuffer.push_back(vertex[3]);
			vertexBuffer.push_back(vertex[4]);
			vertexBuffer.push_back(vertex[5]);
			vertexBuffer.push_back(vertex[6]);
			vertexBuffer.push_back(vertex[7]);
		    }
		    //push indice of vertex
		    indiceBuffer.push_back(uniqueVertices[vertex]);
		}
	    }
	}
	index_offset += fv;

	// per-face material
    }
    meshList_.emplace_back();
    meshList_[meshList_.size() - 1].uploadToGPU(vertexBuffer, indiceBuffer);
    meshList_[meshList_.size() - 1]._name = object_.name;
}
