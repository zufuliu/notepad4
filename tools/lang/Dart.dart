// 3.3 https://dart.dev/
// https://dart.dev/guides/language/language-tour
// 2.13 https://dart.dev/guides/language/spec
// https://github.com/dart-lang/language
// https://github.com/dart-lang/sdk

//! keywords		===========================================================
// https://dart.dev/guides/language/language-tour#keywords
abstract as assert async await
base break
case catch class const continue covariant
default deferred do
else enum export extends extension external
factory false final finally for
get
hide
if implements import in interface is
late library
mixin
native new null
of on operator
part
rethrow return required
sealed set show static super switch sync
this throw true try type typedef
var
when while with
yield

//! types			===========================================================
// https://dart.dev/guides/language/language-tour#built-in-types
dynamic void Function Never
int double
bool num

//! libraries		===========================================================
// https://dart.dev/guides/libraries
library dart._internal {
	@Since()
	class SystemHash
	@patch
}

library dart.core {
	main()
	@pragma()
	class Deprecated
	@Deprecated()
	@deprecated
	@override
	abstract class BigInt implements Comparable<BigInt>
	typedef Comparator<T>
	abstract class Comparable<T>
	class DateTime implements Comparable<DateTime>
	class Duration implements Comparable<Duration>
	abstract class Enum
	class Error
	abstract class Exception
	external bool identical(Object? a, Object? b);
	external int identityHashCode(Object? object);
	abstract class Invocation
	abstract class Iterable<E>
	abstract class Iterator<E>
	abstract class List<E> implements EfficientLengthIterable<E>
	abstract class Map<K, V>
	class MapEntry<K, V>
	class Null
	class Object
	abstract class Pattern
	abstract class Match
	void print(Object? object)
	abstract class Record
	abstract class RegExp implements Pattern
	abstract class RegExpMatch implements Match
	abstract class Set<E> extends EfficientLengthIterable<E>
	abstract class Sink<T>
	abstract class StackTrace
	class Stopwatch
	abstract class String implements Comparable<String>, Pattern
	class Runes extends Iterable<int>
	class RuneIterator implements Iterator<int>
	class StringBuffer implements StringSink
	abstract class StringSink
	abstract class Symbol
	abstract class Type
	abstract class Uri
	class UriData
	class Expando<T extends Object>
	abstract class WeakReference<T extends Object>
}

library dart.async {
	abstract class FutureOr<T>
	abstract class Future<T>
	abstract class Completer<T>
	abstract class Stream<T>
	abstract class Timer
}

library dart.collection {
	abstract class HashMap<K, V> implements Map<K, V>
	abstract class HashSet<E> implements Set<E>
	typedef IterableMixin<E> = Iterable<E>
	typedef IterableBase<E> = Iterable<E>
	abstract class LinkedHashMap<K, V> implements Map<K, V>
	abstract class LinkedHashSet<E> implements Set<E>
	class LinkedList<E extends LinkedListEntry<E>> extends Iterable<E>
	abstract class LinkedListEntry<E extends LinkedListEntry<E>>
	abstract class ListBase<E> implements List<E>
	typedef ListMixin<E> = ListBase<E>
	abstract class MapBase<K, V> extends MapMixin<K, V>
	typedef MapMixin<K, V> = MapBase<K, V>
	class MapView<K, V> implements Map<K, V>
	abstract class Queue<E> implements EfficientLengthIterable<E>
	class DoubleLinkedQueue<E> extends Iterable<E> implements Queue<E>
	class ListQueue<E> extends ListIterable<E> implements Queue<E>
	abstract class SetBase<E> implements Set<E>
	typedef SetMixin<E> = SetBase<E>
	class SplayTreeMap<K, V> extends _SplayTree<K, _SplayTreeMapNode<K, V>>
	class SplayTreeSet<E> extends _SplayTree<E, _SplayTreeSetNode<E>>
}

library dart.io {
	abstract class IOException implements Exception
	class OSError implements Exception
	abstract class Directory implements FileSystemEntity
	class FileMode
	class FileLock
	abstract class File implements FileSystemEntity
	class FileStat
	abstract class FileSystemEntity
	class FileSystemEvent
	class Platform
	abstract class Process
	abstract class ServerSocket implements Stream<Socket>
	class RawSocketEvent
	abstract class RawSocket implements Stream<RawSocketEvent>
	abstract class Socket implements Stream<Uint8List>, IOSink
	class SocketException implements IOException
}

library dart.math {
	class Point<T extends num>
	abstract class Random
	class Rectangle<T extends num> extends _RectangleBase<T>
}
