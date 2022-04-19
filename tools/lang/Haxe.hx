// 4.2.5 https://haxe.org

//! keywords
// https://haxe.org/manual/expression.html#keywords
abstract
break
case cast catch class continue
default do dynamic
else enum extends extern
false final
for function
if implements import
in inline interface
macro
new null
operator overload override
package private public
return
static switch
this throw true try typedef
untyped using
var
while

from to

// http://old.haxe.org/ref/keywords
callback here never super trace

//! preprocessor
// Conditional Compilation
// https://haxe.org/manual/lf-condition-compilation.html
#if
#elseif
#else
#end
#error

//! metadata
// Built-in Compiler Metadata
// https://haxe.org/manual/cr-metadata.html

//! library
package std {
	abstract Void
	abstract Float
	abstract Int to Float
	abstract Single to Float from Float
	abstract Null<T> from T to T
	abstract Bool
	abstract Dynamic<T>
	typedef Iterator<T>
	typedef Iterable<T>
	extern interface ArrayAccess<T>

	abstract Any(Dynamic) from Dynamic to Dynamic
	extern class Array<T>
	abstract Class<T>
	extern class Date
	abstract Enum<T>
	abstract EnumValue
	class EReg
	class IntIterator
	class Lambda
	extern class Math
	extern class Reflect
	extern class Std
	extern class String
	class StringBuf
	extern class Sys
	extern class Type
	enum ValueType
	abstract UInt to Int from Int
	abstract UnicodeString(String) from String to String
	class Xml
	enum abstract XmlType(Int)
}

package std.haxe {
	extern class Exception
	abstract Int32(Int) from Int to Int
	abstract Int64(__Int64) from __Int64 to __Int64
	class Json
	class Resource
	class Serializer
	// TODO: highlight template https://haxe.org/manual/std-template.html
	class Template
	class Timer
	class Unserializer
}

package std.haxe.ds {
	class GenericStack<T>
	abstract HashMap<K:{function hashCode():Int;}, V>(HashMapData<K, V>)
	extern class IntMap<T> implements haxe.Constraints.IMap<Int, T>
	class List<T>
	abstract Map<K, V>(IMap<K, V>)
	enum Option<T>
	extern class ObjectMap<K:{}, V> implements haxe.Constraints.IMap<K, V>
	extern class StringMap<T> implements haxe.Constraints.IMap<String, T>
	abstract Vector<T>(VectorData<T>)
	class WeakMap<K:{}, V> implements haxe.Constraints.IMap<K, V>
}

//! comment
/**
 @see
 */
