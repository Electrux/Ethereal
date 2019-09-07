#!/usr/bin/env bash

compiler="clang++"
os=$(uname)

if [[ "$os" == 'Linux' && "$(uname -o 2>/dev/null)" != "Android" ]]; then
	compiler="g++"
fi

if ! [[ -z "${CXX}" ]]; then
	compiler="${CXX}"
fi

if ! [[ -z "${USE_CCACHE}" ]]; then
	compiler="ccache $compiler"
fi

if [[ -z "${DEBUG}" ]]; then
	opti="-O2"
fi

compiler_version=$($compiler --version)
echo "Using compiler: $compiler, version: $compiler_version"

rm -rf bin lib buildfiles 2>/dev/null

echo "Creating directories ..."

mkdir -p "buildfiles/src/FE/Parser"
mkdir -p "buildfiles/src/VM/Vars"
mkdir -p "buildfiles/std"
mkdir -p "buildfiles/eth"

if [[ -z "${PREFIX}" ]]; then
	PREFIX_DIR=$(pwd)
else
	PREFIX_DIR=${PREFIX}
fi

echo "Using PREFIX = ${PREFIX_DIR}"

VERSION_STRING="-DVERSION_MAIN=0 -DVERSION_SUB=0 -DVERSION_PATCH=2"

EXTRA_INCLUDES=""
EXTRA_FLAGS=""

if [[ "$os" =~ .*BSD.* ]]; then
	EXTRA_INCLUDES="-I/usr/local/include -L/usr/local/lib -Wno-unused-command-line-argument -D_WITH_GETLINE"
else
	EXTRA_FLAGS="-ldl"
fi

# Library: et
if [[ "$os" != "Darwin" ]]; then
	shopt -s lastpipe
fi
find src -name "*.cpp" | grep -v "Main.cpp" | while read -r src_file; do
	if [[ -z "$COMPILE_COMMAND" ]]; then
		echo "Compiling: $src_file ..."
	else
		echo "$compiler ${opti} -fPIC -std=c++11 -c $src_file -o buildfiles/$src_file.o ${EXTRA_INCLUDES} -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}"
	fi
	$compiler ${opti} -fPIC -std=c++11 -c $src_file -o buildfiles/$src_file.o ${EXTRA_INCLUDES} -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}
	if [[ $? != 0 ]]; then
		SRC_FAILED="true"
		break
	fi
done
if [[ "$os" != "Darwin" ]]; then
	shopt -u lastpipe
fi

if [[ "$SRC_FAILED" == "true" ]]; then
	echo "Error in compiling sources, will not continue"
	exit 1
fi

buildfiles=$(find buildfiles -name "*.cpp.o" | paste -sd " " -)

install_name=""
if [[ "$os" == "Darwin" ]]; then
	install_name="-Wl,-install_name -Wl,@rpath/libet.so"
fi

echo
if [[ -z "$COMPILE_COMMAND" ]]; then
	echo "Building library: et ..."
else
	echo "$compiler ${opti} -fPIC -std=c++11 -shared -o buildfiles/libet.so src/VM/Main.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
	$install_name -L./buildfiles/ ${EXTRA_INCLUDES} ${EXTRA_FLAGS} -lgmpxx -lgmp -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}"
fi
$compiler ${opti} -fPIC -std=c++11 -shared -o buildfiles/libet.so src/VM/Main.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
	$install_name -L./buildfiles/ ${EXTRA_INCLUDES} ${EXTRA_FLAGS} -lgmpxx -lgmp -DAS_LIB -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}
if [[ $? != 0 ]]; then
	exit $?
fi

if [[ -z "$COMPILE_COMMAND" ]]; then
	echo "Building binary:  et ..."
else
	echo "$compiler ${opti} -fPIC -std=c++11 -g -o buildfiles/et src/FE/Main.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
	-L./buildfiles/ ${EXTRA_INCLUDES} ${EXTRA_FLAGS} -lgmpxx -lgmp -let -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}"
