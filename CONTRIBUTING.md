# Introduction

First things first, I would like to thank you for considering to contribute to this project.
It is thanks to people like you that this project lives on and continues to improve.

This document defines some guidelines for contributing to the project.
These guidelines help you ease out in the process of contributing to this project,
and helps the existing contributors and maintainers to quickly start working with you!

Therefore, to contribute to this project, please follow the following guidelines.

# Code Formatting

## C++
The C++ codebase follows the **[Linux Kernel's](https://www.kernel.org/doc/html/v4.10/process/coding-style.html)** code style with some minor changes:
1. Having a conditional/loop statement with single line block does not require surrounding it with braces and must be on the same line as the conditional/loop statement itself - **as long as neither of those are too long** (about 120 characters).
2. All the subscripts/indices, arguments for functions, and conditions for conditionals/loops must have spaces between the parentheses/brackets/braces and the arguments.
For example,
```cpp
int func( int a, int b )
{

}

func( a, b );

if( a == b ) {}

while( a ) {}

p[ q ];
{ a, b };
```
3. Classes have similar pattern as functions, and the access modifier keywords are indented at the level of the `class` keyword inside which they reside.
For example,
```cpp
class A
{
	int a;
public:
	int get_a() const;
}
```
4. Same applies for switch cases and goto labels - the cases and labels are at the indentation level of the switch statement and parent block of the goto label respectively. For example,
```cpp
int func()
{
	goto label;
label:
	some_code();
}

switch( x ) {
case 'a':
	func_a();
case 'b':
	func_b();
}
```

## Ethereal
Ethereal's code style is same as **[Linux Kernel's](https://www.kernel.org/doc/html/v4.10/process/coding-style.html)** and this project's C++ style,
with the following changes.
1. All opening braces are on the same line as the entity for which they are used, ie., opening braces are never on new lines - unlike the C++ code.
For example,
```go
func( a ) {
	println( x );
}
```

# Issues

## Creation
As of right now, a particular standard format has not been devised for issues, however, please be as thorough as possible in describing an issue -
possibly with examples, links, images, or anything relevant. That would help a lot in resolving the problem as soon as possible.

## Solving
Before working on any issues, please post a reply on the issue stating that you are working on it (and possibly assign yourself to it).
This will save everyone time by saving them from working on something that is already undertaken by someone else.

Do not start working on any issue that someone is already working on. If the issue seems old and inactive,
you can comment on the issue stating your interest in it. In such case, you will most likely be allowed to takeover that issue -
assuming there are no special circumstances.