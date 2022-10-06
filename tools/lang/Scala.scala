// 3.2 https://www.scala-lang.org/
// https://docs.scala-lang.org/

//! keywords		===========================================================
// https://docs.scala-lang.org/scala3/reference/syntax.html#keywords
abstract
case catch class
def do
else enum export extends
false final finally for
given
if implicit import
lazy
match
new null
object override
package private protected
return
sealed super
then throw trait true try type
val var
while with
yield
// Soft keywords
as
derives
end extension
infix inline
opaque open
throws transparent
using

// 2.13 https://scala-lang.org/files/archive/spec/2.13/01-lexical-syntax.html
forSome macro this


//! api				===========================================================
// 3.2 https://www.scala-lang.org/api/3.2.0/
// https://scala-lang.org/files/archive/spec/2.13/11-annotations.html
package scala {
	abstract open class Any
	final abstract class AnyKind
	class AnyRef
	abstract open class AnyVal
	trait App extends DelayedInit
	final class Array[T](_length: Int) extends Serializable with Cloneable
	final abstract class Boolean extends AnyVal
	final abstract class Byte extends AnyVal
	final abstract class Char extends AnyVal
	object Console extends AnsiColor
	final abstract class Double extends AnyVal
	trait Dynamic
	abstract class Enumeration(initial: Int) extends Serializable
	final abstract class Float extends AnyVal
	final abstract class Int extends AnyVal
	final abstract class Long extends AnyVal
	case object None extends Option[Nothing]
	final abstract open class Nothing
	final abstract open class Null
	sealed abstract class Option[+A] extends IterableOnce[A] with Product with Serializable
	trait Product extends Equals
	@SerialVersionUID()
	final abstract class Short extends AnyVal
	final case class Some[+A](value: A) extends Option[A]
	final class Symbol extends Serializable
	sealed trait Tuple extends Product
	final abstract class Unit extends AnyVal
	@deprecated()
	@deprecatedInheritance()
	@deprecatedName()
	@deprecatedOverriding()
	@inline
	@main
	@native
	@noinline
	@specialized
	@throws
	@transient
	@unchecked
	@volatile

	// https://www.scala-lang.org/api/3.2.0/scala.html
	type Error = Error
	type Exception = Exception
	type Throwable = Throwable

	// https://www.scala-lang.org/api/3.2.0/scala/Predef$.html
	def classOf[T]: Class[T]
	def identity[A](x: A): A
	def implicitly[T](implicit e: T): T
	def locally[T](x: T): T
	def valueOf[T](implicit vt: ValueOf[T]): T
	final def assert(assertion: Boolean, message: => Any): Unit
	final def assume(assumption: Boolean, message: => Any): Unit
	final def require(requirement: Boolean, message: => Any): Unit
	def print(x: Any): Unit
	def printf(text: String, xs: Any*): Unit
	def println(x: Any): Unit

	type Class[T] = Class[T]
	type Function[-A, +B] = A => B
	type String = String
}

package scala.annotation {
	abstract class Annotation
	trait ConstantAnnotation extends StaticAnnotation
	trait StaticAnnotation extends Annotation
	@compileTimeOnly()
	@constructorOnly
	@elidable()
	@experimental
	@implicitAmbiguous()
	@implicitNotFound()
	@newMain
	@nowarn
	@showAsInfix
	@static
	@strictfp
	@switch
	@tailrec
	@targetName()
	@threadUnsafe
	@unspecialized
	@unused()
	@varargs
}

package scala.annotation.meta {
	@beanGetter
	@beanSetter
	@companionClass
	@companionMethod
	@companionObject
	@field
	@getter
	@languageFeature()
	@param
	@setter
}

package scala.annotation.unchecked {
	@uncheckedStable
	@uncheckedVariance
}

package scala.beans {
	@BeanProperty
	@BooleanBeanProperty
}

package scala.collection {
	abstract class AbstractMap[K, +V] extends AbstractIterable[(K, V)] with Map[K, V]
	abstract class AbstractSeq[+A] extends AbstractIterable[A] with Seq[A]
	abstract class AbstractSet[A] extends AbstractIterable[A] with Set[A]
	trait BitSet extends SortedSet[Int] with BitSetOps[BitSet]
	trait IndexedSeq[+A] extends Seq[A]
	trait Iterable[+A] extends IterableOnce[A]
	trait Iterator[+A] extends IterableOnce[A]
	trait Map[K, +V] extends Iterable[(K, V)]
	trait Seq[+A] extends Iterable[A]
	trait Set[A] extends Iterable[A]
	trait SortedMap[K, +V] extends Map[K, V]
	trait SortedSet[A] extends Set[A]
}

package scala.collection.immutable {
	sealed abstract class ArraySeq[+A] extends AbstractSeq[A]
	final class HashMap[K, +V] extends AbstractMap[K, V]
	final class HashSet[A] extends AbstractSet[A]
	sealed abstract class List[+A] extends AbstractSeq[A]
	case object Nil extends List[Nothing]
	sealed class Queue[+A] extends AbstractSeq[A]
	trait SortedMap[K, +V] extends Map[K, V]
	trait SortedSet[A] extends Set[A]
	final class TreeMap[K, +V] extends AbstractMap[K, V]
	final class TreeSet[A] extends AbstractSet[A]
	sealed abstract class Vector[+A] extends AbstractSeq[A]
}

package scala.collection.mutable {
	class HashMap[K, V](initialCapacity: Int, loadFactor: Double) extends AbstractMap[K, V]
	final class HashSet[A](initialCapacity: Int, loadFactor: Double) extends AbstractSet[A]
}

//! scaladoc		===========================================================
// https://docs.scala-lang.org/overviews/scaladoc/for-library-authors.html
// https://docs.scala-lang.org/scala3/guides/scaladoc/docstrings.html
/**
 Class specific tags
 * @constructor
 Method specific tags
 * @return
 Method, Constructor and/or Class tags
 * @throws
 * @param
 * @tparam
 Usage tags
 * @see
 * @note
 * @example
 * @usecase
 Member grouping tags
 * @group <group>
 * @groupname <group> <name>
 * @groupdesc <group> <description>
 * @groupprio <group> <priority>
 Diagram tags
 * @contentDiagram
 * @inheritanceDiagram
 Other tags
 * @author
 * @version
 * @since
 * @todo
 * @deprecated
 * @inheritdoc
 * @documentable
 * @syntax <name>
 Macros
 * @define <name> <definition>
 2.12 tags
 * @shortDescription
 * @hideImplicitConversion
 */
