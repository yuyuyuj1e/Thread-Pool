set -x

mkdir -p `pwd`/Log
mkdir -p `pwd`/build
rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make