#!/usr/bin/env bash

compiler="clang++"
os=$(uname)

if [[ "$os" == 'Linux' ]]; then
	compiler="g++"
fi

if ! [[ -z "${COMPILER}" ]]; then
	compiler="${COMPILER}"
fi

compiler_version=$($compiler --version)
echo "Using compiler: $compiler, version: $compiler_version"

rm -rf bin lib buildfiles 2>/dev/null

echo "Creating directories ..."

mkdir -p "buildfiles/src/FE/Parser"
mkdir -p "buildfiles/src/VM/Vars"

if [[ -z "${PREFIX}" ]]; then
	PREFIX_DIR=$(pwd)
else
	PREFIX_DIR=${PREFIX}
fi

VERSION_STRING="-DVERSION_MAIN=0 -DVERSION_SUB=0 -DVERSION_PATCH=1"

EXTRA_FLAGS=""

if [[ "$os" =~ .*BSD.* ]]; then
	EXTRA_FLAGS="-I/usr/local/include -L/usr/local/lib -Wno-unused-command-line-argument"
fi

# Library: et

find src -name "*.cpp" | grep -v "Main.cpp" | while read -r src_file; do
	echo "Compiling: $src_file ..."
	$compiler -O2 -fPIC -std=c++11 -c $src_file -o buildfiles/$src_file.o ${EXTRA_FLAGS} -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}
	if [[ $? != 0 ]]; then
		break
	fi
done

if [[ $? != 0 ]]; then
	echo "Error in compiling sources, will not continue"
	exit $?
fi

buildfiles=$(find buildfiles -name "*.cpp.o" | paste -sd " " -)

install_name=""
if [[ "$os" == "Darwin" ]]; then
	install_name="-Wl,-install_name -Wl,@rpath/libcore.so"
fi
echo "Building library: et ..."
$compiler -O2 -fPIC -std=c++11 -shared -o buildfiles/libet.so src/VM/Main.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
	$install_name ${EXTRA_FLAGS} -L./buildfiles/ -ldl -lgmpxx -lgmp -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}
if [[ $? != 0 ]]; then
	exit $?
fi

echo "Building binary: et ..."
$compiler -O2 -fPIC -std=c++11 -g -o buildfiles/et src/FE/Main.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
	${EXTRA_FLAGS} -L./buildfiles/ -ldl -lgmpxx -lgmp -let -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}
if [[ $? != 0 ]]; then
	exit $?
fi

# Libraries
for l in "core" "fs" "math" "os" "set" "str" "vec" "map"; do
	echo "Building library: $l ..."
	install_name=""
	if [[ "$os" == "Darwin" ]]; then
		install_name="-Wl,-install_name -Wl,@rpath/lib$l.so"
	fi
	$compiler -O2 -fPIC -std=c++11 -shared -o buildfiles/lib$l.so stdlib/$l.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
		$install_name ${EXTRA_FLAGS} -L./buildfiles/ -ldl -lgmpxx -lgmp -let -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}
	if [[ $? != 0 ]]; then
		exit $?
	fi
done

# Install this

mkdir -p "$PREFIX_DIR/include/ethereal"

if [[ $? != 0 ]]; then
	echo "You might wanna run this as root for installation to dir: ${PREFIX_DIR}"
	exit $?
fi

mkdir -p "$PREFIX_DIR/lib/ethereal"

mkdir -p "$PREFIX_DIR/bin/"

cp_cmd="cp -r "

if [[ "$os" == 'Linux' ]]; then
	cp_cmd="cp -r --remove-destination "
fi

echo "Installing files ..."
$cp_cmd buildfiles/et "$PREFIX_DIR/bin/"
$cp_cmd buildfiles/lib*.so "$PREFIX_DIR/lib/ethereal/"
if [[ "$(pwd)" != "$PREFIX_DIR" ]]; then
	$cp_cmd include/ethereal/* "$PREFIX_DIR/include/ethereal/"
fi
