# Hello World!

A simple **hello world** program in Ethereal can be written as:
```python
println('hello world');
```

Save the code in a file named, say `hello.et`, and run it using the `et` binary which we built in the [installation](./01-install.md) document.
Assuming that no `PREFIX` was set, the binary would be created in the `bin/` directory of the cloned repostory. Hence, the command would be:
```bash
./bin/et hello.et
```

This will output `hello world` on the display.

Congratulations! You have successfully written your first program in Ethereal!

# Additional Notes
The `println` function adds a new line at the end of each call, so we did not have to write a new line character (`\n`).
But if we want to explicitly write new line characters wherever required without the function doing that internally, we can use the `print` function for that.

For example, if we want to display multiple statements, using `print`:
```python
print('first line');
print('second line');
```

The output would be:
```
first linesecond line
```

To correct this, we will add a new line at the end of the first `print` call.
```python
print('first line\n');
print('second line');
```

which then gives us the correct output:
```
first line
second line
```

## Conclusion
The `print` function is different from `println` in that it will not automatically add new line character at the end. Also, the `print` function requires at least one argument, whereas the `println` function requires no argument - in which case, it will simply enter a new line. For all other intents and purposes, the `print` and `println` functions are identical.

Next, we are going to dive deeper in the language and understand the variable system of it.