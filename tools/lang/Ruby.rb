# Ruby 3.1.1 https://www.ruby-lang.org/en/documentation/
# https://ruby-doc.org/
# https://rubyreferences.github.io/

#! Keywords			===========================================================
# keywords_rdoc.html
__ENCODING__
__LINE__
__FILE__
BEGIN
END
alias
and
break
defined?
else
elsif
ensure
false
in
next
nil
not
or
redo
rescue
retry
return
self
super
then
true
undef
when
end
yield

#! Pre-defined constants	===================================================
# globals_rdoc.html#label-Pre-defined+global+constants
TRUE
FALSE
NIL
STDIN
STDOUT
STDERR
ENV
ARGF
ARGV
DATA
TOPLEVEL_BINDING
RUBY_VERSION
RUBY_RELEASE_DATE
RUBY_PLATFORM
RUBY_REVISION
RUBY_COPYRIGHT
RUBY_ENGINE
RUBY_ENGINE_VERSION
RUBY_DESCRIPTION
SCRIPT_LINES__

#! code folding		===========================================================
begin
end
case
end
class end
end
def end
end
do
end
for
end
if
end
module end
end
unless
end
until
end
while
end

#! Pre-defined variables	===================================================
# globals_rdoc.html
$DEBUG
$LOAD_PATH
$LOADED_FEATURES
$FILENAME
$stderr
$stdin
$stdout
$VERBOSE

#! regex			===========================================================
# regex can follow keywords
and begin break case do else elsif if next not or return unless until when
end end end

#! built-in			===========================================================
module Kernel
	def Array(arg) = array
	def Complex(x[, y], exception: true) = numeric or nil
	def Float(arg, exception: true) = float or nil
	def Hash(arg) = hash
	def Integer(arg, base=0, exception: true) = integer or nil
	def Rational(arg, exception: true) = rational or nil
	def String(arg) = string
	def __callee__ = symbol
	def __dir__ = string
	def __method__ = symbol

	def abort(*args) =
	def at_exit = proc
	def autoload(module_, filename) = nil
	def binding = a_binding
	def block_given? = true or false
	def callcc = obj
	def caller(range) = array or nil
	def caller_locations(range) = array or nil
	def catch([tag]) = obj
	def chomp(string) = $_
	def chop = $_
	def clone(freeze: nil) = an_object
	def eval(string [, binding [, filename [,lineno]]]) = obj
	def exec(*args) =
	def exit(*args) =
	def exit!(*args) =
	def fail(exception [, string [, array]], cause: $!) =
	def fork = integer or nil
	def format(format_string [, arguments...] ) = string
	def frozen? = true or false
	def gets(sep, limit [, getline_args]) = string or nil
	def global_variables = array
	def gsub(pattern, replacement) = $_
	def lambda = a_proc
	def load(filename, wrap=false) = true
	def local_variables = array
	def loop = an_enumerator
	def open(path [, mode [, perm]] [, opt]) = io or nil
	def print(obj, ...) = nil
	def printf(io, string [, obj ... ]) = nil
	def proc = a_proc
	def putc(int) = int
	def puts(obj, ...) = nil
	def raise(exception [, string [, array]], cause: $!) =
	def rand(max=0) = number
	def readline(sep, limit) = string
	def readlines(sep, limit) = array
	def require(name) = true or false
	def require_relative(string) = true or false
	def select(read_array [, write_array [, error_array [, timeout]]]) = array or nil
	def set_trace_func(proc) = proc
	def sleep([duration]) = integer
	def spawn([env,] command... [,options]) = pid
	def sprintf(format_string [, arguments...] ) = string
	def srand(number = Random.new_seed) = old_seed
	def sub(pattern, replacement) = $_
	def syscall(num [, args...]) = integer
	def system([env,] command... [,options], exception: false) = true, false or nil
	def tap = obj
	def test(cmd, file1 [, file2] ) = obj
	def then = an_object
	def throw(tag [, obj]) =
	def trace_var(symbol, cmd ) = nil
	def trap( signal, command ) = obj
	def untrace_var(symbol [, cmd] ) = array or nil
	def warn(*msgs, uplevel: nil, category: nil) = nil
	def yield_self = an_object
end

