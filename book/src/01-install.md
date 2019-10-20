# Installation

First of all, to install Ethereal, your system must meet the prerequisites mentioned in the **README** file [here](https://github.com/Electrux/Ethereal/blob/master/README.md).
The installation steps are given in that script as well, but this chapter will describe it to a bit more extent. Feel free to skip this if you are satisfied with the **README**'s installation procedure.

For installing the language, first clone the official GitHub repository: [Electrux/Ethereal](https://github.com/Electrux/Ethereal).
```bash
git clone https://github.com/Electrux/Ethereal
```

Then, `cd` into the directory, create a `build` directory, cd into that, run `cmake ..`, and finally run `make install`.
That will build the language interpreter and you will be pretty much ready to go.
```bash
cd Ethereal && mkdir build && cd build && cmake .. && make install
```

Note that you can also specify number of CPU cores using `make -j<number of cores>`. This will greatly improve the build time
of the project. For example, `cmake .. && make -j8 install`

This will generate the Ethereal libraries and binaries which can be used to execute Ethereal code. The binary which we will use is called `et` and it should be generated in `build/bin/` directory of the repository (assuming no `PREFIX_DIR` is set).

You can also install `ccache` to speed up the build process. CMake will autodetect and use it if it finds it.

The cmake script uses multiple environment variables which can be set for customizing and optimizing the build process. They are described below.

## CMake Environment Variables
### $CXX
This variable is used for specifying the C++ compiler if you do not want to use the ones auto decided by the script which uses `g++` by default for all operating systems except Android and BSD, for which it uses `clang++`.

For example, to explicitly use `clang++` compiler on an ubuntu (linux) machine, you can use:
```bash
CXX=clang++ cmake .. && make install
```

### $PREFIX_DIR
This variable will allow you to set a `PREFIX_DIR` directory for installation of the language after the build.

**NOTE** that once the script is run with a `PREFIX_DIR`, manually moving the generated files to desired directories will not work since the Ethereal's codebase uses this `PREFIX_DIR` internally itself.

Generally, the `/usr` or `/usr/local` directories are used for setting the `PREFIX_DIR`, however that is totally up to you. Default value for this is the directory `/usr/local` is located.

The script will create these directories with respect to `PREFIX_DIR`:
*  `buildfiles/et` -> `$PREFIX_DIR/bin/`
*  `buildfiles/lib*.so` -> `$PREFIX_DIR/lib/ethereal/`
*  `include/ethereal/*` -> `$PREFIX_DIR/include/ethereal/`

An example usage is:
```bash
PREFIX_DIR=/usr/local cmake .. && make install
```

### $DEBUG
This variable, if set, will disable all compiler optimizations and will cause the interpreter to start displaying its internal execution stack values while running any program. This will be helpful for debugging the language codebase, but otherwise, do **NOT** use it.

It can be used as follows:
```bash
DEBUG=true cmake .. && make install
```