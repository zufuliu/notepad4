#include "EditLexer.h"
#include "EditStyle.h"

// https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/ProgrammingWithObjectiveC/Introduction/Introduction.html

static KEYWORDLIST Keywords_CPP = {{
"break case const continue default do else enum extern for "
"goto if inline register restrict return sizeof static struct "
"switch typedef union volatile while "
"_Alignas _Alignof _Atomic _Generic _Noreturn _Static_assert _Thread_local "

"alignas alignof asm catch class consteval constexpr const_cast decltype delete dynamic_cast "
"explicit export false friend mutable namespace new noexcept noreturn nullptr operator "
"private protected public reinterpret_cast static_assert static_cast template "
"this thread_local throw true try typeid typename using virtual "
"override final "
"_Pragma defined __has_include __has_include_next __has_attribute __has_cpp_attribute "
"and and_eq bitand bitor compl not not_eq or or_eq xor xor_eq "
"concept requires audit axiom "
"import module " "co_await co_return co_yield "
// MSVC
"__alignof __asm __assume __based __cdecl __declspec __event __except __fastcall __finally __forceinline __identifier __inline __interface __leave __raise __stdcall __clrcall __super __thiscall __try __unaligned __uuidof __hook __unhook __noop __vectorcall __pragma "
// GCC
"typeof __typeof__ __alignof__ __label__ __asm__ __thread __attribute__ __volatile__ __restrict__ __restrict __inline__ __extension__ "
// clang Objective-C/C++
"__nonnull __nullable __covariant __kindof nullable nonnull "
// Keil
"sfr sfr16 interrupt "

"NULL TRUE FALSE EOF WEOF "
"errno " "stdin stdout stderr " "signgam "
"CONST ENUM "

, // 1 Type Keyword
"__auto_type auto char double float int long short signed unsigned void "
"bool char8_t char16_t char32_t wchar_t nullptr_t nothrow_t "
"_Bool complex _Complex _Imaginary imaginary "
"__w64 __wchar_t __int8 __int16 __int32 __int64 __m128 __m128d __m128i __m256 __m256d __m256i __m64 __int3264 __ptr32 __ptr64 __sptr __uptr "
"__int128 __float80 __float128 __fp16 __complex__ __real__ __imag__ __complex128 _Decimal32 _Decimal64 _Decimal128 decimal32 decimal64 decimal128 "
"int128 qfloat "

// errno.h
"errno_t "
// fenv.h
"fenv_t fexcept_t "
// inttypes.h
"imaxdiv_t "
// math.h
"float_t double_t "
// setjmp.h
"jmp_buf sigjmp_buf "
// signal.h
"sig_atomic_t sigset_t pid_t "
// stdarg.h
"va_list "
// stdatomic.h
"atomic_flag atomic_bool atomic_char atomic_schar atomic_uchar atomic_short atomic_ushort atomic_int atomic_uint atomic_long atomic_ulong atomic_llong atomic_ullong atomic_char16_t atomic_char32_t atomic_wchar_t atomic_intptr_t atomic_uintptr_t atomic_size_t atomic_ptrdiff_t atomic_intmax_t atomic_uintmax_t "
// stddef.h
"size_t rsize_t ptrdiff_t max_align_t "
// stdint.h
"int8_t int16_t int32_t int64_t uint8_t uint16_t uint32_t uint64_t "
"intptr_t uintptr_t intmax_t uintmax_t "
"ssize_t "
"locale_t "
// stdio.h
"FILE fpos_t off_t "
// stdlib.h
"div_t ldiv_t lldiv_t "
// threads.h
"cnd_t thrd_t tss_t mtx_t tss_dtor_t thrd_start_t once_flag "
// time.h
"clock_t time_t "
// wchar.h
"wint_t mbstate_t "
// wctype.h
"wctrans_t wctype_t "
// std::byte
"byte "

, // 2 Preprocessor
" if elif else endif ifdef ifndef define undef include include_next import using pragma line error warning warn message "
"region endregion sccs ident assert unassert "

, // 3 Directive: Objective C/C++
" interface private protected public package property end protocol optional required implementation synthesize try throw catch finally autoreleasepool synchronized dynamic class selector encode compatibility_alias import available "
"property() selector() available() "

, // 4 Attribute
// Objective C/C++ @property()
"getter setter readwrite readonly strong weak copy assign retain nonatomic atomic nonnull nullable null_resettable "
// MSVC __declspec()
"deprecated dllexport dllimport naked noinline noreturn nothrow novtable property selectany thread uuid "
// GCC __attribute__()
"aligned alloc_size returns_twice noinline noclone always_inline flatten pure nothrow sentinel format format_arg no_instrument_function no_split_stack section constructor destructor used unused deprecated weak malloc alias ifunc warn_unused_result nonnull gnu_inline externally_visible hot cold artificial error warning cleanup common nocommon mode packed shared tls_model vector_size const "
"__aligned__ __alloc_size__ __noreturn__ __returns_twice__ __noinline__ __noclone__ __always_inline__ __flatten__ __pure__ __nothrow__ __sentinel__ __format__ __format_arg__ __no_instrument_function__ __no_split_stack__ __section__ __constructor__ __destructor__ __used__ __unused__ __deprecated__ __weak__ __malloc__ __alias__ __ifunc__ __warn_unused_result__ __nonnull__ __gnu_inline__ __externally_visible__ __hot__ __cold__ __artificial__ __error__ __warning__ __cleanup__ __common__ __nocommon__ __mode__ __packed__ __shared__ __tls_model__ __vector_size__ __const__ "
"cdecl stdcall __cdecl__ __stdcall__ __deprecated__ __dllexport__ __dllimport__ __naked__ __noinline__ __noreturn__ __nothrow__ __selectany__ "
"may_alias __may_alias__ visibility __visibility__"
// C++11, 14, 17, 20
"assert carries_dependency fallthrough ensures expects maybe_unused likely unlikely no_unique_address nodiscard optimize_for_synchronized "

, // 5 Class
// locale.h
"lconv "
// time.h
"timespec "

// STL class
"basic_string string wstring u16string u32string "
"vector deque list map multimap multiset queue priority_queue set stack initializer_list forward_list gslice gslice_array "
"unordered_map unordered_set array valarray tuple tuple_element tuple_size bitset hash pair indirect_array mask_array slice slice_array unordered_multimap unordered_multiset rb_tree bool_set dynamic_bitset "

"char_traits ios_base basic_ios "
"basic_streambuf basic_istream basic_ostream basic_iostream "
"basic_stringbuf basic_istringstream basic_ostringstream basic_stringstream "
"basic_filebuf basic_ifstream basic_ofstream basic_fstream "
"ios streambuf istream ostream iostream stringbuf istringstream ostringstream stringstream filebuf ifstream ofstream fstream "
"wios wstreambuf wistream wostream wiostream wstringbuf wistringstream wostringstream wstringstream wfilebuf wifstream wofstream wfstream "

"input_iterator_tag output_iterator_tag forward_iterator_tag bidirectional_iterator_tag random_access_iterator_tag "
"iterator reverse_iterator back_insert_iterator front_insert_iterator insert_iterator move_iterator raw_storage_iterator "
"istream_iterator ostream_iterator istreambuf_iterator ostreambuf_iterator "

"type_info bad_cast bad_typeid "
"type_index bad_alloc "
"exception bad_exception nested_exception "
"shared_ptr weak_ptr enable_shared_from_this bad_weak_ptr "
"default_delete unique_ptr "

"numeric_limits integral_constant "

"pointer_traits allocator_traits allocator uses_allocator scoped_allocator_adaptor "
"array_allocator bitmap_allocator "

"rebind iterator_traits "

"function unary_function binary_function "

"iterator_type value_type allocator_type size_type difference_type reference const_reference pointer pointer const_pointer iterator const_iterator reverse_iterator const_reverse_iterator traits_type iterator_category container_type "

// old class
"hash_map hash_multimap hash_set hash_multiset hashtable auto_ptr auto_ptr_ref "
"strstreambuf istrstream ostrstream strstream binder1st binder2nd "


// MFC
"CString CStringArray CStringList CArray CList CMap CTypedPtrArray CTypedPtrList CTypedPtrMap CTypeLibCacheMap CSimpleList CByteArray CWordArray CDWordArray CUIntArray CPtrArray CObArray CPtrList CObList CMapWordToPtr CMapPtrToWord CMapPtrToPtr CMapWordToOb CMapStringToPtr CMapStringToOb CMapStringToString CTypedSimpleList "
"CRuntimeClass CObject CException CFile CSize CPoint CRect CCmdUI CDataExchange CArchive CGdiObject CPen CBrush CFont CBitmap CPalette CRgn CDC CClientDC CWindowDC CPaintDC CMemDC CMetaFileDC CPreviewDC CImageList CMenu CCmdTarget CWnd CDialog CDialogEx CCommonDialog CFileDialog CFolderPickerDialog CFontDialog CColorDialog CPageSetupDialog CPrintDialog CPrintDialogEx CFindReplaceDialog CPropertyPage CPropertyPageEx CPropertySheet CPropertySheetEx CStatic CButton CSplitButton CBitmapButton CListBox CCheckListBox CDragListBox CComboBox CComboBoxEx CEdit CScrollBar CFrameWnd CFrameWndEx CMDIFrameWnd CMDIFrameWndEx CMDIChildWnd CMDIChildWndEx CMiniFrameWnd CView CCtrlView CListView CTreeView CEditView CRichEditView CScrollView CFormView CDaoRecordView CRecordView CHtmlView CHtmlEditView CPreviewView CPreviewViewEx COleDBRecordView CTabView CBasePane CDockSite CAutoHideDockSite CPane CDockablePane CPaneDivider CStatusBarCtrl CListCtrl CTreeCtrl CSpinButtonCtrl CSliderCtrl CProgressCtrl CHeaderCtrl CHotKeyCtrl CToolTipCtrl CTabCtrl CAnimateCtrl CToolBarCtrl CReBarCtrl CRichEditCtrl CIPAddressCtrl CPagerCtrl CLinkCtrl CNetAddressCtrl CDateTimeCtrl CMonthCalCtrl CControlBar CStatusBar CToolBar CDialogBar CReBar COleResizeBar CDockBar CSplitterWnd CSplitterWndEx CHtmlEditCtrl CMDITabProxyWnd CMDIClientAreaWnd CPaneFrameWnd CSocketWnd CWinThread CWinApp CWinAppEx CDocTemplate CDocument "

// Qt

// GTK

// Mac/iOS
"NSObject NSValue NSNumber NSString NSMutableString NSData NSMutableData NSArray NSMutableArray NSSet NSMutableSet NSDictionary NSMutableDictionary NSNull NSDecimalNumber NSCoder "
"NSThread NSTimer NSLock NSRecursiveLock NSConditionLock NSOperation NSOperationQueue NSAutoreleasePool NSEnumerator "
"NSURL NSURLConnection NSURLRequest NSURLResponse NSStream NSInputStream NSOutputStream "
"NSError NSException NSBundle NSIndexSet NSCharacterSet NSFileManager NSIndexPath NSDate NSLocale NSNotification NSNotificationCenter NSUUID "
"UIApplication UIDevice UIWindow UIScreen UIView UIViewController UIColor UIFont UIImage UIImageView UILabel UIButton UIScrollView UITableView UITableViewCell UISwitch UISlider UISegmentedControl UITextField UIGestureRecognizer UITouch UIEvent UITapGestureRecognizer UISwipeGestureRecognizer UIWebView UINavigationBar UINavigationItem UIBarButtonItem UITextView UIAlertView UISearchBar "

// third lib
"ASIHTTPRequest "

, // 6 Interface
// Win32/MFC
"IUnknown IStream "

// Mac/iOS
"NSCopying "

, // 7 Enumeration
// stdatomic.h
"memory_order "
"memory_order_relaxed memory_order_consume memory_order_acquire memory_order_release memory_order_acq_rel memory_order_seq_cst "
// threads.h
"mtx_plain mtx_recursive mtx_timed "
"thrd_timedout thrd_success thrd_busy thrd_error thrd_nomem "
// ios_base
"fmtflags iostate openmode seekdir "

, // 8 Constant
"__cplusplus __cplusplus_cli __assembler __midl "
"__FILE__ __FUNCTION__ __LINE__ __DATE__ __TIME__ __TIMESTAMP__ __func__ __VA_ARGS__ "
"__STDC__ __STDC_HOSTED__ __STDC_VERSION__ __STDC_IEC_559__ __STDC_IEC_559_COMPLEX__ __STDC_ISO_10646__ "
" __COUNTER__ __STRICT_ANSI__ __PRETTY_FUNCTION__ __STDC_DEC_FP__ "
"_MSC_VER _MSC_FULL_VER "
"__GNUC__ __GNUG__ __GNUC_MINOR__ __GNUC_PATCHLEVEL__ __OBJC__ __ASSEMBLER__ __GFORTRAN__ "
"__clang__ __clang_major__ __clang_minor__ __clang_patchlevel__ __llvm__ "
"__i386__ __x86_64__ __LP64__ __amd64__ __ia64__ __arm__ __aarch64__ "
"__MSVCRT__ __MINGW32__ __MINGW64__ __CYGWIN__ "
"__APPLE__ __linux__ __GLIBC__ "
"_X86_ _AMD64_ _IA64_ _ARM_ _ARM64_ "

// assert.h
"_DEBUG NDEBUG assert() "
// complex.h
"CX_LIMITED_RANGE "
"_Complex_I _Imaginary_I CMPLX() CMPLXF() CMPLXL() "
// ctype.h
"_tolower() _toupper() "
// errno.h
"EDOM EILSEQ ERANGE "
// fenv.h
"FENV_ACCESS "
"FE_INVALID FE_DENORMAL FE_DIVBYZERO FE_OVERFLOW FE_UNDERFLOW FE_INEXACT FE_ALL_EXCEPT "
"FE_TONEAREST FE_DOWNWARD FE_UPWARD FE_TOWARDZERO " "FE_DFL_ENV FE_PC64_ENV FE_PC53_ENV "
// float.h
"DECIMAL_DIG FLT_DECIMAL_DIG DBL_DECIMAL_DIG LDBL_DECIMAL_DIG "
// inttypes.h
"PRId64 PRIi64 PRIu64 PRIx64 PRIX64 SCNd64 SCNi64 SCNo64 SCNu64 SCNx64 "
// limits.h
"CHAR_BIT MB_LEN_MAX PATH_MAX SCHAR_MIN SCHAR_MAX UCHAR_MAX CHAR_MIN CHAR_MAX SHRT_MIN SHRT_MAX USHRT_MAX "
"INT_MIN INT_MAX UINT_MAX LONG_MIN LONG_MAX ULONG_MAX LLONG_MIN LLONG_MAX ULLONG_MAX _I64_MIN _I64_MAX _UI64_MAX "
// locale.h
"LC_ALL LC_COLLATE LC_CTYPE LC_MONETARY LC_NUMERIC LC_TIME LC_MESSAGES "
// math.h
"FP_CONTRACT "
"M_E M_LOG2E M_LOG10E M_LN2 M_LN10 M_PI M_PI_2 M_PI_4 M_1_PI M_2_PI M_2_SQRTPI M_SQRT2 M_SQRT1_2 "
"MAXFLOAT HUGE_VAL HUGE_VALF HUGE_VALL INFINITY NAN "
"FP_INFINITE FP_NAN FP_NORMAL FP_SUBNORMAL FP_ZERO " "FP_FAST_FMA FP_FAST_FMAF FP_FAST_FMAL "
"FP_ILOGB0 FP_ILOGBNAN " "MATH_ERRNO MATH_ERREXCEPT math_errhandling "
"fpclassify() isfinite() isinf() isnan() isnormal() signbit() "
"isgreater() isgreaterequal() isless() islessequal() islessgreater() isunordered() "
// signal.h
"SIG_DFL SIG_ERR SIG_IGN SIG_SGE SIG_ACK "
"SIGABRT SIGFPE SIGILL SIGINT SIGSEGV SIGTERM SIGBREAK "
// stdarg.h
"va_arg() va_copy() va_end() va_start() "
// stdatomic.h
"ATOMIC_BOOL_LOCK_FREE ATOMIC_CHAR_LOCK_FREE ATOMIC_CHAR16_T_LOCK_FREE ATOMIC_CHAR32_T_LOCK_FREE ATOMIC_WCHAR_T_LOCK_FREE ATOMIC_SHORT_LOCK_FREE ATOMIC_INT_LOCK_FREE ATOMIC_LONG_LOCK_FREE ATOMIC_LLONG_LOCK_FREE ATOMIC_POINTER_LOCK_FREE "
"ATOMIC_FLAG_INIT ATOMIC_VAR_INIT() "
// stddef.h
"offsetof() "
// stdint.h
"INT8_MIN INT8_MAX UINT8_MAX INT16_MIN INT16_MAX UINT16_MAX INT32_MIN INT32_MAX UINT32_MAX INT64_MIN INT64_MAX UINT64_MAX "
"INTPTR_MIN INTPTR_MAX UINTPTR_MAX INTMAX_MIN INTMAX_MAX UINTMAX_MAX PTRDIFF_MIN PTRDIFF_MAX "
"SIG_ATOMIC_MIN SIG_ATOMIC_MAX SIZE_MAX RSIZE_MAX WINT_MIN WINT_MAX "
"INT64_C() UINT64_C() INTMAX_C() UINTMAX_C() "
// stdio.h
"_IOFBF _IOLBF _IONBF BUFSIZ FOPEN_MAX FILENAME_MAX TMP_MAX L_tmpnam P_tmpdir SEEK_SET SEEK_CUR SEEK_END "
// stdlib.h
"MAX_PATH EXIT_FAILURE EXIT_SUCCESS RAND_MAX MB_CUR_MAX "
// threads.h
"ONCE_FLAG_INIT TSS_DTOR_ITERATIONS "
// time.h
"CLOCKS_PER_SEC TIME_UTC "
// wchar.h
"WCHAR_MAX WCHAR_MIN "

// Win32/MFC
"_WIN32 WIN32 WINVER _WIN32_WINNT _WIN32_IE _WIN64 _UNICODE UNICODE "
"CDECL CALLBACK WINAPI WINAPIV APIENTRY APIPRIVATE PASCAL WINAPI_INLINE "
"DECLARE_HANDLE() DECLSPEC_UUID() DEFINE_GUID() GET_X_LPARAM() GET_Y_LPARAM() MAKEWORD() MAKELONG() MAKEWPARAM() MAKELPARAM() MAKELRESULT() LOWORD() HIWORD() LOBYTE() HIBYTE() RGB() CMYK() TEXT() __TEXT() "
"WM_APP WM_USER "
"SUCCEEDED() FAILED() ASSERT() VERIFY() TRACE() UNREFERENCED_PARAMETER() DECLARE_DYNAMIC() afx_msg BEGIN_MESSAGE_MAP() END_MESSAGE_MAP() "

// Python
"PY_MAJOR_VERSION PyObject_HEAD PyObject_HEAD_INIT() PyObject_VAR_HEAD PyVarObject_HEAD_INIT() "
"Py_INCREF() Py_DECREF() Py_None Py_RETURN_NONE Py_True Py_False Py_RETURN_TRUE Py_RETURN_FALSE "
"PyMODINIT_FUNC METH_VARARGS METH_KEYWORDS METH_NOARGS PyModuleDef_HEAD_INIT "
// Mac/iOS
"IBOutlet IBAction __has_feature() "

, // 9 2nd Keyword: Objective C/C++
"id instancetype Class Protocol SEL IMP self super "
"nil Nil BOOL YES NO oneway in out inout bycopy byref "
"__unsafe_unretained __strong __weak __bridge __bridge_transfer __bridge_retained __block __autoreleasing "

, // 10 2nd Type Keyword
// Win32
"VOID PVOID LPVOID LPCVOID BOOL PBOOL LPBOOL BOOLEAN PBOOLEAN BYTE PBYTE LPBYTE WORD PWORD LPWORD ATOM DWORD PDWORD LPDWORD "
"UCHAR PUCHAR SHORT PSHORT USHORT PUSHORT INT PINT LPINT UINT PUINT LONG PLONG LPLONG ULONG PULONG FLOAT PFLOAT "
"LONGLONG PLONGLONG ULONGLONG PULONGLONG DWORDLONG PDWORDLONG LARGE_INTEGER PLARGE_INTEGER ULARGE_INTEGER PULARGE_INTEGER FLOAT128 PFLOAT128 M128A PM128A LONG64 PLONG64 ULONG64 PULONG64 DWORD64 PDWORD64 LONG32 PLONG32 ULONG32 PULONG32 DWORD32 PDWORD32 DWORD_PTR PDWORD_PTR "
"SHANDLE_PTR HANDLE_PTR UHALF_PTR PUHALF_PTR HALF_PTR PHALF_PTR "
"INT_PTR PINT_PTR UINT_PTR PUINT_PTR LONG_PTR PLONG_PTR ULONG_PTR PULONG_PTR "
"INT8 INT16 INT32 INT64 UINT8 UINT16 UINT32 UINT64 PINT8 PINT16 PINT32 PINT64 PUINT8 PUINT16 PUINT32 PUINT64 "
"CHAR LPSTR LPCSTR WCHAR LPWSTR LPCWSTR TCHAR LPTSTR LPCTSTR "
"WPARAM LPARAM LRESULT HRESULT "
"HANDLE PHANDLE HGLOBAL HLOCAL HINSTANCE HMODULE HWND HHOOK HEVENT HGDIOBJ HKEY HACCEL HBITMAP HBRUSH HCOLORSPACE HDC HGLRC HDESK HENHMETAFILE HFONT HICON HMENU HMETAFILE HPALETTE HPEN HRGN HRSRC HSPRITE HLSURF HSTR HTASK HWINSTA HKL HWINEVENTHOOK HMONITOR HUMPD HFILE HCURSOR HDWP HINTERNET PHKEY "
"MSG LPMSG NMHDR LPNMHDR "
"LCID PLCID LANGID PSID SID PISID LUID PLUID ACL PACL LGRPID LCTYPE CALTYPE CALID CPINFO LPCPINFO GEOID GEOTYPE GEOCLASS XFORM LPXFORM CONTEXT PCONTEXT ACCESS_MASK KSPIN_LOCK "
"SIZE_T PSIZE_T SSIZE_T PSSIZE_T RECT LPRECT RECTL LPRECTL POINT LPPOINT POINTL SIZE LPSIZE SIZEL LPSIZEL POINTS "
"COLORREF LPCOLORREF RGBQUAD LPRGBQUAD BITMAP LPBITMAP "
"FILETIME LPFILETIME SYSTEMTIME LPSYSTEMTIME "
"GUID LPGUID LPCGUID IID LPIID CLSID LPCLSID FMTID LPFMTID REFGUID REFIID REFCLSID REFFMTID "
"WNDCLASS WNDCLASSEX LOGFONT LOGBRUSH VARIANT "
// OpenGL
"GLenum GLboolean GLbitfield GLvoid GLbyte GLshort GLint GLubyte GLushort GLuint GLsizei GLfloat GLclampf GLdouble GLclampd "
// sqlite3
"sqlite3 sqlite3_stmt "
// JNI
"jint jlong jbyte jboolean jchar jshort jfloat jdouble jsize "
"jobject jclass jthrowable jstring jarray jweak jvalue "
"jbooleanArray jbyteArray jcharArray jshortArray jintArray jlongArray jfloatArray jdoubleArray jobjectArray "
"jfieldID jmethodID JNIEnv JavaVM "
// Python
"Py_ssize_t Py_complex Py_buffer PyCFunction "
"PyObject PyVarObject PyTypeObject "
"PyMethodDef PyModuleDef "
// pthread
"pthread_t pthread_attr_t pthread_mutex_t pthread_cond_t "
// Mac/iOS
"NSInteger NSUInteger NSTimeInterval NSZone NSRange "
"CGFloat CGRect CGSize CGPoint dispatch_queue_t unichar "

, // 11 Assembler Intruction
"aaa aad aam aas adc add and arpl bound bsf bsr bswap bt btc btr bts call cbw cwde "
"cdqe clc cld cli clts cmc cmova cmovae cmovb cmovbe cmovc cmove cmovg cmovge cmovl "
"cmovle cmovna cmovnae cmovnb cmovnbe cmovnc cmovne cmovng cmovnge cmovnl cmovnle "
"cmovno cmovnp cmovns cmovnz cmovo cmovp cmovpe cmovpo cmovs cmovz cmp cmps cmpsb "
"cmpsw cmpsd cmpsq cmpxchg cmpxch8b cmpxchg8b cmpxchg16b cpuid cwd cdq cqo daa das "
"dec div emms enter hlt idiv imul in inc ins insb insw insd int int3 into invd invlpg "
"iret iretd ja jae jb jbe jc jcxz jecxz jrcxz je jg jge jl jle jna jnae jnb jnbe "
"jnc jne jng jnge jnl jnle jno jnp jns jnz jo jp jpe jpo js jz jmp lahf lar lds les "
"lfs lgs lss lea leave lgdt lidt lldt lmsw lock lods lodsb lodsw lodsd lodsq loop "
"loope loopne loopnz loopz lsl ltr mov movbe movs movsb movsw movsd movsq movsx "
"movsxd movzx mul neg nop not or out outs outsb outsw outsd pause pop popa popad "
"popf popfd popfq push pusha pushad pushf pushfd rcl rcr rol ror rdmsr rdpmc rdtsc "
"rdtscp rep repe repz repne repnz ret retf rsm sahf sal sar shl shr sbb scas scasb "
"scasw scasd seta setae setb setbe setc sete setg setge setl setle setna setnae "
"setnb setnbe setnc setne setng setnge setnl setnle setno setnp setns setnz seto "
"setp setpe setpo sets setz sgdt shld shrd sidt sldt smsw stc std sti stos stosb "
"stosw stosd stosq str sub swapgs syscall sysenter sysexit sysret test ud2 verr "
"verw wait wbinvd wrmsr xadd xchg xgetbv xlat xlatb xor xrstor xsave xsetbv "
// AMD
"clgi invlpga iretq scasq stgi "
// GNU Assembler Instruction
"adcl adcb adcq addl addq addw andb andl andq andw bsrl btl btsl cbtw cltd cmpb cmpl "
"cmpq cmpw decl decq decw divl fldl fstpl idivl imull incl incq incw insl jmpl jmpq "
"leal leaq lgdtl lidtl ljmp ljmpl lret lretq lretw movabs movb movl movsbl movsl "
"movswl movw movzb movzbl movzwl mull mulq negl negw notl notq outsl orb orl orw popal popaw "
"popfl popl popq popw pushal pushfl pushfq pushl pushq rclb rcll rcrl retl retq roll "
"sall salq sarl sarb sbbb sbbl setaeb shldl shll shrb shrdl shrl shrq shrw stosl subb "
"subl subq subw testb testl testq testw xorb xorl xorq xorw "
// FPU Instruction
"f2xm1 fabs fadd faddp fiadd fbld fbstp fchs fclex fnclex fcmovb fcmove fcmovbe "
"fcmovu fcmovnb fcmovne fcmovnbe fcmovnu fcom fcomp fcompp fcomi fcomip fucomi "
"fucomip fcos fdecstp fdiv fdivp fidiv fdivr fdivrp fidivr ffree ficom ficomp fild "
"fincstp finit fninit fist fistp fisttp fld fld1 fldl2t fldl2e fldpi fldlg2 fldln2 "
"fldz fldcw fldenv fmul fmulp fimul fnop fpatan fprem fprem1 fptan frndint frstor "
"fsave fnsave fscale fsin fsincos fsqrt fst fstp fstcw fnstcw fstenv fnstenv fstsw "
"fnstsw fsub fsubp fisub fsubr fsubrp fisubr ftst fucom fucomp fucompp fxam fxch "
"fxrstor fxsave fxtract fyl2x fyl2xp1 fwait "
"faddl fstpt flds "
// XMM SSE SSE2 SSE4
"addpd addps addsd addss addsubpd addsubps andpd andps andnpd andnps blendpd blendps "
"blendvpd blendvps clflush cmppd cmpeqpd cmpltpd cmpltpd cmplepd cmpunordpd cmpneqpd "
"cmpnltpd cmpnlepd cmpordpd cmpps cmpeqps cmpltps cmpleps cmpunordps cmpneqps "
"cmpnltps cmpnleps cmpordps cmpsd cmpeqsd cmpltsd cmplesd cmpunordsd cmpneqsd "
"cmpnltsd cmpnlesd cmpordsd cmpss cmpeqss cmpltss cmpunordss cmpneqss cmpnltss "
"cmpnless cmpordss comisd comiss crc32 cvtdq2pd cvtdq2ps cvtpd2dq cvtpd2pi cvtpd2ps "
"cvtpi2pd cvtpi2ps cvtps2dq cvtps2pd cvtps2pi cvtsd2si cvtsd2ss cvtsi2sd cvtsi2ss "
"cvtss2sd cvtss2si cvttpd2dq cvttpd2pi cvttps2dq cvttps2pi cvttsd2si cvttss2si "
"divpd divps divsd divss dppd dpps extractps haddpd haddps hsubpd hsubps insertps "
"lddqu ldmxcsr lfence maskmovdqu maskmovq maxpd maxps maxsd maxss mfence minpd minps "
"minsd minss monitor movapd movaps movd movq movddup movdqa movdqu movdq2q movhlps "
"movhpd movhps movlhps movlpd movlps movmskpd movmskps movntdqa movntdq movnti "
"movntpd movntps movntq movq2dq movsd movshdup movsldup movss movupd movups mpsadbw "
"mulpd mulps mulsd mulss mwait "
"orpd orps pabsb padsw pabsd packsswb packssdw packusdw packuswb paddb paddw "
"paddd paddq paddsb paddsw paddusb paddusw palignr pand pandn pavgb pavgw "
"pblendvb pblendw pcmpeqb pcmpeqw pcmpeqd pcmpeqq pcmpestri pcmpestrm pcmpistri "
"pcmpistrm pcmpgtb pcmpgtw pcmpgtd pcmpgtq pextrb pextrd pextrq pextrw phaddw "
"phaddd phaddsw phminposuw phsubw phsubd phsubsw pinsrb pinsrd pinsrq pinsrw "
"pmaddubsw pmaddwd pmaxsb pmaxsd pmaxsw pmaxub pmaxud pmaxuw pminsb pminsd pminsw "
"pminub pminud pminuw pmovmskb pmovsx pmovzx pmuldq pmulhrsw pmulhuw pmulhw pmulld "
"pmullw pmuludq popcnt por prefetcht0 prefetcht1 prefetcht2 prefetchnta psadbw "
"pshufb pshufd pshufhw pshuflw pshufw psignb psignw psignd pslldq psllw pslld psllq "
"psraw psrad psrldq psrlw psrld psrlq psubb psubw psubd psubq psubsb psubsw psubusb "
"psubusw ptest punpckhbw punpckhwd punpckhdq punpckhqdq punpcklbw punpcklwd punpckldq "
"punpcklqdq pxor rcpps rcpss roundpd roundps roundsd roundss rsqrtps rsqrtss sfence "
"shufpd shufps sqrtpd sqrtps sqrtsd sqrtss stmxcsr subpd subps subsd subss ucomisd "
"ucomiss unpckhpd unpckhps unpcklpd unpcklps xorpd xorps "
// VMX
"invept invvpid vmcall vmclear vmlaunch vmresume vmptrld vmptrst vmread vmwrite vmxoff vmxon "
// AMD
"lzcnt prefetch prefetchw skinit vmload vmmcall vmrun vmsave "
"extrq insertq movntsd movntss "
// AMD 3DNow!
"femms pavgusb pf2id pf2iw pfacc pfadd pfcmpeq pfcmpge pfcmpgt "
"pfmax pfmin pfmul pfnacc pfpnacc pfrcp pfrcpit1 "
"pfrcpit2 pfrsqit1 pfrsqrt pfsub pfsubr pi2fd pi2fw pmulhrw pswapd "
// AMD XOP/FMA4

, // 12 Assembler Register
"eax ebx ecx edx esi edi esp ebp "
"ax ah al bx bh bl cx ch cl dx dh dl "
"si di sp bp cs ds ss es fs gs eflags eip "
"st0 st1 st2 st3 st4 st5 st6 st7 "
"mm0 mm1 mm2 mm3 mm4 mm5 mm6 mm7 mxcsr msr "
"xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7 "
"xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15 "
"cr0 cr2 cr3 cr4 gdtr ldtr idtr dr0 dr1 dr2 dr3 dr6 dr7 "
"rax rbx rcx rdx rsi rdi rbp rsp "
"r8 r9 r10 r11 r12 r13 r14 r15 "
"r8d r9d r10d r11d r12d r13d r14d r15d "
"r8w r9w r10w r11w r12w r13w r14w r15w "
"sil dil bpl spl r8l r9l r10l r11l r12l r13l r14l r15l "
"r8b r9b r10b r11b r12b r13b r14b r15b "
"ymm0 ymm1 ymm2 ymm3 ymm4 ymm5 ymm6 ymm7 "
"ymm8 ymm9 ymm10 ymm11 ymm12 ymm13 ymm14 ymm15 "
"rip st tr3 tr4 tr5 tr6 tr7 eiz "

, // 13 C Function
// complex.h
"cacos() casin() catan() ccos() csin() ctan() cacosh() casinh() catanh() ccosh() csinh() ctanh() "
"cexp() clog() cabs() cpow() csqrt() carg() cimag() creal() conj() cproj() "
// ctype.h
"isalnum() isalpha() isblank() iscntrl() isdigit() isgraph() islower() isprint() ispunct() isspace() isupper() isxdigit() "
"tolower() toupper() "
// fenv.h
"feclearexcept() fegetexceptflag() feraiseexcept() fesetexceptflag() fetestexcept() fegetround() fesetround() "
"fegetenv() feholdexcept() fesetenv() feupdateenv() "
// inttypes.h
"imaxabs() imaxdiv() strtoimax() strtoumax() wcstoimax() wcstoumax() "
// locale.h
"setlocale() localeconv() "
// math.h
"acos() asin() atan() atan2() cos() sin() tan() acosh() asinh() atanh() cosh() sinh() tanh() "
"exp() exp2() expm1() frexp() ilogb() ldexp() log() log10() log1p() log2() logb() modf() scalbn() scalbln() "
"cbrt() fabs() hypot() pow() sqrt() erf() erfc() lgamma() tgamma() "
"ceil() floor() nearbyint() rint() lint() llint() round() lround() llround() trunc() "
"fmod() remainder() remquo() copysign() nan() nextafter() nexttoward() fdim() fmax() fmin() fma() "
// setjmp.h
"_longjmp() longjmp() siglongjmp() _setjmp() setjmp() sigsetjmp() "
// signal.h
"signal() raise() "
// stdatomic.h
"atomic_init() kill_dependency() atomic_thread_fence() atomic_signal_fence() atomic_is_lock_free() "
"atomic_store() atomic_store_explicit() atomic_load() atomic_load_explicit() atomic_exchange() atomic_exchange_explicit() "
"atomic_compare_exchange_strong() atomic_compare_exchange_strong_explicit() atomic_compare_exchange_weak() atomic_compare_exchange_weak_explicit() "
"atomic_fetch_add() atomic_fetch_add_explicit() atomic_fetch_sub() atomic_fetch_sub_explicit() atomic_fetch_or() atomic_fetch_or_explicit() atomic_fetch_xor() atomic_fetch_xor_explicit() atomic_fetch_and() atomic_fetch_and_explicit() "
"atomic_flag_test_and_set() atomic_flag_test_and_set_explicit() atomic_flag_clear() atomic_flag_clear_explicit() "
// stdio.h
"remove() rename() tmpfile() tmpnam() " "fopen() freopen() fflush() fclose() setbuf() setvbuf() "
"fprintf() fscanf() printf() scanf() snprintf() sprintf() sscanf() vfprintf() vfscanf() vsnprintf() vsprintf() vsscanf() vprintf() vscanf() "
"fgetc() fgets() getc() getchar() putc() putchar() puts() fputc() fputs() ungetc() " "fread() fwrite() "
"fgetpos() fseek() fsetpos() ftell() rewind() " "clearerr() feof() ferror() perror() "
// stdlib.h
"atof() atoi() atol() atoll() strtod() strtof() strtold() strtol() strtoll() strtoul() strtoull() "
"rand() srand() " "aligned_alloc() calloc() free() malloc() realloc() "
"abort() atexit() at_quick_exit() exit() _Exit() getenv() quick_exit() system() "
"qsort() bsearch() " "abs() labs() llabs() div() ldiv() lldiv() "
"mblen() mbtowc() wctomb() mbstowcs() wcstombs() "
// string.h
"memcpy() memmove() memcmp() memchr() memset() "
"strcpy() strncpy() strcat() strncat() strcmp() strcoll() strncmp() strxfrm() "
"strchr() strcspn() strpbrk() strrchr() strspn() strstr() strtok() " "strerror() strlen() "
// threads.h
"call_once() cnd_broadcast() cnd_destroy() cnd_init() cnd_signal() cnd_timedwait() cnd_wait() "
"mtx_destroy() mtx_init() mtx_lock() mtx_timedlock() mtx_trylock() mtx_unlock() "
"thrd_create() thrd_current() thrd_detach() thrd_equal() thrd_exit() thrd_join() thrd_sleep() thrd_yield() "
"tss_create() tss_delete() tss_get() tss_set() "
// time.h
"clock() difftime() mktime() time() timespec_get() " "asctime() ctime() gmtime() localtime() strftime() "
// uchar.h
"mbrtoc16() c16rtomb() mbrtoc32() c32rtomb() "
// wchar.h
"fwprintf() fwscanf() swprintf() swscanf() vfwprintf() vfwscanf() vswprintf() vswscanf() vwprintf() vwscanf() wprintf() wscanf() "
"fgetwc() fgetws() fputwc() fputws() fwide() getwc() getwchar() putwc() putwchar() ungetwc() "
"wcstod() wsctof() wcstold() wcstol() wcstoll() wcstoul() wcstoull() "
"wmemcpy() wmemmove() wmemcmp() wmemchr() wmemset() "
"wcscpy() wcsncpy() wcscat() wcsncat() wcscmp() wcscoll() wcsncmp() wcsxfrm() "
"wcschr() wcscspn() wcspbrk() wcsrchr() wcsspn() wcsstr() wcstok() wcslen() wcsftime() "
"btowc() wctob() mbsinit() mbrlen() mbrtowc() wcrtomb() mbstortowcs() wcsrtombs() "
// wctype.h
"iswalnum() iswalpha() iswblank() iswcntrl() iswdigit() iswgraph() iswlower() iswprint() iswpunct() iswspace() iswupper() iswxdigit() "
"iswctype() wctype() towlower() towupper() towctrans() wctrans() "

, // 14 C++ Function
// algorithm
"forward() move() move_if_noexcept() move_backward() addressof() swap() get() " // move
"distance() advance() next() prev() base() back_inserter() front_inserter() inserter() "
"adjacent_find() all_of() any_of() binary_search() copy() copy_backward() copy_if() copy_n() count() count_if() equal() equal_range() fill() fill_n() find() find_end() find_first_of() find_if() find_if_not() for_each() generate() generate_n() includes() inplace_merge() is_heap() is_heap_until() is_partitioned() is_permutation() is_sorted() is_sorted_until() iter_swap() lexicographical_compare() lower_bound() make_heap() max() max_element() merge() min() min_element() minmax() minmax_element() mismatch() next_permutation() none_of() nth_element() partial_sort() partial_sort_copy() partition() partition_copy() partition_point() pop_heap() prev_permutation() push_heap() random_shuffle() remove() remove_copy() remove_copy_if() remove_if() replace() replace_copy() replace_copy_if() replace_if() reverse() reverse_copy() rotate() rotate_copy() search() search_n() set_difference() set_intersection() set_symmetric_difference() set_union() shuffle() sort() sort_heap() stable_partition() stable_sort() swap_ranges() transform() unique() unique_copy() upper_bound() "
// heap
"get_temporary_buffer() return_temporary_buffer() "
// pair
"make_pair() "
// basic_string
"to_string() to_wstring() getline() stoi() stol() stoul() stoll() stoull() stof() stod() stold() "

// vector
"get_allocator() assign() begin() end() rbegin() rend() cbegin() cend() crbegin() crend() size() max_size() resize() shrink_to_fit() capacity() empty() reserve() at() front() back() data() push_back() pop_back() emplace() insert() erase() clear() "
// basic_string
"length() append() c_str() rfind() find_first_of() find_last_of() find_first_not_of() find_last_not_of() substr() compare() "
// list
"push_front() pop_front() emplace_front() emplace_back() splice() "
// stack
"push() pop() top() "
// forward_list
"before_begin() cbefore_begin() emplace_after() insert_after() erase_after() splice_after() "

// Mac/iOS
"NSLog() "

, // 15 Code Snippet
"_Pragma() defined() comment() __has_include() __has_include_next() __has_attribute() "
"sizeof() for^() if^() switch^() while^() catch^() else^if^() else^{} "
"alignas() alignof() delete[] decltype() noexcept() typeid() typeof() static_assert() requires() "
"static_cast<> const_cast<> dynamic_cast<> reinterpret_cast<> "
"_Alignas() _Alignof() _Generic() _Static_assert() "
"__attribute__() __typeof__() __alignof__() "
"__except^() __alignof() __declspec() __uuidof() __pragma() "
}};

