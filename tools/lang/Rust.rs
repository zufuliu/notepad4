// https://www.rust-lang.org/
// https://doc.rust-lang.org/book/
// https://doc.rust-lang.org/reference/index.html
// Rust 1.38 https://doc.rust-lang.org/std/index.html

//! Keywords		===========================================================
// https://doc.rust-lang.org/std/index.html#keywords
// https://doc.rust-lang.org/reference/keywords.html
as async await
break
const continue crate
dyn
else enum extern
fn for
if impl in
let loop
match mod move mut
pub
ref return
static struct
trait type
union unsafe use
where while

false self Self super true

//! Reserved Keywords	=======================================================
// https://doc.rust-lang.org/reference/keywords.html#reserved-keywords
abstract
become box
do
final
macro
override
priv
typeof try
unsized
virtual
yield

//! Primitive Types		=======================================================
// https://doc.rust-lang.org/std/index.html#primitives
array
bool
char
f32 f64
i8 i16 i32 i64 i128 isize
pointer
reference
slice str
tuple
u8 u16 u32 u64 u128 unit usize
never

//! Macros			===========================================================
// https://doc.rust-lang.org/reference/macros.html
// https://doc.rust-lang.org/std/index.html#macros
macro_rules!
assert!()
assert_eq!()
assert_ne!()
cfg!()
column!()
compile_error!()
concat!()
dbg!()
debug_assert!()
debug_assert_eq!()
debug_assert_ne!()
env!()
eprint!()
eprintln!()
file!()
format!()
format_args!()
include!()
include_bytes!()
include_str!()
is_x86_feature_detected!()
line!()
module_path!()
option_env!()
panic!()
print!()
println!()
stringify!()
thread_local!()
unimplemented!()
unreachable!()
vec!()
write!()
writeln!()

//! Attribute		===========================================================
// https://doc.rust-lang.org/reference/attributes.html#built-in-attributes-index
// Conditional compilation
#[cfg(target_arch, target_feature, target_os, target_family, unix, windows, target_env, target_endian, target_pointer_width, target_vendor, test, debug_assertions, proc_macro)]
#[cfg(all(), any(), not())]
#[cfg_attr()]
// Testing
#[test]
#[ignore]
#[should_panic(expected)]
// Derive
#[derive()]
// Macros
#[macro_export]
#[macro_export(local_inner_macros)]
#[macro_use]
#[macro_use(lazy_static)]
#[proc_macro]
#[proc_macro_derive()]
#[proc_macro_attribute]
// Diagnostics
#[allow()]
#[warn()]
#[deny()]
#[forbid()]
#[deprecated]
#[deprecated()]
#[must_use]
// ABI, linking, symbols, and FFI
#[link(name, kind)]
#[link_name]
#[no_link]
#[repr(C, align(), packed())]
#![crate_type]
#![no_main]
#[export_name]
#[link_section]
#[no_mangle]
#[used]
#![crate_name]
// Code generation
#[inline]
#[inline(always, never)]
#[cold]
#[no_builtins]
#[target_feature(enable)]
// Preludes
#![no_std]
#![no_implicit_prelude]
// Modules
#[path]
// Limits
#![recursion_limit]
#![type_length_limit]
// Runtime
#[panic_handler]
#[global_allocator]
#![windows_subsystem]

//! Modules			===========================================================
// https://doc.rust-lang.org/std/index.html#modules

mod std::alloc {
	struct Layout {
		fn from_size_align(size: usize, align: usize) -> Result<Layout, LayoutErr>
		const unsafe fn from_size_align_unchecked(size: usize, align: usize) -> Layout
		fn size(&self) -> usize
		fn align(&self) -> usize
		fn new<T>() -> Layout
		fn for_value<T>(t: &T) -> Layout
	}
	struct LayoutErr;
	struct System;
	unsafe trait GlobalAlloc {
		unsafe fn alloc(&self, layout: Layout) -> *mut u8
		unsafe fn dealloc(&self, ptr: *mut u8, layout: Layout)
		unsafe fn alloc_zeroed(&self, layout: Layout) -> *mut u8
		unsafe fn realloc(&self, ptr: *mut u8, layout: Layout, new_size: usize) -> *mut u8
	}
	unsafe fn alloc(layout: Layout) -> *mut u8
	unsafe fn alloc_zeroed(layout: Layout) -> *mut u8
	unsafe fn dealloc(ptr: *mut u8, layout: Layout)
	fn handle_alloc_error(layout: Layout) -> !
	unsafe fn realloc(ptr: *mut u8, layout: Layout, new_size: usize) -> *mut u8
}

mod std::any {
	struct TypeId {
		fn of<T>() -> TypeId
	}
	trait Any: 'static {
		fn type_id(&self) -> TypeId
		fn is<T>(&self) -> bool
		fn downcast_ref<T>(&self) -> Option<&T>
		fn downcast_mut<T>(&mut self) -> Option<&mut T>
	}
}

mod std::array {
	struct TryFromSliceError;
}

mod std::ascii {
	struct EscapeDefault {}
	fn escape_default(c: u8) -> EscapeDefault
}

mod std::borrow {
	enum Cow<'a, B> {
		Borrowed(&'a B),
		Owned(<B as ToOwned>::Owned),
		fn to_mut(&mut self) -> &mut <B as ToOwned>::Owned
		fn into_owned(self) -> <B as ToOwned>::Owned
	}
	trait Borrow<Borrowed> {
		fn borrow(&self) -> &Borrowed
	}
	trait BorrowMut<Borrowed>: Borrow<Borrowed> {
		fn borrow_mut(&mut self) -> &mut Borrowed
	}
	trait ToOwned {
		type Owned: Borrow<Self>;
		fn to_owned(&self) -> Self::Owned
	}
}

mod std::boxed {
	struct Box<T> {
		fn new(x: T) -> Box<T>
		fn pin(x: T) -> Pin<Box<T>>
		unsafe fn from_raw(raw: *mut T) -> Box<T>
		fn into_raw(b: Box<T>) -> *mut T
		fn leak<'a>(b: Box<T>) -> &'a mut T
		fn downcast<T>(self) -> Result<Box<T>, Box<dyn Any + 'static>>
	}
}

