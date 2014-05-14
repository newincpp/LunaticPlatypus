#ifndef RENDERTARGER_H_
# define RENDERTARGER_H_

namespace GLnewin {
    class Shader;
    class IRenderable {
	public:
	    virtual void draw() const noexcept = 0;
    };

    class IGLContext {
	public:
	   struct Size {
	       int x;
	       int y;
	   };
	    virtual void initalize(unsigned long, unsigned long) = 0;
	    virtual Size getSize() const noexcept = 0;
	    virtual void flush() const noexcept = 0;
	    virtual ~IGLContext() {}
    };
}

#endif /* !RENDERTARGER_H_ */
