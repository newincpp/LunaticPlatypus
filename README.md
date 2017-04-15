LunaticPlatypus
=======

LunaticPlatypus is a game engine designed with the philosophy "If less is more then nothing is everything".

To build this engine go to the GLnewin folder and use `INSTALL="/path/to/my/project/" make extractLibs && make && make install && make project` alternatively you can also just `INSTALL="/path/to/my/project/" make full`.
make install will copy Makefile.base and libPlatypus.a into the INSTALL directory so it will overwrite the destination makefile if you have one (this behavior will obviously change).
There is a few options that you can change:
IMPORTER change the importer implementation library, possible values: TINYOBJLOADER, ALEMBIC, ASSIMP (even if obj contain a few data with the current state of the engine it also have the most predictable result)
INSTALL this is the folder where your project is, by default it is ../demo/ but it can be virtually anywhere.

-> The building process was deliberatly made as simple as possible to make anyone able to modify it without any problems
-> I provide static libraries of the dependancies in the libs/ folder but libAlembic.a may not work for you



The ux is:
Make your level in blender, maya etc... export it as Alembic file (.abc).
The most basic code is:
```
#include <PlatypusHeart.hh>
class MyGame : public Heart::IGamelogic {
public:
  MyGame() {
    _scene = "./sponza.abc"; // my default level
  }
  virtual void update() {}
};
SetGameClass(MyGame)
```
You will also need to write a `compositor.glsl` and a `compositorVert.glsl`
This is the shader executed at the latest stage of the deferred pipeline.

There is 4 type that LunaticPlatypus will not get from the Alembic file:
Skeleton Animations: (not implemented yet in LunaticPlatypus)
Textures: (not implemented yet in LunaticPlatypus)
Materials: LunaticPlatypus organise material in folder looking like this:
  name.material/
     fragment.glsl #opengl fragment shader
     vertex.glsl #opengl vertex shader
     bin.spirv #vulkan spirv bytecode (when supported)
     # .metal (when supported)
     # .pssl (when supported)
(I don't plan to support directx)
Scripts: add an empty object called "myscript_GameClass" in the scene file and LunaticPlatypus will automatically load "myscript.gameClass/myscript.so"
  This dynamic library (for now) need:
    void init();
    void(*getTickFun())(float);
    unsigned int getRemainingTickFunSize();
    void destroy();
  With "external "C"" or any equivalent of your favorite native language (C, C++, D, Go, Ocaml, Rust, nativised C#, asm...). (bytecode and script language such as lua or ruby are planed but in a really really far future).



For any help/question: @newincpp
Have fun
