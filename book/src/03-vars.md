# Variables

## What are Variables
Variables are pretty much aliases that the programmers make for using memory to allocate some data.
Obviously, we do not want, for our sanity's sake, to use memory addresses directly for accessing our data.
That is really complicated, hard to remember, and absolutely a nightmare to maintain.
So, we instead assign these memory locations, containing our data, to names which we can use in our programs as needed.

Clearly, variables are a crucial part of a programming language, and hence, Ethereal has it as well.

Creating a variable is really easy. For example, if we want an integer data, say `20`, to be stored in memory (and be accessible later),
we can create a variable for that, say `a`, and use that when required. To do this, the variable can be created directly as:
```go
a = 20;
```

Since Ethereal is a dynamically typed language, we need not mention the data type, of the value that we want to store, manually.
We can then use it later on, say in `println`, to print the value of this variable. To do that, we can do the following:
```go
println(a);
```

The will produce the output:
```go
20
```

## Variable Reassignment
We can also reassign the variables to different values later on. However, we need to remember that the `data type` of that variable cannot be changed throughout its lifetime. So, for example, if we create a variable `a` with value `20`, we can change it later to, say `40`, but we cannot change it to `'hi'`.
If we do something like that, the interpreter will throw an error.

For example, if we write this program:
```go
a = 20;
a = 'hi';
```
and execute it, the interpreter will throw the following error:
```
tmp.et 2[3]: error: variable 'a' already declared at previous location, but with different data type (original: int, new: str)
a = 'hi';
  ^
tmp.et 1[3]: error: original declared here
a = 20;
  ^
```

The interpreter also tells the reason for this error: original data type was `int` but when we assigned `hi`, that would mean changing the type to `str`.
We will understand more about these types in the next chapter - [Data Types](./04-data-types.md).

## Variable Scopes
The scope of a variable basically defines its lifetime - when it is created to when it is destroyed.
Ethereal, like many other languages, uses braces to define chunks, or **blocks**, of code.
Any variable that is created, has its lifetime bound to this block. It will not be accessible outside this block.

For example:
```go
a = 20;
```
is globally scoped - it is not inside any set of braces, which means that it can be globally used anywhere and once created,
it will not be destroyed until the end of the program (it will be available exactly from the location where it is created).

On the other hand, if we declare the same variable as:
```go
{
	a = 20;
}
```
it will be accessible inside the set of braces (albeit after the creation - declaration of the variable), but not outside them.

So, if we use the variable outside this set of braces (scope), say as follows:
```go
{
	a = 20;
}
println(a);
```
we will be greeted with the following error:
```
tmp.et 4[9]: error: variable 'a' does not exist
println(a);
        ^
```

Of course, if we create another variable named `a` with some data outside the scope, that would work just fine. So something like this works:
```go
{
	a = 20;
}
a = 'hi';
println(a);
```
which will produce the following output:
```
hi
```

Note that we can choose a different data type even when the variable name is same.
The reason being that even though having same names, both of the declared variables are actually totally different.
The first one, having data `20`, no longer exists after the scope, therefore, the second one, having data `hi`,
is a new variable declaration rather than reassignment.

## Variable Naming
One last important thing about variables is their naming. Ethereal defines specific rules based on which you can name variables,
quite similar to most other languages. These rules are that variable names:
1. Must begin with an alphabet (irrelevant of the case) or underscore
2. Can contain numbers anywhere except the first character
3. Cannot contain any symbol other than alphabets, numbers, and underscores.

Well, that is basically how variables, their reassignment, and their scopes, work. Not much to learn or understand and pretty easy - which is the goal!

Next up, we'll understand the concept of `data types` and see some of the fundamental data types in Ethereal.