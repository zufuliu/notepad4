// 5.7 https://docs.swift.org/
// https://docs.swift.org/swift-book/
// https://swift.org/documentation/
// https://github.com/apple/swift

//! keywords		===========================================================
// Keywords and Punctuation
// https://docs.swift.org/swift-book/ReferenceManual/LexicalStructure.html
// Keywords used in declarations
associatedtype
class
deinit
enum extension
fileprivate func
import init inout internal
let
open operator
private precedencegroup protocol public
rethrows
static struct subscript
typealias
var

// Keywords used in statements
break
case catch continue
default defer do
else
fallthrough for
guard
if in
repeat return
throw
switch
where while

// Keywords used in expressions and types
Any as
catch
false
is
nil
rethrows
self Self super
throw throws
true try

// Keywords reserved in particular contexts
associativity
convenience
didSet dynamic
final
get
indirect infix
lazy left
mutating
none nonmutating
optional override
postfix precedence prefix Protocol
required right
set some
Type
unowned
weak willSet

async await

//! directive		===========================================================
// Keywords that begin with a number sign
// Compiler Control Statements
// https://docs.swift.org/swift-book/ReferenceManual/Statements.html
#if
#elseif
#else
#endif
#sourceLocation(file: file-path, line: line-number)
#error(diagnostic-message)
#warning(diagnostic-message)
#available(availability-arguments)

// Literal Expression
// https://docs.swift.org/swift-book/ReferenceManual/Expressions.html
#file
#fileID
#filePath
#line
#column
#function
#dsohandle
// playground-literal
#colorLiteral(red: expression, green: expression, blue: expression, alpha: expression)
#fileLiteral(resourceName : expression)
#imageLiteral(resourceName : expression)

// Selector Expression
#selector(method name)
#selector(getter: property name)
#selector(setter: property name)
// Key-Path String Expression
#keyPath(property name)

//! attribute		===========================================================
// https://docs.swift.org/swift-book/ReferenceManual/Attributes.html
// Declaration Attributes
@available()
@discardableResult
@dynamicCallable
@dynamicMemberLookup
@frozen
@GKInspectable
@inlinable
@main
@nonobjc
@NSApplicationMain
@NSCopying
@NSManaged
@objc
@objcMembers
@propertyWrapper
@resultBuilder
@requires_stored_property_inits
@testable
@UIApplicationMain
@usableFromInline
@warn_unqualified_access

// Type Attributes
@autoclosure
@convention
@escaping

// Switch Case Attributes
@unknown

//! library			===========================================================
// Swift Standard Library
// https://developer.apple.com/documentation/swift/swift_standard_library/
// Numbers and Basic Values
@frozen struct Bool
@frozen struct Int
@frozen struct Double
@frozen struct Float
@frozen struct Range<Bound> where Bound : Comparable
@frozen struct ClosedRange<Bound> where Bound : Comparable
protocol Error
@frozen enum Result<Success, Failure> where Failure : Error
@frozen enum Optional<Wrapped>
@frozen struct UInt
@frozen struct UInt8
@frozen struct UInt16
@frozen struct UInt32
@frozen struct UInt64
@frozen struct Int8
@frozen struct Int16
@frozen struct Int32
@frozen struct Int64
typealias Float32 = Float
typealias Float64 = Double
@frozen struct SystemRandomNumberGenerator
protocol RandomNumberGenerator
func min<T>(_ x: T, _ y: T) -> T where T : Comparable
func max<T>(_ x: T, _ y: T) -> T where T : Comparable
func abs<T>(_ x: T) -> T where T : Comparable, T : SignedNumeric

// Strings and Text
@frozen struct String
@frozen struct Character
@frozen enum Unicode
@frozen struct StaticString

// Collections
@frozen struct Array<Element>
@frozen struct Dictionary<Key, Value> where Key : Hashable
@frozen struct Set<Element> where Element : Hashable
protocol OptionSet
protocol Sequence
protocol Collection
protocol MutableCollection where Self.SubSequence : MutableCollection
func stride<T>(from start: T, to end: T, by stride: T.Stride) -> StrideTo<T> where T : Strideable
func repeatElement<T>(_ element: T, count n: Int) -> Repeated<T>
func sequence<T>(first: T, next: @escaping (T) -> T?) -> UnfoldFirstSequence<T>
func zip<Sequence1, Sequence2>(_ sequence1: Sequence1, _ sequence2: Sequence2) -> Zip2Sequence<Sequence1, Sequence2> where Sequence1 : Sequence, Sequence2 : Sequence

// Basic Behaviors
@frozen struct Slice<Base> where Base : Collection
protocol Equatable
protocol Comparable
protocol Identifiable
protocol Hashable
@frozen struct Hasher

// Input and Output
func print(_ items: Any..., separator: String = " ", terminator: String = "\n")
func readLine(strippingNewline: Bool = true) -> String?
@frozen enum CommandLine
protocol TextOutputStream

// Debugging and Reflection
func assert(_ condition: @autoclosure () -> Bool, _ message: @autoclosure () -> String = String(), file: StaticString = #file, line: UInt = #line)
func fatalError(_ message: @autoclosure () -> String = String(), file: StaticString = #file, line: UInt = #line) -> Never
struct Mirror
@frozen enum Never
func type<T, Metatype>(of value: T) -> Metatype

// Key-Path Expressions
class KeyPath<Root, Value> : PartialKeyPath<Root>
class PartialKeyPath<Root> : AnyKeyPath
class AnyKeyPath
class WritableKeyPath<Root, Value> : KeyPath<Root, Value>

// Type Casting and Existential Types
typealias AnyObject
typealias AnyClass = AnyObject.Type
typealias Void = ()
