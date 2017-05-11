LunaticPlatypus
=======

LunaticPlatypus is a game engine without level editor and use 3D files (dae obj abc or fbx)
It is NOT production ready at all (far from it).

The ux is:
Make your level in blender, maya, Unreal Engine or any other software that can export dae obj abc or fbx
load it using `_scene = "scene.dae"; _lunaticPlatypus->loadScene();`
The scene file will be automatically reloaded if there is any change so you can iterate just by exporting from your favorite software

-> for now the reload freeze the renderer but that won't be the case in the future


To build this engine go to the LunaticPlatypus folder and use `make extractLibs; make ; make ; make install; make project` alternatively you can also just `make full`
make install will copy Makefile.base and libPlatypus.a into the INSTALL directory so it will overwrite the destination makefile if you have one (this behavior will obviously change)
There is a few options that you can change:
IMPORTER change the importer implementation library, possible values: TINYOBJLOADER, ALEMBIC, ASSIMP (even if obj contain a few data with the current state of the engine it also have the most predictable result)
INSTALL this is the folder where your project is, by default it is ../demo/ but it can be virtually anywhere


For now there is only a few thing mandatory to make an actual game:
include PlatypusHeart.hh
make a class that inherit fom Heart::IGamelogic
implement virtual void update() in this class
and add SetGameClass(MyClass) in one C++ file
that's it

Do not write any void main() or anything it's in the lib for you

there will be Heart* _lunaticPlatypus which give you access to the rest of the engine and a EventInterface _event which is a remote to the event manager

For any help/question: @newincpp
Have fun
