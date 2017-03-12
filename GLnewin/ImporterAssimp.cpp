#warning "using assimp"
inline glm::mat4 Importer::aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;

    to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

    return to;
}

void Importer::load(std::string& file, DrawBuffer& s_) {
    std::cout << "Import using Assimp\n";

    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(file,
	    aiProcess_CalcTangentSpace       |
	    aiProcess_Triangulate            |
	    aiProcess_JoinIdenticalVertices  |
	    aiProcess_SortByPType);

    // If the import failed, report it
    s_._cameras.emplace_back(s_._fb[0]);
    if( !scene) {
	std::cout << "fuu: " << importer.GetErrorString() << '\n';
	return;
    }
    genMesh(scene, s_);
    //scene->mCameras[0]->GetCameraMatrix(m);
    Camera& mainCamera = s_._cameras[0];
    if (scene->mNumCameras) {
	aiCamera* c = scene->mCameras[0];
	std::cout << "position: " << c->mPosition[0] << ' ' << c->mPosition[1] << ' ' << c->mPosition[2] << '\n';

	mainCamera.lookAt(glm::vec3(c->mLookAt[2] * 1000.0f, c->mLookAt[1] * 1000.0f, c->mLookAt[0] * 1000.0f));
	mainCamera.setPos(glm::vec3(c->mPosition[0] / 1.0f, c->mPosition[1] / 1.0f, c->mPosition[2] / 1.0f));
	//_mainCamera.fieldOfview(c->mHorizontalFOV);
	mainCamera.fieldOfview(1.571f);
	mainCamera.clipPlane(glm::vec2(c->mClipPlaneNear, c->mClipPlaneFar));
	mainCamera.upVector(glm::vec3(c->mUp[0], c->mUp[1], c->mUp[2]));
    } else {
	std::cout << "no Camera Detected\n";
	mainCamera.lookAt(glm::vec3(.0f, 3.7f, 8.4f)); // Nelo.obj
	//mainCamera.setPos(glm::vec3(-9.3, 4.4f, 15.9)); // Nelo.obj
	mainCamera.setPos(glm::vec3(-15.3, 4.4f, 0.0)); // NeloNOARM.obj

	//mainCamera.lookAt(glm::vec3(59.0f, 131.0f, 582.0f)); // DemoCity.obj
	//mainCamera.setPos(glm::vec3(136.0f, 231.0f, 218.0f)); //DemoCity.obj
	mainCamera.fieldOfview(1.623f);
	mainCamera.clipPlane(glm::vec2(0.1f, 10000.0f));
	mainCamera.upVector(glm::vec3(0.0f,1.0f,0.0f));
	s_._cameras.emplace_back(s_._cameras[0]);
	for(unsigned int i = 0; i < 250; ++i) {
	    s_._cameras.emplace_back(s_._cameras[0]);
	}
    }
}

void Importer::genMesh(const aiScene* scene_, DrawBuffer& s_) {
    s_._meshes.reserve(scene_->mNumMeshes);
    for(unsigned int m = 0 ; m < scene_->mNumMeshes ; ++m) {
	std::vector<GLfloat> vertexBuffer;
	std::vector<GLuint> indiceBuffer;
	aiMesh* mesh = scene_->mMeshes[m];
	vertexBuffer.reserve(mesh->mNumVertices * 8);
	indiceBuffer.reserve(mesh->mNumFaces * 3);


	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
	    vertexBuffer.push_back(mesh->mVertices[i].x);
	    vertexBuffer.push_back(mesh->mVertices[i].y);
	    vertexBuffer.push_back(mesh->mVertices[i].z);
	    vertexBuffer.push_back(mesh->mNormals[i].x);
	    vertexBuffer.push_back(mesh->mNormals[i].y);
	    vertexBuffer.push_back(mesh->mNormals[i].z);


	    if (mesh->HasTextureCoords(0)) {
		vertexBuffer.push_back(mesh->mTextureCoords[0][i].x);
		vertexBuffer.push_back(mesh->mTextureCoords[0][i].y);
	    } else {
		vertexBuffer.push_back(0.0f);
		vertexBuffer.push_back(0.0f);
	    }

	    //vertexBuffer.push_back(mesh->mBitangents[i].x);
	    //vertexBuffer.push_back(mesh->mBitangents[i].y);
	    //vertexBuffer.push_back(mesh->mBitangents[i].z);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[0]);
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[1]);
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[2]);
	}

	s_._meshes.emplace_back();
	s_._meshes[s_._meshes.size() - 1].uploadToGPU(vertexBuffer, indiceBuffer);
	//s_._meshes[s_._meshes.size() - 1]._name = mesh->mName.C_Str();

	//_meshBuffer.emplace();
	//_meshBuffer.top().uploadToGPU(vertexBuffer, indiceBuffer);
	//_meshBuffer.top()._name = mesh->mName.C_Str();

	aiNode* n = scene_->mRootNode->FindNode(mesh->mName);
	if (n) {
	s_._meshes[s_._meshes.size() - 1].uMeshTransform = aiMatrix4x4ToGlm(n->mTransformation);
	}
    }
}