static EDITSTYLE Styles_CPP[] = {
	EDITSTYLE_DEFAULT,
	{ MULTI_STYLE(SCE_C_WORD, SCE_C_2NDWORD, 0, 0), NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, EDITSTYLE_HOLE(L"Type Keyword"), L"fore:#0000FF" },
	{ SCE_C_PREPROCESSOR, NP2STYLE_Preprocessor, EDITSTYLE_HOLE(L"Preprocessor"), L"fore:#FF8000" },
	{ SCE_C_ATTRIBUTE, NP2STYLE_Attribute, EDITSTYLE_HOLE(L"Attribute"), L"fore:#FF8000" },
	{ SCE_C_DIRECTIVE, NP2STYLE_Directive, EDITSTYLE_HOLE(L"Directive"), L"fore:#0000FF" },
	{ SCE_C_2NDWORD2, NP2STYLE_Type, EDITSTYLE_HOLE(L"Type"), L"fore:#007F7F" },
	{ SCE_C_CLASS, NP2STYLE_Class, EDITSTYLE_HOLE(L"Class"), L"fore:#0080FF" },
	{ SCE_C_STRUCT, NP2STYLE_Struct, EDITSTYLE_HOLE(L"Struct"), L"fore:#0080FF" },
	{ SCE_C_UNION, NP2STYLE_Union, EDITSTYLE_HOLE(L"Union"), L"fore:#0080FF" },
	{ SCE_C_INTERFACE, NP2STYLE_Interface, EDITSTYLE_HOLE(L"Interface"), L"bold; fore:#1E90FF" },
	{ SCE_C_FUNCTION, NP2STYLE_Function, EDITSTYLE_HOLE(L"Function"), L"fore:#A46000" },
	{ SCE_C_ENUMERATION, NP2STYLE_Enumeration, EDITSTYLE_HOLE(L"Enumeration"), L"fore:#FF8000" },
	{ SCE_C_CONSTANT, NP2STYLE_Constant, EDITSTYLE_HOLE(L"Constant"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_MACRO2, SCE_C_MACRO, 0, 0), NP2STYLE_Macro, EDITSTYLE_HOLE(L"Macro"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, EDITSTYLE_HOLE(L"Doc Comment Tag"), L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2STYLE_DocComment, EDITSTYLE_HOLE(L"Doc Comment"), L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_STRINGRAW, NP2STYLE_RawString, EDITSTYLE_HOLE(L"Raw String"), L"fore:#E24000" },
	{ SCE_C_LABEL, NP2STYLE_Label, EDITSTYLE_HOLE(L"Label"), L"back:#FFC040" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ SCE_C_ASM_INSTRUCTION, NP2STYLE_Instruction, EDITSTYLE_HOLE(L"Instruction"), L"fore:#0080FF" },
	{ SCE_C_ASM_REGISTER, NP2STYLE_Register, EDITSTYLE_HOLE(L"Register"), L"fore:#FF8000" },
};

EDITLEXER lexCPP = {
	SCLEX_CPP, NP2LEX_CPP,
	EDITLEXER_HOLE(L"C/C++ Source", Styles_CPP),
	L"c; cpp; cxx; cc; h; hpp; hxx; hh; inl; pch; idl; odl; midl; mm; xpm",
	&Keywords_CPP,
	Styles_CPP
};