class BasicObject
	def new =
	def __id__ = integer
	def object_id = integer
	def send(symbol [, args...]) = obj
	def __send__(symbol [, args...]) = obj
	def equal?(other) = true or false
	def instance_eval(string [, filename [, lineno]] ) = obj
	def instance_exec(arg...) = obj

	def method_missing(symbol [, *args] ) = result
	def singleton_method_added(symbol) =
	def singleton_method_removed(symbol) =
	def singleton_method_undefined(symbol) =
end

class Object < BasicObject
	def const_missing(c) =

	def callable_methods() =
	def define_singleton_method(symbol, method) = symbol
	def display(port=$>) = nil
	def dup = an_object
	def enum_for(method = :each, *args) = enum
	def equal?(other) = true or false
	def extend(module_, ...) = obj
	def freeze = obj
	def hash = integer
	def inspect = string
	def instance_of?(class_) = true or false
	def instance_variable_defined?(symbol) = true or false
	def instance_variable_get(symbol) = obj
	def instance_variable_set(symbol, obj) = obj
	def instance_variables = array
	def is_a?(class_) = true or false
	def itself = obj
	def kind_of?(class_) = true or false
	def matching_methods(s = '', m = callable_methods) =
	def method(sym) = method
	def methods(regular=true) = array
	def nil? = true or false
	def private_methods(all=true) = array
	def protected_methods(all=true) = array
	def public_method(sym) = method
	def public_methods(all=true) = array
	def public_send(symbol [, args...]) = obj
	def remove_instance_variable(symbol) = obj
	def respond_to?(symbol, include_all=false) = true or false
	def respond_to_missing?(symbol, include_all) = true or false
	def shortest_abbreviation(s = '', m = callable_methods) =
	def singleton_class = class_
	def singleton_method(sym) = method
	def singleton_methods(all=true) = array
	def taint = obj
	def tainted? = false
	def to_enum(method = :each, *args) = enum
	def to_s = string
	def trust = obj
	def untaint = obj
	def untrust = obj
	def untrusted? = false

	def do_until() =
	def do_while() =
end

class Module
	def constants = array
	def constants(inherited) = array
	def nesting = array
	def new = mod
	def used_modules = array

	def alias_method(new_name, old_name) = symbol
	def ancestors = array
	def attr(name, ...) = array
	def attr_accessor(symbol, ...) = array
	def attr_reader(symbol, ...) = array
	def attr_writer(symbol, ...) = array
	def autoload?(name, inherit=true) = String or nil
	def class_eval(string [, filename [, lineno]]) = obj
	def class_exec(arg...) = obj
	def class_variable_defined?(symbol) = true or false
	def class_variable_get(symbol) = obj
	def class_variable_set(symbol, obj) = obj
	def class_variables(inherit=true) = array
	def const_defined?(sym, inherit=true) = true or false
	def const_get(sym, inherit=true) = obj
	def const_missing(sym) = obj
	def const_set(sym, obj) = obj
	def const_source_location(sym, inherit=true) = [String, Integer]
	def define_method(symbol, method) = symbol
	def deprecate_constant(symbol, ...) => mod
	def freeze = mod
	def include(module_, ...) = self
	def include?(module_) = true or false
	def included_modules = array
	def inspect() =
	def instance_method(symbol) = unbound_method
	def instance_methods(include_super=true) = array
	def method_defined?(symbol, inherit=true) = true or false
	def module_eval(string [, filename [, lineno]]) = obj
	def module_exec(arg...) = obj
	def name = string
	def prepend(module_, ...) = self
	def private_class_method(symbol, ...) = mod
	def private_constant(symbol, ...) => mod
	def private_instance_methods(include_super=true) = array
	def private_method_defined?(symbol, inherit=true) = true or false
	def protected_instance_methods(include_super=true) = array
	def protected_method_defined?(symbol, inherit=true) = true or false
	def public_class_method(symbol, ...) = mod
	def public_constant(symbol, ...) => mod
	def public_instance_method(symbol) = unbound_method
	def public_instance_methods(include_super=true) = array
	def public_method_defined?(symbol, inherit=true) = true or false
	def remove_class_variable(sym) = obj
	def remove_method(symbol) = self
	def singleton_class? = true or false
	def to_s = string
	def undef_method(symbol) = self

	def append_features(mod) = mod
	def extend_object(obj) = obj
	def extended(othermod) =
	def included(othermod) =
	def method_added(method_name) =
	def method_removed(method_name) =
	def method_undefined(method_name) =
	def module_function(method_name, method_name, ...) = array
	def prepend_features(mod) = mod
	def prepended(othermod) =
	def private(method_name, method_name, ...) = array
	def protected(method_name, method_name, ...) = array
	def public(method_name, method_name, ...) = array
	def refine(mod) = module_
	def remove_const(sym) = obj
	def ruby2_keywords(method_name, ...) = nil
	def using(module_) = self

	class Refinement < Module
		def import_methods(module_, ...) = self
	end
