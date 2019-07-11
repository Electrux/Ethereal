# Code Examples

1. Hello world

```
println( "Hello world" );
```

2. Variables

```
var = 2 + 4 * 6 / 8;
println( var );
```

3. Arrays/Lists

```
var = L( 1, 2, 3, 4 );
println( var[ 1 ] );
```

4. Maps/Dictionaries

```
import "../src/VM/Modules/map" as map;
var = map.create();
var[ "str" ] = "test";
println( var[ "str" ] );
```

5. Functions

```
fn hello( to ) {
	print( "Hello ", to );
}

hello( "world" );
```

6. Conditionals

```
if 1 == 1 {
	print( "One" );
} else {
	print( "1 ain't 1" );
}
```

7. Loops

```
num = num( scan( "Enter factorial of: " ) );
fact = 1;

for x = num; x >= 2; --x {
	fact *= x;
}

println( "Factorial of ", num, ": ", fact );
```

8. Structures & Objects

```
struct C {
	a = 10;
	b = 20;
}

fn mult_by( c, x ) { return c.a * c.b * x; }

c = @C();
print( mult_by( c, 5 ) );
```

9. References

```
fn inc( & x ) {
	++x;
}
```

10. Structure Member functions

```
# first argument is implicitly the calling variable itself: as 'self'
mfn< str > len() {
	return len( self );
}

str = "string";
println( "str len: ", str.len() );
```