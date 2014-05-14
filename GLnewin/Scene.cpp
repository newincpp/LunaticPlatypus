#include "Scene.hh"

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