mod std::cell {
	struct BorrowError;
	struct BorrowMutError
	struct Cell<T> {
		fn get(&self) -> T
		const fn new(value: T) -> Cell<T>
		fn set(&self, val: T)
		fn swap(&self, other: &Cell<T>)
		fn replace(&self, val: T) -> T
		fn into_inner(self) -> T
		const fn as_ptr(&self) -> *mut T
		fn get_mut(&mut self) -> &mut T
		fn from_mut(t: &mut T) -> &Cell<T>
		fn take(&self) -> T
		fn as_slice_of_cells(&self) -> &[Cell<T>]
	}
	struct Ref<'b, T> {
		fn clone(orig: &Ref<'b, T>) -> Ref<'b, T>
		fn map<U, F>(orig: Ref<'b, T>, f: F) -> Ref<'b, U>
		fn map_split<U, V, F>(orig: Ref<'b, T>, f: F) -> (Ref<'b, U>, Ref<'b, V>)
	}
	struct RefCell<T> {
		const fn new(value: T) -> RefCell<T>
		fn into_inner(self) -> T
		fn replace(&self, t: T) -> T
		fn replace_with<F>(&self, f: F) -> T
		fn swap(&self, other: &RefCell<T>)
		fn borrow(&self) -> Ref<T>
		fn try_borrow(&self) -> Result<Ref<T>, BorrowError>
		fn borrow_mut(&self) -> RefMut<T>
		fn try_borrow_mut(&self) -> Result<RefMut<T>, BorrowMutError>
		fn as_ptr(&self) -> *mut T
		fn get_mut(&mut self) -> &mut T
		unsafe fn try_borrow_unguarded(&self) -> Result<&T, BorrowError>
	}
	struct RefMut<'b, T> {
		fn map<U, F>(orig: RefMut<'b, T>, f: F) -> RefMut<'b, U>
		fn map_split<U, V, F>(orig: RefMut<'b, T>, f: F) -> (RefMut<'b, U>, RefMut<'b, V>)
	}
	struct UnsafeCell<T> {
		const fn new(value: T) -> UnsafeCell<T>
		fn into_inner(self) -> T
		const fn get(&self) -> *mut T
	}
}

mod std::char {
	// Primitive Type
	fn is_digit(self, radix: u32) -> bool
	fn to_digit(self, radix: u32) -> Option<u32>
	fn escape_unicode(self) -> EscapeUnicode
	fn escape_debug(self) -> EscapeDebug
	fn escape_default(self) -> EscapeDefault
	fn len_utf8(self) -> usize
	fn len_utf16(self) -> usize
	fn encode_utf8(self, dst: &mut [u8]) -> &mut str
	fn encode_utf16(self, dst: &mut [u16]) -> &mut [u16]
	fn is_alphabetic(self) -> bool
	fn is_lowercase(self) -> bool
	fn is_uppercase(self) -> bool
	fn is_whitespace(self) -> bool
	fn is_alphanumeric(self) -> bool
	fn is_control(self) -> bool
	fn is_numeric(self) -> bool
	fn to_lowercase(self) -> ToLowercase
	fn to_uppercase(self) -> ToUppercase
	const fn is_ascii(&self) -> bool
	fn to_ascii_uppercase(&self) -> char
	fn to_ascii_lowercase(&self) -> char
	fn eq_ignore_ascii_case(&self, other: &char) -> bool
	fn make_ascii_uppercase(&mut self)
	fn make_ascii_lowercase(&mut self)
	fn is_ascii_alphabetic(&self) -> bool
	fn is_ascii_uppercase(&self) -> bool
	fn is_ascii_lowercase(&self) -> bool
	fn is_ascii_alphanumeric(&self) -> bool
	fn is_ascii_digit(&self) -> bool
	fn is_ascii_hexdigit(&self) -> bool
	fn is_ascii_punctuation(&self) -> bool
	fn is_ascii_graphic(&self) -> bool
	fn is_ascii_whitespace(&self) -> bool
	fn is_ascii_control(&self) -> bool

	// Module
	struct CharTryFromError;
	struct DecodeUtf16<I> {}
	struct DecodeUtf16Error {
		fn unpaired_surrogate(&self) -> u16
	}
	struct EscapeDebug;
	struct EscapeDefault;
	struct EscapeUnicode;
	struct ParseCharError;
	struct ToLowercase;
	struct ToUppercase;
	const MAX: char
	const REPLACEMENT_CHARACTER: char
	fn decode_utf16<I>(iter: I) -> DecodeUtf16<<I as IntoIterator>::IntoIter>
	fn from_digit(num: u32, radix: u32) -> Option<char>
	fn from_u32(i: u32) -> Option<char>
	unsafe fn from_u32_unchecked(i: u32) -> char
}

mod std::clone {
	trait Clone {
		fn clone(&self) -> Self
		fn clone_from(&mut self, source: &Self)
	}
}

mod std::cmp {
	struct Reverse<T>(pub T);
	enum Ordering {
		Less,
		Equal,
		Greater,
		fn reverse(self) -> Ordering
		fn then(self, other: Ordering) -> Ordering
		fn then_with<F>(self, f: F) -> Ordering
	}
	trait Eq: PartialEq<Self> {}
	trait Ord: Eq + PartialOrd<Self> {
		fn cmp(&self, other: &Self) -> Ordering
		fn max(self, other: Self) -> Self
		fn min(self, other: Self) -> Self
	}
	trait PartialEq<Rhs = Self> {
		fn eq(&self, other: &Rhs) -> bool
		fn ne(&self, other: &Rhs) -> bool
	}
	trait PartialOrd<Rhs = Self>: PartialEq<Rhs> {
		fn partial_cmp(&self, other: &Rhs) -> Option<Ordering>
		fn lt(&self, other: &Rhs) -> bool
		fn le(&self, other: &Rhs) -> bool
		fn gt(&self, other: &Rhs) -> bool
		fn ge(&self, other: &Rhs) -> bool
	}
	fn max<T>(v1: T, v2: T) -> T
	fn min<T>(v1: T, v2: T) -> T
}

mod std::collections {
	struct BTreeMap<K, V> {
		fn new() -> BTreeMap<K, V>
		fn clear(&mut self)
		fn get<Q>(&self, key: &Q) -> Option<&V>
		fn contains_key<Q>(&self, key: &Q) -> bool
		fn get_mut<Q>(&mut self, key: &Q) -> Option<&mut V>
		fn insert(&mut self, key: K, value: V) -> Option<V>
		fn remove<Q>(&mut self, key: &Q) -> Option<V>
		fn append(&mut self, other: &mut BTreeMap<K, V>)
		fn range<T, R>(&self, range: R) -> Range<K, V>
		fn range_mut<T, R>(&mut self, range: R) -> RangeMut<K, V>
		fn entry(&mut self, key: K) -> Entry<K, V>
		fn split_off<Q>(&mut self, key: &Q) -> BTreeMap<K, V>
		fn iter(&self) -> Iter<K, V>
		fn iter_mut(&mut self) -> IterMut<K, V>
		fn keys(&'a self) -> Keys<'a, K, V>
		fn values(&'a self) -> Values<'a, K, V>
		fn values_mut(&mut self) -> ValuesMut<K, V>
		fn len(&self) -> usize
		fn is_empty(&self) -> bool
	}
	struct BTreeSet<T> {
		fn new() -> BTreeSet<T>
		fn range<K, R>(&self, range: R) -> Range<T>
		fn difference(&'a self, other: &'a BTreeSet<T>) -> Difference<'a, T>
		fn symmetric_difference(&'a self, other: &'a BTreeSet<T>) -> SymmetricDifference<'a, T>
		fn intersection(&'a self, other: &'a BTreeSet<T>) -> Intersection<'a, T>
		fn union(&'a self, other: &'a BTreeSet<T>) -> Union<'a, T>
		fn clear(&mut self)
		fn contains<Q>(&self, value: &Q) -> bool
		fn get<Q>(&self, value: &Q) -> Option<&T>
		fn is_disjoint(&self, other: &BTreeSet<T>) -> bool
		fn is_subset(&self, other: &BTreeSet<T>) -> bool
		fn is_superset(&self, other: &BTreeSet<T>) -> bool
		fn insert(&mut self, value: T) -> bool
		fn replace(&mut self, value: T) -> Option<T>
		fn remove<Q>(&mut self, value: &Q) -> bool
		fn take<Q>(&mut self, value: &Q) -> Option<T>
		fn append(&mut self, other: &mut BTreeSet<T>)
		fn split_off<Q>(&mut self, key: &Q) -> BTreeSet<T>
		fn iter(&self) -> Iter<T>
		fn len(&self) -> usize
		fn is_empty(&self) -> bool
	}
	struct BinaryHeap<T> {
		fn new() -> BinaryHeap<T>
		fn with_capacity(capacity: usize) -> BinaryHeap<T>
		fn peek_mut(&mut self) -> Option<PeekMut<T>>
		fn pop(&mut self) -> Option<T>
		fn push(&mut self, item: T)
		fn into_sorted_vec(self) -> Vec<T>
		fn append(&mut self, other: &mut BinaryHeap<T>)
		fn iter(&self) -> Iter<T>
		fn peek(&self) -> Option<&T>
		fn capacity(&self) -> usize
		fn reserve_exact(&mut self, additional: usize)
		fn reserve(&mut self, additional: usize)
		fn shrink_to_fit(&mut self)
		fn into_vec(self) -> Vec<T>
		fn len(&self) -> usize
		fn is_empty(&self) -> bool
		fn drain(&mut self) -> Drain<T>
		fn clear(&mut self)
	}
	struct HashMap<K, V, S = RandomState> {
		fn new() -> HashMap<K, V, RandomState>
		fn with_capacity(capacity: usize) -> HashMap<K, V, RandomState>
		fn capacity(&self) -> usize
		fn keys(&self) -> Keys<K, V>
		fn values(&self) -> Values<K, V>
		fn values_mut(&mut self) -> ValuesMut<K, V>
		fn iter(&self) -> Iter<K, V>
		fn iter_mut(&mut self) -> IterMut<K, V>
		fn len(&self) -> usize
		fn is_empty(&self) -> bool
		fn drain(&mut self) -> Drain<K, V>
		fn clear(&mut self)
		fn with_hasher(hash_builder: S) -> HashMap<K, V, S>
		fn with_capacity_and_hasher(capacity: usize, hash_builder: S) -> HashMap<K, V, S>
		fn hasher(&self) -> &S
		fn reserve(&mut self, additional: usize)
		fn shrink_to_fit(&mut self)
		fn entry(&mut self, key: K) -> Entry<K, V>
		fn get<Q: ?Sized>(&self, k: &Q) -> Option<&V>
		fn contains_key<Q: ?Sized>(&self, k: &Q) -> bool
		fn get_mut<Q: ?Sized>(&mut self, k: &Q) -> Option<&mut V>
		fn insert(&mut self, k: K, v: V) -> Option<V>
		fn remove<Q: ?Sized>(&mut self, k: &Q) -> Option<V>
		fn remove_entry<Q: ?Sized>(&mut self, k: &Q) -> Option<(K, V)>
		fn retain<F>(&mut self, f: F)
	}
	struct HashSet<T, S = RandomState> {
		fn new() -> HashSet<T, RandomState>
		fn with_capacity(capacity: usize) -> HashSet<T, RandomState>
		fn capacity(&self) -> usize
		fn iter(&self) -> Iter<T>
		fn len(&self) -> usize
		fn is_empty(&self) -> bool
		fn drain(&mut self) -> Drain<T>
		fn clear(&mut self)
		fn with_hasher(hasher: S) -> HashSet<T, S>
		fn with_capacity_and_hasher(capacity: usize, hasher: S) -> HashSet<T, S>
		fn hasher(&self) -> &S
		fn reserve(&mut self, additional: usize)
		fn shrink_to_fit(&mut self)
		fn difference<'a>(&'a self, other: &'a HashSet<T, S>) -> Difference<'a, T, S>
		fn symmetric_difference<'a>(&'a self, other: &'a HashSet<T, S>) -> SymmetricDifference<'a, T, S>
		fn intersection<'a>(&'a self, other: &'a HashSet<T, S>) -> Intersection<'a, T, S>
		fn union<'a>(&'a self, other: &'a HashSet<T, S>) -> Union<'a, T, S>
		fn contains<Q: ?Sized>(&self, value: &Q) -> bool
		fn get<Q: ?Sized>(&self, value: &Q) -> Option<&T>
		fn is_disjoint(&self, other: &HashSet<T, S>) -> bool
		fn is_subset(&self, other: &HashSet<T, S>) -> bool
		fn is_superset(&self, other: &HashSet<T, S>) -> bool
		fn insert(&mut self, value: T) -> bool
		fn replace(&mut self, value: T) -> Option<T>
		fn remove<Q: ?Sized>(&mut self, value: &Q) -> bool
		fn take<Q: ?Sized>(&mut self, value: &Q) -> Option<T>
		fn retain<F>(&mut self, f: F)
	}
	struct LinkedList<T> {
		fn new() -> LinkedList<T>
		fn append(&mut self, other: &mut LinkedList<T>)
		fn iter(&self) -> Iter<T>
		fn iter_mut(&mut self) -> IterMut<T>
		fn is_empty(&self) -> bool
		fn len(&self) -> usize
		fn clear(&mut self)
		fn contains(&self, x: &T) -> bool
		fn front(&self) -> Option<&T>
		fn front_mut(&mut self) -> Option<&mut T>
		fn back(&self) -> Option<&T>
		fn back_mut(&mut self) -> Option<&mut T>
		fn push_front(&mut self, elt: T)
		fn pop_front(&mut self) -> Option<T>
		fn push_back(&mut self, elt: T)
		fn pop_back(&mut self) -> Option<T>
		fn split_off(&mut self, at: usize) -> LinkedList<T>
	}
	struct VecDeque<T> {
		fn new() -> VecDeque<T>
		fn with_capacity(capacity: usize) -> VecDeque<T>
		fn get(&self, index: usize) -> Option<&T>
		fn get_mut(&mut self, index: usize) -> Option<&mut T>
		fn swap(&mut self, i: usize, j: usize)
		fn capacity(&self) -> usize
		fn reserve_exact(&mut self, additional: usize)
		fn reserve(&mut self, additional: usize)
		fn shrink_to_fit(&mut self)
		fn truncate(&mut self, len: usize)
		fn iter(&self) -> Iter<T>
		fn iter_mut(&mut self) -> IterMut<T>
		fn as_slices(&self) -> (&[T], &[T])
		fn as_mut_slices(&mut self) -> (&mut [T], &mut [T])
		fn len(&self) -> usize
		fn is_empty(&self) -> bool
		fn drain<R>(&mut self, range: R) -> Drain<T>
		fn clear(&mut self)
		fn contains(&self, x: &T) -> bool
		fn front(&self) -> Option<&T>
		fn front_mut(&mut self) -> Option<&mut T>
		fn back(&self) -> Option<&T>
		fn back_mut(&mut self) -> Option<&mut T>
		fn pop_front(&mut self) -> Option<T>
		fn push_front(&mut self, value: T)
		fn push_back(&mut self, value: T)
		fn pop_back(&mut self) -> Option<T>
		fn swap_remove_back(&mut self, index: usize) -> Option<T>
		fn swap_remove_front(&mut self, index: usize) -> Option<T>
		fn insert(&mut self, index: usize, value: T)
		fn remove(&mut self, index: usize) -> Option<T>
		fn split_off(&mut self, at: usize) -> VecDeque<T>
		fn append(&mut self, other: &mut VecDeque<T>)
		fn retain<F>(&mut self, f: F)
		fn resize_with<impl FnMut() -> T>(&mut self, new_len: usize, generator: impl FnMut() -> T)
		fn rotate_left(&mut self, mid: usize)
		fn rotate_right(&mut self, k: usize)
		fn resize(&mut self, new_len: usize, value: T)
	}
}

mod std::convert {
	enum Infallible;
	trait AsMut<T> {
		fn as_mut(&mut self) -> &mut T
	}
	trait AsRef<T> {
		fn as_ref(&self) -> &T
	}
	trait From<T> {
		fn from(T) -> Self
	}
	trait Into<T> {
		fn into(self) -> T
	}
	trait TryFrom<T> {
		type Error;
		fn try_from(value: T) -> Result<Self, Self::Error>
	}
	trait TryInto<T> {
		type Error;
		 fn try_into(self) -> Result<T, Self::Error>
	}
	const fn identity<T>(x: T) -> T
}

mod std::default {
	trait Default {
		fn default() -> Self;
	}
}

mod std::env {
	mod consts {
		const ARCH: &str
		const DLL_EXTENSION: &str
		const DLL_PREFIX: &str
		const DLL_SUFFIX: &str
		const EXE_EXTENSION: &str
		const EXE_SUFFIX: &str
		const FAMILY: &str
		const OS: &str
	}
	struct Args;
	struct ArgsOs;
	struct JoinPathsError;
	struct SplitPaths<'a>;
	struct Vars;
	struct VarsOs;
	enum VarError {
		NotPresent,
		NotUnicode(OsString),
	}
	fn args() -> Args
	fn args_os() -> ArgsOs
	fn current_dir() -> Result<PathBuf>
	fn current_exe() -> Result<PathBuf>
	fn join_paths<I, T>(paths: I) -> Result<OsString, JoinPathsError>
	fn remove_var<K: AsRef<OsStr>>(k: K)
	fn set_current_dir<P: AsRef<Path>>(path: P) -> Result<()>
	fn set_var<K: AsRef<OsStr>, V: AsRef<OsStr>>(k: K, v: V)
	fn split_paths<T: AsRef<OsStr> + ?Sized>(unparsed: &T) -> SplitPaths
	fn temp_dir() -> PathBuf
	fn var<K: AsRef<OsStr>>(key: K) -> Result<String, VarError>
	fn var_os<K: AsRef<OsStr>>(key: K) -> Option<OsString>
	fn vars() -> Vars
	fn vars_os() -> VarsOs
}

mod std::error {
	trait Error: Debug + Display {
		fn description(&self) -> &str
		fn source(&self) -> Option<&(dyn Error + 'static)>
	}
}

mod std::f32 {
	const DIGITS: u32
	const EPSILON: f32
	const INFINITY: f32
	const MANTISSA_DIGITS: u32
	const MAX: f32
	const MAX_10_EXP: i32
	const MAX_EXP: i32
	const MIN: f32
	const MIN_10_EXP: i32
	const MIN_EXP: i32
	const MIN_POSITIVE: f32
	const NAN: f32
	const NEG_INFINITY: f32
	const RADIX: u32
}

mod std::f64 {
	// Primitive Type
	fn floor(self) -> f64
	fn ceil(self) -> f64
	fn round(self) -> f64
	fn trunc(self) -> f64
	fn fract(self) -> f64
	fn abs(self) -> f64
	fn signum(self) -> f64
	fn copysign(self, sign: f64) -> f64
	fn mul_add(self, a: f64, b: f64) -> f64
	fn div_euclid(self, rhs: f64) -> f64
	fn rem_euclid(self, rhs: f64) -> f64
	fn powi(self, n: i32) -> f64
	fn powf(self, n: f64) -> f64
	fn sqrt(self) -> f64
	fn exp(self) -> f64
	fn exp2(self) -> f64
	fn ln(self) -> f64
	fn log(self, base: f64) -> f64
	fn log2(self) -> f64
	fn log10(self) -> f64
	fn cbrt(self) -> f64
	fn hypot(self, other: f64) -> f64
	fn sin(self) -> f64
	fn cos(self) -> f64
	fn tan(self) -> f64
	fn asin(self) -> f64
	fn acos(self) -> f64
	fn atan(self) -> f64
	fn atan2(self, other: f64) -> f64
	fn sin_cos(self) -> (f64, f64)
	fn exp_m1(self) -> f64
	fn ln_1p(self) -> f64
	fn sinh(self) -> f64
	fn cosh(self) -> f64
	fn tanh(self) -> f64
	fn asinh(self) -> f64
	fn acosh(self) -> f64
	fn atanh(self) -> f64
	fn is_nan(self) -> bool
	fn is_infinite(self) -> bool
	fn is_finite(self) -> bool
	fn is_normal(self) -> bool
	fn classify(self) -> FpCategory
	fn is_sign_positive(self) -> bool
	fn is_sign_negative(self) -> bool
	fn recip(self) -> f64
	fn to_degrees(self) -> f64
	fn to_radians(self) -> f64
	fn max(self, other: f64) -> f64
	fn min(self, other: f64) -> f64
	fn to_bits(self) -> u64
	fn from_bits(v: u64) -> f64

	// Module
	const DIGITS: u32
	const EPSILON: f64
	const INFINITY: f64
	const MANTISSA_DIGITS: u32
	const MAX: f64
	const MAX_10_EXP: i32
	const MAX_EXP: i32
	const MIN: f64
	const MIN_10_EXP: i32
	const MIN_EXP: i32
	const MIN_POSITIVE: f64
	const NAN: f64
	const NEG_INFINITY: f64
	const RADIX: u32
}

mod std::ffi {
	struct CStr {
		unsafe fn from_ptr<'a>(ptr: *const c_char) -> &'a CStr
		fn from_bytes_with_nul(bytes: &[u8]) -> Result<&CStr, FromBytesWithNulError>
		const unsafe fn from_bytes_with_nul_unchecked(bytes: &[u8]) -> &CStr
		const fn as_ptr(&self) -> *const c_char
		fn to_bytes(&self) -> &[u8]
		fn to_bytes_with_nul(&self) -> &[u8]
		fn to_str(&self) -> Result<&str, Utf8Error>
		fn to_string_lossy(&self) -> Cow<str>
		fn into_c_string(self: Box<CStr>) -> CString
	}
	struct CString {
		fn new<T: Into<Vec<u8>>>(t: T) -> Result<CString, NulError>
		unsafe fn from_vec_unchecked(v: Vec<u8>) -> CString
		unsafe fn from_raw(ptr: *mut c_char) -> CString
		fn into_raw(self) -> *mut c_char
		fn into_string(self) -> Result<String, IntoStringError>
		fn into_bytes(self) -> Vec<u8>
		fn into_bytes_with_nul(self) -> Vec<u8>
		fn as_bytes(&self) -> &[u8]
		fn as_bytes_with_nul(&self) -> &[u8]
		fn as_c_str(&self) -> &CStr
		fn into_boxed_c_str(self) -> Box<CStr>
		// Methods from Deref<Target = CStr>
	}
	struct FromBytesWithNulError;
	struct IntoStringError {
		fn into_cstring(self) -> CString
		fn utf8_error(&self) -> Utf8Error
	}
	struct NulError {
		fn nul_position(&self) -> usize
		fn into_vec(self) -> Vec<u8>
	}
	struct OsStr {
		fn new<S: AsRef<OsStr> + ?Sized>(s: &S) -> &OsStr
		fn to_str(&self) -> Option<&str>
		fn to_string_lossy(&self) -> Cow<str>
		fn to_os_string(&self) -> OsString
		fn is_empty(&self) -> bool
		fn len(&self) -> usize
		fn into_os_string(self: Box<OsStr>) -> OsString
	}
	struct OsString {
		fn new() -> OsString
		fn as_os_str(&self) -> &OsStr
		fn into_string(self) -> Result<String, OsString>
		fn push<T: AsRef<OsStr>>(&mut self, s: T)
		fn with_capacity(capacity: usize) -> OsString
		fn clear(&mut self)
		fn capacity(&self) -> usize
		fn reserve(&mut self, additional: usize)
		fn reserve_exact(&mut self, additional: usize)
		fn shrink_to_fit(&mut self)
		fn into_boxed_os_str(self) -> Box<OsStr>
		// Methods from Deref<Target = OsStr>
	}
	enum c_void;
}

mod std::fmt {
	struct Arguments<'a>;
	struct DebugList<'a, 'b> {
		fn entry(&mut self, entry: &dyn Debug) -> &mut DebugList<'a, 'b>
		fn entries<D, I>(&mut self, entries: I) -> &mut DebugList<'a, 'b>
		fn finish(&mut self) -> Result<(), Error>
	}
	struct DebugMap<'a, 'b> {
		fn entry(&mut self, key: &dyn Debug, value: &dyn Debug) -> &mut DebugMap<'a, 'b>
		fn entries<K, V, I>(&mut self, entries: I) -> &mut DebugMap<'a, 'b>
		fn finish(&mut self) -> Result<(), Error>
	}
	struct DebugSet<'a, 'b> {
		fn entry(&mut self, entry: &dyn Debug) -> &mut DebugSet<'a, 'b>
		fn entries<D, I>(&mut self, entries: I) -> &mut DebugSet<'a, 'b>
		fn finish(&mut self) -> Result<(), Error>
	}
	struct DebugStruct<'a, 'b> {
		fn field(&mut self, name: &str, value: &dyn Debug) -> &mut DebugStruct<'a, 'b>
		fn finish(&mut self) -> Result<(), Error>
	}
	struct DebugTuple<'a, 'b> {
		fn field(&mut self, value: &dyn Debug) -> &mut DebugTuple<'a, 'b>
		fn finish(&mut self) -> Result<(), Error>
	}
	struct Error;
	struct Formatter<'a> {
		fn pad_integral(&mut self, is_nonnegative: bool, prefix: &str, buf: &str) -> Result<(), Error>
		fn pad(&mut self, s: &str) -> Result<(), Error>
		fn write_str(&mut self, data: &str) -> Result<(), Error>
		fn write_fmt(&mut self, fmt: Arguments) -> Result<(), Error>
		fn fill(&self) -> char
		fn align(&self) -> Option<Alignment>
		fn width(&self) -> Option<usize>
		fn precision(&self) -> Option<usize>
		fn sign_plus(&self) -> bool
		fn sign_minus(&self) -> bool
		fn alternate(&self) -> bool
		fn sign_aware_zero_pad(&self) -> bool
		fn debug_struct(&'b mut self, name: &str) -> DebugStruct<'b, 'a>
		fn debug_tuple(&'b mut self, name: &str) -> DebugTuple<'b, 'a>
		fn debug_list(&'b mut self) -> DebugList<'b, 'a>
		fn debug_set(&'b mut self) -> DebugSet<'b, 'a>
		fn debug_map(&'b mut self) -> DebugMap<'b, 'a>
	}
	enum Alignment {
		Left,
		Right,
		Center,
	}
	trait Binary {
		fn fmt(&self, f: &mut Formatter) -> Result<(), Error>;
	}
	trait Debug {
		fn fmt(&self, f: &mut Formatter) -> Result<(), Error>
	}
	trait Display {
		fmt(&self, f: &mut Formatter) -> Result<(), Error>
	}
	trait LowerExp {
		fn fmt(&self, f: &mut Formatter) -> Result<(), Error>;
	}
	trait LowerHex {
		fn fmt(&self, f: &mut Formatter) -> Result<(), Error>;
	}
	trait Octal {
		fn fmt(&self, f: &mut Formatter) -> Result<(), Error>
	}
	trait Pointer {
		fmt(&self, f: &mut Formatter) -> Result<(), Error>
	}
	trait UpperExp {
		fn fmt(&self, f: &mut Formatter) -> Result<(), Error>
	}
	trait UpperHex {
		fn fmt(&self, f: &mut Formatter) -> Result<(), Error>
	}
	trait Write {
		fn write_str(&mut self, s: &str) -> Result<(), Error>
		fn write_char(&mut self, c: char) -> Result<(), Error>
		fn write_fmt(&mut self, args: Arguments) -> Result<(), Error>
	}
	fn format(args: Arguments) -> String
	fn write(output: &mut dyn Write, args: Arguments) -> Result<(), Error>
	type Result = Result<(), Error>;
}

mod std::fs {
	struct DirBuilder {
		fn new() -> DirBuilder
		fn recursive(&mut self, recursive: bool) -> &mut Self
		fn create<P: AsRef<Path>>(&self, path: P) -> Result<()>
	}
	struct DirEntry {
		fn path(&self) -> PathBuf
		fn metadata(&self) -> Result<Metadata>
		fn file_type(&self) -> Result<FileType>
		fn file_name(&self) -> OsString
	}
	struct File {
		fn open<P: AsRef<Path>>(path: P) -> Result<File>
		fn create<P: AsRef<Path>>(path: P) -> Result<File>
		fn sync_all(&self) -> Result<()>
		fn sync_data(&self) -> Result<()>
		fn set_len(&self, size: u64) -> Result<()>
		fn metadata(&self) -> Result<Metadata>
		fn try_clone(&self) -> Result<File>
		fn set_permissions(&self, perm: Permissions) -> Result<()>
	}
	struct FileType {
		fn is_dir(&self) -> bool
		fn is_file(&self) -> bool
		fn is_symlink(&self) -> bool
	}
	struct Metadata {
		fn file_type(&self) -> FileType
		fn is_dir(&self) -> bool
		fn is_file(&self) -> bool
		fn len(&self) -> u64
		fn permissions(&self) -> Permissions
		fn modified(&self) -> Result<SystemTime>
		fn accessed(&self) -> Result<SystemTime>
		fn created(&self) -> Result<SystemTime>
	}
	struct OpenOptions {
		fn new() -> OpenOptions
		fn read(&mut self, read: bool) -> &mut OpenOptions
		fn write(&mut self, write: bool) -> &mut OpenOptions
		fn append(&mut self, append: bool) -> &mut OpenOptions
		fn truncate(&mut self, truncate: bool) -> &mut OpenOptions
		fn create(&mut self, create: bool) -> &mut OpenOptions
		fn create_new(&mut self, create_new: bool) -> &mut OpenOptions
		fn open<P: AsRef<Path>>(&self, path: P) -> Result<File>
	}
	struct Permissions {
		fn readonly(&self) -> bool
		fn set_readonly(&mut self, readonly: bool)
	}
	struct ReadDir;
	fn canonicalize<P: AsRef<Path>>(path: P) -> Result<PathBuf>
	fn copy<P: AsRef<Path>, Q: AsRef<Path>>(from: P, to: Q) -> Result<u64>
	fn create_dir<P: AsRef<Path>>(path: P) -> Result<()>
	fn create_dir_all<P: AsRef<Path>>(path: P) -> Result<()>
	fn hard_link<P: AsRef<Path>, Q: AsRef<Path>>(src: P, dst: Q) -> Result<()>
	fn metadata<P: AsRef<Path>>(path: P) -> Result<Metadata>
	fn read<P: AsRef<Path>>(path: P) -> Result<Vec<u8>>
	fn read_dir<P: AsRef<Path>>(path: P) -> Result<ReadDir>
	fn read_link<P: AsRef<Path>>(path: P) -> Result<PathBuf>
	fn read_to_string<P: AsRef<Path>>(path: P) -> Result<String>
	fn remove_dir<P: AsRef<Path>>(path: P) -> Result<()>
	fn remove_dir_all<P: AsRef<Path>>(path: P) -> Result<()>
	fn remove_file<P: AsRef<Path>>(path: P) -> Result<()>
	fn rename<P: AsRef<Path>, Q: AsRef<Path>>(from: P, to: Q) -> Result<()>
	fn set_permissions<P: AsRef<Path>>(path: P, perm: Permissions) -> Result<()>
	fn symlink_metadata<P: AsRef<Path>>(path: P) -> Result<Metadata>
	fn write<P: AsRef<Path>, C: AsRef<[u8]>>(path: P, contents: C) -> Result<()>
}

mod std::future {
	trait Future {
		type Output;
		fn poll(self: Pin<&mut Self>, cx: &mut Context) -> Poll<Self::Output>
	}
}

mod std::hash {
	struct BuildHasherDefault<H>;
	trait BuildHasher {
		type Hasher: Hasher;
		fn build_hasher(&self) -> Self::Hasher
	}
	trait Hash {
		fn hash<H>(&self, state: &mut H)
		fn hash_slice<H>(data: &[Self], state: &mut H)
	}
	trait Hasher {
		fn finish(&self) -> u64
		fn write(&mut self, bytes: &[u8])
		fn write_u8(&mut self, i: u8)
		fn write_u16(&mut self, i: u16)
		fn write_u32(&mut self, i: u32)
		fn write_u64(&mut self, i: u64)
		fn write_u128(&mut self, i: u128)
		fn write_usize(&mut self, i: usize)
		fn write_i8(&mut self, i: i8)
		fn write_i16(&mut self, i: i16)
		fn write_i32(&mut self, i: i32)
		fn write_i64(&mut self, i: i64)
		fn write_i128(&mut self, i: i128)
		fn write_isize(&mut self, i: isize)
	}
}

mod std::hint {
	unsafe fn unreachable_unchecked() -> !
}

mod std::i8 {
	const MAX: i8
	const MIN: i8
}

mod std::i16 {
	const MAX: i16
	const MIN: i16
}

mod std::i32 {
	const MAX: i32
	const MIN: i32
}

mod std::i64 {
	const MAX: i64
	const MIN: i64
}

mod std::i128 {
	// Primitive Type
	const fn min_value() -> i128
	const fn max_value() -> i128
	fn from_str_radix(src: &str, radix: u32) -> Result<i128, ParseIntError>
	const fn count_ones(self) -> u32
	const fn count_zeros(self) -> u32
	const fn leading_zeros(self) -> u32
	const fn trailing_zeros(self) -> u32
	const fn rotate_left(self, n: u32) -> i128
	const fn rotate_right(self, n: u32) -> i128
	const fn swap_bytes(self) -> i128
	const fn reverse_bits(self) -> i128
	const fn from_be(x: i128) -> i128
	const fn from_le(x: i128) -> i128
	const fn to_be(self) -> i128
	const fn to_le(self) -> i128
	fn checked_add(self, rhs: i128) -> Option<i128>
	fn checked_sub(self, rhs: i128) -> Option<i128>
	fn checked_mul(self, rhs: i128) -> Option<i128>
	fn checked_div(self, rhs: i128) -> Option<i128>
	fn checked_div_euclid(self, rhs: i128) -> Option<i128>
	fn checked_rem(self, rhs: i128) -> Option<i128>
	fn checked_rem_euclid(self, rhs: i128) -> Option<i128>
	fn checked_neg(self) -> Option<i128>
	fn checked_shl(self, rhs: u32) -> Option<i128>
	fn checked_shr(self, rhs: u32) -> Option<i128>
	fn checked_abs(self) -> Option<i128>
	fn checked_pow(self, exp: u32) -> Option<i128>
	fn saturating_add(self, rhs: i128) -> i128
	fn saturating_sub(self, rhs: i128) -> i128
	fn saturating_mul(self, rhs: i128) -> i128
	fn saturating_pow(self, exp: u32) -> i128
	const fn wrapping_add(self, rhs: i128) -> i128
	const fn wrapping_sub(self, rhs: i128) -> i128
	const fn wrapping_mul(self, rhs: i128) -> i128
	fn wrapping_div(self, rhs: i128) -> i128
	fn wrapping_div_euclid(self, rhs: i128) -> i128
	fn wrapping_rem(self, rhs: i128) -> i128
	fn wrapping_rem_euclid(self, rhs: i128) -> i128
	const fn wrapping_neg(self) -> i128
	const fn wrapping_shl(self, rhs: u32) -> i128
	const fn wrapping_shr(self, rhs: u32) -> i128
	fn wrapping_abs(self) -> i128
	fn wrapping_pow(self, exp: u32) -> i128
	const fn overflowing_add(self, rhs: i128) -> (i128, bool)
	const fn overflowing_sub(self, rhs: i128) -> (i128, bool)
	const fn overflowing_mul(self, rhs: i128) -> (i128, bool)
	fn overflowing_div(self, rhs: i128) -> (i128, bool)
	fn overflowing_div_euclid(self, rhs: i128) -> (i128, bool)
	fn overflowing_rem(self, rhs: i128) -> (i128, bool)
	fn overflowing_rem_euclid(self, rhs: i128) -> (i128, bool)
	const fn overflowing_neg(self) -> (i128, bool)
	const fn overflowing_shl(self, rhs: u32) -> (i128, bool)
	const fn overflowing_shr(self, rhs: u32) -> (i128, bool)
	fn overflowing_abs(self) -> (i128, bool)
	fn overflowing_pow(self, exp: u32) -> (i128, bool)
	fn pow(self, exp: u32) -> i128
	fn div_euclid(self, rhs: i128) -> i128
	fn rem_euclid(self, rhs: i128) -> i128
	fn abs(self) -> i128
	fn signum(self) -> i128
	const fn is_positive(self) -> bool
	const fn is_negative(self) -> bool
	fn to_be_bytes(self) -> [u8; 16]
	fn to_le_bytes(self) -> [u8; 16]
	fn to_ne_bytes(self) -> [u8; 16]
	fn from_be_bytes(bytes: [u8; 16]) -> i128
	fn from_le_bytes(bytes: [u8; 16]) -> i128
	fn from_ne_bytes(bytes: [u8; 16]) -> i128

	// Module
	const MAX: i128
	const MIN: i128
}

mod std::io {
	struct BufReader<R> {
		fn new(inner: R) -> BufReader<R>
		fn with_capacity(capacity: usize, inner: R) -> BufReader<R>
		fn get_ref(&self) -> &R
		fn get_mut(&mut self) -> &mut R
		fn buffer(&self) -> &[u8]
		fn into_inner(self) -> R
	}
	struct BufWriter<W: Write> {
		fn new(inner: W) -> BufWriter<W>
		fn with_capacity(capacity: usize, inner: W) -> BufWriter<W>
		fn get_ref(&self) -> &W
		fn get_mut(&mut self) -> &mut W
		fn buffer(&self) -> &[u8]
		fn into_inner(self) -> Result<W, IntoInnerError<BufWriter<W>>>
	}
	struct Bytes<R>;
	struct Chain<T, U> {
		fn into_inner(self) -> (T, U)
		fn get_ref(&self) -> (&T, &U)
		fn get_mut(&mut self) -> (&mut T, &mut U)
	}
	struct Cursor<T> {
		fn new(inner: T) -> Cursor<T>
		fn into_inner(self) -> T
		fn get_ref(&self) -> &T
		fn get_mut(&mut self) -> &mut T
		fn position(&self) -> u64
		fn set_position(&mut self, pos: u64)
	}
	struct Empty;
	struct Error {
		fn new<E>(kind: ErrorKind, error: E) -> Error
		fn last_os_error() -> Error
		fn from_raw_os_error(code: i32) -> Error
		fn raw_os_error(&self) -> Option<i32>
		fn get_ref(&self) -> Option<&(dyn Error + Send + Sync + 'static)>
		fn get_mut(&mut self) -> Option<&mut (dyn Error + Send + Sync + 'static)>
		fn into_inner(self) -> Option<Box<dyn Error + Send + Sync>>
		fn kind(&self) -> ErrorKind
	}
	struct IntoInnerError<W> {
		fn error(&self) -> &Error
		fn into_inner(self) -> W
	}
	struct IoSlice<'a> {
		fn new(buf: &'a [u8]) -> IoSlice<'a>
		// Methods from Deref<Target = [u8]>
	}
	struct IoSliceMut<'a> {
		fn new(buf: &'a mut [u8]) -> IoSliceMut<'a>
		// Methods from Deref<Target = [u8]>
	}
	struct LineWriter<W: Write> {
		fn new(inner: W) -> LineWriter<W>
		fn with_capacity(capacity: usize, inner: W) -> LineWriter<W>
		fn get_ref(&self) -> &W
		fn get_mut(&mut self) -> &mut W
		fn into_inner(self) -> Result<W, IntoInnerError<LineWriter<W>>>
	}
	struct Lines<B>;
	struct Repeat;
	struct Sink;
	struct Split<B>;
	struct Stderr {
		fn lock(&self) -> StderrLock
	}
	struct StderrLock<'a>;
	struct Stdin {
		fn lock(&self) -> StdinLock
		fn read_line(&self, buf: &mut String) -> Result<usize>
	}
	struct StdinLock<'a>;
	struct Stdout {
		fn lock(&self) -> StdoutLock
	}
	struct StdoutLock<'a>;
	struct Take<T> {
		fn limit(&self) -> u64
		fn set_limit(&mut self, limit: u64)
		fn into_inner(self) -> T
		fn get_ref(&self) -> &T
		fn get_mut(&mut self) -> &mut T
	}

	enum ErrorKind {
		NotFound,
		PermissionDenied,
		ConnectionRefused,
		ConnectionReset,
		ConnectionAborted,
		NotConnected,
		AddrInUse,
		AddrNotAvailable,
		BrokenPipe,
		AlreadyExists,
		WouldBlock,
		InvalidInput,
		InvalidData,
		TimedOut,
		WriteZero,
		Interrupted,
		Other,
		UnexpectedEof,
	}
	enum SeekFrom {
		Start(u64),
		End(i64),
		Current(i64),
	}
	trait BufRead: Read {
		fn fill_buf(&mut self) -> Result<&[u8]>
		fn consume(&mut self, amt: usize)
		fn read_until(&mut self, byte: u8, buf: &mut Vec<u8>) -> Result<usize>
		fn read_line(&mut self, buf: &mut String) -> Result<usize>
		fn split(self, byte: u8) -> Split<Self>
		fn lines(self) -> Lines<Self>
	}
	trait Read {
		fn read(&mut self, buf: &mut [u8]) -> Result<usize>
		fn read_vectored(&mut self, bufs: &mut [IoSliceMut]) -> Result<usize>
		fn read_to_end(&mut self, buf: &mut Vec<u8>) -> Result<usize>
		fn read_to_string(&mut self, buf: &mut String) -> Result<usize>
		fn read_exact(&mut self, buf: &mut [u8]) -> Result<()>
		fn by_ref(&mut self) -> &mut Self
		fn bytes(self) -> Bytes<Self>
		fn chain<R: Read>(self, next: R) -> Chain<Self, R>
		fn take(self, limit: u64) -> Take<Self>
	}
	trait Seek {
		fn seek(&mut self, pos: SeekFrom) -> Result<u64>
	}
	trait Write {
		fn write(&mut self, buf: &[u8]) -> Result<usize>
		fn flush(&mut self) -> Result<()>
		fn write_vectored(&mut self, bufs: &[IoSlice]) -> Result<usize>
		fn write_all(&mut self, buf: &[u8]) -> Result<()>
		fn write_fmt(&mut self, fmt: Arguments) -> Result<()>
		fn by_ref(&mut self) -> &mut Self
	}
	fn copy<R: ?Sized, W: ?Sized>(reader: &mut R, writer: &mut W) -> Result<u64>
	fn empty() -> Empty
	fn repeat(byte: u8) -> Repeat
	fn sink() -> Sink
	fn stderr() -> Stderr
	fn stdin() -> Stdin
	fn stdout() -> Stdout
	type Result<T> = Result<T, Error>;
}

mod std::isize {
	const MAX: isize
	const MIN: isize
}

mod std::iter {
	struct Chain<A, B>;
	struct Cloned<I>;
	struct Copied<I>;
	struct Cycle<I>;
	struct Empty<T>;
	struct Enumerate<I>;
	struct Filter<I, P>;
	struct FilterMap<I, F>;
	struct FlatMap<I, U, F>;
	struct Flatten<I>;
	struct FromFn<F>;
	struct Fuse<I>;
	struct Inspect<I, F>;
	struct Map<I, F>;
	struct Once<T>;
	struct Peekable<I> {
		fn peek(&mut self) -> Option<&<I as Iterator>::Item>
	}
	struct Repeat<A>;
	struct RepeatWith<F>;
	struct Rev<T>;
	struct Scan<I, St, F>;
	struct Skip<I>;
	struct SkipWhile<I, P>;
	struct StepBy<I>;
	struct Successors<T, F>;
	struct Take<I>;
	struct TakeWhile<I, P>;
	struct Zip<A, B>;
	trait DoubleEndedIterator: Iterator {
		fn next_back(&mut self) -> Option<Self::Item>
		fn nth_back(&mut self, n: usize) -> Option<Self::Item>
		fn try_rfold<B, F, R>(&mut self, init: B, f: F) -> R
		fn rfold<B, F>(self, accum: B, f: F) -> B
		fn rfind<P>(&mut self, predicate: P) -> Option<Self::Item>
	}
	trait ExactSizeIterator: Iterator {
		fn len(&self) -> usize
	}
	trait Extend<A> {
		fn extend<T>(&mut self, iter: T)
	}
	trait FromIterator<A> {
		fn from_iter<T>(iter: T) -> Self
	}
	trait FusedIterator: Iterator;
	trait IntoIterator {
		type Item;
		type IntoIter: Iterator;
		fn into_iter(self) -> Self::IntoIter
	}
	trait Iterator {
		type Item;
		fn next(&mut self) -> Option<Self::Item>
		fn size_hint(&self) -> (usize, Option<usize>)
		fn count(self) -> usize
		fn last(self) -> Option<Self::Item>
		fn nth(&mut self, n: usize) -> Option<Self::Item>
		fn step_by(self, step: usize) -> StepBy<Self>
		fn chain<U>(self, other: U) -> Chain<Self, <U as IntoIterator>::IntoIter>
		fn zip<U>(self, other: U) -> Zip<Self, <U as IntoIterator>::IntoIter>
		fn map<B, F>(self, f: F) -> Map<Self, F>
		fn for_each<F>(self, f: F)
		fn filter<P>(self, predicate: P) -> Filter<Self, P>
		fn filter_map<B, F>(self, f: F) -> FilterMap<Self, F>
		fn enumerate(self) -> Enumerate<Self>
		fn peekable(self) -> Peekable<Self>
		fn skip_while<P>(self, predicate: P) -> SkipWhile<Self, P>
		fn take_while<P>(self, predicate: P) -> TakeWhile<Self, P>
		fn skip(self, n: usize) -> Skip<Self>
		fn take(self, n: usize) -> Take<Self>
		fn scan<St, B, F>(self, initial_state: St, f: F) -> Scan<Self, St, F>
		fn flat_map<U, F>(self, f: F) -> FlatMap<Self, U, F>
		fn flatten(self) -> Flatten<Self>
		fn fuse(self) -> Fuse<Self>
		fn inspect<F>(self, f: F) -> Inspect<Self, F>
		fn by_ref(&mut self) -> &mut Self
		fn collect<B>(self) -> B
		fn partition<B, F>(self, f: F) -> (B, B)
		fn try_fold<B, F, R>(&mut self, init: B, f: F) -> R
		fn try_for_each<F, R>(&mut self, f: F) -> R
		fn fold<B, F>(self, init: B, f: F) -> B
		fn all<F>(&mut self, f: F) -> bool
		fn any<F>(&mut self, f: F) -> bool
		fn find<P>(&mut self, predicate: P) -> Option<Self::Item>
		fn find_map<B, F>(&mut self, f: F) -> Option<B>
		fn position<P>(&mut self, predicate: P) -> Option<usize>
		fn rposition<P>(&mut self, predicate: P) -> Option<usize>
		fn max(self) -> Option<Self::Item>
		fn min(self) -> Option<Self::Item>
		fn max_by_key<B, F>(self, f: F) -> Option<Self::Item>
		fn max_by<F>(self, compare: F) -> Option<Self::Item>
		fn min_by_key<B, F>(self, f: F) -> Option<Self::Item>
		fn min_by<F>(self, compare: F) -> Option<Self::Item>
		fn rev(self) -> Rev<Self>
		fn unzip<A, B, FromA, FromB>(self) -> (FromA, FromB)
		fn copied<'a, T>(self) -> Copied<Self>
		fn cloned<'a, T>(self) -> Cloned<Self>
		fn cycle(self) -> Cycle<Self>
		fn sum<S>(self) -> S
		fn product<P>(self) -> P
		fn cmp<I>(self, other: I) -> Ordering
		fn partial_cmp<I>(self, other: I) -> Option<Ordering>
		fn eq<I>(self, other: I) -> bool
		fn ne<I>(self, other: I) -> bool
		fn lt<I>(self, other: I) -> bool
		fn le<I>(self, other: I) -> bool
		fn gt<I>(self, other: I) -> bool
		fn ge<I>(self, other: I) -> bool
	}
	trait Product<A = Self> {
		fn product<I>(iter: I) -> Self
	}
	trait Sum<A = Self> {
		fn sum<I>(iter: I) -> Self
	}
	const fn empty<T>() -> Empty<T>
	fn from_fn<T, F>(f: F) -> FromFn<F>
	fn once<T>(value: T) -> Once<T>
	fn repeat<T>(elt: T) -> Repeat<T>
	fn repeat_with<A, F>(repeater: F) -> RepeatWith<F>
	fn successors<T, F>(first: Option<T>, succ: F) -> Successors<T, F>
}

mod std::marker {
	struct PhantomData<T>;
	struct PhantomPinned;
	trait Copy: Clone;
	unsafe auto trait Send;
	trait Sized;
	unsafe auto trait Sync;
	auto trait Unpin;
}

mod std::mem {
	struct Discriminant<T>;
	struct ManuallyDrop<T> {
		const fn new(value: T) -> ManuallyDrop<T>
		const fn into_inner(slot: ManuallyDrop<T>) -> T
		unsafe fn drop(slot: &mut ManuallyDrop<T>)
	}
	const fn align_of<T>() -> usize
	fn align_of_val<T>(val: &T) -> usize
	fn discriminant<T>(v: &T) -> Discriminant<T>
	fn drop<T>(_x: T)
	fn forget<T>(t: T)
	const fn needs_drop<T>() -> bool
	fn replace<T>(dest: &mut T, src: T) -> T
	const fn size_of<T>() -> usize
	fn size_of_val<T>(val: &T) -> usize
	fn swap<T>(x: &mut T, y: &mut T)
	unsafe extern "rust-intrinsic" fn transmute<T, U>(e: T) -> U
	unsafe fn transmute_copy<T, U>(src: &T) -> U
	unsafe fn zeroed<T>() -> T
	union MaybeUninit<T> {
		const fn new(val: T) -> MaybeUninit<T>
		const fn uninit() -> MaybeUninit<T>
		fn zeroed() -> MaybeUninit<T>
		fn as_ptr(&self) -> *const T
		fn as_mut_ptr(&mut self) -> *mut T
		unsafe fn assume_init(self) -> T
	}
}

mod std::net {
	struct AddrParseError;
	struct Incoming<'a>;
	struct Ipv4Addr {
		const fn new(a: u8, b: u8, c: u8, d: u8) -> Ipv4Addr
		const LOCALHOST: Self
		const UNSPECIFIED: Self
		const BROADCAST: Self
		fn octets(&self) -> [u8; 4]
		const fn is_unspecified(&self) -> bool
		fn is_loopback(&self) -> bool
		fn is_private(&self) -> bool
		fn is_link_local(&self) -> bool
		fn is_multicast(&self) -> bool
		fn is_broadcast(&self) -> bool
		fn is_documentation(&self) -> bool
		fn to_ipv6_compatible(&self) -> Ipv6Addr
		fn to_ipv6_mapped(&self) -> Ipv6Addr
	}
	struct Ipv6Addr {
		const fn new(a: u16, b: u16, c: u16, d: u16, e: u16, f: u16, g: u16, h: u16) -> Ipv6Addr
		const LOCALHOST: Self
		const UNSPECIFIED: Self
		fn segments(&self) -> [u16; 8]
		fn is_unspecified(&self) -> bool
		fn is_loopback(&self) -> bool
		fn is_multicast(&self) -> bool
		fn to_ipv4(&self) -> Option<Ipv4Addr>
		const fn octets(&self) -> [u8; 16]
	}
	struct SocketAddrV4 {
		fn new(ip: Ipv4Addr, port: u16) -> SocketAddrV4
		fn ip(&self) -> &Ipv4Addr
		fn set_ip(&mut self, new_ip: Ipv4Addr)
		fn port(&self) -> u16
		fn set_port(&mut self, new_port: u16)
	}
	struct SocketAddrV6 {
		fn new(ip: Ipv6Addr, port: u16, flowinfo: u32, scope_id: u32) -> SocketAddrV6
		fn ip(&self) -> &Ipv6Addr
		fn set_ip(&mut self, new_ip: Ipv6Addr)
		fn port(&self) -> u16
		fn set_port(&mut self, new_port: u16)
		fn flowinfo(&self) -> u32
		fn set_flowinfo(&mut self, new_flowinfo: u32)
		fn scope_id(&self) -> u32
		fn set_scope_id(&mut self, new_scope_id: u32)
	}
	struct TcpListener {
		fn bind<A: ToSocketAddrs>(addr: A) -> Result<TcpListener>
		fn local_addr(&self) -> Result<SocketAddr>
		fn try_clone(&self) -> Result<TcpListener>
		fn accept(&self) -> Result<(TcpStream, SocketAddr)>
		fn incoming(&self) -> Incoming
		fn set_ttl(&self, ttl: u32) -> Result<()>
		fn ttl(&self) -> Result<u32>
		fn take_error(&self) -> Result<Option<Error>>
		fn set_nonblocking(&self, nonblocking: bool) -> Result<()>
	}
	struct TcpStream {
		fn connect<A: ToSocketAddrs>(addr: A) -> Result<TcpStream>
		fn connect_timeout(addr: &SocketAddr, timeout: Duration) -> Result<TcpStream>
		fn peer_addr(&self) -> Result<SocketAddr>
		fn local_addr(&self) -> Result<SocketAddr>
		fn shutdown(&self, how: Shutdown) -> Result<()>
		fn try_clone(&self) -> Result<TcpStream>
		fn set_read_timeout(&self, dur: Option<Duration>) -> Result<()>
		fn set_write_timeout(&self, dur: Option<Duration>) -> Result<()>
		fn read_timeout(&self) -> Result<Option<Duration>>
		fn write_timeout(&self) -> Result<Option<Duration>>
		fn peek(&self, buf: &mut [u8]) -> Result<usize>
		fn set_nodelay(&self, nodelay: bool) -> Result<()>
		fn nodelay(&self) -> Result<bool>
		fn set_ttl(&self, ttl: u32) -> Result<()>
		fn ttl(&self) -> Result<u32>
		fn take_error(&self) -> Result<Option<Error>>
		fn set_nonblocking(&self, nonblocking: bool) -> Result<()>
	}
	struct UdpSocket {
		fn bind<A: ToSocketAddrs>(addr: A) -> Result<UdpSocket>
		fn recv_from(&self, buf: &mut [u8]) -> Result<(usize, SocketAddr)>
		fn peek_from(&self, buf: &mut [u8]) -> Result<(usize, SocketAddr)>
		fn send_to<A: ToSocketAddrs>(&self, buf: &[u8], addr: A) -> Result<usize>
		fn local_addr(&self) -> Result<SocketAddr>
		fn try_clone(&self) -> Result<UdpSocket>
		fn set_read_timeout(&self, dur: Option<Duration>) -> Result<()>
		fn set_write_timeout(&self, dur: Option<Duration>) -> Result<()>
		fn read_timeout(&self) -> Result<Option<Duration>>
		fn write_timeout(&self) -> Result<Option<Duration>>
		fn set_broadcast(&self, broadcast: bool) -> Result<()>
		fn broadcast(&self) -> Result<bool>
		fn set_multicast_loop_v4(&self, multicast_loop_v4: bool) -> Result<()>
		fn multicast_loop_v4(&self) -> Result<bool>
		fn set_multicast_ttl_v4(&self, multicast_ttl_v4: u32) -> Result<()>
		fn multicast_ttl_v4(&self) -> Result<u32>
		fn set_multicast_loop_v6(&self, multicast_loop_v6: bool) -> Result<()>
		fn multicast_loop_v6(&self) -> Result<bool>
		fn set_ttl(&self, ttl: u32) -> Result<()>
		fn ttl(&self) -> Result<u32>
		fn join_multicast_v4(&self, multiaddr: &Ipv4Addr, interface: &Ipv4Addr) -> Result<()>
		fn join_multicast_v6(&self, multiaddr: &Ipv6Addr, interface: u32) -> Result<()>
		fn leave_multicast_v4(&self, multiaddr: &Ipv4Addr, interface: &Ipv4Addr) -> Result<()>
		fn leave_multicast_v6(&self, multiaddr: &Ipv6Addr, interface: u32) -> Result<()>
		fn take_error(&self) -> Result<Option<Error>>
		fn connect<A: ToSocketAddrs>(&self, addr: A) -> Result<()>
		fn send(&self, buf: &[u8]) -> Result<usize>
		fn recv(&self, buf: &mut [u8]) -> Result<usize>
		fn peek(&self, buf: &mut [u8]) -> Result<usize>
		fn set_nonblocking(&self, nonblocking: bool) -> Result<()>
	}
	enum IpAddr {
		V4(Ipv4Addr),
		V6(Ipv6Addr),
		fn is_unspecified(&self) -> bool
		fn is_loopback(&self) -> bool
		fn is_multicast(&self) -> bool
		fn is_ipv4(&self) -> bool
		fn is_ipv6(&self) -> bool
	}
	enum Shutdown {
		Read,
		Write,
		Both,
	}
	enum SocketAddr {
		V4(SocketAddrV4),
		V6(SocketAddrV6),
		fn new(ip: IpAddr, port: u16) -> SocketAddr
		fn ip(&self) -> IpAddr
		fn set_ip(&mut self, new_ip: IpAddr)
		fn port(&self) -> u16
		fn set_port(&mut self, new_port: u16)
		fn is_ipv4(&self) -> bool
		fn is_ipv6(&self) -> bool
	}
	trait ToSocketAddrs {
		type Iter: Iterator<Item = SocketAddr>;
		fn to_socket_addrs(&self) -> Result<Self::Iter>
	}
}

mod std::num {
	struct NonZeroI8;
	struct NonZeroI16;
	struct NonZeroI32;
	struct NonZeroI64;
	struct NonZeroI128 {
		const unsafe fn new_unchecked(n: i128) -> NonZeroI128
		fn new(n: i128) -> Option<NonZeroI128>
		const fn get(self) -> i128
	}
	struct NonZeroIsize;
	struct NonZeroU8;
	struct NonZeroU16;
	struct NonZeroU32;
	struct NonZeroU64;
	struct NonZeroU128;
	struct NonZeroUsize;
	struct ParseFloatError;
	struct ParseIntError;
	struct TryFromIntError;
	struct Wrapping<T>;
	enum FpCategory {
		Nan,
		Infinite,
		Zero,
		Subnormal,
		Normal,
	}
}

mod std::ops {
	struct Range<Idx> {
		pub start: Idx,
		pub end: Idx,
		fn contains<U>(&self, item: &U) -> bool
	}
	struct RangeFrom<Idx> {
		pub start: Idx,
		fn contains<U>(&self, item: &U) -> bool
	}
	struct RangeFull;
	struct RangeInclusive<Idx> {
		const fn new(start: Idx, end: Idx) -> RangeInclusive<Idx>
		const fn start(&self) -> &Idx
		const fn end(&self) -> &Idx
		fn into_inner(self) -> (Idx, Idx)
		fn contains<U>(&self, item: &U) -> bool
	}
	struct RangeTo<Idx> {
		pub end: Idx,
		fn contains<U>(&self, item: &U) -> bool
	}
	struct RangeToInclusive<Idx> {
		pub end: Idx,
		fn contains<U>(&self, item: &U) -> bool
	}
	enum Bound<T> {
		Included(T),
		Excluded(T),
		Unbounded,
	}
	trait Add<Rhs = Self> {
		type Output;
		fn add(self, rhs: Rhs) -> Self::Output
	}
	trait AddAssign<Rhs = Self> {
		fn add_assign(&mut self, rhs: Rhs)
	}
	trait BitAnd<Rhs = Self> {
		type Output;
		fn bitand(self, rhs: Rhs) -> Self::Output
	}
	trait BitAndAssign<Rhs = Self> {
		fn bitand_assign(&mut self, rhs: Rhs)
	}
	trait BitOr<Rhs = Self> {
		type Output;
		fn bitor(self, rhs: Rhs) -> Self::Output
	}
	trait BitOrAssign<Rhs = Self> {
		fn bitor_assign(&mut self, rhs: Rhs)
	}
	trait BitXor<Rhs = Self> {
		type Output;
		fn bitxor(self, rhs: Rhs) -> Self::Output
	}
	trait BitXorAssign<Rhs = Self> {
		fn bitxor_assign(&mut self, rhs: Rhs)
	}
	trait Deref {
		type Target: ?Sized;
		fn deref(&self) -> &Self::Target
	}
	trait DerefMut: Deref {
		fn deref_mut(&mut self) -> &mut Self::Target
	}
	trait Div<Rhs = Self> {
		type Output;
		fn div(self, rhs: Rhs) -> Self::Output
	}
	trait DivAssign<Rhs = Self> {
		fn div_assign(&mut self, rhs: Rhs)
	}
	trait Drop {
		fn drop(&mut self)
	}
	trait Fn<Args>: FnMut<Args>;
	trait FnMut<Args>: FnOnce<Args>;
	trait FnOnce<Args> {
		type Output;
	}
	trait Index<Idx> {
		type Output: ?Sized;
		fn index(&self, index: Idx) -> &Self::Output
	}
	trait IndexMut<Idx>: Index<Idx> {
		fn index_mut(&mut self, index: Idx) -> &mut Self::Output
	}
	trait Mul<Rhs = Self> {
		type Output;
		fn mul(self, rhs: Rhs) -> Self::Output
	}
	trait MulAssign<Rhs = Self> {
		fn mul_assign(&mut self, rhs: Rhs)
	}
	trait Neg {
		type Output;
		fn neg(self) -> Self::Output
	}
	trait Not {
		type Output;
		fn not(self) -> Self::Output
	}
	trait RangeBounds<T> {
		fn start_bound(&self) -> Bound<&T>
		fn end_bound(&self) -> Bound<&T>
		fn contains<U>(&self, item: &U) -> bool
	}
	trait Rem<Rhs = Self> {
		type Output;
		fn rem(self, rhs: Rhs) -> Self::Output
	}
	trait RemAssign<Rhs = Self> {
		fn rem_assign(&mut self, rhs: Rhs)
	}
	trait Shl<Rhs = Self> {
		type Output;
		fn shl(self, rhs: Rhs) -> Self::Output
	}
	trait ShlAssign<Rhs = Self> {
		fn shl_assign(&mut self, rhs: Rhs)
	}
	trait Shr<Rhs = Self> {
		type Output;
		fn shr(self, rhs: Rhs) -> Self::Output
	}
	trait ShrAssign<Rhs = Self> {
		fn shr_assign(&mut self, rhs: Rhs)
	}
	trait Sub<Rhs = Self> {
		type Output;
		fn sub(self, rhs: Rhs) -> Self::Output
	}
	trait SubAssign<Rhs = Self> {
		fn sub_assign(&mut self, rhs: Rhs);
	}
}

mod std::option {
	struct IntoIter<A>;
	struct Iter<'a, A>;
	struct IterMut<'a, A>;
	enum Option<T> {
		None,
		Some(T),
		fn is_some(&self) -> bool
		fn is_none(&self) -> bool
		fn as_ref(&self) -> Option<&T>
		fn as_mut(&mut self) -> Option<&mut T>
		fn as_pin_ref(self: Pin<&'a Option<T>>) -> Option<Pin<&'a T>>
		fn as_pin_mut(self: Pin<&'a mut Option<T>>) -> Option<Pin<&'a mut T>>
		fn expect(self, msg: &str) -> T
		fn unwrap(self) -> T
		fn unwrap_or(self, def: T) -> T
		fn unwrap_or_else<F>(self, f: F) -> T
		fn map<U, F>(self, f: F) -> Option<U>
		fn map_or<U, F>(self, default: U, f: F) -> U
		fn map_or_else<U, D, F>(self, default: D, f: F) -> U
		fn ok_or<E>(self, err: E) -> Result<T, E>
		fn ok_or_else<E, F>(self, err: F) -> Result<T, E>
		fn iter(&self) -> Iter<T>
		fn iter_mut(&mut self) -> IterMut<T>
		fn and<U>(self, optb: Option<U>) -> Option<U>
		fn and_then<U, F>(self, f: F) -> Option<U>
		fn filter<P>(self, predicate: P) -> Option<T>
		fn or(self, optb: Option<T>) -> Option<T>
		fn or_else<F>(self, f: F) -> Option<T>
		fn xor(self, optb: Option<T>) -> Option<T>
		fn get_or_insert(&mut self, v: T) -> &mut T
		fn get_or_insert_with<F>(&mut self, f: F) -> &mut T
		fn take(&mut self) -> Option<T>
		fn replace(&mut self, value: T) -> Option<T>
		fn copied(self) -> Option<T>
		fn cloned(self) -> Option<T>
		fn unwrap_or_default(self) -> T
		fn transpose(self) -> Result<Option<T>, E>
	}
}

mod std::os::raw {
	type c_char = i8;
	type c_double = f64;
	type c_float = f32;
	type c_int = i32;
	type c_long = i64;
	type c_longlong = i64;
	type c_schar = i8;
	type c_short = i16;
	type c_uchar = u8;
	type c_uint = u32;
	type c_ulong = u64;
	type c_ulonglong = u64;
	type c_ushort = u16;
}
// platform-specific extensions

mod std::panic {
	struct AssertUnwindSafe<T>;
	struct Location<'a> {
		fn file(&self) -> &str
		fn line(&self) -> u32
		fn column(&self) -> u32
	}
	struct PanicInfo<'a> {
		fn payload(&self) -> &(dyn Any + 'static + Send)
		fn location(&self) -> Option<&Location>
	}
	auto trait RefUnwindSafe;
	auto trait UnwindSafe;
	fn catch_unwind<F: FnOnce() -> R + UnwindSafe, R>(f: F) -> Result<R>
	fn resume_unwind(payload: Box<dyn Any + Send>) -> !
	fn set_hook(hook: Box<dyn Fn(&PanicInfo) + Sync + Send + 'static>)
	fn take_hook() -> Box<dyn Fn(&PanicInfo) + Sync + Send + 'static>
}

mod std::path {
	struct Ancestors<'a>;
	struct Components<'a> {
		fn as_path(&self) -> &'a Path
	}
	struct Display<'a>;
	struct Iter<'a> {
		fn as_path(&self) -> &'a Path
	}
	struct Path {
		fn new<S: AsRef<OsStr> + ?Sized>(s: &S) -> &Path
		fn as_os_str(&self) -> &OsStr
		fn to_str(&self) -> Option<&str>
		fn to_string_lossy(&self) -> Cow<str>
		fn to_path_buf(&self) -> PathBuf
		fn is_absolute(&self) -> bool
		fn is_relative(&self) -> bool
		fn has_root(&self) -> bool
		fn parent(&self) -> Option<&Path>
		fn ancestors(&self) -> Ancestors
		fn file_name(&self) -> Option<&OsStr>
		fn strip_prefix<P>(&self, base: P) -> Result<&Path, StripPrefixError>
		fn starts_with<P: AsRef<Path>>(&self, base: P) -> bool
		fn ends_with<P: AsRef<Path>>(&self, child: P) -> bool
		fn file_stem(&self) -> Option<&OsStr>
		fn extension(&self) -> Option<&OsStr>
		fn join<P: AsRef<Path>>(&self, path: P) -> PathBuf
		fn with_file_name<S: AsRef<OsStr>>(&self, file_name: S) -> PathBuf
		fn with_extension<S: AsRef<OsStr>>(&self, extension: S) -> PathBuf
		fn components(&self) -> Components
		fn iter(&self) -> Iter
		fn display(&self) -> Display
		fn metadata(&self) -> Result<Metadata>
		fn symlink_metadata(&self) -> Result<Metadata>
		fn canonicalize(&self) -> Result<PathBuf>
		fn read_link(&self) -> Result<PathBuf>
		fn read_dir(&self) -> Result<ReadDir>
		fn exists(&self) -> bool
		fn is_file(&self) -> bool
		fn is_dir(&self) -> bool
		fn into_path_buf(self: Box<Path>) -> PathBuf
	}
	struct PathBuf {
		fn new() -> PathBuf
		fn as_path(&self) -> &Path
		fn push<P: AsRef<Path>>(&mut self, path: P)
		fn pop(&mut self) -> bool
		fn set_file_name<S: AsRef<OsStr>>(&mut self, file_name: S)
		fn set_extension<S: AsRef<OsStr>>(&mut self, extension: S) -> bool
		fn into_os_string(self) -> OsString
		fn into_boxed_path(self) -> Box<Path>
		// Methods from Deref<Target = Path>
	}
	struct PrefixComponent<'a> {
		fn kind(&self) -> Prefix<'a>
		fn as_os_str(&self) -> &'a OsStr
	}
	struct StripPrefixError;
	enum Component<'a> {
		Prefix(PrefixComponent<'a>),
		RootDir,
		CurDir,
		ParentDir,
		Normal(&'a OsStr),
		fn as_os_str(self) -> &'a OsStr
	}
	enum Prefix<'a> {
		Verbatim(&'a OsStr),
		VerbatimUNC(&'a OsStr, &'a OsStr),
		VerbatimDisk(u8),
		DeviceNS(&'a OsStr),
		UNC(&'a OsStr, &'a OsStr),
		Disk(u8),
		fn is_verbatim(&self) -> bool
	}
	const MAIN_SEPARATOR: char
	fn is_separator(c: char) -> bool
}

mod std::pin {
	struct Pin<P> {
		fn new(pointer: P) -> Pin<P>
		unsafe fn new_unchecked(pointer: P) -> Pin<P>
		fn as_ref(&self) -> Pin<&<P as Deref>::Target>
		fn as_mut(&mut self) -> Pin<&mut <P as Deref>::Target>
		fn set(&mut self, value: <P as Deref>::Target)
		unsafe fn map_unchecked<U, F>(self, func: F) -> Pin<&'a U>
		fn get_ref(self) -> &'a T
		fn into_ref(self) -> Pin<&'a T>
		fn get_mut(self) -> &'a mut T
		unsafe fn get_unchecked_mut(self) -> &'a mut T
		unsafe fn map_unchecked_mut<U, F>(self, func: F) -> Pin<&'a mut U>
	}
}

mod std::prelude {}

mod std::process {
	struct Child {
		pub stdin: Option<ChildStdin>
		pub stdout: Option<ChildStdout>
		pub stderr: Option<ChildStderr>
		fn kill(&mut self) -> Result<()>
		fn id(&self) -> u32
		fn wait(&mut self) -> Result<ExitStatus>
		fn try_wait(&mut self) -> Result<Option<ExitStatus>>
		fn wait_with_output(self) -> Result<Output>
	}
	struct ChildStderr;
	struct ChildStdin;
	struct ChildStdout;
	struct Command {
		fn new<S: AsRef<OsStr>>(program: S) -> Command
		fn arg<S: AsRef<OsStr>>(&mut self, arg: S) -> &mut Command
		fn args<I, S>(&mut self, args: I) -> &mut Command
		fn env<K, V>(&mut self, key: K, val: V) -> &mut Command
		fn envs<I, K, V>(&mut self, vars: I) -> &mut Command
		fn env_remove<K: AsRef<OsStr>>(&mut self, key: K) -> &mut Command
		fn env_clear(&mut self) -> &mut Command
		fn current_dir<P: AsRef<Path>>(&mut self, dir: P) -> &mut Command
		fn stdin<T: Into<Stdio>>(&mut self, cfg: T) -> &mut Command
		fn stdout<T: Into<Stdio>>(&mut self, cfg: T) -> &mut Command
		fn stderr<T: Into<Stdio>>(&mut self, cfg: T) -> &mut Command
		fn spawn(&mut self) -> Result<Child>
		fn output(&mut self) -> Result<Output>
		fn status(&mut self) -> Result<ExitStatus>
	}
	struct ExitStatus {
		fn success(&self) -> bool
		fn code(&self) -> Option<i32>
	}
	struct Output {
		pub status: ExitStatus
		pub stdout: Vec<u8>
		pub stderr: Vec<u8>
	}
	struct Stdio {
		fn piped() -> Stdio
		fn inherit() -> Stdio
		fn null() -> Stdio
	}
	fn abort() -> !
	fn exit(code: i32) -> !
	fn id() -> u32
}

mod std::ptr {
	// Primitive Type pointer
	// impl<T> *const T
	fn is_null(self) -> bool
	const fn cast<U>(self) -> *const U
	unsafe fn as_ref<'a>(self) -> Option<&'a T>
	unsafe fn offset(self, count: isize) -> *const T
	fn wrapping_offset(self, count: isize) -> *const T
	unsafe fn add(self, count: usize) -> *const T
	unsafe fn sub(self, count: usize) -> *const T
	fn wrapping_add(self, count: usize) -> *const T
	fn wrapping_sub(self, count: usize) -> *const T
	unsafe fn read(self) -> T
	unsafe fn read_volatile(self) -> T
	unsafe fn read_unaligned(self) -> T
	unsafe fn copy_to(self, dest: *mut T, count: usize)
	unsafe fn copy_to_nonoverlapping(self, dest: *mut T, count: usize)
	fn align_offset(self, align: usize) -> usize
	// impl<T> *mut T
	unsafe fn as_mut<'a>(self) -> Option<&'a mut T>
	unsafe fn copy_from(self, src: *const T, count: usize)
	unsafe fn copy_from_nonoverlapping(self, src: *const T, count: usize)
	unsafe fn drop_in_place(self)
	unsafe fn write(self, val: T)
	unsafe fn write_bytes(self, val: u8, count: usize)
	unsafe fn write_volatile(self, val: T)
	unsafe fn write_unaligned(self, val: T)
	unsafe fn replace(self, src: T) -> T
	unsafe fn swap(self, with: *mut T)

	// Module
	struct NonNull<T> {
		const fn dangling() -> NonNull<T>
		const unsafe fn new_unchecked(ptr: *mut T) -> NonNull<T>
		fn new(ptr: *mut T) -> Option<NonNull<T>>
		const fn as_ptr(self) -> *mut T
		unsafe fn as_ref(&self) -> &T
		unsafe fn as_mut(&mut self) -> &mut T
		const fn cast<U>(self) -> NonNull<U>
	}
	unsafe fn copy<T>(src: *const T, dst: *mut T, count: usize)
	unsafe fn copy_nonoverlapping<T>(src: *const T, dst: *mut T, count: usize)
	unsafe fn drop_in_place<T>(to_drop: *mut T)
	fn eq<T>(a: *const T, b: *const T) -> bool
	fn hash<T, S>(hashee: *const T, into: &mut S)
	const fn null<T>() -> *const T
	const fn null_mut<T>() -> *mut T
	unsafe fn read<T>(src: *const T) -> T
	unsafe fn read_unaligned<T>(src: *const T) -> T
	unsafe fn read_volatile<T>(src: *const T) -> T
	unsafe fn replace<T>(dst: *mut T, src: T) -> T
	unsafe fn swap<T>(x: *mut T, y: *mut T)
	unsafe fn swap_nonoverlapping<T>(x: *mut T, y: *mut T, count: usize)
	unsafe fn write<T>(dst: *mut T, src: T)
	unsafe fn write_bytes<T>(dst: *mut T, val: u8, count: usize)
	unsafe fn write_unaligned<T>(dst: *mut T, src: T)
	unsafe fn write_volatile<T>(dst: *mut T, src: T)
}

mod std::rc {
	struct Rc<T> {
		fn new(value: T) -> Rc<T>
		fn pin(value: T) -> Pin<Rc<T>>
		fn try_unwrap(this: Rc<T>) -> Result<T, Rc<T>>
		fn into_raw(this: Rc<T>) -> *const T
		unsafe fn from_raw(ptr: *const T) -> Rc<T>
		fn downgrade(this: &Rc<T>) -> Weak<T>
		fn weak_count(this: &Rc<T>) -> usize
		fn strong_count(this: &Rc<T>) -> usize
		fn get_mut(this: &mut Rc<T>) -> Option<&mut T>
		fn ptr_eq(this: &Rc<T>, other: &Rc<T>) -> bool
		fn make_mut(this: &mut Rc<T>) -> &mut T
		fn downcast<T>(self) -> Result<Rc<T>, Rc<dyn Any + 'static>>
	}
	struct Weak<T> {
		fn new() -> Weak<T>
		fn upgrade(&self) -> Option<Rc<T>>
	}
}

mod std::result {
	struct IntoIter<T>;
	struct Iter<'a, T>;
	struct IterMut<'a, T>;
	enum Result<T, E> {
		Ok(T),
		Err(E),
		fn is_ok(&self) -> bool
		fn is_err(&self) -> bool
		fn ok(self) -> Option<T>
		fn err(self) -> Option<E>
		fn as_ref(&self) -> Result<&T, &E>
		fn as_mut(&mut self) -> Result<&mut T, &mut E>
		fn map<U, F>(self, op: F) -> Result<U, E>
		fn map_err<F, O>(self, op: O) -> Result<T, F>
		fn iter(&self) -> Iter<T>
		fn iter_mut(&mut self) -> IterMut<T>
		fn and<U>(self, res: Result<U, E>) -> Result<U, E>
		fn and_then<U, F>(self, op: F) -> Result<U, E>
		fn or<F>(self, res: Result<T, F>) -> Result<T, F>
		fn or_else<F, O>(self, op: O) -> Result<T, F>
		fn unwrap_or(self, optb: T) -> T
		fn unwrap_or_else<F>(self, op: F) -> T
		fn unwrap(self) -> T
		fn expect(self, msg: &str) -> T
		fn unwrap_err(self) -> E
		fn expect_err(self, msg: &str) -> E
		fn unwrap_or_default(self) -> T
		fn transpose(self) -> Option<Result<T, E>>
	}
}

mod std::slice {
	// Primitive Type
	fn len(&self) -> usize
	fn is_empty(&self) -> bool
	fn first(&self) -> Option<&T>
	fn first_mut(&mut self) -> Option<&mut T>
	fn split_first(&self) -> Option<(&T, &[T])>
	fn split_first_mut(&mut self) -> Option<(&mut T, &mut [T])>
	fn split_last(&self) -> Option<(&T, &[T])>
	fn split_last_mut(&mut self) -> Option<(&mut T, &mut [T])>
	fn last(&self) -> Option<&T>
	fn last_mut(&mut self) -> Option<&mut T>
	fn get<I>(&self, index: I) -> Option<&<I as SliceIndex<[T]>>::Output>
	fn get_mut<I>(&mut self, index: I) -> Option<&mut <I as SliceIndex<[T]>>::Output>
	unsafe fn get_unchecked<I>(&self, index: I) -> &<I as SliceIndex<[T]>>::Output
	unsafe fn get_unchecked_mut<I>(&mut self, index: I) -> &mut <I as SliceIndex<[T]>>::Output
	const fn as_ptr(&self) -> *const T
	fn as_mut_ptr(&mut self) -> *mut T
	fn swap(&mut self, a: usize, b: usize)
	fn reverse(&mut self)
	fn iter(&self) -> Iter<T>
	fn iter_mut(&mut self) -> IterMut<T>
	fn windows(&self, size: usize) -> Windows<T>
	fn chunks(&self, chunk_size: usize) -> Chunks<T>
	fn chunks_mut(&mut self, chunk_size: usize) -> ChunksMut<T>
	fn chunks_exact(&self, chunk_size: usize) -> ChunksExact<T>
	fn chunks_exact_mut(&mut self, chunk_size: usize) -> ChunksExactMut<T>
	fn rchunks(&self, chunk_size: usize) -> RChunks<T>
	fn rchunks_mut(&mut self, chunk_size: usize) -> RChunksMut<T>
	fn rchunks_exact(&self, chunk_size: usize) -> RChunksExact<T>
	fn rchunks_exact_mut(&mut self, chunk_size: usize) -> RChunksExactMut<T>
	fn split_at(&self, mid: usize) -> (&[T], &[T])
	fn split_at_mut(&mut self, mid: usize) -> (&mut [T], &mut [T])
	fn split<F>(&self, pred: F) -> Split<T, F>
	fn split_mut<F>(&mut self, pred: F) -> SplitMut<T, F>
	fn rsplit<F>(&self, pred: F) -> RSplit<T, F>
	fn rsplit_mut<F>(&mut self, pred: F) -> RSplitMut<T, F>
	fn splitn<F>(&self, n: usize, pred: F) -> SplitN<T, F>
	fn splitn_mut<F>(&mut self, n: usize, pred: F) -> SplitNMut<T, F>
	fn rsplitn<F>(&self, n: usize, pred: F) -> RSplitN<T, F>
	fn rsplitn_mut<F>(&mut self, n: usize, pred: F) -> RSplitNMut<T, F>
	fn contains(&self, x: &T) -> bool
	fn starts_with(&self, needle: &[T]) -> bool
	fn ends_with(&self, needle: &[T]) -> bool
	fn binary_search(&self, x: &T) -> Result<usize, usize>
	fn binary_search_by<'a, F>(&'a self, f: F) -> Result<usize, usize>
	fn binary_search_by_key<'a, B, F>(&'a self, b: &B, f: F) -> Result<usize, usize>
	fn sort_unstable(&mut self)
	fn sort_unstable_by<F>(&mut self, compare: F)
	fn sort_unstable_by_key<K, F>(&mut self, f: F)
	fn rotate_left(&mut self, mid: usize)
	fn rotate_right(&mut self, k: usize)
	fn clone_from_slice(&mut self, src: &[T])
	fn copy_from_slice(&mut self, src: &[T])
	fn copy_within<R>(&mut self, src: R, dest: usize)
	fn swap_with_slice(&mut self, other: &mut [T])
	unsafe fn align_to<U>(&self) -> (&[T], &[U], &[T])
	unsafe fn align_to_mut<U>(&mut self) -> (&mut [T], &mut [U], &mut [T])
	// impl [u8]
	fn is_ascii(&self) -> bool
	fn eq_ignore_ascii_case(&self, other: &[u8]) -> bool
	fn make_ascii_uppercase(&mut self)
	fn make_ascii_lowercase(&mut self)
	fn to_ascii_uppercase(&self) -> Vec<u8>
	fn to_ascii_lowercase(&self) -> Vec<u8>
	// impl<T> [T]
	fn sort(&mut self)
	fn sort_by<F>(&mut self, compare: F)
	fn sort_by_key<K, F>(&mut self, f: F)
	fn sort_by_cached_key<K, F>(&mut self, f: F)
	fn to_vec(&self) -> Vec<T>
	fn into_vec(self: Box<[T]>) -> Vec<T>
	fn concat<Item>(&self) -> <[T] as Concat<Item>>::Output
	fn join<Separator>(&self, sep: Separator) -> <[T] as Join<Separator>>::Output

	// Module
	struct Chunks<'a, T>;
	struct ChunksExact<'a, T> {
		fn remainder(&self) -> &'a [T]
	}
	struct ChunksExactMut<'a, T> {
		fn into_remainder(self) -> &'a mut [T]
	}
	struct ChunksMut<'a, T>;
	struct Iter<'a, T> {
		fn as_slice(&self) -> &'a [T]
	}
	struct IterMut<'a, T> {
		fn into_slice(self) -> &'a mut [T]
	}
	struct RChunks<'a, T>;
	struct RChunksExact<'a, T> {
		fn remainder(&self) -> &'a [T]
	}
	struct RChunksExactMut<'a, T> {
		fn into_remainder(self) -> &'a mut [T]
	}
	struct RChunksMut<'a, T>;
	struct RSplit<'a, T, P>;
	struct RSplitMut<'a, T, P>;
	struct RSplitN<'a, T, P>;
	struct RSplitNMut<'a, T, P>;
	struct Split<'a, T, P>;
	struct SplitMut<'a, T, P>;
	struct SplitN<'a, T, P>;
	struct SplitNMut<'a, T, P>;
	struct Windows<'a, T>;
	trait SliceIndex<T>: Sealed {
		type Output: ?Sized
	}
	fn from_mut<T>(s: &mut T) -> &mut [T]
	unsafe fn from_raw_parts<'a, T>(data: *const T, len: usize) -> &'a [T]
	unsafe fn from_raw_parts_mut<'a, T>(data: *mut T, len: usize) -> &'a mut [T]
	fn from_ref<T>(s: &T) -> &[T]
}

mod std::str {
	// Primitive Type
	fn len(&self) -> usize
	fn is_empty(&self) -> bool
	fn is_char_boundary(&self, index: usize) -> bool
	fn as_bytes(&self) -> &[u8]
	unsafe fn as_bytes_mut(&mut self) -> &mut [u8]
	const fn as_ptr(&self) -> *const u8
	fn as_mut_ptr(&mut self) -> *mut u8
	fn get<I>(&self, i: I) -> Option<&<I as SliceIndex<str>>::Output>
	fn get_mut<I>(&mut self, i: I) -> Option<&mut <I as SliceIndex<str>>::Output>
	unsafe fn get_unchecked<I>(&self, i: I) -> &<I as SliceIndex<str>>::Output
	unsafe fn get_unchecked_mut<I>(&mut self, i: I) -> &mut <I as SliceIndex<str>>::Output
	fn split_at(&self, mid: usize) -> (&str, &str)
	fn split_at_mut(&mut self, mid: usize) -> (&mut str, &mut str)
	fn chars(&self) -> Chars
	fn char_indices(&self) -> CharIndices
	fn bytes(&self) -> Bytes
	fn split_whitespace(&self) -> SplitWhitespace
	fn split_ascii_whitespace(&self) -> SplitAsciiWhitespace
	fn lines(&self) -> Lines
	fn encode_utf16(&self) -> EncodeUtf16
	fn contains<'a, P>(&'a self, pat: P) -> bool
	fn starts_with<'a, P>(&'a self, pat: P) -> bool
	fn ends_with<'a, P>(&'a self, pat: P) -> bool
	fn find<'a, P>(&'a self, pat: P) -> Option<usize>
	fn rfind<'a, P>(&'a self, pat: P) -> Option<usize>
	fn split<'a, P>(&'a self, pat: P) -> Split<'a, P>
	fn rsplit<'a, P>(&'a self, pat: P) -> RSplit<'a, P>
	fn split_terminator<'a, P>(&'a self, pat: P) -> SplitTerminator<'a, P>
	fn rsplit_terminator<'a, P>(&'a self, pat: P) -> RSplitTerminator<'a, P>
	fn splitn<'a, P>(&'a self, n: usize, pat: P) -> SplitN<'a, P>
	fn rsplitn<'a, P>(&'a self, n: usize, pat: P) -> RSplitN<'a, P>
	fn matches<'a, P>(&'a self, pat: P) -> Matches<'a, P>
	fn rmatches<'a, P>(&'a self, pat: P) -> RMatches<'a, P>
	fn match_indices<'a, P>(&'a self, pat: P) -> MatchIndices<'a, P>
	fn rmatch_indices<'a, P>(&'a self, pat: P) -> RMatchIndices<'a, P>
	fn trim(&self) -> &str
	fn trim_start(&self) -> &str
	fn trim_end(&self) -> &str
	fn trim_matches<'a, P>(&'a self, pat: P) -> &'a str
	fn trim_start_matches<'a, P>(&'a self, pat: P) -> &'a str
	fn trim_end_matches<'a, P>(&'a self, pat: P) -> &'a str
	fn parse<F>(&self) -> Result<F, <F as FromStr>::Err>
	fn is_ascii(&self) -> bool
	fn eq_ignore_ascii_case(&self, other: &str) -> bool
	fn make_ascii_uppercase(&mut self)
	fn make_ascii_lowercase(&mut self)
	fn escape_debug(&self) -> EscapeDebug
	fn escape_default(&self) -> EscapeDefault
	fn escape_unicode(&self) -> EscapeUnicode
	fn into_boxed_bytes(self: Box<str>) -> Box<[u8]>
	fn replace<'a, P>(&'a self, from: P, to: &str) -> String
	fn replacen<'a, P>(&'a self, pat: P, to: &str, count: usize) -> String
	fn to_lowercase(&self) -> String
	fn to_uppercase(&self) -> String
	fn into_string(self: Box<str>) -> String
	fn repeat(&self, n: usize) -> String
	fn to_ascii_uppercase(&self) -> String
	fn to_ascii_lowercase(&self) -> String

	// Module
	struct Bytes<'a>;
	struct CharIndices<'a> {
		fn as_str(&self) -> &'a str
	}
	struct Chars<'a> {
		fn as_str(&self) -> &'a str
	}
	struct EncodeUtf16<'a>;
	struct EscapeDebug<'a>;
	struct EscapeDefault<'a>;
	struct EscapeUnicode<'a>;
	struct Lines<'a>;
	struct MatchIndices<'a, P>;
	struct Matches<'a, P>;
	struct ParseBoolError;
	struct RMatchIndices<'a, P>;
	struct RMatches<'a, P>;
	struct RSplit<'a, P>;
	struct RSplitN<'a, P>;
	struct RSplitTerminator<'a, P>;
	struct Split<'a, P>;
	struct SplitAsciiWhitespace<'a>;
	struct SplitN<'a, P>;
	struct SplitTerminator<'a, P>;
	struct SplitWhitespace<'a>;
	struct Utf8Error {
		fn valid_up_to(&self) -> usize
		fn error_len(&self) -> Option<usize>
	}
	trait FromStr {
		type Err;
		fn from_str(s: &str) -> Result<Self, Self::Err>
	}
	unsafe fn from_boxed_utf8_unchecked(v: Box<[u8]>) -> Box<str>
	fn from_utf8_mut(v: &mut [u8]) -> Result<&mut str, Utf8Error>
	unsafe fn from_utf8_unchecked(v: &[u8]) -> &str
	fn from_utf8(v: &[u8]) -> Result<&str, Utf8Error>
	unsafe fn from_utf8_unchecked_mut(v: &mut [u8]) -> &mut str
}

