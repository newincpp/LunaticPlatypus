#ifndef RENDERTARGER_H_
# define RENDERTARGER_H_
#include "IShader.hh"

namespace GLnewin {
    class IRenderable {
	public:
	    virtual void draw() noexcept = 0;
	    virtual void setShader(IShader*) noexcept = 0;
    };

    class IRendertarget {
	public:
	    virtual void initalize(unsigned long, unsigned long) = 0;
	    virtual void getSize() const noexcept = 0;
	    virtual void render() noexcept = 0;
	    virtual void pushRenderCandidate(IRenderable*) noexcept = 0;
	    virtual ~IRendertarget() {}
    };
}

#endif /* !RENDERTARGER_H_ */
