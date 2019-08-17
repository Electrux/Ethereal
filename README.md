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

# Roadmap

* Write primary language documentation

Contributions are definitely accepted and greatly appreciated. ❤️