#libAlembic.a  libassimp.a  libGLEW.a  libHalf.a  libhdf5.a  libz.a
FILES=`ls *.a`
rm -Rf Objectsoup
mkdir -p Objectsoup
for f in $FILES
do
    echo "Processing $f"
    mkdir -p Objectsoup/${f%.*}
    cp $f Objectsoup/${f%.*}
    cd Objectsoup/${f%.*}
    ar -x $f
    cd -
    # do something on $f
done