mod std::string {
	struct Drain<'a>;
	struct FromUtf8Error {
		fn as_bytes(&self) -> &[u8]
		fn into_bytes(self) -> Vec<u8>
		fn utf8_error(&self) -> Utf8Error
	}
	struct FromUtf16Error;
	struct String {
		fn new() -> String
		fn with_capacity(capacity: usize) -> String
		fn from_utf8(vec: Vec<u8>) -> Result<String, FromUtf8Error>
		fn from_utf8_lossy(v: &'a [u8]) -> Cow<'a, str>
		fn from_utf16(v: &[u16]) -> Result<String, FromUtf16Error>
		fn from_utf16_lossy(v: &[u16]) -> String
		unsafe fn from_raw_parts(buf: *mut u8, length: usize, capacity: usize) -> String
		unsafe fn from_utf8_unchecked(bytes: Vec<u8>) -> String
		fn into_bytes(self) -> Vec<u8>
		fn as_str(&self) -> &str
		fn as_mut_str(&mut self) -> &mut str
		fn push_str(&mut self, string: &str)
		fn capacity(&self) -> usize
		fn reserve(&mut self, additional: usize)
		fn reserve_exact(&mut self, additional: usize)
		fn shrink_to_fit(&mut self)
		fn push(&mut self, ch: char)
		fn as_bytes(&self) -> &[u8]
		fn truncate(&mut self, new_len: usize)
		fn pop(&mut self) -> Option<char>
		fn remove(&mut self, idx: usize) -> char
		fn retain<F>(&mut self, f: F)
		fn insert(&mut self, idx: usize, ch: char)
		fn insert_str(&mut self, idx: usize, string: &str)
		unsafe fn as_mut_vec(&mut self) -> &mut Vec<u8>
		fn len(&self) -> usize
		fn is_empty(&self) -> bool
		fn split_off(&mut self, at: usize) -> String
		fn clear(&mut self)
		fn drain<R>(&mut self, range: R) -> Drain
		fn replace_range<R>(&mut self, range: R, replace_with: &str)
		fn into_boxed_str(self) -> Box<str>
		// Methods from Deref<Target = str>
	}
	trait ToString {
		fn to_string(&self) -> String
	}
	type ParseError = Infallible;
}

mod std::sync {
	struct Arc<T> {
		fn new(data: T) -> Arc<T>
		fn pin(data: T) -> Pin<Arc<T>>
		fn try_unwrap(this: Arc<T>) -> Result<T, Arc<T>>
		fn into_raw(this: Arc<T>) -> *const T
		unsafe fn from_raw(ptr: *const T) -> Arc<T>
		fn downgrade(this: &Arc<T>) -> Weak<T>
		fn weak_count(this: &Arc<T>) -> usize
		fn strong_count(this: &Arc<T>) -> usize
		fn ptr_eq(this: &Arc<T>, other: &Arc<T>) -> bool
		fn make_mut(this: &mut Arc<T>) -> &mut T
		fn get_mut(this: &mut Arc<T>) -> Option<&mut T>
		fn downcast<T>(self) -> Result<Arc<T>, Arc<dyn Any + 'static + Send + Sync>>
	}
	struct Barrier {
		fn new(n: usize) -> Barrier
		fn wait(&self) -> BarrierWaitResult
	}
	struct BarrierWaitResult {
		fn is_leader(&self) -> bool
	}
	struct Condvar {
		fn new() -> Condvar
		fn wait<'a, T>(&self, guard: MutexGuard<'a, T>) -> LockResult<MutexGuard<'a, T>>
		fn wait_timeout<'a, T>(&self, guard: MutexGuard<'a, T>, dur: Duration) -> LockResult<(MutexGuard<'a, T>, WaitTimeoutResult)>
		fn notify_one(&self)
		fn notify_all(&self)
	}
	struct Mutex<T: ?Sized> {
		fn new(t: T) -> Mutex<T>
		fn lock(&self) -> LockResult<MutexGuard<T>>
		fn try_lock(&self) -> TryLockResult<MutexGuard<T>>
		fn is_poisoned(&self) -> bool
		fn into_inner(self) -> LockResult<T>
		fn get_mut(&mut self) -> LockResult<&mut T>
	}
	struct MutexGuard<'a, T: ?Sized + 'a>;
	struct Once {
		const fn new() -> Once
		fn call_once<F>(&self, f: F)
	}
	struct PoisonError<T> {
		fn new(guard: T) -> PoisonError<T>
		fn into_inner(self) -> T
		fn get_ref(&self) -> &T
		fn get_mut(&mut self) -> &mut T
	}
	struct RwLock<T: ?Sized> {
		fn new(t: T) -> RwLock<T>
		fn read(&self) -> LockResult<RwLockReadGuard<T>>
		fn try_read(&self) -> TryLockResult<RwLockReadGuard<T>>
		fn write(&self) -> LockResult<RwLockWriteGuard<T>>
		fn try_write(&self) -> TryLockResult<RwLockWriteGuard<T>>
		fn is_poisoned(&self) -> bool
		fn into_inner(self) -> LockResult<T>
		fn get_mut(&mut self) -> LockResult<&mut T>
	}
	struct RwLockReadGuard<'a, T: ?Sized + 'a>;
	struct RwLockWriteGuard<'a, T: ?Sized + 'a>;
	struct WaitTimeoutResult {
		fn timed_out(&self) -> bool
	}
	struct Weak<T> {
		fn new() -> Weak<T>
		fn upgrade(&self) -> Option<Arc<T>>
	}
	enum TryLockError<T> {
		Poisoned(PoisonError<T>),
		WouldBlock,
	}
	type LockResult<Guard> = Result<Guard, PoisonError<Guard>>;
	type TryLockResult<Guard> = Result<Guard, TryLockError<Guard>>;
}

