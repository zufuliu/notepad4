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

// Kotlin Standard Library
// https://kotlinlang.org/api/latest/jvm/stdlib/index.html

//! class			============================================================
// Basic Types
// https://kotlinlang.org/docs/reference/basic-types.html
Any
Array
Boolean BooleanArray
Byte ByteArray
Char CharArray
Double DoubleArray
Enum
Error
Exception
Float FloatArray
Int IntArray
Long LongArray
Nothing
Number
Pair
Result
Short ShortArray
String
Throwable
Triple
UByte UByteArray
UInt UIntArray
ULong ULongArray
Unit
UShort UShortArray

// kotlin.collections
ArrayList
HashMap
HashSet
LinkedHashMap
LinkedHashSet

// kotlin.text
Regex
StringBuilder

//! interface		===========================================================
Annotation
CharSequence
Comparable
Comparator
Function
Lazy

// kotlin.collections
Collection
Iterable
Iterator
List
Map
Set
Grouping

//! enum
// kotlin.annotation
AnnotationRetention
AnnotationTarget

//! annotation		===========================================================
@Deprecated()
@Metadata()
@Suppress()

// kotlin.annotation
@MustBeDocumented
@Repeatable
@Retention()
@Target()


//! KDoc
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
