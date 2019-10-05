# Data Types

The data we use and store always has some type. The computer does not really need it, but we do because we don't want to work in binary.
Data types are abstractions over binary sequences which define what kind of data will a variable using a said data type will contain,
and how will that data be represented in memory.

If you have prior experience in programming, you may know very well about this. Data types are ubiquitous across programming and are virtually essential
for writing software.
Some common data types are:
```
char (character)
int (integer)
float (floating point)
bool (boolean)
string
```

In Ethereal, the `char` data type does not exist as even a single character, in Ethereal, is a string.
Therefore, all these data types except `char` are the fundamental types in Ethereal.

However, Ethereal is a `dynamically typed` language. This means that when we create variables, we do not provide types for the variable.
Instead, these are deduced by the interpreter at run time based on the value that we provide to the variable. There are some pros and cons
to this method. The biggest pro being that writing code is quicker, while the biggest con is that the code may be difficult to understand
at a glance.

For example, if we write something like:
```python
s = 'hi';
i = 5;
```
Then, `s` is a variable which is deduced by the interpreter to be of type `string` because the expression `hi` is a string,
whereas `i` is a variable that is deduced by the interpreter to be of type `int` because `5` is an integer.

There are also complex data types in Ethereal which are created by grouping of fundamental types. We will understand that later.

For now, let's understand a bit in-depth about the fundamental data types in Ethereal.

## Fundamental Types

These data types are always available for the programmer and are easily deduced by the interpreter based on values.
Without these types, a lot of things would not be possible in Ethereal. For example, if the `int` type was absent,
you would not be able to write any code requiring calculations. Booleans are exceptions to this as you can imitate
the booleans using integers (say, integer `1` for `true`, integer `0` for `false`).

There are some important things to understand about these data types, so let's do that first.

### Integers

Integers represent all the negative and positive numbers in Ethereal - all the numbers without decimal that is.
Theoretically, it can contain any number in the range: `(-∞,∞)`.

Note that unlike many (especially compiled) languages, numbers are not limited to 32, 64, or even 128 bit.
So yes, you can have as big a number as you like, so long as it fits your system's memory.

For example,
```python
num = 13721736912389172367234538762354786253478652374587235648923749872394623864;
negative = -12378126387512836512678358761253871625365412578631263816287357125387123123768162;
```
are both valid integer variables.

### Floats

Floating point numbers are the decimal numbers - any number which is an imperfect fraction.
These are useful when precise mathematical calculations are required, as integers won't provide decimal precision.
As with integers, floating point values can also be arbitrarily long with a very high precision.
For example,
```python
flt = 13721736912389172367234538.762354786253478652374587235648923749872394623864;
negative = -123781263875128365.12678358761253871625365412578631263816287357125387123123768162;
```

Do note that to classify a number as floating point, the use of decimal point (.) to signify decimal is a **must**,
even if the number does not actually have anything after the decimal point (.), in which case,
we can simply have a `zero (0)` after that.
For example,
```python
flt = 12.0;
```

### Bools

Booleans are the `true` and `false` values in Ethereal. These are used when you want to know something in a yes or no fashion.
The `true` and `false` are special (constant) variables in Ethereal which signify their respective boolean values.
These variables cannot ever be modified and are always available globally throughout the program's life.

For example,
```rust
t = true;
f = false;
```