mod std::sync::atomic {
	struct AtomicBool;
	struct AtomicI8;
	struct AtomicI16;
	struct AtomicI32;
	struct AtomicI64;
	struct AtomicIsize {
		const fn new(v: isize) -> AtomicIsize
		fn get_mut(&mut self) -> &mut isize
		fn into_inner(self) -> isize
		fn load(&self, order: Ordering) -> isize
		fn store(&self, val: isize, order: Ordering)
		fn swap(&self, val: isize, order: Ordering) -> isize
		fn compare_and_swap(&self, current: isize, new: isize, order: Ordering) -> isize
		fn compare_exchange(&self, current: isize, new: isize, success: Ordering, failure: Ordering) -> Result<isize, isize>
		fn compare_exchange_weak(&self, current: isize, new: isize, success: Ordering, failure: Ordering) -> Result<isize, isize>
		fn fetch_add(&self, val: isize, order: Ordering) -> isize
		fn fetch_sub(&self, val: isize, order: Ordering) -> isize
		fn fetch_and(&self, val: isize, order: Ordering) -> isize
		fn fetch_nand(&self, val: isize, order: Ordering) -> isize
		fn fetch_or(&self, val: isize, order: Ordering) -> isize
		fn fetch_xor(&self, val: isize, order: Ordering) -> isize
	}
	struct AtomicPtr<T>;
	struct AtomicU8;
	struct AtomicU16;
	struct AtomicU32;
	struct AtomicU64;
	struct AtomicUsize;
	enum Ordering {
		Relaxed,
		Release,
		Acquire,
		AcqRel,
		SeqCst,
	}
	fn compiler_fence(order: Ordering)
	fn fence(order: Ordering)
	fn spin_loop_hint()
}

