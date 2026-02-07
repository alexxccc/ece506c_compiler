
# Scanner Part 1 - Lexical Specification

## Input language: 
Specify the input language of your compiler here.

Arduino Sketch
## Token Categories/Types:

List ALL the tokens that your language will use. For example:

### C Based Keywords: 
These are reserved words in your language 
	#include
	void
	return
	int
	bool
	if
	else
	while
	true
	false
	break

### Arduino Sketch Keywords:
	Serial
	print
	println
	setup
	loop
	delay
	pinMode
	analogRead
	analogWrite
	digitalRead
	digitalWrite

### Identifiers: 
Names of variables, functions, and classes. Identifiers are arbitrary sequences of letters, digits, and the underscore character (_). They cannot contain spaces and must not begin with a digit.

##### Test Cases:
	usecasezero
	useCase1
	use_case_two
	UseCase_3

### Numbers: 
Scope limited to integers for now. Possible TODO: floating point, alternative bases (binary/hex)

##### Test Cases:
	0
	1
	100
	555
	20000000000000

These may be formatted as integers, floating-point values, fractions, or numbers in alternate bases such as binary, octal, or hexadecimal.

**Note:** 
Read Section 3.1 for more information on tokens. Do not forget to include mathematical and logical operators, special symbols, punctuation, comments, and whitespace.

### Arithmetic Operators:
	=
	+
	-
	*
	/
	%

### Boolean Operators:
	&& 
	! 
	|| 

### Comparison Operators:
	==
	\>
	\>=
	< 
	<= 
	!= 

## Comment: Separate from misc. characters because all code after a comment should be ignored.
	//
	
## Whitespace: Spaces in between different tokens. Mostly ignored but still useful to categorize.
	' '

## Text Formatting:
	"
	\n
	\t
	TODO : %d

## Code Formatting:
	;
	,
	.
	(
	)
	{
	}

## Formal Specification:

For each token category, write a regular expression (using the basic regular expressions learned in class) that precisely defines how the token is formed from the input character stream.

For example:


### Keywords:

```
(#include)|(void)|(return)|(int)|(bool)|(if)|(else)|(while)|(true)|(false)|(break)
```

### Arduino Keywords

```
(Serial)|(print)|(println)|(setup)|(loop)|(delay)|(pinMode)|(analogRead)|(analogWrite)|(digitalRead)|(digitalWrite)
```

### Identifiers:

```
([A-Za-z_])([A-Za-z0-9_])*
```

### Numbers:
```
[0-9]+
```

### Arithmetic Operators

```
(=)|(+)|(-)|(*)|(/)|(%)
```

### Boolean Operators
```
(&&)|(!)|(||)
```

### Comparison Operators

```
(==)|(\>)|(\>=)|(<)|(<=)|(!=)
```

### Comment
```
//.*
```

### Whitespace
```
[ ]
```

## Text Formatting:
```
(")|(\n)|(\t)
```

## Code Formatting:
```
(;)|(,)|(.)|(\()|(\))|(\{)
```

## Examples: 
Include clear, runnable examples for each token type, showing how they appear in source code. For example:


### C Based Keywords: 
```
int a = 0;
int b = 0;

while(a < 10){
	b = b * 2;
	a = a + 1;
}
```

### Arduino Sketch Keywords:
```
setup() {
	int value = 5;
}

loop() {
	if(value < 100)
		value = value + 1;
	else()
		Serial.println("100 reached.");
}
```

### Identifiers: 
```
class X{
	int y;
};
```

### Numbers: 
```
int z = 255;
```

### Arithmetic Operators
```
int a = 5;
int b = 10;
b = b - 5;
int c = a + b;
```

### Boolean Operators
```
bool xx = True;
bool yy = False;
if(xx || yy){
	xx = True;
	yy = True;
}
if(xx == True){
	if(xx&&yy){
		xx = False;
	}
	else{
		yy = False;
	}
}
```

### Comparison Operators
```
int a = 5;
int b = 6;

if (a != b){
	b = b -1;
}
if (b >= a){
	a = a + 1;
}
```

### Text Formatting:
```
Serial.print("Hello World!\n");
Serial.println("The cooler Hello World!");
```

### Code Formatting:
```
int lineOfCodeVar = 5;
if (lineOfCodeVar >= 4) {
	Serial.println("working as intended.");
}
```
