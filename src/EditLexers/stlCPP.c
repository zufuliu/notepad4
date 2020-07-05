#include "EditLexer.h"
#include "EditStyleX.h"

// https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/ProgrammingWithObjectiveC/Introduction/Introduction.html

static KEYWORDLIST Keywords_CPP = {{
"break case const continue default do else enum extern for "
"goto if inline register restrict return sizeof static struct "
"switch typedef union volatile while "
"_Alignas _Alignof _Atomic _Generic _Noreturn _Static_assert _Thread_local "

"alignas alignof asm catch class consteval constexpr constinit const_cast decltype delete dynamic_cast "
"explicit export false friend mutable namespace new noexcept noreturn nullptr operator "
"private protected public reinterpret_cast static_assert static_cast template "
"this thread_local throw true try typeid typename using virtual "
"override final "
"_Pragma defined __has_include __has_include_next __has_attribute __has_cpp_attribute "
"and and_eq bitand bitor compl not not_eq or or_eq xor xor_eq "
"concept requires audit axiom "
"import module " "co_await co_return co_yield "
// MSVC
"__alignof __asm __assume __based __cdecl __clrcall __declspec __event __except __fastcall __finally __forceinline __hook "
"__identifier __if_exists __if_not_exists __inline __interface __leave __multiple_inheritance __noop __pragma __raise __restrict "
"__single_inheritance __stdcall __super __thiscall __try __unaligned __unhook __uuidof __vectorcall __virtual_inheritance "
// COM
"interface "
// GCC
"typeof __typeof__ __alignof__ __label__ __asm__ __thread __attribute__ __volatile__ __restrict__ __inline__ __extension__ "
// clang Objective-C/C++
"__nonnull __nullable __covariant __kindof nullable nonnull "
// Intel
"__regcall "
// Keil
"sfr sfr16 interrupt "

"NULL TRUE FALSE EOF WEOF "
"errno " "stdin stdout stderr " "signgam "
"CONST ENUM "

, // 1 Type Keyword
"__auto_type auto char double float int long short signed unsigned void "
"bool char8_t char16_t char32_t wchar_t nullptr_t nothrow_t "
"_Bool complex _Complex _Imaginary imaginary "
"__w64 __wchar_t __int8 __int16 __int32 __int64 __m64 __m128 __m128d __m128i __m256 __m256d __m256i __m512 __m512d __m512i __mmask8 __mmask16 __mmask32 __mmask64 __int3264 __ptr32 __ptr64 __sptr __uptr "
"__int128 __float80 __float128 __fp16 __complex__ __real__ __imag__ __complex128 _Decimal32 _Decimal64 _Decimal128 decimal32 decimal64 decimal128 "
"int128 qfloat "

// errno.h
"errno_t "
// fenv.h
"fenv_t femode_t fexcept_t "
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

// C++ STL class
// <limits>
"numeric_limits "
// <new>
"bad_alloc bad_array_new_length "
// <typeinfo>
"type_info bad_cast bad_typeid "
// <source_location>
"source_location "
// <contract>
//"contract_violation "
// <compare>
"partial_ordering weak_ordering strong_ordering common_comparison_category compare_three_way_result compare_three_way "
// <coroutine>
"coroutine_traits coroutine_handle noop_coroutine_promise noop_coroutine_handle suspend_never suspend_always "
// <exception>
"exception bad_exception nested_exception "
// <initializer_list>
"initializer_list "
"iterator const_iterator "
// <stdexcept>
"logic_error domain_error invalid_argument length_error out_of_range "
"runtime_error range_error overflow_error underflow_error "
// <system_error>
"error_category error_code error_condition system_error "
// <utility>
"to_chars_result from_chars_result integer_sequence pair tuple_size tuple_element "
// <tuple>
"tuple "
// <optional>
"optional nullopt_t bad_optional_access "
// <variant>
"variant variant_size variant_alternative monostate bad_variant_access "
// <any>
"any bad_any_cast "
// <bitset>
"bitset "
// <memory>
"pointer_traits rebind uses_allocator allocator_traits allocator default_delete unique_ptr "
"bad_weak_ptr shared_ptr weak_ptr owner_less enable_shared_from_this "
// <memory_resource>
"memory_resource polymorphic_allocator pool_options synchronized_pool_resource unsynchronized_pool_resource monotonic_buffer_resource "
// <scoped_allocator>
"scoped_allocator_adaptor "
// <functional>
"reference_wrapper identity is_bind_expression is_placeholder function bad_function_call "
"default_searcher boyer_moore_searcher boyer_moore_horspool_searcher "
// <type_traits>
"integral_constant bool_constant true_type false_type "
"alignment_of decay decay_t enable_if common_type common_type_t invoke_result invoke_result_t "
// <ratio>
"ratio_add ratio_subtract ratio_multiply ratio_divide ratio_equal ratio_not_equal ratio_less ratio_less_equal ratio_greater ratio_greater_equal "
// <chrono>
"time_point system_clock sys_time sys_seconds sys_days utc_clock utc_time utc_seconds leap_second_info tai_clock tai_time tai_seconds gps_clock gps_time gps_seconds file_clock file_time steady_clock high_resolution_clock local_t local_time local_seconds local_days clock_time_conversion weekday_indexed weekday_last month_day month_day_last month_weekday month_weekday_last year_month year_month_day year_month_day_last year_month_weekday year_month_weekday_last tzdb_list nonexistent_local_time ambiguous_local_time sys_info local_info time_zone zoned_time zoned_seconds "
// <typeindex>
"type_index "
// <execution>
"is_execution_policy sequenced_policy parallel_policy parallel_unsequenced_policy unsequenced_policy "
// <format>
"format_to_n_result formatter basic_format_parse_context format_parse_context wformat_parse_context basic_format_context format_context wformat_context basic_format_args format_args wformat_args format_error format_args_t "
// <string>
"char_traits basic_string string u8string u16string u32string wstring " "u8streampos u16streampos u32streampos "
// <string_view>
"basic_string_view string_view u8string_view u16string_view u32string_view wstring_view "
// <locale> <codecvt>
"locale facet ctype ctype_base ctype_byname codecvt_base codecvt codecvt_byname "
"num_get num_put numpunct numpunct_byname "
"collate collate_byname time_base time_get time_get_byname time_put time_put_byname "
"money_base money_get money_put moneypunct moneypunct_byname "
"messages_base messages_byname "
// <array> <deque> <forward_list> <list> <vector>
"array deque forward_list list vector "
// <map> <set>
"map multimap set multiset "
// <unordered_map> <unordered_set>
"hash unordered_map unordered_multimap unordered_set unordered_multiset "
// <queue> <stack> <span>
"queue priority_queue stack " "span "
// <iterator>
"incrementable_traits indirectly_readable_traits iterator_traits input_iterator_tag output_iterator_tag forward_iterator_tag bidirectional_iterator_tag random_access_iterator_tag contiguous_iterator_tag "
"reverse_iterator back_insert_iterator front_insert_iterator insert_iterator move_iterator common_iterator counted_iterator move_sentinel "
"istream_iterator ostream_iterator istreambuf_iterator ostreambuf_iterator "
// <ranges>
"view_base view_interface subrange dangling empty_view single_view iota_view all_view ref_view filter_view transform_view take_view take_while_view drop_view drop_while_view join_view split_view common_view basic_istream_view elements_view keys_view values_view "
// <algorithm>
// <random>
"linear_congruential_engine mersenne_twister_engine subtract_with_carry_engine discard_block_engine independent_bits_engine shuffle_order_engine random_device seed_seq "
"uniform_int_distribution uniform_real_distribution bernoulli_distribution binomial_distribution geometric_distribution negative_binomial_distribution poisson_distribution exponential_distribution gamma_distribution weibull_distribution extreme_value_distribution normal_distribution lognormal_distribution chi_squared_distribution cauchy_distribution fisher_f_distribution student_t_distribution discrete_distribution piecewise_constant_distribution piecewise_linear_distribution "
// <valarray>
"valarray slice slice_array gslice gslice_array mask_array indirect_array "
// <ios>
"streamoff streamsize streampos wstreampos "
"ios_base basic_ios ios wios "
// <streambuf>
"basic_streambuf streambuf wstreambuf "
// <istream>
"basic_istream istream wistream "
// <ostream>
"basic_ostream ostream wostream "
// <iostream>
"basic_iostream iostream wiostream "
// <sstream>
"basic_stringbuf stringbuf wstringbuf basic_istringstream istringstream wistringstream basic_ostringstream ostringstream wostringstream basic_stringstream stringstream wstringstream "
// <fstream>
"basic_filebuf filebuf wfilebuf basic_ifstream ifstream wifstream basic_ofstream ofstream wofstream basic_fstream fstream wfstream "
// <syncstream>
"basic_syncbuf syncbuf wsyncbuf basic_osyncstream osyncstream wosyncstream "
// <filesystem>
"filesystem_error directory_entry directory_iterator recursive_directory_iterator file_status space_info file_time_type "
// <regex>
"regex_error regex_traits basic_regex regex wregex " "locale_type regex_type "
"sub_match csub_match wcsub_match ssub_match wssub_match "
"match_results cmatch wcmatch smatch wsmatch "
"regex_iterator cregex_iterator wcregex_iterator sregex_iterator wsregex_iterator "
"regex_token_iterator cregex_token_iterator wcregex_token_iterator sregex_token_iterator wsregex_token_iterator "
// <atomic> <stop_token>
"atomic atomic_ref " "stop_token stop_source nostopstate_t stop_callback "
// <thread>
"thread jthread "
// <mutex> <shared_mutex>
"mutex recursive_mutex timed_mutex recursive_timed_mutex shared_mutex shared_timed_mutex "
"lock_guard scoped_lock unique_lock shared_lock "
// <condition_variable>
"condition_variable condition_variable_any "
// <semaphore> <latch> <barrier>
"counting_semaphore binary_semaphore latch barrier arrival_token "
// <future>
"future_error promise future shared_future packaged_task "

// common typedef
"const_reverse_iterator "
"traits_type allocator_type iterator_category container_type element_type deleter_type weak_type "
"string_type iter_type comparison_category "
"key_type mapped_type key_compare node_type insert_return_type value_compare "
"hasher key_equal local_iterator const_local_iterator "
"istream_type ostream_type streambuf_type syncbuf_type "

// old class
"rb_tree dynamic_bitset "
"raw_storage_iterator "
"array_allocator bitmap_allocator "
"iterator_type "
"hash_map hash_multimap hash_set hash_multiset hashtable "
"wstring_convert wbuffer_convert codecvt_utf8 codecvt_utf16 codecvt_utf8_utf16 " // C++14
"auto_ptr auto_ptr_ref unary_function binary_function binder1st binder2nd " // C++03
"strstreambuf istrstream ostrstream strstream " // C++98

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

// <limits>
"float_round_style float_denorm_style "
// <utility>
"chars_format "
// <memory>
"pointer_safety "
// <locale> <codecvt>
"dateorder "
"codecvt_mode " // C++14
// ios_base
"fmtflags iostate openmode seekdir "
// <filesystem>
"file_type perm_options copy_options directory_options "
// <regex>
"syntax_option_type match_flag_type error_type "
// <ranges>
"subrange_kind "
// <condition_variable>
"cv_status "
// <future>
"future_errc future_status "

, // 8 Constant
"__cplusplus __cplusplus_cli __assembler __midl "
"__FILE__ __FUNCTION__ __LINE__ __DATE__ __TIME__ __TIMESTAMP__ __func__ __VA_ARGS__ __VA_OPT__ "
"__STDC__ __STDC_HOSTED__ __STDC_VERSION__ __STDC_IEC_60559_BFP__ __STDC_IEC_559__ __STDC_IEC_60559_DFP__ __STDC_IEC_60559_COMPLEX__ __STDC_IEC_559_COMPLEX__ __STDC_ISO_10646__ "
"__COUNTER__ __STRICT_ANSI__ __PRETTY_FUNCTION__ __STDC_DEC_FP__ "
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
"FENV_ACCESS FENV_DEC_ROUND FENV_ROUND FE_DYNAMIC "
"FE_INVALID FE_DENORMAL FE_DIVBYZERO FE_DFL_MODE FE_OVERFLOW FE_UNDERFLOW FE_INEXACT FE_ALL_EXCEPT "
"FE_TONEAREST FE_DOWNWARD FE_UPWARD FE_TOWARDZERO " "FE_DFL_ENV FE_PC64_ENV FE_PC53_ENV "
"FE_DEC_DOWNWARD FE_DEC_TONEAREST FE_DEC_TONEARESTFROMZERO FE_DEC_TOWARDZERO FE_DEC_UPWARD "
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
"MAXFLOAT HUGE_VAL HUGE_VALF HUGE_VALL HUGE_VAL_D32 HUGE_VAL_D64 HUGE_VAL_D128 "
"INFINITY DEC_INFINITY NAN DEC_NAN SNANF SNAN SNANL SNAND32 SNAND64 SNAND128 "
"FP_INFINITE FP_NAN FP_NORMAL FP_SUBNORMAL FP_ZERO FP_INT_UPWARD FP_INT_DOWNWARD FP_INT_TOWARDZERO FP_INT_TONEARESTFROMZERO FP_INT_TONEAREST " 
"FP_FAST_FMA FP_FAST_FMAF FP_FAST_FMAL FP_FAST_FMAD32 FP_FAST_FMAD64 FP_FAST_FMAD128 "
"FP_ILOGB0 FP_ILOGBNAN FP_LLOGB0 FP_LLOGBNAN " "MATH_ERRNO MATH_ERREXCEPT math_errhandling "
"fpclassify() iscanonical() isfinite() isinf() isnan() isnormal() signbit() issignaling() issubnormal() iszero() "
"isgreater() isgreaterequal() isless() islessequal() islessgreater() isunordered() iseqsig() "
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

// <version>
"^__cpp_lib_ " // 85+

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
// <new>
"new_handler align_val_t "
"promise_type " // <coroutine>
// <exception>
"terminate_handler exception_ptr "
// <initializer_list>
"value_type reference const_reference size_type "
// <memory>
"pointer const_pointer void_pointer const_void_pointer element_type difference_type "
// <functional>
"result_type "
// <string>
"char_type int_type off_type pos_type state_type "
"index_type " // <span>
// <regex>
"char_class_type flag_type "
// <thread>
"native_handle_type "

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
"feclearexcept() fegetexceptflag() feraiseexcept() fesetexcept() fesetexceptflag() fetestexceptflag() fetestexcept() "
"fegetmode() fegetround() fe_dec_getround() fesetmode() fesetround() fe_dec_setround() "
"fegetenv() feholdexcept() fesetenv() feupdateenv() "
// inttypes.h
"imaxabs() imaxdiv() strtoimax() strtoumax() wcstoimax() wcstoumax() "
// locale.h
"setlocale() localeconv() "
// math.h
"acos() asin() atan() atan2() cos() sin() tan() acospi() asinpi() atanpi() atan2pi() cospi() sinpi() tanpi() "
"acosh() asinh() atanh() cosh() sinh() tanh() "
"exp() exp10() exp10m1() exp2() exp2m1() expm1() frexp() ilogb() ldexp() llogb() "
"log() log10() log10p1() log1p() logp1() log2() log2p1() logb() modf() scalbn() scalbln() "
"cbrt() compoundn() fabs() hypot() pow() pown() powr() rootn() rsqrt() sqrt() erf() erfc() lgamma() tgamma() "
"ceil() floor() nearbyint() rint() lint() llint() round() lround() llround() roundeven() trunc() fromfp() ufromfp() fromfpx() ufromfpx() "
"fmod() remainder() remquo() copysign() nan() nextafter() nexttoward() nextup() nextdown() canonicalize() "
"fdim() fmax() fmin() fmaxmag() fminmag() fma() "
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
"strchr() strcspn() strpbrk() strrchr() strspn() strstr() strtok() " "strerror() strlen() strdup() strndup() "
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
"to_integer() "
// <limits>
"min() max() epsilon() round_error() infinity() quiet_NaN() signaling_NaN() denorm_min() " // numeric_limits
// <new>
"get_new_handler() set_new_handler() launder() "
// <typeinfo>
"before() hash_code() name() " // type_info
// <source_location>
"current() line() column() file_name() function_name() "
// <contract>
//"line_number() comment() assertion_level() "
// <compare>
"is_eq() is_neq() is_lt() is_lteq() is_gt() is_gteq() " "strong_order() weak_order() partial_order() strong_equal() weak_equal() "
"strong_order() weak_order() partial_order() compare_strong_order_fallback() compare_weak_order_fallback() compare_partial_order_fallback() "
// <coroutine>
"address() from_address() done() resume() from_promise() promise() noop_coroutine() await_ready() await_suspend() await_resume() "
// <exception>
"what() " // exception
"rethrow_nested() nested_ptr() " // nested_exception
"get_terminate() set_terminate() terminate() "
"uncaught_exceptions() current_exception() rethrow_exception() make_exception_ptr() throw_with_nested() rethrow_if_nested() "
// <initializer_list>
"size() begin() end() " // initializer_list
// <system_error>
"default_error_condition() equivalent() message() " // error_category
"generic_category() system_category() make_error_code() make_error_condition() "
"assign() clear() value() " // error_code
"code() " // system_error
// <utility>
"swap() exchange() forward() move() move_if_noexcept() as_const() declval() to_chars() from_chars() "
"cmp_equal() cmp_not_equal() cmp_less() cmp_greater() cmp_less_equal() cmp_greater_equal() in_range() "
"make_pair() get() " // pair
// <tuple>
"make_tuple() forward_as_tuple() tie() tuple_cat() apply() make_from_tuple() "
// <optional>
"emplace() has_value() value_or() reset() " // optional
"make_optional() "
// <variant>
"valueless_by_exception() index() " // variant
"holds_alternative() get_if() visit() "
// <any>
"type() " // any
"make_any() any_cast() "
// <bitset>
"flip() to_ulong() to_ullong() to_string() count() test() all() any() none() "
// <memory>
"pointer_to() " // pointer_traits
"to_address() get_pointer_safety() align() assume_aligned() "
"allocate() deallocate() construct() destroy() max_size() " // allocator_traits
"addressof() construct_at() destroy_at() destroy() destroy_n() "
"get_deleter() release() " // unique_ptr
"make_unique() make_unique_for_overwrite() "
"use_count() owner_before() " // shared_ptr
"make_shared() allocate_shared() make_shared_for_overwrite() allocate_shared_for_overwrite() static_pointer_cast() dynamic_pointer_cast() const_pointer_cast() reinterpret_pointer_cast() "
"expired() lock() " // weak_ptr
"shared_from_this() weak_from_this() "
// <memory_resource>
"is_equal() " // memory_resource
"allocate_bytes() deallocate_bytes() allocate_object() deallocate_object() new_object() delete_object() resource() " // polymorphic_allocator
"new_delete_resource() null_memory_resource() set_default_resource() get_default_resource() "
"upstream_resource() options() " // synchronized_pool_resource
// <scoped_allocator>
"inner_allocator() outer_allocator() "
// <functional>
"invoke() ref() cref() not_fn() bind_front() bind() mem_fn() "
"target_type() target() " // function
// <chrono>
"zero() duration_cast() time_point_cast() time_since_epoch() to_stream() from_stream() get_leap_second_info() "
"now() to_time_t() from_time_t() to_sys() from_sys() to_utc() from_utc() clock_cast() to_duration() is_am() is_pm() make24() make12() "
"locate_zone() current_zone() get_tzdb() get_tzdb_list() reload_tzdb() remote_version() get_info() to_local() "
"get_time_zone() get_local_time() get_sys_time() date() parse() "
// <format>
"format() vformat() format_to() vformat_to() format_to_n() formatted_size() advance_to() next_arg_id() check_arg_id() out() visit_format_arg() make_format_args() make_wformat_args() "
// <string>
"eq() lt() compare() length() find() copy() not_eof() to_char_type() to_int_type() eq_int_type() eof() " // char_traits
"getline() erase() erase_if() stoi() stol() stoul() stoll() stoull() stof() stod() stold() to_wstring() "
"rbegin() rend() cbegin() cend() crbegin() crend() " // basic_string iterator
"resize() capacity() reserve() shrink_to_fit() clear() empty() at() front() back() append() push_back() assign() insert() pop_back() replace() c_str() data() rfind() find_first_of() find_last_of() find_first_not_of() find_last_not_of() substr() starts_with() ends_with() " // basic_string
// <string_view>
"remove_prefix() remove_suffix() "
// <locale> <codecvt>
"global() classic() " // locale
"use_facet() has_facet() "
"is() scan_is() scan_not() widen() narrow() " // ctype
"unshift() in() encoding() always_noconv() max_length() " // codecvt
"put() " // num_put
"decimal_point() thousands_sep() grouping() truename() falsename() " // numpunct
"transform() " // collate
"date_order() get_time() get_date() get_weekday() get_monthname() get_year() "
"curr_symbol() positive_sign() negative_sign() frac_digits() pos_format() neg_format() " // moneypunct
"open() close() " // messages
// <array> <deque> <forward_list> <list> <vector>
"fill() to_array() " // array
"get_allocator() emplace_front() emplace_back() push_front() pop_front() " // deque
"before_begin() cbefore_begin() emplace_after() insert_after() erase_after() splice_after() remove() remove_if() unique() merge() sort() reverse() " // forward_list
"splice() " // list
// <map> <set>
"emplace_hint() extract() try_emplace() insert_or_assign() key_comp() value_comp() contains() lower_bound() upper_bound() equal_range() " // map
// <unordered_map> <unordered_set>
"hash_function() key_eq() bucket_count() max_bucket_count() bucket_size() bucket() load_factor() max_load_factor() rehash() " // unordered_map
// <queue> <stack> <span>
"push() pop() top() "
"size_bytes() as_bytes() as_writable_bytes() "
// <iterator>
"advance() distance() next() prev() base() iter_move() iter_swap() "
"make_reverse_iterator() back_inserter() front_inserter() inserter() make_move_iterator() "
// <ranges>
"single() filter() take() take_while() drop() drop_while() split() counted() common() istream_view() elements() keys() values() "
// <algorithm>
"all_of() any_of() none_of() for_each() for_each_n() find_if() find_if_not() find_end() adjacent_find() "
"count_if() mismatch() equal() is_permutation() search() search_n() "
"copy_n() copy_if() copy_backward() move_backward() swap_ranges() replace_if() replace_copy() replace_copy_if() fill_n() "
"generate() generate_n() remove_copy() remove_copy_if() unique_copy() reverse_copy() rotate() rotate_copy() sample() shuffle() shift_left() shift_right() "
"is_partitioned() partition() stable_partition() partition_copy() partition_point() " // partitions
"stable_sort() partial_sort() partial_sort_copy() is_sorted() is_sorted_until() nth_element() " // sorting and related operations
"binary_search() merge() inplace_merge() includes() "
"set_union() set_intersection() set_difference() set_symmetric_difference() "
"push_heap() pop_heap() make_heap() sort_heap() is_heap() is_heap_until() "
"minmax() min_element() max_element() minmax_element() clamp() lexicographical_compare() compare_3way() lexicographical_compare_three_way() next_permutation() prev_permutation() "
// <complex>
"real() imag() arg() norm() proj() polar() "
// <random>
"entropy() " // random_device
"param() " // seed_seq
"generate_canonical() "
"mean() "// poisson_distribution
"lambda() " // exponential_distribution
"alpha() beta() " // gamma_distribution
"stddev() " // normal_distribution
"probabilities() " // discrete_distribution
"intervals() densities() " // piecewise_constant_distribution
// <valarray>
"shift() cshift() start() " // valarray
"stride() " // slice
// <numeric>
"accumulate() reduce() inner_product() transform_reduce() partial_sum() exclusive_scan() inclusive_scan() transform_exclusive_scan() transform_inclusive_scan() adjacent_difference() iota() gcd() lcm() midpoint() "
// <bit>
"bit_cast() has_single_bit() bit_ceil() bit_floor() bit_width() rotl() rotr() countl_zero() countl_one() countr_zero() countr_one() popcount() "
// <cmath>
"lerp() assoc_laguerre() assoc_legendre() comp_ellint_1() comp_ellint_2() comp_ellint_3() cyl_bessel_i() cyl_bessel_j() cyl_bessel_k() cyl_neumann() ellint_1() ellint_2() ellint_3() expint() hermite() laguerre() legendre() riemann_zeta() sph_bessel() sph_legendre() sph_neumann() "
// Input/output library
"state() " "flags() setf() unsetf() precision() width() imbue() getloc() xalloc() iword() pword() register_callback() sync_with_stdio() " // ios_base
"rdstate() setstate() good() fail() bad() exceptions() rdbuf() copyfmt() " // basic_ios
"flush() tellp() seekp() " // basic_ostream
"resetiosflags() setiosflags() setbase() setfill() setprecision() setw() get_money() put_money() put_time() quoted() "
"str() is_open() emit() get_wrapped() set_emit_on_sync() "
// <filesystem>
"concat() make_preferred() remove_filename() replace_filename() replace_extension() native() "
"root_name() root_directory() root_path() relative_path() parent_path() filename() stem() extension() "
"has_root_name() has_root_directory() has_root_path() has_relative_path() has_parent_path() has_filename() has_stem() has_extension() "
"is_absolute() is_relative() lexically_normal() lexically_relative() lexically_proximate() " // path
"generic_string() generic_wstring() generic_u8string() generic_u16string() generic_u32string() hash_value() "
"refresh() exists() is_block_file() is_character_file() is_directory() is_fifo() is_other() is_regular_file() is_socket() is_symlink() "
"file_size() hard_link_count() last_write_time() status() symlink_status() " // directory_entry
"increment() depth() recursion_pending() disable_recursion_pending() " // recursive_directory_iterator
"type() permissions() " // file_status
"absolute() canonical() copy_file() copy_symlink() create_directories() create_directory() create_directory_symlink() create_hard_link() create_symlink() current_path() "
"equivalent() proximate() read_symlink() relative() remove_all() resize_file() space() status_known() temp_directory_path() weakly_canonical() "
// <regex>
"translate() translate_nocase() transform_primary() lookup_collatename() lookup_classname() isctype() getloc() "
"mark_count() ready() position() prefix() suffix() "
"regex_match() regex_search() regex_replace() "
// <atomic>
"is_lock_free() store() load() compare_exchange_weak() compare_exchange_strong() "
"fetch_add() fetch_sub() fetch_and() fetch_or() fetch_xor() test_and_set() "
// <stop_token>
"stop_requested() stop_possible() get_token() request_stop() "
// <thread>
"decay_copy() get_id() yield() sleep_until() sleep_for() "
"joinable() join() detach() native_handle() hardware_concurrency() get_stop_source() get_stop_token() "
// <mutex>
"try_lock() unlock() try_lock_for() try_lock_until() lock_shared() try_lock_shared() unlock_shared() try_lock_shared_for() try_lock_shared_until() owns_lock() "
// <condition_variable>
"notify_all_at_thread_exit() notify_one() notify_all() wait() wait_until() wait_for() "
// <semaphore> <latch> <barrier>
"acquire() try_acquire() try_acquire_for() try_acquire_until() count_down() try_wait() arrive_and_wait() arrive() arrive_and_drop() "
// <future>
"future_category() get_future() set_value() set_exception() set_value_at_thread_exit() set_exception_at_thread_exit() "
"share() valid() make_ready_at_thread_exit() async() "

// old algorithm
"random_shuffle() "
// heap
"get_temporary_buffer() return_temporary_buffer() "

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

// C++
"pmr ranges execution chrono numbers filesystem this_thread "  // namespace
"nothrow " // <new>
"equivalent nonequivalent equal nonequal less greater unordered " // <compare>
"fmt " // <utility>
"scientific fixed hex general " // chars_format
"first second " // pair
"ignore " // <tuple>
"nullopt " // <optional>
"variant_npos " // <variant>
// <functional>
"equal_to not_equal_to greater less greater_equal less_equal " // ranges
// <type_traits>
"rank extent "
// <ratio>
"ratio num den "
"seq par par_unseq unseq " // <execution>
// <chrono>
"duration rep period clock day month year weekday choose leap link tzdb "
"nanoseconds microseconds milliseconds seconds minutes hours days weeks years months last Sunday Monday Tuesday Wednesday Thursday Friday Saturday January February March April May June July August September October November December "
"npos " // <string>
"default_sentinel unreachable_sentinel " //  <iterator>
// <locale> <codecvt>
"id category messages "
// <random>
"minstd_rand0 minstd_rand mt19937 mt19937_64 ranlux24_base ranlux48_base ranlux24 ranlux48 knuth_b default_random_engine "
// Input/output library
"boolalpha noboolalpha showbase noshowbase showpoint noshowpoint showpos noshowpos skipws noskipws uppercase nouppercase unitbuf nounitbuf internal left right dec hex oct fixed scientific hexfloat defaultfloat "
"cin cout cerr clog wcin wcout wcerr wclog " "endl ends flush emit_on_flush noemit_on_flush flush_emit "
// <filesystem>
"path perms "
// <regex>
"regex_constants "
// <thread> <mutex>
"defer_lock try_to_lock adopt_lock nostopstate "
"launch async deferred ready timeout "
}};