mod std::sync::mpsc {
	struct IntoIter<T>;
	struct Iter<'a, T: 'a>;
	struct Receiver<T> {
		fn try_recv(&self) -> Result<T, TryRecvError>
		fn recv(&self) -> Result<T, RecvError>
		fn recv_timeout(&self, timeout: Duration) -> Result<T, RecvTimeoutError>
		fn iter(&self) -> Iter<T>
		fn try_iter(&self) -> TryIter<T>
	}
	struct RecvError;
	struct SendError<T>;
	struct Sender<T> {
		fn send(&self, t: T) -> Result<(), SendError<T>>
	}
	struct SyncSender<T> {
		fn send(&self, t: T) -> Result<(), SendError<T>>
		fn try_send(&self, t: T) -> Result<(), TrySendError<T>>
	}
	struct TryIter<'a, T: 'a>;
	enum RecvTimeoutError {
		Timeout,
		Disconnected,
	}
	enum TryRecvError {
		Empty,
		Disconnected,
	}
	enum TrySendError<T> {
		Full(T),
		Disconnected(T),
	}
	fn channel<T>() -> (Sender<T>, Receiver<T>)
	fn sync_channel<T>(bound: usize) -> (SyncSender<T>, Receiver<T>)
}

mod std::task {
	struct Context<'a> {
		fn from_waker(waker: &'a Waker) -> Context<'a>
		fn waker(&self) -> &'a Waker
	}
	struct RawWaker {
		const fn new(data: *const (), vtable: &'static RawWakerVTable) -> RawWaker
	}
	struct RawWakerVTable {
		const fn new(clone: unsafe fn(*const ()) -> RawWaker, wake: unsafe fn(*const ()), wake_by_ref: unsafe fn(*const ()), drop: unsafe fn(*const ())) -> RawWakerVTable
	}
	struct Waker {
		fn wake(self)
		fn wake_by_ref(&self)
		fn will_wake(&self, other: &Waker) -> bool
		unsafe fn from_raw(waker: RawWaker) -> Waker
	}
	enum Poll<T> {
		Ready(T),
		Pending,
		fn map<U, F>(self, f: F) -> Poll<U>
		fn is_ready(&self) -> bool
		fn is_pending(&self) -> bool
		fn map_ok<U, F>(self, f: F) -> Poll<Result<U, E>>
		fn map_err<U, F>(self, f: F) -> Poll<Result<T, U>>
	}
}

mod std::thread {
	struct AccessError;
	struct Builder {
		fn new() -> Builder
		fn name(self, name: String) -> Builder
		fn stack_size(self, size: usize) -> Builder
		fn spawn<F, T>(self, f: F) -> Result<JoinHandle<T>>
	}
	struct JoinHandle<T> {
		fn thread(&self) -> &Thread
		fn join(self) -> Result<T>
	}
	struct LocalKey<T: 'static> {
		fn with<F, R>(&'static self, f: F) -> R
		fn try_with<F, R>(&'static self, f: F) -> Result<R, AccessError>
	}
	struct Thread {
		fn unpark(&self)
		fn id(&self) -> ThreadId
		fn name(&self) -> Option<&str>
	}
	struct ThreadId;
	fn current() -> Thread
	fn panicking() -> bool
	fn park()
	fn park_timeout(dur: Duration)
	fn sleep(dur: Duration)
	fn spawn<F, T>(f: F) -> JoinHandle<T>
	fn yield_now()
	type Result<T> = Result<T, Box<dyn Any + Send + 'static>>;
}

mod std::time {
	struct Duration {
		fn new(secs: u64, nanos: u32) -> Duration
		const fn from_secs(secs: u64) -> Duration
		const fn from_millis(millis: u64) -> Duration
		const fn from_micros(micros: u64) -> Duration
		const fn from_nanos(nanos: u64) -> Duration
		const fn as_secs(&self) -> u64
		const fn subsec_millis(&self) -> u32
		const fn subsec_micros(&self) -> u32
		const fn subsec_nanos(&self) -> u32
		const fn as_millis(&self) -> u128
		const fn as_micros(&self) -> u128
		const fn as_nanos(&self) -> u128
		fn checked_add(self, rhs: Duration) -> Option<Duration>
		fn checked_sub(self, rhs: Duration) -> Option<Duration>
		fn checked_mul(self, rhs: u32) -> Option<Duration>
		fn checked_div(self, rhs: u32) -> Option<Duration>
		fn as_secs_f64(&self) -> f64
		fn as_secs_f32(&self) -> f32
		fn from_secs_f64(secs: f64) -> Duration
		fn from_secs_f32(secs: f32) -> Duration
		fn mul_f64(self, rhs: f64) -> Duration
		fn mul_f32(self, rhs: f32) -> Duration
		fn div_f64(self, rhs: f64) -> Duration
		fn div_f32(self, rhs: f32) -> Duration
	}
	struct Instant {
		fn now() -> Instant
		fn duration_since(&self, earlier: Instant) -> Duration
		fn elapsed(&self) -> Duration
		fn checked_add(&self, duration: Duration) -> Option<Instant>
		fn checked_sub(&self, duration: Duration) -> Option<Instant>
	}
	struct SystemTime {
		const UNIX_EPOCH: SystemTime
		fn now() -> SystemTime
		fn duration_since(&self, earlier: SystemTime) -> Result<Duration, SystemTimeError>
		fn elapsed(&self) -> Result<Duration, SystemTimeError>
		fn checked_add(&self, duration: Duration) -> Option<SystemTime>
		fn checked_sub(&self, duration: Duration) -> Option<SystemTime>
	}
	struct SystemTimeError {
		fn duration(&self) -> Duration
	}
	const UNIX_EPOCH: SystemTime
}

mod std::u8 {
	// Primitive Type
	fn is_ascii(&self) -> bool
	fn to_ascii_uppercase(&self) -> u8
	fn to_ascii_lowercase(&self) -> u8
	fn eq_ignore_ascii_case(&self, other: &u8) -> bool
	fn make_ascii_uppercase(&mut self)
	fn make_ascii_lowercase(&mut self)
	fn is_ascii_alphabetic(&self) -> bool
	fn is_ascii_uppercase(&self) -> bool
	fn is_ascii_lowercase(&self) -> bool
	fn is_ascii_alphanumeric(&self) -> bool
	fn is_ascii_digit(&self) -> bool
	fn is_ascii_hexdigit(&self) -> bool
	fn is_ascii_punctuation(&self) -> bool
	fn is_ascii_graphic(&self) -> bool
	fn is_ascii_whitespace(&self) -> bool
	fn is_ascii_control(&self) -> bool

