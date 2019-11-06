// Kotlin 1.3 https://kotlinlang.org/docs/reference/kotlin-doc.html

//! Keywords		===========================================================
// https://kotlinlang.org/docs/reference/keyword-reference.html
// Hard Keywords
as
break
class continue
do
else
false for fun
if in interface is
null
object
package
return
super
this throw true try typealias typeof
val var
when while
// Soft Keywords
by
catch constructor
delegate dynamic
field file finally
get
import init
param property
receiver
set setparam where

// Special Identifiers
field
it

// Modifier Keywords
actual abstract annotation
companion const crossinline
data
enum expect external
final
infix inline inner internal
lateinit
noinline
open operator out override
private protected public
reified
sealed suspend
tailrec
vararg

//! Library			===========================================================
// Kotlin Standard Library
// https://kotlinlang.org/api/latest/jvm/stdlib/index.html
// Basic Types
// https://kotlinlang.org/docs/reference/basic-types.html

package kotlin {
	// Types
	interface Annotation
	open class Any
	class Array<T>
	class Boolean : Comparable<Boolean>
	class BooleanArray
	class Byte : Number, Comparable<Byte>
	class ByteArray
	class Char : Comparable<Char>
	class CharArray
	interface CharSequence
	interface Comparable<in T>
	interface Comparator<T>
	enum class DeprecationLevel
	class Double : Number, Comparable<Double>
	class DoubleArray
	abstract class Enum<E : Enum<E>> : Comparable<E>
	open class Error : Throwable
	open class Exception : Throwable
	class Float : Number, Comparable<Float>
	class FloatArray
	interface Function<out R>
	class Int : Number, Comparable<Int>
	class IntArray
	interface Lazy<out T>
	enum class LazyThreadSafetyMode
	class Long : Number, Comparable<Long>
	class LongArray
	class Nothing
	abstract class Number
	data class Pair<out A, out B> : Serializable
	inline class Result<out T> : Serializable
	open class RuntimeException : Exception
	class Short : Number, Comparable<Short>
	class ShortArray
	class String : Comparable<String>, CharSequence
	open class Throwable
	data class Triple<out A, out B, out C> : Serializable
	inline class UByte : Comparable<UByte>
	inline class UByteArray : Collection<UByte>
	inline class UInt : Comparable<UInt>
	inline class UIntArray : Collection<UInt>
	inline class ULong : Comparable<ULong>
	inline class ULongArray : Collection<ULong>
	inline class UShort : Comparable<UShort>
	inline class UShortArray : Collection<UShort>
	object Unit

	// Annotations
	annotation class Deprecated(message: String, replaceWith: ReplaceWith, level: DeprecationLevel)
	annotation class Metadata()
	annotation class ReplaceWith(expression: String, vararg imports: String)
	annotation class Suppress(vararg names: String)

	// Functions
	fun assert(value: Boolean)
	fun check(value: Boolean)
	fun error(message: Any): Nothing
	fun require(value: Boolean)
}

package kotlin.annotation {
	// Types
	enum class AnnotationRetention
	enum class AnnotationTarget

	// Annotations
	annotation class MustBeDocumented
	annotation class Repeatable
	annotation class Retention(value: AnnotationRetention)
	annotation class Target(vararg allowedTargets: AnnotationTarget)
}

package kotlin.collections {
	// Types
	abstract class AbstractCollection<out E> : Collection<E>
	abstract class AbstractIterator<T> : Iterator<T>
	abstract class AbstractList<out E> : AbstractCollection<E>, List<E>
	abstract class AbstractMap<K, out V> : Map<K, V>
	abstract class AbstractMutableCollection<E> : MutableCollection<E>
	abstract class AbstractMutableList<E> : MutableList<E>
	abstract class AbstractMutableMap<K, V> : MutableMap<K, V>
	abstract class AbstractMutableSet<E> : MutableSet<E>
	abstract class AbstractSet<out E> : AbstractCollection<E>, Set<E>
	class ArrayList<E> : MutableList<E>, RandomAccess
	interface Collection<out E> : Iterable<E>
	interface Grouping<T, out K>
	class HashMap<K, V> : MutableMap<K, V>
	class HashSet<E> : MutableSet<E>
	data class IndexedValue<out T>
	interface Iterable<out T>
	interface Iterator<out T>
	class LinkedHashMap<K, V> : MutableMap<K, V>
	class LinkedHashSet<E> : MutableSet<E>
	interface List<out E> : Collection<E>
	interface ListIterator<out T> : Iterator<T>
	interface Map<K, out V>
	interface MutableCollection<E> : Collection<E>, MutableIterable<E>
    interface MutableIterable<out T> : Iterable<T>
    interface MutableIterator<out T> : Iterator<T>
    interface MutableList<E> : List<E>, MutableCollection<E>
    interface MutableListIterator<T> : ListIterator<T>, MutableIterator<T>
    interface MutableMap<K, V> : Map<K, V>
    interface MutableSet<E> : Set<E>, MutableCollection<E>
    interface RandomAccess
	interface Set<out E> : Collection<E>
}

package kotlin.io {
	// Functions
	fun print(message: Any?)
	fun println(message: Any?)
	fun readLine(): String?
}

package kotlin.random {
	// Types
	abstract class Random
}

package kotlin.text {
	// Types
	interface Appendable
	class MatchGroup
	interface MatchGroupCollection : Collection<MatchGroup?>
	interface MatchResult
	class Regex
	enum class RegexOption
	class StringBuilder : Appendable, CharSequence
}

//! KDoc			===========================================================
// https://kotlinlang.org/docs/reference/kotlin-doc.html
/**
 * @param <name>
 * @return
 * @constructor
 * @receiver
 * @property <name>
 * @throws <class>
 * @exception <class>
 * @sample <identifier>
 * @see <identifier>
 * @author
 * @since
 * @suppress
 */