static EDITSTYLE Styles_CPP[] = {
	EDITSTYLE_DEFAULT,
	{ MULTI_STYLE(SCE_C_WORD, SCE_C_2NDWORD, 0, 0), NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2StyleX_TypeKeyword, L"fore:#0000FF" },
	{ SCE_C_PREPROCESSOR, NP2StyleX_Preprocessor, L"fore:#FF8000" },
	{ SCE_C_ATTRIBUTE, NP2StyleX_Attribute, L"fore:#FF8000" },
	{ SCE_C_DIRECTIVE, NP2StyleX_Directive, L"fore:#0000FF" },
	{ SCE_C_2NDWORD2, NP2StyleX_Type, L"fore:#007F7F" },
	{ SCE_C_CLASS, NP2StyleX_Class, L"fore:#0080FF" },
	{ SCE_C_STRUCT, NP2StyleX_Structure, L"fore:#0080FF" },
	{ SCE_C_UNION, NP2StyleX_Union, L"fore:#0080FF" },
	{ SCE_C_INTERFACE, NP2StyleX_Interface, L"bold; fore:#1E90FF" },
	{ SCE_C_FUNCTION, NP2StyleX_Function, L"fore:#A46000" },
	{ SCE_C_ENUMERATION, NP2StyleX_Enumeration, L"fore:#FF8000" },
	{ SCE_C_CONSTANT, NP2StyleX_Constant, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_MACRO2, SCE_C_MACRO, 0, 0), NP2StyleX_Macro, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2StyleX_DocCommentTag, L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2StyleX_DocComment, L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_STRINGRAW, NP2StyleX_RawString, L"fore:#E24000" },
	{ SCE_C_LABEL, NP2StyleX_Label, L"back:#FFC040" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	{ SCE_C_ASM_INSTRUCTION, NP2StyleX_Instruction, L"fore:#0080FF" },
	{ SCE_C_ASM_REGISTER, NP2StyleX_Register, L"fore:#FF8000" },
};

EDITLEXER lexCPP = {
	SCLEX_CPP, NP2LEX_CPP,
	EDITLEXER_HOLE(L"C/C++ Source", Styles_CPP),
	L"c; cpp; cxx; cc; h; hpp; cppm; hxx; hh; inl; pch; idl; odl; midl; mm; xpm; cl; cu",
	&Keywords_CPP,
	Styles_CPP
};