end

#! core				===========================================================
class ARGF
end

class Array
end

class Binding
end

class Class
end

module Comparable
end

module Continuation
end

class Dir
end

class ENV
end

class Encoding
	class EncodingError < StandardError
		class CompatibilityError end
		class ConverterNotFoundError end
		class InvalidByteSequenceError end
		class UndefinedConversionError end
	end
	class Converter
	end
end

module Enumerable
end

class Enumerator
	class ArithmeticSequence
	end
	class Chain
	end
	class Generator
	end
	class Lazy
	end
	class Producer
	end
	class Yielder
	end
end

module Errno
end

class Exception
	class NoMemoryError end
	class ScriptError
		class LoadError end
		class NotImplementedError end
		class SyntaxError end
	end
	class SecurityError end
	class StandardError
		class ArgumentError
			class UncaughtThrowError end
		end
		class FiberError end
		class IOError
			class EOFError end
		end
		class IndexError
			class KeyError
			end
			class StopIteration
				class ClosedQueueError
				end
			end
		end
		class LocalJumpError end
		class NameError
			class NoMethodError end
		end
		class NoMatchingPatternError
			class NoMatchingPatternKeyError end
		end
		class RangeError
			class FloatDomainError end
		end
		class RuntimeError
			class FrozenError end
		end
		class SystemCallError end
		class ThreadError end
		class TypeError end
		class ZeroDivisionError end
	end
	class SystemExit end
	class SystemStackError end
	class SignalException
		class Interrupt end
	end
end

class FalseClass
end

class Fiber
	class SchedulerInterface
	end
end

class File < IO
	module Constants
	end
	class Stat
	end
end

module FileTest
end

module GC
	module Profiler
	end
end

class IO
	class Buffer
		class AccessError end
		class AllocationError end
		class InvalidatedError end
		class LockedError end
	end
	module WaitReadable
		class EAGAINWaitReadable end
		class EINPROGRESSWaitReadable end
		class EWOULDBLOCKWaitReadable end
	end
	module WaitWritable
		class EAGAINWaitWritable end
		class EINPROGRESSWaitWritable end
		class EWOULDBLOCKWaitWritable end
	end
end

class Hash
end

module Marshal
end

class MatchData
end

module Math
	class DomainError < StandardError end
end

class Method
end

class NilClass
end

class Numeric
	class Complex
	end
	class Float
	end
	class Integer
	end
	class Rational
	end
end

module ObjectSpace
	class WeakMap
	end
end

class Proc
end

class Pool < Fiber
end

module Process
	module GID
	end
	class Status
	end
	module Sys
	end
	module UID
	end
end

class Ractor
	class ClosedError < StopIteration end
	class Error < RuntimeError
		class IsolationError end
		class MovedError end
		class RemoteError end
		class UnsafeError end
	end
	class MovedObject < BasicObject
	end
end

class Random
	module Formatter
	end
end

class Range
end

class Ripper
end

class RubyVM
	module AbstractSyntaxTree
		class Node
		end
	end
	class InstructionSequence
	end
end

module Signal
end

class String
end

class Struct
end

class Symbol
end

class Thread
	class Backtrace
		class Location
		end
	end
	class ConditionVariable
	end
	class Mutex
	end
	class Queue
	end
	class SizedQueue
	end
end

class ThreadGroup
end

class Time
end

class TracePoint
end

class TrueClass
end

class UnboundMethod
end

module UnicodeNormalize
end

module Warning
end

#! library			===========================================================