fi
$compiler ${opti} -fPIC -std=c++11 -g -o buildfiles/et src/FE/Main.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
	-L./buildfiles/ ${EXTRA_INCLUDES} ${EXTRA_FLAGS} -lgmpxx -lgmp -let -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}
if [[ $? != 0 ]]; then
	exit $?
fi

# Standard Libraries
echo
echo 'Building Standard Libraries ...'
for l in "core" "fs" "map" "math" "opt" "os" "set" "str" "term" "time" "tuple" "vec"; do
	install_name=""
	if [[ "$os" == "Darwin" ]]; then
		install_name="-Wl,-install_name -Wl,@rpath/std/lib$l.so"
	fi

	if [[ -z "$COMPILE_COMMAND" ]]; then
		echo "-> std.$l ..."
	else
		echo "$compiler ${opti} -fPIC -std=c++11 -shared -o buildfiles/std/lib$l.so modules/std/$l.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
		$install_name -L./buildfiles/ ${EXTRA_INCLUDES} ${EXTRA_FLAGS} -lgmpxx -lgmp -let -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}"
	fi
	$compiler ${opti} -fPIC -std=c++11 -shared -o buildfiles/std/lib$l.so modules/std/$l.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
		$install_name -L./buildfiles/ ${EXTRA_INCLUDES} ${EXTRA_FLAGS} -lgmpxx -lgmp -let -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}
	if [[ $? != 0 ]]; then
		exit 1
	fi
done

# Ethereal VM Libraries
echo
echo 'Building Ethereal VM Libraries ...'
for l in "vm"; do
	install_name=""
	if [[ "$os" == "Darwin" ]]; then
		install_name="-Wl,-install_name -Wl,@rpath/eth/lib$l.so"
	fi

	if [[ -z "$COMPILE_COMMAND" ]]; then
		echo "-> eth.$l ..."
	else
		echo "$compiler ${opti} -fPIC -std=c++11 -shared -o buildfiles/eth/lib$l.so modules/eth/$l.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
		$install_name -L./buildfiles/ ${EXTRA_INCLUDES} ${EXTRA_FLAGS} -lgmpxx -lgmp -let -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}"
	fi
	$compiler ${opti} -fPIC -std=c++11 -shared -o buildfiles/eth/lib$l.so modules/eth/$l.cpp $buildfiles -Wl,-rpath,${PREFIX_DIR}/lib/ethereal \
		$install_name -L./buildfiles/ ${EXTRA_INCLUDES} ${EXTRA_FLAGS} -lgmpxx -lgmp -let -DBUILD_PREFIX_DIR=${PREFIX_DIR} ${VERSION_STRING}
	if [[ $? != 0 ]]; then
		exit 1
	fi
done

# Install this
echo

mkdir -p "$PREFIX_DIR/include/ethereal/std"

if [[ $? != 0 ]]; then
	echo "You might wanna run this as root for installation to dir: ${PREFIX_DIR}"
	exit $?
fi

mkdir -p "$PREFIX_DIR/include/ethereal/eth"
mkdir -p "$PREFIX_DIR/lib/ethereal/std"
mkdir -p "$PREFIX_DIR/lib/ethereal/eth"
mkdir -p "$PREFIX_DIR/bin/"

cp_cmd="cp -r "

if [[ "$os" == 'Linux' ]]; then
	cp_cmd="cp -r --remove-destination "
fi

echo "Installing files ..."
$cp_cmd buildfiles/et "$PREFIX_DIR/bin/"
$cp_cmd buildfiles/libet.so "$PREFIX_DIR/lib/ethereal/"
$cp_cmd buildfiles/std/lib*.so "$PREFIX_DIR/lib/ethereal/std/"
$cp_cmd buildfiles/eth/lib*.so "$PREFIX_DIR/lib/ethereal/eth/"
if [[ "$(pwd)" != "$PREFIX_DIR" ]]; then
	$cp_cmd include/ethereal/* "$PREFIX_DIR/include/ethereal/"
fi
