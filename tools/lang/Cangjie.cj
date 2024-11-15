// 2024-10-24 0.53.13 https://cangjie-lang.cn/
// 2024-10-26 0.22.17 https://developer.huawei.com/consumer/cn/cangjie

//! keywords		===========================================================
abstract as
break
case catch class const continue
do
else enum extend
false finally for from foreign func
if import in init inout interface is
let
macro main match mut
open operator override
package private prop protected public
quote
redef return
spawn static struct super synchronized
this throw true try type
unsafe
var
where while

// Contextual Keyword
get sealed set

//! types			===========================================================
Nothing This VArray
// std.core types
Byte Int UInt
Int8 Int16 Int32 Int64 IntNative
UInt8 UInt16 UInt32 UInt64 UIntNative
Float16 Float32 Float64
Bool Rune Unit
CPointer CString

//! macro			===========================================================
@sourcePackage()
@sourceFile()
@sourceLine()
@Intrinsic
@FastNative
@When

@C
@CallingConv

//! annotation		===========================================================
@Annotation

//! api				===========================================================
package std.core {
	func alignOf<T>(): UIntNative where T <: CType
	func eprint(str: String, flush!: Bool = true): Unit
	func eprintln(str: String): Unit
	func ifNone<T>(o: Option<T>, action: () -> Unit): Unit
	func ifSome<T>(o: Option<T>, action: (T) -> Unit): Unit
	func print<T>(arg: T, flush!: Bool = false): Unit where T <: ToString
	func println<T>(arg: T): Unit where T <: ToString
	func refEq(a: Object, b: Object): Bool
	func sizeOf<T>(): UIntNative where T <: CType
	func zeroValue<T>(): T

	interface Any
	interface CType
	interface Collection<T> <: Iterable<T>
	interface Comparable<T> <: Equatable<T> & Less<T> & Greater<T> & LessOrEqual<T> & GreaterOrEqual<T>
	interface Countable<T>
	interface Equal<T>
	interface Equatable<T>
	interface Hashable
	interface Hasher
	interface Iterable<E>
	interface Resource
	interface ThreadContext
	interface ToString

	class ArrayIterator<T> <: Iterator<T>
	class Box<T>
	class Future<T>
	abstract class Iterator<T> <: Iterable<T>
	open class Object <: Any
	class RangeIterator<T> <: Iterator<T> where T <: Countable<T> & Comparable<T> & Equatable<T>
	class StringBuilder <: ToString
	class Thread
	class ThreadLocal<T>

	enum Endian
	enum Option<T>
	enum Ordering

	struct Array<T>
	struct CPointerHandle<T> where T <: CType
	struct CPointerResource<T> <: Resource where T <: CType
	struct CStringResource <: Resource
	struct DefaultHasher <: Hasher
	struct Range<T> <: Iterable<T> where T <: Countable<T> & Comparable<T> & Equatable<T>
	struct String <: Collection<Byte> & Equatable<String> & Comparable<String> & Hashable & ToString

	open class Error <: ToString
	open class Exception <: ToString
}

package std.collection {
	func all<T>(predicate: (T) -> Bool): (Iterable<T>) -> Bool
	func any<T>(predicate: (T) -> Bool): (Iterable<T>) -> Bool

	interface Map<K, V> <: Collection<(K, V)> where K <: Equatable<K>
	interface Set<T> <: Collection<T> where T <: Equatable<T>

	class ArrayList<T> <: Collection<T>
	class HashMap<K, V> <: Map<K, V> where K <: Hashable & Equatable<K>
	class HashSet<T> <: Set<T> where T <: Hashable & Equatable<T>
	class LinkedList<T> <: Collection<T>
	class TreeMap<K, V> <: Map<K, V> where K <: Comparable<K>
}

package std.fs {
	class Directory <: Iterable<FileInfo>
	class File <: Resource & IOStream & Seekable

	enum OpenOption

	struct FileDescriptor
	struct FileInfo <: Equatable<FileInfo>
	struct Path <: Equatable<Path> & Hashable & ToString
}

package std.io {
	interface IOStream <: InputStream & OutputStream
	interface InputStream
	interface OutputStream
	interface Seekable

	class BufferedInputStream<T> <: InputStream where T <: InputStream
	class BufferedOutputStream<T> <: OutputStream where T <: OutputStream
	class ByteArrayStream <: IOStream & Seekable
	class StringReader<T> where T <: InputStream
	class StringWriter<T> where T <: OutputStream

	enum SeekPosition
	class IOException <: Exception
}
