#warning "using tinyObjLoader"

void Importer::load(std::string& file, DrawBuffer& s_) {
    std::cout << "Import using tinyObjLoader\n";
    /*
       std::vector<Mesh> Scene::_meshes;
       std::vector<Camera> Scene::_cameras;
       */
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file.c_str())) {
	std::cout << "the file is fucked: " << file << '\n';
    }
    std::cout << "loaded with warning:\n" << err << '\n';

    // Loop over shapes
    s_._meshes.reserve(shapes.size());
    for (tinyobj::shape_t object: shapes) {
	std::cout << "loading: " << object.name << '\n';
	genMesh(object, attrib, s_);
	std::cout << "finished...\n";
    }

    std::cout << "generating cameras\n";
    s_._cameras.emplace_back(s_._fb[0]);
    Camera& mainCamera = s_._cameras[0];

    mainCamera.lookAt(glm::vec3(.0f, 3.7f, 8.4f)); // Nelo.obj
    mainCamera.setPos(glm::vec3(-9.3, 4.4f, 15.9)); // Nelo.obj
    //mainCamera.lookAt(glm::vec3(59.0f, 131.0f, 582.0f)); // DemoCity.obj
    //mainCamera.setPos(glm::vec3(136.0f, 231.0f, 218.0f)); // DemoCity.obj
    mainCamera.fieldOfview(1.623f);
    mainCamera.clipPlane(glm::vec2(0.1f, 10000.0f));
    mainCamera.upVector(glm::vec3(0.0f,1.0f,0.0f));
    //for(unsigned int i = 0; i < 250; ++i) {
    //    s_._cameras.emplace_back(s_._cameras[0]);
    //}
}

void Importer::genMesh(const tinyobj::shape_t& object, const tinyobj::attrib_t& attrib, DrawBuffer& s_) {
	// Loop over faces(polygon)
	size_t index_offset = 0;
	std::vector<GLfloat> vertexBuffer;
	std::vector<GLuint> indiceBuffer;

	vertexBuffer.reserve(object.mesh.num_face_vertices.size() * 8);
	indiceBuffer.reserve(object.mesh.indices.size() * 3);
	std::map<std::vector<float>, int> uniqueVertices; //to easily compare vertices (the vector inside is always 8 values: vertex, normals, uvs) and access their indices (should become unordered but problem of hashes)
	for (size_t f = 0; f < object.mesh.num_face_vertices.size(); f++) {
	    size_t fv = object.mesh.num_face_vertices[f];

	    // Loop over vertices in the face.
	    for (size_t v = 0; v < fv; v++) {
		// access to vertex
		if (object.mesh.indices.size() > index_offset + v) {
			tinyobj::index_t idx = object.mesh.indices[index_offset + v];
			std::vector<float> vertex = { attrib.vertices[3*idx.vertex_index+0], attrib.vertices[3*idx.vertex_index+1], attrib.vertices[3*idx.vertex_index+2],
				attrib.normals[3*idx.normal_index+0], attrib.normals[3*idx.normal_index+1], attrib.normals[3*idx.normal_index+2] };
			if (idx.texcoord_index < 0) {
				//push useless uv because we always upload 8 values and if there's no uv it does shitty things
				vertex.push_back(0.0f);
				vertex.push_back(0.0f);
			} else {
				vertex.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
				vertex.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
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
	    index_offset += fv;

	    // per-face material
	    object.mesh.material_ids[f];
	}
	s_._meshes.emplace_back();
	s_._meshes[s_._meshes.size() - 1].uploadToGPU(vertexBuffer, indiceBuffer);
	s_._meshes[s_._meshes.size() - 1]._name = object.name;
}

