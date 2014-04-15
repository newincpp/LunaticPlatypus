#include "Scene.hh"

GLnewin::Scene::Scene() : _cam(NULL) {
    _setTrivialShader();
    _cam = new Camera(_shader);
}

GLnewin::Scene::Scene(const Shader& s) : _shader(s), _cam(new Camera(_shader)) {
}

GLnewin::Scene::~Scene() {
    delete _cam;
}

void GLnewin::Scene::setShader(const Shader& n) {
    _shader = n;
    _cam->reGenUniform(_shader);
}

void GLnewin::Scene::draw() const noexcept {
    _internalRender();
}

void GLnewin::Scene::draw() noexcept {
    render();
}

void GLnewin::Scene::_internalRender() const {
    _shader.use();
    _cam->setActive();
    for (const IRenderable* ent : _objects) {
	ent->draw();
    }
}

void GLnewin::Scene::render() {
    std::function<void(void)> _handler = std::bind(&Scene::_internalRender, this);
    std::cout << "bind and function successfully constructed" << std::endl;
    std::future<void>&& f = std::async(std::launch::async, _handler);
    std::cout << "async ... same" << std::endl;
    _worker.push(std::move(f));
    std::cout << "achievement get" << std::endl;
}

void GLnewin::Scene::pushRenderCandidate(const IRenderable* a) noexcept {
    _objects.push_back(a);
}
