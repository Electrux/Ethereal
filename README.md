# Ethereal

A simple, dynamically typed, interpreted, generic programming language.

# Prerequisites

* GCC/Clang with full C++ 11 support
* GMP library with CXX support (will be almost always built with support for CXX in your official distribution package)

# About

The language syntax is inspired from Python and C. It contains sufficient features to enjoy working with it, but avoids complex features like OOP.

Ethereal does use a concept of member functions which are actually nothing but plain functions bound to a particular struct/type. And they allow the use of `self` to use the calling variable inside the function itself.

One can easily make extensions for the language in the form of language modules or c++ dynamic libraries (if high performance is needed). See the existing modules for more information about it (language modules: `include/ethereal/`, C++ modules: `src/VM/Modules/`).

Documentation is under development and will take some time to build, but till then, feel free to go through the [Code Samples](https://github.com/Electrux/Ethereal/blob/master/code_samples.md) and hack around with the programs in the `tests` directory.

# Installation

Once the prerequisites are met, just execute the `bootstrap.sh` script. It will build the language interpreter along with the stdlib modules, and install them to your `PREFIX` directory if it is set, or to the repository directory itself.

Note that if you use PREFIX, you may need root access depending on the directory you chose.

The following items will be installed:

* buildfiles/et -> PREFIX_DIR/bin/
* buildfiles/lib*.so PREFIX_DIR/lib/ethereal/
* include/ethereal/* PREFIX_DIR/include/ethereal/

Also, the interpreter code internally uses `PREFIX` to locate the `lib` and `include` directories, so you will have to rebuild the codebase if you change `PREFIX` and then install.

# Roadmap

* Add terminal related functions (like term width, height) in libterm
* Write primary language and standard library documentation
* Perhaps use `union` type for operand value of `instr_code_t` to avoid using conversions like `std::stoi` in ExecInternal

Contributions are definitely accepted and greatly appreciated. ❤️