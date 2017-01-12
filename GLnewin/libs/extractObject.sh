#libAlembic.a  libassimp.a  libGLEW.a  libHalf.a  libhdf5.a  libz.a
FILES=`ls *.a`
rm -R Objectsoup
for f in $FILES
do
     echo "Processing $f"
     if [ "$f" != "libAlembic.a" ]; then
	     ar -x $f
     fi
      # do something on $f
done
mkdir -p Objectsoup
mkdir -p Objectsoup/Alembic
mv *.o Objectsoup  
ar -x libAlembic.a
mv *.o Objectsoup/Alembic
