Build Notepad2 and metapath
	https://github.com/zufuliu/notepad2/wiki/Build-Notepad2

echo | clang -dM -E -
echo | clang-cl -Xclang -dM -E -
echo | gcc -dM -E -

https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
-fsanitize=undefined

/wd4201 /wd4204
Disabled warnings:
	C4201: nonstandard extension used: nameless struct/union.
	C4204: nonstandard extension used: non-constant aggregate initializer.

/wd26429 /wd26446 /wd26451 /wd26454 /wd26472 /wd26481 /wd26482 /wd26485 /wd26486 /wd26489 /wd26494 /wd26495
Disabled Code Analysis warnings:
	C26429: Symbol is never tested for nullness, it can be marked as not_null (f.23).
	C26446: Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
	C26451: Arithmetic overflow
	C26454: Arithmetic overflow
	C26472: Don't use a static_cast for arithmetic conversions.
	C26481: Don't use pointer arithmetic. Use span instead (bounds.1)
	C26482: Only index into arrays using constant expressions (bounds.2).
	C26485: No array to pointer decay (bounds.3).
	C26486: Don't pass a pointer that may be invalid to a function. Parameter (lifetime.1).
	C26489: Don't dereference a pointer that may be invalid (lifetime.1).
	C26494: Variable is uninitialized. Always initialize an object (type.5).
	C26495: Variable is uninitialized. Always initialize a member variable (type.6).
