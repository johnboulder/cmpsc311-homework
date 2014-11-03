rm -f simple.txt firecracker.txt raven.txt hamlet.txt penn-state-alma-mater.txt solitutde.txt 
./crud_sim -v workload-one.txt
./crud_sim -v workload-two.txt
./crud_sim -v -x simple.txt
./crud_sim -v -x firecracker.txt
./crud_sim -v -x raven.txt
./crud_sim -v -x hamlet.txt
./crud_sim -v -x penn-state-alma-mater.txt
./crud_sim -v -x solitutde.txt

echo Checking simple.txt
diff simple.txt simple.txt.orig

echo Checking firecracker.txt
diff firecracker.txt firecracker.txt.orig

echo Checking raven.txt
diff raven.txt raven.txt.orig

echo Checking hamlet.txt
diff hamlet.txt hamlet.txt.orig

echo Checking penn-state-alma-mater.txt
diff penn-state-alma-mater.txt penn-state-alma-mater.txt.orig

echo Checking solitutde.txt
diff solitutde.txt solitutde.txt.orig


