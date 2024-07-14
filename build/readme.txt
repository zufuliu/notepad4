Build Notepad4 and matepath
	https://github.com/zufuliu/notepad4/wiki/Build-Notepad4
	https://github.com/zufuliu/notepad4/wiki/Localization

echo | clang -dM -E -
echo | clang-cl -Xclang -dM -E -
echo | gcc -dM -E -

echo | clang -dM -E -xc++ -
echo | clang-cl -Xclang -dM -E -Xclang -xc++ -
echo | gcc -dM -E -xc++ -

lexlib, src
cl /utf-8 /W4 /c /EHsc /std:c++20 /O2 /GS- /GR- /Gv /FAcs /DNDEBUG /DUNICODE /DNOMINMAX /I../include
lexers
cl /utf-8 /W4 /c /EHsc /std:c++20 /O2 /GS- /GR- /Gv /FAcs /DNDEBUG /DUNICODE /DNOMINMAX /I../include /I../lexlib
lexilla
cl /utf-8 /W4 /c /EHsc /std:c++20 /O2 /GS- /GR- /Gv /FAcs /DNDEBUG /DUNICODE /DNOMINMAX /I../include /I../lexlib /I../../scintilla/include
win32
cl /utf-8 /W4 /c /EHsc /std:c++20 /O2 /GS- /GR- /Gv /FAcs /DNDEBUG /DUNICODE /DNOMINMAX /I../include /I../src
src
cl /utf-8 /W4 /wd4996 /c /O2 /GS- /GR- /Gv /FAcs /DNDEBUG /DUNICODE /DNOMINMAX /I../scintilla/include
cl /utf-8 /W4 /wd4996 /c /EHsc /std:c++20 /O2 /GS- /GR- /Gv /FAcs /DNDEBUG /DUNICODE /DNOMINMAX /I../scintilla/include

https://clang.llvm.org/docs/AddressSanitizer.html
https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
https://github.com/google/sanitizers/wiki/AddressSanitizer
https://learn.microsoft.com/en-us/cpp/sanitizers/asan
	1. add <LLVM Dir>\lib\clang\<version>\lib\windows to Linker -> General -> Additional Library Directories
	2. add -fsanitize=address,undefined,integer to LLVM -> Additional Compiler Options or C/C++ -> Command Line
	3. change C/C++ -> Code Generation -> Runtime Library to Multi-threaded DLL (/MD)
	4. add _DISABLE_STL_ANNOTATION to C/C++ -> Preprocessor -> Preprocessor Definitions to fix missing stl_asan.lib
	5. change C/C++ -> Optimization -> Omit Frame Pointers to No (/Oy-)
	6. Linker -> Input -> Additional Dependencies for -fsanitize=address:
		clang_rt.asan_dynamic-x86_64.lib
		clang_rt.asan_dynamic_runtime_thunk-x86_64.lib
		copy clang_rt.asan_dynamic-x86_64.dll to exe folder

https://github.com/google/sanitizers/wiki/AddressSanitizerFlags#run-time-flags
SetEnvironmentVariable(UBSAN_OPTIONS, L"log_path=" WC_NOTEPAD4 L"-UBSan.log");
SetEnvironmentVariable(UBSAN_OPTIONS, L"log_path=" WC_MATEPATH L"-UBSan.log");
SetEnvironmentVariable(UBSAN_OPTIONS, L"log_path=" WC_NOTEPAD4 L"-UBSan.log,print_stacktrace=1");
SetEnvironmentVariable(UBSAN_OPTIONS, L"log_path=" WC_MATEPATH L"-UBSan.log,print_stacktrace=1");

/wd4201 /wd4204 /wd4221
Disabled warnings:
	C4201: nonstandard extension used: nameless struct/union.
	C4204: nonstandard extension used: non-constant aggregate initializer.
	C4221: nonstandard extension used: 'identifier' : cannot be initialized using address of automatic variable.

/wd26429 /wd26446 /wd26472 /wd26481 /wd26482 /wd26485 /wd26486 /wd26489
Disabled Code Analysis warnings:
	C26429: Symbol is never tested for nullness, it can be marked as not_null (f.23).
	C26446: Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
	C26472: Don't use a static_cast for arithmetic conversions.
	C26481: Don't use pointer arithmetic. Use span instead (bounds.1)
	C26482: Only index into arrays using constant expressions (bounds.2).
	C26485: No array to pointer decay (bounds.3).
	C26486: Don't pass a pointer that may be invalid to a function. Parameter (lifetime.1).
	C26489: Don't dereference a pointer that may be invalid (lifetime.1).
