
# Scanner part 1 - Lexical Specification


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

// teehee
### Boolean Operators
	&& 
	! 
	|| 

### Comparison Operators
	==
	\>
	\>=
	< 
	<= 
	!= 

## Miscellaneous: 
Special Characters.
	;
	,
	.
	//
	"
	'
	(
	)
	{
	}
	\n
	\t

## Formal Specification:

For each token category, write a regular expression (using the basic regular expressions learned in class) that precisely defines how the token is formed from the input character stream.

For example:


### Keywords:

```
(#include)|(void)|(return)|(int)|(bool)|(if)|(else)|(while)|(true)|(false)|(break)
```

### Arduino Keywords

```
(setup)|(loop)|(delay)|(pinMode)|(analogRead)|(analogWrite)|(digitalRead)|(digitalWrite)
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

### Miscellaneous
```
(;)|(,)|(.)|(//)|(")|(')|(\()|(\))|(\{)|(\n)|(\t)
```

## Examples: 


Include clear, runnable examples for each token type, showing how they appear in source code. For example:


### C Based Keywords: 

```
*while*(*true*){
	break;
}
```

### Arduino Sketch Keywords:
**setup**:

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



Your examples should be comprehensive —not as incomplete as the short list shown here.