	// Module
	const MAX: u8
	const MIN: u8
}

mod std::u16 {
	const MAX: u16
	const MIN: u16
}

mod std::u32 {
	const MAX: u32
	const MIN: u32
}

mod std::u64 {
	const MAX: u64
	const MIN: u64
}

mod std::u128 {
	// Primitive Type
	fn is_power_of_two(self) -> bool
	fn next_power_of_two(self) -> u128
	fn checked_next_power_of_two(self) -> Option<u128>

	// Module
	const MAX: u128
	const MIN: u128
}

mod std::usize {
	const MAX: usize
	const MIN: usize
}

mod std::vec {
	struct Drain<'a, T> {}
	struct IntoIter<T> {
		fn as_slice(&self) -> &[T]
		fn as_mut_slice(&mut self) -> &mut [T]
	}
	struct Splice<'a, I>;
	struct Vec<T> {
		fn new() -> Vec<T>
		fn with_capacity(capacity: usize) -> Vec<T>
		unsafe fn from_raw_parts(ptr: *mut T, length: usize, capacity: usize) -> Vec<T>
		fn capacity(&self) -> usize
		fn reserve(&mut self, additional: usize)
		fn reserve_exact(&mut self, additional: usize)
		fn shrink_to_fit(&mut self)
		fn into_boxed_slice(self) -> Box<[T]>
		fn truncate(&mut self, len: usize)
		fn as_slice(&self) -> &[T]
		fn as_mut_slice(&mut self) -> &mut [T]
		fn as_ptr(&self) -> *const T
		fn as_mut_ptr(&mut self) -> *mut T
		unsafe fn set_len(&mut self, new_len: usize)
		fn swap_remove(&mut self, index: usize) -> T
		fn insert(&mut self, index: usize, element: T)
		fn remove(&mut self, index: usize) -> T
		fn retain<F>(&mut self, f: F)
		fn dedup_by_key<F, K>(&mut self, key: F)
		fn dedup_by<F>(&mut self, same_bucket: F)
		fn push(&mut self, value: T)
		fn pop(&mut self) -> Option<T>
		fn append(&mut self, other: &mut Vec<T>)
		fn drain<R>(&mut self, range: R) -> Drain<T>
		fn clear(&mut self)
		fn len(&self) -> usize
		fn is_empty(&self) -> bool
		fn split_off(&mut self, at: usize) -> Vec<T>
		fn resize_with<F>(&mut self, new_len: usize, f: F)
		fn resize(&mut self, new_len: usize, value: T)
		fn extend_from_slice(&mut self, other: &[T])
		fn dedup(&mut self)
		fn splice<R, I>(&mut self, range: R, replace_with: I) -> Splice<<I as IntoIterator>::IntoIter>
		fn first(&self) -> Option<&T>
		// Methods from Deref<Target = [T]>
	}
}

mod std::intrinsics {
}

mod std::raw {
}
