# Code Examples

1.  Hello world
```python
println( 'Hello world' );
```

2.  Variables
```python
var = 2 + 4 * 6 / 8;
println( var );
```

3.  Arrays/Lists
```python
var = [ 1, 2, 3, 4 ];
println( var[ 1 ] );
```

4.  Maps/Dictionaries
```python
var = { 'str', 'test' };
var[ 'str' ] = 'test2';
println( var[ 'str' ] );
```

5.  Functions
```python
fn hello( to ) {
	println( 'Hello ', to );
}

hello( 'world' );
```

6.  Conditionals
```python
if 1 == 1 {
	print( 'One' );
} else {
	print( '1 ain\'t 1' );
}
```

7.  Loops (For)
```python
num = num( scan( 'Enter factorial of: ' ) );
fact = 1;

for x = num; x >= 2; --x {
	fact *= x;
}

println( 'Factorial of ', num, ': ', fact );
```

8.  Loops (Foreach)
```python
import std.vec;

a = [ 1, 2, 3 ];
for x in a.iter() {
	println( x );
}
```

9.  Structures & Objects
```python
struct C {
	a = 10;
	b = 20;
}

fn mult_by( c, x ) { return c.a * c.b * x; }

c = C{};
print( mult_by( c, 5 ) );
```

10.  Structure Member functions
```python
# first argument is implicitly the calling variable itself: used as 'self'
mfn< str > print() {
	println( self );
}

str = 'string';
str.print();
```
