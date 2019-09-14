# Code Examples

1.  Hello world
```perl
println( 'Hello world' );
```

2.  Variables
```perl
var = 2 + 4 * 6 / 8;
println( var );
```

3.  Arrays/Lists
```perl
var = [ 1, 2, 3, 4 ];
println( var[ 1 ] );
```

4.  Maps/Dictionaries
```perl
var = {};
var[ 'str' ] = "test";
println( var[ 'str' ] );
```

5.  Functions
```perl
fn hello( to ) {
	println( 'Hello ', to );
}

hello( 'world' );
```

6.  Conditionals
```perl
if 1 == 1 {
	print( 'One' );
} else {
	print( '1 ain\'t 1' );
}
```

7.  Loops (For)
```perl
num = num( scan( "Enter factorial of: " ) );
fact = 1;

for x = num; x >= 2; --x {
	fact *= x;
}

println( "Factorial of ", num, ": ", fact );
```

8.  Loops (Foreach)
```perl
import std.vec;

a = [ 1, 2, 3 ];
for x in a.iter() {
	println( x );
}
```

9.  Structures & Objects
```perl
struct C {
	a = 10;
	b = 20;
}

fn mult_by( c, x ) { return c.a * c.b * x; }

c = C{};
print( mult_by( c, 5 ) );
```

10.  Structure Member functions
```perl
# first argument is implicitly the calling variable itself: used as 'self'
mfn< str > print() {
	println( self );
}

str = "string";
str.print();
```