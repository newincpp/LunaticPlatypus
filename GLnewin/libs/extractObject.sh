#libAlembic.a  libassimp.a  libGLEW.a  libHalf.a  libhdf5.a  libz.a
FILES=`ls *.a`
rm -R Objectsoup
for f in $FILES
do
     echo "Processing $f"
     ar -x $f
      # do something on $f
done
mkdir -p Objectsoup
mv *.o Objectsoup  
