# Functions

## What are Functions
In simple words, functions are groups of statements/instructions that are combined to perform some tasks, and can be used repeatedly. We can say, functions are made to perform a unique and often repetitive function.

When we create a function with its group of statements, it is called `function definition`. The group of statements is called
`function body`. And, when we use the function, it is called function call.

We may need to provide functions with some additional data, or retrieve some data from it. For that, we use function
arguments and function return values respectively.

Now, let's move on to how these functions are implemented in Ethereal.

## Functions in Ethereal
Ethereal allows functions to be written in two different modes, each with two different styles, totalling four total varities for writing functions.
They are:
1. As C++ functions - Since Ethereal interpreter is written in C++ programming language, its functions can be written in it too, allowing you to bind C++ and Ethereal, essentially, enabling the use of the plethora of C++ libraries.
2. As Ethereal functions - Ethereal allows you to write functions in the Ethereal language itself, which is what you will be doing for the most part and what this guide will elaborate on.

Both of these types have 2 styles each - as plain global functions, and as `pseudo member` functions. We will understand this in more detail but let's first grasp the concepts of simple (plain) functions.

### Our First Function!
In Ethereal, we define functions using the keyword `fn`. This marks the beginning of a function definition and we then describe the function.
Let's work on this using our dear old `Hello world` example.

Last time, we were able to ask user to enter their name, store it in a variable, and display our hello message to the user in the `Hello, <user>` format.
This time, we will create a function named `hello` to which we will pass the name of the user as an argument and that function will print
the hello message for us.

So, here's the code for that.
```python
# function definition
fn hello( name ) {
	println('Hello, ', name);
}

# function call
name = scan('Enter your name: ');
hello(name);
```
Do note that variable names can be anything you like. The names given here are **not** set in stone.
But yes, congratulations on making your first function in Ethereal! Now, to understand what we have written.

So, as noted before, function definitions start with the keyword `fn`. Then comes the name of the function (here `hello`), then the argument names for
the function within parentheses - multiple arguments separated by commas and empty parentheses for no argument, and then we have our function body
within curly braces.

To use a function, we simply call it by using the function name with parentheses and the actual arguments (our data) within the parentheses.
Again, for no arguments, empty parentheses are required.