// 2.12 https://dart.dev/
// https://dart.dev/guides/language/language-tour
// https://dart.dev/guides/language/spec
// https://github.com/dart-lang/sdk

//! keywords		===========================================================
// https://dart.dev/guides/language/language-tour#keywords
abstract as assert async await
break
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
set show static super switch sync
this throw true try typedef
var
while with
yield

//! types			===========================================================
// https://dart.dev/guides/language/language-tour#built-in-types
dynamic void Function
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
	abstract class BigInt implements Comparable<BigInt>
	typedef Comparator<T>
	abstract class Comparable<T>
	class DateTime implements Comparable<DateTime>
	class Deprecated
	@Deprecated()
	@deprecated
	@override
	class Duration implements Comparable<Duration>
	class Error
	abstract class Exception
	class Expando<T extends Object>
	external bool identical(Object? a, Object? b);
	external int identityHashCode(Object? object);
	abstract class Invocation
	abstract class Iterable<E>
	abstract class BidirectionalIterator<E> implements Iterator<E>
	abstract class Iterator<E>
	abstract class List<E> implements EfficientLengthIterable<E>
	abstract class Map<K, V>
	class MapEntry<K, V>
	class Null
	class Object
	abstract class Pattern
	abstract class Match
	void print(Object? object)
	abstract class RegExp implements Pattern
	abstract class RegExpMatch implements Match
	class RuneIterator implements BidirectionalIterator<int>
	class Runes extends Iterable<int>
	abstract class Set<E> extends EfficientLengthIterable<E>
	abstract class Sink<T>
	abstract class StackTrace
	class Stopwatch
	abstract class String implements Comparable<String>, Pattern
	class Runes extends Iterable<int>
	class RuneIterator implements BidirectionalIterator<int>
	class StringBuffer implements StringSink
	abstract class StringSink
	abstract class Symbol
	abstract class Type
	abstract class Uri
	class UriData
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
	abstract class IterableMixin<E> implements Iterable<E>
	abstract class IterableBase<E> extends Iterable<E>
	class HasNextIterator<E>
	abstract class LinkedHashMap<K, V> implements Map<K, V>
	abstract class LinkedHashSet<E> implements Set<E>
	class LinkedList<E extends LinkedListEntry<E>> extends Iterable<E>
	abstract class LinkedListEntry<E extends LinkedListEntry<E>>
	abstract class ListBase<E> extends Object with ListMixin<E>
	abstract class ListMixin<E> implements List<E>
	abstract class MapBase<K, V> extends MapMixin<K, V>
	abstract class MapMixin<K, V> implements Map<K, V>
	class MapView<K, V> implements Map<K, V>
	abstract class Queue<E> implements EfficientLengthIterable<E>
	class DoubleLinkedQueueEntry<E> extends _DoubleLink<DoubleLinkedQueueEntry<E>>
	class DoubleLinkedQueue<E> extends Iterable<E> implements Queue<E>
	class ListQueue<E> extends ListIterable<E> implements Queue<E>
	abstract class SetMixin<E> implements Set<E>
	abstract class SetBase<E> with SetMixin<E>
	class SplayTreeMap<K, V> extends _SplayTree<K, _SplayTreeMapNode<K, V>>
	class SplayTreeSet<E> extends _SplayTree<E, _SplayTreeSetNode<E>>
}

library dart.io {
	abstract class IOException implements Exception
	class OSError implements Exception
	abstract class Directory implements FileSystemEntity
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
