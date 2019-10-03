# Installation

First of all, to install Ethereal, your system must meet the prerequisites mentioned in the **README** file [here](https://github.com/Electrux/Ethereal/blob/master/README.md).
The installation steps are given in that script as well, but this chapter will describe it to a bit more extent. Feel free to skip this if you are satisfied with the **README**'s installation procedure.

For installing the language, first clone the official GitHub repository: [Electrux/Ethereal](https://github.com/Electrux/Ethereal).
```bash
git clone https://github.com/Electrux/Ethereal
```

Then, `cd` into the directory and execute the `bootstrap.sh` shell script. That will build the language interpreter and you will be pretty much ready to go.
```bash
cd Ethereal && ./bootstrap.sh
```

The `bootstrap.sh` script uses multiple environment variables which can be set for customizing and optimizing the build process. They are described below.

## Bootstrap Environment Variables
### $CXX
This variable is used for specifying the C++ compiler if you do not want to use the ones auto decided by the script which uses `g++` by default for all operating systems except Android and BSD, for which it uses `clang++`.

For example, to explicitly use `clang++` compiler on an ubuntu (linux) machine, you can use:
```bash
CXX=clang++ ./bootstrap.sh
```

### $PREFIX
This variable will allow you to set a `PREFIX` directory for installation of the language after the build.

**NOTE** that once the script is run with a `PREFIX`, manually moving the generated files to desired directories will not work since the Ethereal's codebase uses this `PREFIX` internally itself.

Generally, the `/usr` or `/usr/local` directories are used for setting the `PREFIX`, however that is totally up to you. Default value for this is the directory where `bootstrap.sh` is located.

The script will create these directories with respect to `PREFIX`:
*  `buildfiles/et` -> `$PREFIX/bin/`
*  `buildfiles/lib*.so` -> `$PREFIX/lib/ethereal/`
*  `include/ethereal/*` -> `$PREFIX/include/ethereal/`

An example usage is:
```bash
PREFIX=/usr/local ./bootstrap.sh
```

### $USE_CCACHE
This variable causes the compilation commands to be prepended with `ccache`. If you have `ccache` installed on your system, this option is highly recommended since it will reduce the build times of the project after the first build.

Usage example for this variable is:
```bash
USE_CCACHE=true ./bootstrap.sh
```

Of course, multiple variables can be used in a single execution of `bootstrap.sh` script in bash.

For example:
```bash
USE_CCACHE=true PREFIX=/usr/local ./bootstrap.sh
```

### $DEBUG
This variable, if set, will disable all compiler optimizations and will cause the interpreter to start displaying its internal execution stack values while running any program. This will be helpful for debugging the language codebase, but otherwise, do **NOT** use it.

It can be used as follows:
```bash
DEBUG=true ./bootstrap.sh
```