set -x

cd `pwd`/build && 
	make

cd ..

cd `pwd`/bin &&
	./normal_test