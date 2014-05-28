#include "Scene.hh"

template <typename T>
template <unsigned int RENDERTYPE, bool DEBUG>
GLnewin::Mesh<RENDERTYPE>* GLnewin::Scene<T>::genMesh(const std::string& file) {
    std::vector<GLfloat> verts;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_CalcTangentSpace|aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_SortByPType);
    if( !scene) {
	std::cout << "error: " << importer.GetErrorString() << std::endl;
    }
    aiMesh* paiMesh = scene->mMeshes[0];

    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
	const aiVector3D& pPos = paiMesh->mVertices[i];
	//const aiVector3D* pNormal = &(paiMesh->mNormals[i]) : &Zero3D;
	//const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

	verts.push_back(pPos.x);
	verts.push_back(pPos.y);
	verts.push_back(pPos.z);
    }
    if (DEBUG) { //TODO delete this code at compiling
	for (GLfloat x : verts) {
	    std::cout << "value: " << x << std::endl;
	}
    }
    Mesh<RENDERTYPE>* m = new Mesh<RENDERTYPE>(verts);
    m->bindMatrixModifier(_shader.genUniform(glm::mat4(), "model"));
    return m;
}

template <typename T>
template<unsigned int RENDERTYPE>
void GLnewin::Scene<T>::bindMatrix(Mesh<RENDERTYPE>& m) const noexcept {
    m.bindMatrixModifier(_shader.genUniform(glm::mat4(), "model"));
}

template <typename T>
GLnewin::Scene<T>::Scene() : _cam(NULL) {
    _setTrivialShader();
    _cam = new Camera(_shader);
}

template <typename T>
GLnewin::Scene<T>::Scene(const Shader& s) : _shader(s), _cam(new Camera(_shader)) {
}

template <typename T>
GLnewin::Scene<T>::~Scene() {
    delete _cam;
}

template <typename T>
void GLnewin::Scene<T>::setShader(const Shader& n) {
    _shader = n;
    _cam->reGenUniform(_shader);
}

template <typename T>
void GLnewin::Scene<T>::draw() const noexcept {
    render();
}

template <typename T>
void GLnewin::Scene<T>::draw() noexcept {
    render();
}

template <typename T>
void GLnewin::Scene<T>::render() const {
    _shader.use();
    _cam->setActive();
    for (const IRenderable* ent : _objects) {
	ent->draw();
    }
}

template <typename T>
std::vector<const T*>& GLnewin::Scene<T>::getObjectList() const noexcept {
    return _objects;
}

template <typename T>
void GLnewin::Scene<T>::pushRenderCandidate(const IRenderable* a) noexcept {
    _objects.push_back(a);
}
