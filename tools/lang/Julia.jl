# https://julialang.org/
# 1.3 https://docs.julialang.org/

#! Keywords			===========================================================
# https://docs.julialang.org/en/v1/base/base/#Keywords-1
break
catch const continue
export
finally
global
else elseif
import
local
return
using
var

# reserved two-word sequences
abstract type
end
mutable struct
end
primitive type
end

# others
where
in
isa

#! Constant
ans
true
false

#! code fold		===========================================================
baremodule
end
begin
end
do
end
for
end
function
end
if
end
let
end
macro
end
module
end
quote
end
struct
end
try
end
while
end

#! Core				===========================================================
# https://docs.julialang.org/en/v1/base/base/
# base/boot.jl
module Core
	isa(x, type) -> Bool
	ifelse(condition::Bool, x, y)
	typeassert(x, type)
	typeof(x)
	tuple(xs...)
	getfield(value, name::Symbol)
	setfield!(value, name::Symbol, x)
	isdefined(m::Module, s::Symbol)

	# Memory layout
	fieldtype(T, name::Symbol | index::Int)

	# Special Types
	Any
	DataType
	Union{Types...}
	Union{}
	UnionAll
	Tuple{Types...}
	Type
	TypeVar
	NTuple{N,T}
	NamedTuple
	Vararg{T,N}
	Nothing
	const nothing
	const Cvoid
	Expr(head::Symbol, args...)
	QuoteNode
	LineNumberNode
	GlobalRef
	Symbol
	Symbol(x...) -> Symbol
	Module

	# Generic Functions
	Function
	Method
	WeakRef
	VecElement
	applicable(f, args...) -> Bool
	invoke(f, argtypes::Type, args...; kwargs...)

	# Syntax
	eval(m::Module, expr)

	# Errors
	Exception
	throw(e)

	# Reflection
	nfields(x) -> Int
end

module Base
	# functions marked as keyword in the document
	new()
	ccall((function_name, library), returntype, (argtype1, ...), argvalue1, ...)

	# others
	include([m::Module,] path::AbstractString)

	nameof(t::DataType) -> Symbol
	sizeof(T::DataType)

	error(message::AbstractString)
	rethrow([e])

	signed(x)
	unsigned(x) -> Unsigned
	float(x)
	complex(r, [i])
	string(n::Integer; base::Integer = 10, pad::Integer = 1)

	show(x)
	print([io::IO], xs...)
	println([io::IO], xs...)
	repr(mime, x; context=nothing)
end

module Main end

#! Modules			===========================================================
# https://docs.julialang.org/en/v1/base/base/
# base/exports.jl
module Base
	exit(code=0)
	atexit(f)
	isinteractive() -> Bool
	summarysize(obj; exclude=Union{...}, chargeall=Union{...}) -> Int
	require(into::Module, module::Symbol)
	compilecache(module::PkgId)
	__precompile__(isprecompilable::Bool)
	include_string(m::Module, code::AbstractString, filename::AbstractString="string")
	include_dependency(path::AbstractString)
	which(f, types)
	methods(f, [types])
	@show
	isequal(x, y)
	isless(x, y)
	ntuple(f::Function, n::Integer)
	objectid(x)
	hash(x[, h::UInt])
	finalizer(f, x)
	finalize(x)
	copy(x)
	deepcopy(x)
	getproperty(value, name::Symbol)
	setproperty!(value, name::Symbol, x)
	propertynames(x, private=false)
	hasproperty(x, s::Symbol)
	@isdefined s -> Bool
	convert(T, x)
	promote(xs...)
	oftype(x, y)
	widen(x)
	identity(x)

	# Properties of Types
	supertype(T::DataType)
	typejoin(T, S)
	typeintersect(T, S)
	promote_type(type1, type2)
	promote_rule(type1, type2)
	isdispatchtuple(T)

	# Declared structure
	isimmutable(v) -> Bool
	isabstracttype(T)
	isprimitivetype(T) -> Bool
	isstructtype(T) -> Bool
	fieldnames(x::DataType)
	fieldname(x::DataType, i::Integer)
	hasfield(T::Type, name::Symbol)

	# Memory layout
	isconcretetype(T)
	isbits(x)
	isbitstype(T)
	fieldtypes(T::Type)
	fieldcount(t::Type)
	fieldoffset(type, i)
	datatype_alignment(dt::DataType) -> Int
	datatype_haspadding(dt::DataType) -> Bool
	datatype_pointerfree(dt::DataType) -> Bool

	# Special values
	typemin(T)
	typemax(T)
	floatmin(T)
	floatmax(T)
	maxintfloat(T=Float64)
	eps(::Type{T}) where T<:AbstractFloat
	instances(T::Type)

	# Special Types
	Val{c}()
	isnothing(x)
	Some{T}
	something(x, y...)
	Enum{T<:Integer}
		@enum EnumName[::BaseType] value1[=x] value2[=y]

	# Generic Functions
	hasmethod(f, t::Type{<:Tuple}[, kwnames]; world=typemax(UInt)) -> Bool
	invokelatest(f, args...; kwargs...)

	# Syntax
	@eval [mod,] ex
	evalfile(path::AbstractString, args::Vector{String}=String[])
	esc(e)
	@inbounds(blk)
	@boundscheck(blk)
	@propagate_inbounds
	@inline
	@noinline
	@nospecialize
	@specialize
	gensym([tag])
	@gensym
	@goto name
	@label name
	@polly
	@generated f
	@pure ex
	@deprecate old new [ex=true]

	# Missing Values
	Missing
	const missing = Missing()
	coalesce(x, y...)
	ismissing(x)
	skipmissing(itr)

	# System
	run(command, args...; wait::Bool = true)
	const devnull
	success(command)
	process_running(p::Process)
	process_exited(p::Process)
	kill(p::Process, signum=SIGTERM)
	ignorestatus(command)
	detach(command)
	Cmd(cmd::Cmd; ignorestatus, detach, windows_verbatim, windows_hide, env, dir)
	setenv(command::Cmd, env; dir="")
	withenv(f::Function, kv::Pair...)
	pipeline(from, to, ...)
	time_ns()
	@time
	@timev
	@timed
	@elapsed
	@allocated
	EnvDict() -> EnvDict
	const ENV
	@static

	# Versioning
	VersionNumber

	# Events
	Timer(callback::Function, delay; interval = 0)
	AsyncCondition()

	# Reflection
	parentmodule(m::Module) -> Module
	pathof(m::Module)
	moduleroot(m::Module) -> Module
	@__MODULE__ -> Module
	fullname(m::Module)
	names(x::Module; all::Bool = false, imported::Bool = false)
	isconst(m::Module, s::Symbol) -> Bool
	functionloc(f::Function, types)

	# Internals
	macroexpand(m::Module, x; recursive=true)
	@macroexpand
	@macroexpand1
	code_lowered(f, types; generated=true, debuginfo=:default)
	code_typed(f, types; optimize=true, debuginfo=:default)
	precompile(f, args::Tuple{Vararg{Any}})

	module Docs
	end

	module Meta
		# Internals
		lower(m, x)
		@lower [m] x
		parse(str, start; greedy=true, raise=true, depwarn=true)
		@dump expr
	end

	module Sys
		# System
		set_process_title(title::AbstractString)
		get_process_title()
	end

	module Threads
	end

	module GC
		# Internals
		gc()
		enable(on::Bool)
		@preserve x1 x2 ... xn expr
	end
end

# Errors
# https://docs.julialang.org/en/v1/base/base/#Errors-1
module Core
	ArgumentError(msg)
	AssertionError([msg])
	BoundsError([a],[i])
	DivideError()
	DomainError(val)
	ErrorException(msg)
	InexactError(name::Symbol, T, val)
	InterruptException()
	LoadError(file::AbstractString, line::Int, error)
	MethodError(f, args)
	OutOfMemoryError()
	ReadOnlyMemoryError()
	OverflowError(msg)
	StackOverflowError()
	SegmentationFault
	TypeError(func::Symbol, context::AbstractString, expected::Type, got)
	UndefKeywordError(var::Symbol)
	UndefRefError()
	UndefVarError(var::Symbol)
	InitError(mod::Symbol, error)
end

module Base
	backtrace()
	catch_backtrace()
	catch_stack(task=current_task(); [inclue_bt=true])
	@assert cond [text]
	CapturedException
	CompositeException
	DimensionMismatch([msg])
	EOFError()
	InvalidStateException
	KeyError(key)
	MissingException(msg)
	ProcessFailedException
	SystemError(prefix::AbstractString, [errno::Int32])
	StringIndexError(str, i)
	retry(f;  delays=ExponentialBackOff(), check=nothing) -> Function
	ExponentialBackOff(; n=1, first_delay=0.05, max_delay=10.0, factor=5.0, jitter=0.1)
end

# Collections and Data Structures
# https://docs.julialang.org/en/v1/base/collections/
module Base
	iterate(iter [, state]) -> Union{Nothing, Tuple{Any, Any}}
	IteratorSize(itertype::Type) -> IteratorSize
	IteratorEltype(itertype::Type) -> IteratorEltype

	# Constructors and Types
	AbstractRange{T}
	OrdinalRange{T, S} <: AbstractRange{T}
	AbstractUnitRange{T} <: OrdinalRange{T, T}
	StepRange{T, S} <: OrdinalRange{T, S}
	UnitRange{T<:Real}
	LinRange{T}

	# General Collections
	isempty(collection) -> Bool
	empty!(collection) -> collection
	length(collection) -> Integer

	# Iterable Collections
	in(item, collection) -> Bool
	eltype(type)
	indexin(a, b)
	unique(itr)
	unique!(f, A::AbstractVector)
	allunique(itr) -> Bool
	reduce(op, itr; [init])
	foldl(op, itr; [init])
	foldr(op, itr; [init])
	maximum(f, itr)
	maximum!(r, A)
	minimum(f, itr)
	minimum!(r, A)
	extrema(itr) -> Tuple
	argmax(itr) -> Integer
	argmin(itr) -> Integer
	findmax(itr) -> (x, index)
	findmin(itr) -> (x, index)
	findmax!(rval, rind, A) -> (maxval, index)
	findmin!(rval, rind, A) -> (minval, index)
	sum(f, itr)
	sum!(r, A)
	prod(f, itr)
	prod!(r, A)
	any(itr) -> Bool
	any!(r, A)
	all!(r, A)
	count(p, itr) -> Integer
	any(p, itr) -> Bool
	all(p, itr) -> Bool
	foreach(f, c...) -> Nothing
	map(f, c...) -> collection
	map!(function, destination, collection...)
	mapreduce(f, op, itrs...; [init])
	mapfoldl(f, op, itr; [init])
	mapfoldr(f, op, itr; [init])
	first(coll)
	last(coll)
	front(x::Tuple)::Tuple
	tail(x::Tuple)::Tuple
	step(r)
	collect(collection)
	filter(f, a::AbstractArray)
	filter!(f, a::AbstractVector)
	replace(A, old_new::Pair...; [count::Integer])
	replace!(A, old_new::Pair...; [count::Integer])

	# Indexable Collections
	getindex(collection, key...)
	setindex!(collection, value, key...)
	firstindex(collection) -> Integer
	lastindex(collection) -> Integer

	# Dictionaries
	AbstractDict{K, V}
	Dict([itr])
	IdDict([itr])
	WeakKeyDict([itr])
	ImmutableDict(KV::Pair)
	haskey(collection, key) -> Bool
	get(collection, key, default)
	get!(collection, key, default)
	getkey(collection, key, default)
	delete!(collection, key)
	pop!(collection, key[, default])
	keys(iterator)
	values(iterator)
	pairs(collection)
	merge(d::AbstractDict, others::AbstractDict...)
	merge!(d::AbstractDict, others::AbstractDict...)
	sizehint!(s, n)
	keytype(T::Type{<:AbstractArray})
	valtype(T::Type{<:AbstractArray})

	# Set-Like Collections
	AbstractSet{T}
	Set([itr])
	BitSet([itr])
	union(s, itrs...)
	union!(s::Union{AbstractSet,AbstractVector}, itrs...)
	intersect(s, itrs...)
	setdiff(s, itrs...)
	symdiff(s, itrs...)
	intersect!(s::Union{AbstractSet,AbstractVector}, itrs...)
	issubset(a, b)
	issetequal(a, b)

	# Dequeues
	push!(collection, items...) -> collection
	pop!(collection) -> item
	pushfirst!(collection, items...) -> collection
	popfirst!(collection) -> item
	insert!(a::Vector, index::Integer, item)
	deleteat!(a::Vector, i::Integer)
	splice!(a::Vector, index::Integer, [replacement]) -> item
	resize!(a::Vector, n::Integer) -> Vector
	append!(collection, collection2) -> collection.
	prepend!(a::Vector, items) -> collection

	# Utility Collections
	Pair(x, y)
end

# Mathematics
# https://docs.julialang.org/en/v1/base/math/
module Base
	fma(x, y, z)
	muladd(x, y, z)
	inv(x)
	div(x, y)
	fld(x, y)
	cld(x, y)
	mod(x, y)
	rem(x, y, RoundDown)

	divrem(x, y)
	fldmod(x, y)
	mod1(x, y)
	fldmod1(x, y)
	rationalize([T<:Integer=Int,] x; tol::Real=eps(x))
	numerator(x)
	denominator(x)
	range(start[, stop]; length, stop, step=1)
	OneTo(n)
	StepRangeLen{T,R,S}(ref::R, step::S, len, [offset=1]) where {T,R,S}
	cmp(x,y)
	xor(x, y)

	# Mathematical Functions
	isapprox(x, y; rtol::Real=atol>0 ? 0 : √eps, atol::Real=0, nans::Bool=false, norm::Function)
	sin(x)
	cos(x)
	tan(x)
	sinh(x)
	cosh(x)
	tanh(x)
	asin(x)
	acos(x)
	atan(y)
	asinh(x)
	acosh(x)
	atanh(x)
	log(x)
	log2(x)
	log10(x)
	log1p(x)
	exp(x)
	exp2(x)
	exp10(x)
	expm1(x)
	round([T,] x, [r::RoundingMode])
	ceil([T,] x)
	floor([T,] x)
	trunc([T,] x)
	unsafe_trunc(T, x)
	min(x, y, ...)
	abs(x)
	abs2(x)
	copysign(x, y) -> z
	sign(x)
	signbit(x)
	flipsign(x, y)
	sqrt(x)
	isqrt(n::Integer)
	real(z)
	imag(z)
	reim(z)
	conj(z)
	angle(z)
	cis(z)
	binomial(n::Integer, k::Integer)
	factorial(n::Integer)
	gcd(x,y)
	lcm(x,y)
	gcdx(x,y)
	ispow2(n::Integer) -> Bool
	nextpow(a, x)
	prevpow(a, x)
	nextprod([k_1, k_2,...], n)
	invmod(x,m)
	powermod(x::Integer, p::Integer, m)
	ndigits(n::Integer; base::Integer=10, pad::Integer=1)
	widemul(x, y)

	module FastMath
		@fastmath expr
	end
end

module Base.Math
	rem2pi(x, r::RoundingMode)
	mod2pi(x)

	# Mathematical Functions
	sincos(x)
	sind(x)
	cosd(x)
	tand(x)
	sinpi(x)
	cospi(x)
	asind(x)
	acosd(x)
	atand(y)
	atand(y,x)
	sec(x)
	csc(x)
	cot(x)
	secd(x)
	cscd(x)
	cotd(x)
	asec(x)
	acsc(x)
	acot(x)
	asecd(x)
	acscd(x)
	acotd(x)
	sech(x)
	csch(x)
	coth(x)
	asech(x)
	acsch(x)
	acoth(x)
	sinc(x)
	cosc(x)
	cosc(x)
	deg2rad(x)
	rad2deg(x)
	hypot(x, y)
	frexp(val)
	ldexp(x, n)
	modf(x)
	clamp(x, lo, hi)
	clamp!(array::AbstractArray, lo, hi)
	cbrt(x::Real)
	@evalpoly(z, c...)
end

module Base.Checked
	checked_abs(x)
	checked_neg(x)
	checked_add(x, y)
	checked_sub(x, y)
	checked_mul(x, y)
	checked_div(x, y)
	checked_rem(x, y)
	checked_fld(x, y)
	checked_mod(x, y)
	checked_cld(x, y)
	add_with_overflow(x, y) -> (r, f)
	sub_with_overflow(x, y) -> (r, f)
	mul_with_overflow(x, y) -> (r, f)
end

# Numbers
# https://docs.julialang.org/en/v1/base/numbers/
module Core
	# Standard Numeric Types
	Number
	Real <: Number
	AbstractFloat <: Real
	Integer <: Real
	Signed <: Integer
	Int
	Unsigned <: Integer
	UInt
	AbstractIrrational <: Real

	# Concrete number types
	Float16 <: AbstractFloat
	Float32 <: AbstractFloat
	Float64 <: AbstractFloat
	Bool <: Integer
	Int8 <: Signed
	UInt8 <: Unsigned
	Int16 <: Signed
	UInt16 <: Unsigned
	Int32 <: Signed
	UInt32 <: Unsigned
	Int64 <: Signed
	UInt64 <: Unsigned
	Int128 <: Signed
	UInt128 <: Unsigned
end

module Base
	# Concrete number types
	Complex{T<:Real} <: Number
	ComplexF64
	ComplexF32
	ComplexF16
	Rational{T<:Integer} <: Real
	Irrational{sym} <: AbstractIrrational

	module MPFR
		BigFloat <: AbstractFloat
		BigInt <: Signed
	end

	# Data Formats
	digits([T<:Integer], n::Integer; base::T = 10, pad::Integer = 1)
	digits!(array, n::Integer; base::Integer = 10)
	bitstring(n)
	parse(type, str; base)
	tryparse(type, str; base)
	big(x)

	module Math
		significand(x)
		exponent(x) -> Int
	end

	bswap(n)
	hex2bytes(s::Union{AbstractString,AbstractVector{UInt8}})
	hex2bytes!(d::AbstractVector{UInt8}, s::Union{String,AbstractVector{UInt8}})
	bytes2hex(a::AbstractArray{UInt8}) -> String

	# General Number Functions and Constants
	one(x)
	oneunit(x::T)
	zero(x)

	const im
	const Inf
	const Inf64
	const Inf32
	const Inf16
	const NaN
	const NaN64
	const NaN32
	const NaN16

	module MathConstants
		const pi
		const catalan
		const eulergamma
		const golden
		const catalan
	end

	issubnormal(f) -> Bool
	isfinite(f) -> Bool
	isinf(f) -> Bool
	isnan(f) -> Bool
	iszero(x)
	isone(x)
	nextfloat(x::AbstractFloat, n::Integer)
	prevfloat(x::AbstractFloat, n::Integer)
	isinteger(x) -> Bool
	isreal(x) -> Bool

	module Rounding
		RoundingMode
		const RoundNearest
		const RoundNearestTiesAway
		const RoundNearestTiesUp
		const RoundToZero
		const RoundFromZero
		const RoundUp
		const RoundDown
		rounding(T)
		setrounding(T, mode)
		get_zero_subnormals() -> Bool
		set_zero_subnormals(yes::Bool) -> Bool
	end

	# Integers
	count_ones(x::Integer) -> Integer
	count_zeros(x::Integer) -> Integer
	leading_zeros(x::Integer) -> Integer
	leading_ones(x::Integer) -> Integer
	trailing_zeros(x::Integer) -> Integer
	trailing_ones(x::Integer) -> Integer
	isodd(x::Integer) -> Bool
	iseven(x::Integer) -> Bool

	# BigFloats and BigInts
	precision(num::AbstractFloat)
end

# Strings
# https://docs.julialang.org/en/v1/base/strings/
module Core
	AbstractChar
	Char(c::Union{Number,AbstractChar})
	AbstractString
	String(s::AbstractString)
	IO
end

module Base
	codepoint(c::AbstractChar) -> Integer
	length(s::AbstractString) -> Int
	repeat(s::AbstractString, r::Integer)
	repr(x; context=nothing)
	SubString(s::AbstractString, i::Integer, j::Integer=lastindex(s))
	transcode(T, src)
	unsafe_string(p::Ptr{UInt8}, [length::Integer])
	ncodeunits(s::AbstractString) -> Int
	codeunit(s::AbstractString) -> Type{<:Union{UInt8, UInt16, UInt32}}
	codeunits(s::AbstractString)
	ascii(s::AbstractString)
	Regex
	RegexMatch
	SubstitutionString(substr)
	isvalid(value) -> Bool
	match(r::Regex, s::AbstractString[, idx::Integer[, addopts]])
	eachmatch(r::Regex, s::AbstractString; overlap::Bool=false)
	isless(a::AbstractString, b::AbstractString) -> Bool
	cmp(a::AbstractString, b::AbstractString) -> Int
	lpad(s, n::Integer, p::Union{AbstractChar,AbstractString}=' ') -> String
	rpad(s, n::Integer, p::Union{AbstractChar,AbstractString}=' ') -> String
	findfirst(pattern::AbstractString, string::AbstractString)
	findnext(pattern::AbstractString, string::AbstractString, start::Integer)
	findlast(pattern::AbstractString, string::AbstractString)
	findprev(pattern::AbstractString, string::AbstractString, start::Integer)
	occursin(needle::Union{AbstractString,Regex,AbstractChar}, haystack::AbstractString)
	reverse(s::AbstractString) -> AbstractString
	replace(s::AbstractString, pat=>r; [count::Integer])
	split(str::AbstractString, dlm; limit::Integer=0, keepempty::Bool=true)
	rsplit(s::AbstractString; limit::Integer=0, keepempty::Bool=false)
	strip([pred=isspace,] str::AbstractString)
	lstrip([pred=isspace,] str::AbstractString)
	rstrip([pred=isspace,] str::AbstractString)
	startswith(s::AbstractString, prefix::AbstractString)
	endswith(s::AbstractString, suffix::AbstractString)
	first(s::AbstractString, n::Integer)
	last(s::AbstractString, n::Integer)

	module Unicode
		uppercase(s::AbstractString)
		lowercase(s::AbstractString)
		titlecase(s::AbstractString; [wordsep::Function], strict::Bool=true) -> String
		uppercasefirst(s::AbstractString) -> String
		lowercasefirst(s::AbstractString)
		textwidth(c)
		iscntrl(c::AbstractChar) -> Bool
		isdigit(c::AbstractChar) -> Bool
		isletter(c::AbstractChar) -> Bool
		islowercase(c::AbstractChar) -> Bool
		isnumeric(c::AbstractChar) -> Bool
		isprint(c::AbstractChar) -> Bool
		ispunct(c::AbstractChar) -> Bool
		isspace(c::AbstractChar) -> Bool
		isuppercase(c::AbstractChar) -> Bool
		isxdigit(c::AbstractChar) -> Bool
	end

	join([io::IO,] strings, delim, [last])
	chop(s::AbstractString; head::Integer = 0, tail::Integer = 1)
	chomp(s::AbstractString)
	thisind(s::AbstractString, i::Integer) -> Int
	nextind(str::AbstractString, i::Integer, n::Integer=1) -> Int
	prevind(str::AbstractString, i::Integer, n::Integer=1) -> Int
	isascii(c::Union{AbstractChar,AbstractString}) -> Bool
	escape_string(str::AbstractString[, esc])::AbstractString
	unescape_string(str::AbstractString)::AbstractString
end

# Arrays
# https://docs.julialang.org/en/v1/base/arrays/
module Core
	# Constructors and Types
	AbstractArray{T,N}
	DenseArray
	Array{T,N} <: AbstractArray{T,N}
	UndefInitializer
	const undef
end

module Base
	# Constructors and Types
	AbstractVector{T}
	AbstractMatrix{T}
	AbstractVecOrMat{T}
	Vector{T} <: AbstractVector{T}
	Matrix{T} <: AbstractMatrix{T}
	VecOrMat{T}
	DenseVector{T}
	DenseMatrix{T}
	DenseVecOrMat{T}
	StridedArray{T, N}
	StridedVector{T}
	SubArray
	StridedMatrix{T}
	StridedVecOrMat{T}

	getindex(type[, elements...])
	zeros([T=Float64,] dims...)
	ones([T=Float64,] dims...)
	BitArray{N} <: AbstractArray{Bool, N}
	BitVector
	BitMatrix
	trues(dims)
	falses(dims)
	fill(x, dims)
	fill!(A, x)
	similar(array, [element_type=eltype(array)], [dims=size(array)])
	similar(storagetype, axes)

	# Basic functions
	ndims(A::AbstractArray) -> Integer
	size(A::AbstractArray, [dim])
	axes(A)
	axes(A, d)
	length(A::AbstractArray)
	eachindex(A...)
	IndexStyle(A)
	IndexLinear()
	IndexCartesian()
	conj!(A)
	stride(A, k::Integer)
	strides(A)

	# Broadcast and vectorization
	module Broadcast
		broadcast(f, As...)
		broadcast!(f, dest, As...)
		BroadcastStyle
		AbstractArrayStyle{N} <: BroadcastStyle
		ArrayStyle{MyArrayType}()
		DefaultArrayStyle{N}()
		broadcastable(x)
		combine_axes(As...) -> Tuple
		combine_styles(cs...) -> BroadcastStyle
		result_style(s1::BroadcastStyle[, s2::BroadcastStyle]) -> BroadcastStyle
	end

	getindex(A, inds...)
	setindex!(A, X, inds...)
	copyto!(dest, Rdest::CartesianIndices, src, Rsrc::CartesianIndices) -> dest
	isassigned(array, i) -> Bool
	Colon()

	module IteratorsMD
		CartesianIndex(i, j, k...)   -> I
		CartesianIndex((i, j, k...)) -> I
		CartesianIndices(sz::Dims) -> R
		CartesianIndices((istart:istop, jstart:jstop, ...)) -> R
	end

	Dims{N}
	LinearIndices(A::AbstractArray)
	to_indices(A, I::Tuple)
	checkbounds(Bool, A, I...)
	checkindex(Bool, inds::AbstractUnitRange, index)

	# Views (SubArrays and other view types)
	view(A, inds...)
	@view A[inds...]
	@views expression
	parent(A)
	parentindices(A)
	selectdim(A, d::Integer, i)
	reinterpret(type, A)
	reshape(A, dims...) -> AbstractArray
	dropdims(A; dims)
	vec(a::AbstractArray) -> AbstractVector

	# Concatenation and permutation
	cat(A...; dims=dims)
	vcat(A...)
	hcat(A...)
	hvcat(rows::Tuple{Vararg{Int}}, values...)
	vect(X...)
	circshift(A, shifts)
	circshift!(dest, src, shifts)
	circcopy!(dest, src)
	findall(A)
	findall(f::Function, A)
	findfirst(A)
	findlast(A)
	findnext(A, i)
	findprev(A, i)
	permutedims(A::AbstractArray, perm)
	permutedims!(dest, src, perm)
	PermutedDimsArray(A, perm) -> B
	promote_shape(s1, s2)

	# Array functions
	accumulate(op, A; dims::Integer, [init])
	accumulate!(op, B, A; [dims], [init])
	cumprod(A; dims::Integer)
	cumprod!(B, A; dims::Integer)
	cumsum(A; dims::Integer)
	cumsum!(B, A; dims::Integer)
	diff(A::AbstractVector)
	repeat(A::AbstractArray, counts::Integer...)
	rot180(A)
	rotl90(A)
	rotr90(A)
	mapslices(f, A; dims)

	# Combinatorics
	invperm(v)
	isperm(v) -> Bool
	permute!(v, p)
	reverse(v [, start=1 [, stop=length(v) ]] )
	reverseind(v, i)
	reverse!(v [, start=1 [, stop=length(v) ]]) -> v
end

# Tasks
# https://docs.julialang.org/en/v1/base/parallel/
module Core
	Task(func)
end

module Base
	@task
	@async
	@sync
	asyncmap(f, c...; ntasks=0, batch_size=nothing)
	asyncmap!(f, results, c...; ntasks=0, batch_size=nothing)
	fetch(t::Task)
	current_task()
	istaskdone(t::Task) -> Bool
	istaskstarted(t::Task) -> Bool
	task_local_storage(key)

	# Scheduling
	yield()
	yieldto(t::Task, arg = nothing)
	sleep(seconds)
	wait([x])
	timedwait(testcb::Function, secs::Float64; pollint::Float64=0.1)
	Condition()
	notify(condition, val=nothing; all=true, error=false)
	schedule(t::Task, [val]; error=false)
	Event()
	Semaphore(sem_size)
	acquire(s::Semaphore)
	release(s::Semaphore)
	AbstractLock
	lock(lock)
	unlock(lock)
	trylock(lock) -> Success (Boolean)
	islocked(lock) -> Status (Boolean)
	ReentrantLock()
	AbstractChannel
	Channel{T}(sz::Int)
	put!(c::Channel, v)
	take!(c::Channel)
	isready(c::Channel)
	fetch(c::Channel)
	close(c::Channel[, excp::Exception])
	bind(chnl::Channel, task::Task)
end

# Multi-Threading
# https://docs.julialang.org/en/v1/base/multi-threading/
module Base.Threads
	threadid()
	nthreads()
	@threads
	Atomic{T}
	atomic_cas!(x::Atomic{T}, cmp::T, newval::T) where T
	atomic_xchg!(x::Atomic{T}, newval::T) where T
	atomic_add!(x::Atomic{T}, val::T) where T <: ArithmeticTypes
	atomic_sub!(x::Atomic{T}, val::T) where T <: ArithmeticTypes
	atomic_and!(x::Atomic{T}, val::T) where T
	atomic_nand!(x::Atomic{T}, val::T) where T
	atomic_or!(x::Atomic{T}, val::T) where T
	atomic_xor!(x::Atomic{T}, val::T) where T
	atomic_max!(x::Atomic{T}, val::T) where T
	atomic_min!(x::Atomic{T}, val::T) where T
	atomic_fence()
	Mutex()
	SpinLock()
end

# Constants
# https://docs.julialang.org/en/v1/base/constants/
module Core
	const nothing
end

module Base
	const PROGRAM_FILE
	const ARGS
	const C_NULL
	const VERSION
	const DEPOT_PATH
	const LOAD_PATH

	module Sys
		const BINDIR
		const CPU_THREADS
		const WORD_SIZE
		const KERNEL
		const ARCH
		const MACHINE
	end
end

# Filesystem
# https://docs.julialang.org/en/v1/base/file/
module Base
	download(url::AbstractString, [localfile::AbstractString])
end

module Base.Filesystem
	pwd() -> AbstractString
	cd(dir::AbstractString=homedir())
	cd(f::Function, dir::AbstractString=homedir())
	readdir(dir::AbstractString=".") -> Vector{String}
	walkdir(dir; topdown=true, follow_symlinks=false, onerror=throw)
	mkdir(path::AbstractString; mode::Unsigned = 0o777)
	mkpath(path::AbstractString; mode::Unsigned = 0o777)
	symlink(target::AbstractString, link::AbstractString)
	readlink(path::AbstractString) -> AbstractString
	chmod(path::AbstractString, mode::Integer; recursive::Bool=false)
	chown(path::AbstractString, owner::Integer, group::Integer=-1)
	RawFD
	stat(file)
	lstat(file)
	ctime(file)
	mtime(file)
	filemode(file)
	filesize(path...)
	uperm(file)
	gperm(file)
	operm(file)
	cp(src::AbstractString, dst::AbstractString; force::Bool=false, follow_symlinks::Bool=false)
	mv(src::AbstractString, dst::AbstractString; force::Bool=false)
	rm(path::AbstractString; force::Bool=false, recursive::Bool=false)
	touch(path::AbstractString)
	tempname()
	tempdir()
	mktemp(parent=tempdir())
	mktempdir(parent=tempdir(); prefix="jl_")
	isblockdev(path) -> Bool
	ischardev(path) -> Bool
	isdir(path) -> Bool
	isfifo(path) -> Bool
	isfile(path) -> Bool
	islink(path) -> Bool
	ismount(path) -> Bool
	ispath(path) -> Bool
	issetgid(path) -> Bool
	issetuid(path) -> Bool
	issocket(path) -> Bool
	issticky(path) -> Bool
	homedir() -> AbstractString
	dirname(path::AbstractString) -> AbstractString
	basename(path::AbstractString) -> AbstractString
	@__FILE__ -> AbstractString
	@__DIR__ -> AbstractString
	@__LINE__ -> Int
	isabspath(path::AbstractString) -> Bool
	isdirpath(path::AbstractString) -> Bool
	joinpath(parts...) -> AbstractString
	abspath(path::AbstractString) -> AbstractString
	abspath(path::AbstractString, paths::AbstractString...) -> AbstractString
	normpath(path::AbstractString) -> AbstractString
	realpath(path::AbstractString) -> AbstractString
	relpath(path::AbstractString, startpath::AbstractString = ".") -> AbstractString
	expanduser(path::AbstractString) -> AbstractString
	splitdir(path::AbstractString) -> (AbstractString, AbstractString)
	splitdrive(path::AbstractString) -> (AbstractString, AbstractString)
	splitext(path::AbstractString) -> (AbstractString, AbstractString)
	splitpath(path::AbstractString) -> Vector{String}
end

# I/O and Network
# https://docs.julialang.org/en/v1/base/io-network/
module Base
	const stdout
	const stderr
	const stdin
	open(filename::AbstractString; keywords...) -> IOStream
	IOStream
	IOBuffer([data::AbstractVector{UInt8}]; keywords...) -> IOBuffer
	take!(b::IOBuffer)
	fdio([name::AbstractString, ]fd::Integer[, own::Bool=false]) -> IOStream
	flush(stream)
	close(stream)
	write(io::IO, x)
	read(io::IO, T)
	readbytes!(stream::IO, b::AbstractVector{UInt8}, nb=length(b))
	unsafe_read(io::IO, ref, nbytes::UInt)
	unsafe_write(io::IO, ref, nbytes::UInt)
	position(s)
	seek(s, pos)
	seekstart(s)
	seekend(s)
	skip(s, offset)
	mark(s)
	unmark(s)
	reset(s)
	ismarked(s)
	eof(stream) -> Bool
	isreadonly(io) -> Bool
	iswritable(io) -> Bool
	isreadable(io) -> Bool
	isopen(object) -> Bool

	module Grisu
		print_shortest(io::IO, x)
	end

	Pipe
	fd(stream)
	redirect_stdout([stream]) -> (rd, wr)
	redirect_stderr([stream]) -> (rd, wr)
	redirect_stdin([stream]) -> (rd, wr)
	readchomp(x)
	truncate(file, n)
	skipchars(predicate, io::IO; linecomment=nothing)
	countlines(io::IO; eol::AbstractChar = '\n')
	PipeBuffer(data::Vector{UInt8}=UInt8[]; maxsize::Integer = typemax(Int))
	readavailable(stream)
	IOContext

	# Text I/O
	summary(io::IO, x)
	printstyled([io], xs...; bold::Bool=false, color::Union{Symbol,Int}=:normal)
	sprint(f::Function, args...; context=nothing, sizehint=0)
	showerror(io, e)
	dump(x; maxdepth=8)
	readline(io::IO=stdin; keep::Bool=false)
	readuntil(stream::IO, delim; keep::Bool = false)
	readlines(io::IO=stdin; keep::Bool=false)
	eachline(io::IO=stdin; keep::Bool=false)
	displaysize([io::IO]) -> (lines, columns)

	# Multimedia I/O
	module Multimedia
		AbstractDisplay
		display(x)
		redisplay(x)
		displayable(mime) -> Bool
		showable(mime, x)
		MIME
		pushdisplay(d::AbstractDisplay)
		popdisplay()
		TextDisplay(io::IO)
		istextmime(m::MIME)
	end

	# Network I/O
	bytesavailable(io)
	ntoh(x)
	hton(x)
	ltoh(x)
	htol(x)
	const ENDIAN_BOM
end

# Sorting and Related Functions
# https://docs.julialang.org/en/v1/base/sort/
module Base
	sort!(v; alg::Algorithm=defalg(v), lt=isless, by=identity, rev::Bool=false, order::Ordering=Forward)
	sort(v; alg::Algorithm=defalg(v), lt=isless, by=identity, rev::Bool=false, order::Ordering=Forward)
	sortperm(v; alg::Algorithm=DEFAULT_UNSTABLE, lt=isless, by=identity, rev::Bool=false, order::Ordering=Forward)

	module Sort
		const InsertionSort
		const MergeSort
		const QuickSort
		PartialQuickSort{T <: Union{Int,OrdinalRange}}

		# Order-Related Functions
		searchsorted(a, x; by=<transform>, lt=<comparison>, rev=false)
		searchsortedfirst(a, x; by=<transform>, lt=<comparison>, rev=false)
		searchsortedlast(a, x; by=<transform>, lt=<comparison>, rev=false)
		partialsort!(v, k; by=<transform>, lt=<comparison>, rev=false)
		partialsort(v, k, by=<transform>, lt=<comparison>, rev=false)
		partialsortperm(v, k; by=<transform>, lt=<comparison>, rev=false)
		partialsortperm!(ix, v, k; by=<transform>, lt=<comparison>, rev=false, initialized=false)
	end

	sortslices(A; dims, alg::Algorithm=DEFAULT_UNSTABLE, lt=isless, by=identity, rev::Bool=false, order::Ordering=Forward)

	# Order-Related Functions
	issorted(v, lt=isless, by=identity, rev:Bool=false, order::Ordering=Forward)
end

# Iteration utilities
# https://docs.julialang.org/en/v1/base/iterators/
module Base.Iterators
	Pairs(values, keys) <: AbstractDict{eltype(keys), eltype(values)}

	Stateful(itr)
	zip(iters...)
	enumerate(iter)
	rest(iter, state)
	countfrom(start=1, step=1)
	take(iter, n)
	drop(iter, n)
	cycle(iter)
	repeated(x[, n::Int])
	product(iters...)
	flatten(iter)
	partition(collection, n)
	filter(flt, itr)
	reverse(itr)
end

# C Interface
# https://docs.julialang.org/en/v1/base/c/
module Core
	Ptr{T}
	Ref{T}

	module Intrinsics
		cglobal((symbol, library) [, type=Cvoid])
		llvmcall(IR::String, ReturnType, (ArgumentType1, ...), ArgumentValue1, ...)
	end
end

module Base
	@cfunction(callable, ReturnType, (ArgumentTypes...,)) -> Ptr{Cvoid}
	@cfunction($callable, ReturnType, (ArgumentTypes...,)) -> CFunction
	CFunction
	unsafe_convert(T, x)
	cconvert(T,x)
	unsafe_load(p::Ptr{T}, i::Integer=1)
	unsafe_store!(p::Ptr{T}, x, i::Integer=1)
	unsafe_copyto!(dest::Ptr{T}, src::Ptr{T}, N)
	copyto!(dest::AbstractMatrix, src::UniformScaling)
	pointer(array [, index])
	unsafe_wrap(Array, pointer::Ptr{T}, dims; own = false)
	pointer_from_objref(x)
	unsafe_pointer_to_objref(p::Ptr)
	disable_sigint(f::Function)
	reenable_sigint(f::Function)
	systemerror(sysfunc, iftrue)
	Cchar
	Cuchar
	Cshort
	Cstring
	Cushort
	Cint
	Cuint
	Clong
	Culong
	Clonglong
	Culonglong
	Cintmax_t
	Cuintmax_t
	Csize_t
	Cssize_t
	Cptrdiff_t
	Cwchar_t
	Cwstring
	Cfloat
	Cdouble
end

# C Standard Library
# https://docs.julialang.org/en/v1/base/libc/
module Base.Libc
	# System
	gethostname() -> AbstractString
	getpid() -> Int32
	getpid(process) -> Int32
	time()

	malloc(size::Integer) -> Ptr{Cvoid}
	calloc(num::Integer, size::Integer) -> Ptr{Cvoid}
	realloc(addr::Ptr, size::Integer) -> Ptr{Cvoid}
	free(addr::Ptr)
	errno([code])
	strerror(n=errno())
	time(t::TmStruct)
	strftime([format], time)
	strptime([format], timestr)
	TmStruct([seconds])
	flush_cstdio()
	systemsleep(s::Real)
end

# StackTraces
# https://docs.julialang.org/en/v1/base/stacktraces/
module Base.StackTraces
	StackFrame
	StackTrace
	stacktrace([trace::Vector{Ptr{Cvoid}},] [c_funcs::Bool=false]) -> StackTrace
	lookup(pointer::Union{Ptr{Cvoid}, UInt}) -> Vector{StackFrame}
	remove_frames!(stack::StackTrace, name::Symbol)
end#

# Standard Library
# Base64
# https://docs.julialang.org/en/v1/stdlib/Base64/
module Base64
	Base64EncodePipe(ostream)
	base64encode(writefunc, args...; context=nothing)
	Base64DecodePipe(istream)
	base64decode(string)
	stringmime(mime, x; context=nothing)
end

# CRC32c
# https://docs.julialang.org/en/v1/stdlib/CRC32c/
module CRC32c
	crc32c(data, crc::UInt32=0x00000000)
end

# Dates
# https://docs.julialang.org/en/v1/stdlib/Dates/
module Dates
	Period
	Year
	Month
	Week
	Day
	Hour
	Minute
	Second
	Millisecond
	Microsecond
	Nanosecond
	CompoundPeriod
	Instant
	UTInstant{T}
	TimeType
	DateTime
	Date
	Time

	# Dates Functions
	format(io::IO, tok::AbstractDateToken, dt::TimeType, locale)
	DateFormat(format::AbstractString, locale="english") -> DateFormat
	now() -> DateTime
	eps(::DateTime) -> Millisecond

	# Accessor Functions
	year(dt::TimeType) -> Int64
	month(dt::TimeType) -> Int64
	week(dt::TimeType) -> Int64
	day(dt::TimeType) -> Int64
	hour(dt::DateTime) -> Int64
	minute(dt::DateTime) -> Int64
	second(dt::DateTime) -> Int64
	millisecond(dt::DateTime) -> Int64
	microsecond(t::Time) -> Int64
	nanosecond(t::Time) -> Int64
	yearmonth(dt::TimeType) -> (Int64, Int64)
	monthday(dt::TimeType) -> (Int64, Int64)
	yearmonthday(dt::TimeType) -> (Int64, Int64, Int64)

	# Query Functions
	dayname(dt::TimeType; locale="english") -> String
	dayabbr(dt::TimeType; locale="english") -> String
	dayofweek(dt::TimeType) -> Int64
	dayofmonth(dt::TimeType) -> Int64
	dayofweekofmonth(dt::TimeType) -> Int
	daysofweekinmonth(dt::TimeType) -> Int
	monthname(dt::TimeType; locale="english") -> String
	monthabbr(dt::TimeType; locale="english") -> String
	daysinmonth(dt::TimeType) -> Int
	isleapyear(dt::TimeType) -> Bool
	dayofyear(dt::TimeType) -> Int
	daysinyear(dt::TimeType) -> Int
	quarterofyear(dt::TimeType) -> Int
	dayofquarter(dt::TimeType) -> Int

	# Adjuster Functions
	trunc(dt::TimeType, ::Type{Period}) -> TimeType
	firstdayofweek(dt::TimeType) -> TimeType
	lastdayofweek(dt::TimeType) -> TimeType
	firstdayofmonth(dt::TimeType) -> TimeType
	lastdayofmonth(dt::TimeType) -> TimeType
	firstdayofyear(dt::TimeType) -> TimeType
	lastdayofyear(dt::TimeType) -> TimeType
	firstdayofquarter(dt::TimeType) -> TimeType
	lastdayofquarter(dt::TimeType) -> TimeType
	tonext(dt::TimeType, dow::Int; same::Bool=false) -> TimeType
	toprev(dt::TimeType, dow::Int; same::Bool=false) -> TimeType
	tofirst(dt::TimeType, dow::Int; of=Month) -> TimeType
	tolast(dt::TimeType, dow::Int; of=Month) -> TimeType

	# Periods
	value(x::Period) -> Int64
	default(p::Period) -> Period

	# Rounding Functions
	floor(dt::TimeType, p::Period) -> TimeType
	round(dt::TimeType, p::Period, [r::RoundingMode]) -> TimeType
	floorceil(dt::TimeType, p::Period) -> (TimeType, TimeType)
	epochdays2date(days) -> Date
	epochms2datetime(milliseconds) -> DateTime
	date2epochdays(dt::Date) -> Int64
	datetime2epochms(dt::DateTime) -> Int64

	# Conversion Functions
	today() -> Date
	unix2datetime(x) -> DateTime
	datetime2unix(dt::DateTime) -> Float64
	julian2datetime(julian_days) -> DateTime
	datetime2julian(dt::DateTime) -> Float64
	rata2datetime(days) -> DateTime
	datetime2rata(dt::TimeType) -> Int64

	# Constants
	#Monday	Mon	1
	#Tuesday	Tue	2
	#Wednesday	Wed	3
	#Thursday	Thu	4
	#Friday	Fri	5
	#Saturday	Sat	6
	#Sunday	Sun	7
	#January	Jan	1
	#February	Feb	2
	#March	Mar	3
	#April	Apr	4
	#May	May	5
	#June	Jun	6
	#July	Jul	7
	#August	Aug	8
	#September	Sep	9
	#October	Oct	10
	#November	Nov	11
	#December	Dec	12
end

# Delimited Files
# https://docs.julialang.org/en/v1/stdlib/DelimitedFiles/
module DelimitedFiles
	readdlm(source, delim::AbstractChar, T::Type, eol::AbstractChar; header=false, skipstart=0, skipblanks=true, use_mmap, quotes=true, dims, comments=false, comment_char='#')
	writedlm(f, A, delim='\t'; opts)
end

# Distributed Computing
# https://docs.julialang.org/en/v1/stdlib/Distributed/
module Base
	put!(rr::RemoteChannel, args...)
	take!(rr::RemoteChannel, args...)
	isready(rr::RemoteChannel, args...)
	kill(manager::ClusterManager, pid::Int, config::WorkerConfig)
end

module Distributed
	addprocs(manager::ClusterManager; kwargs...) -> List of process identifiers
	nprocs()
	nworkers()
	procs()
	workers()
	rmprocs(pids...; waitfor=typemax(Int))
	interrupt(pids::Integer...)
	myid()
	pmap(f, [::AbstractWorkerPool], c...; distributed=true, batch_size=1, on_error=nothing, retry_delays=[], retry_check=nothing) -> collection
	RemoteException(captured)
	Future(pid::Integer=myid())
	RemoteChannel(pid::Integer=myid())
	fetch(x::Future)
	remotecall(f, id::Integer, args...; kwargs...) -> Future
	remotecall_wait(f, id::Integer, args...; kwargs...)
	remotecall_fetch(f, id::Integer, args...; kwargs...)
	remote_do(f, id::Integer, args...; kwargs...) -> nothing
	AbstractWorkerPool
	WorkerPool(workers::Vector{Int})
	WorkerPool(workers::Vector{Int})
	default_worker_pool()
	clear!(pool::CachingPool) -> pool
	remote([p::AbstractWorkerPool], f) -> Function
	@spawn
	@spawnat
	@fetch
	@fetch
	@distributed
	@everywhere [procs()] expr
	remoteref_id(r::AbstractRemoteRef) -> RRID
	channel_from_id(id) -> c
	worker_id_from_socket(s) -> pid
	cluster_cookie() -> cookie

	# Cluster Manager Interface
	ClusterManager
	WorkerConfig
	launch(manager::ClusterManager, params::Dict, launched::Array, launch_ntfy::Condition)
	manage(manager::ClusterManager, id::Integer, config::WorkerConfig. op::Symbol)
	init_worker(cookie::AbstractString, manager::ClusterManager=DefaultClusterManager())
	start_worker([out::IO=stdout], cookie::AbstractString=readline(stdin); close_stdin::Bool=true, stderr_to_stdout::Bool=true)
	process_messages(r_stream::IO, w_stream::IO, incoming::Bool=true)
end

# File Events
# https://docs.julialang.org/en/v1/stdlib/FileWatching/
module FileWatching
	poll_fd(fd, timeout_s::Real=-1; readable=false, writable=false)
	poll_file(path::AbstractString, interval_s::Real=5.007, timeout_s::Real=-1) -> (previous::StatStruct, current)
	watch_file(path::AbstractString, timeout_s::Real=-1)
	watch_folder(path::AbstractString, timeout_s::Real=-1)
	unwatch_folder(path::AbstractString)
end

# Linear Algebra
# https://docs.julialang.org/en/v1/stdlib/LinearAlgebra/
module Base
	adjoint(A)
end

module LinearAlgebra
	SingularException
	PosDefException
	dot(x, y)
	cross(x, y)
	factorize(A)
	Diagonal(A::AbstractMatrix)
	Bidiagonal(dv::V, ev::V, uplo::Symbol) where V <: AbstractVector
	SymTridiagonal(dv::V, ev::V) where V <: AbstractVector
	Tridiagonal(dl::V, d::V, du::V) where V <: AbstractVector
	Symmetric(A, uplo=:U)
	Hermitian(A, uplo=:U)
	LowerTriangular(A::AbstractMatrix)
	UpperTriangular(A::AbstractMatrix)
	UnitLowerTriangular(A::AbstractMatrix)
	UnitUpperTriangular(A::AbstractMatrix)
	UniformScaling{T<:Number}
	lu(A, pivot=Val(true); check = true) -> F::LU
	lu!(A, pivot=Val(true); check = true) -> LU
	cholesky(A, Val(false); check = true) -> Cholesky
	cholesky(A, Val(true); tol = 0.0, check = true) -> CholeskyPivoted
	cholesky!(A, Val(false); check = true) -> Cholesky
	cholesky!(A, Val(true); tol = 0.0, check = true) -> CholeskyPivoted
	lowrankupdate(C::Cholesky, v::StridedVector) -> CC::Cholesky
	lowrankdowndate(C::Cholesky, v::StridedVector) -> CC::Cholesky
	lowrankupdate!(C::Cholesky, v::StridedVector) -> CC::Cholesky
	lowrankdowndate!(C::Cholesky, v::StridedVector) -> CC::Cholesky
	ldlt(S::SymTridiagonal) -> LDLt
	ldlt!(S::SymTridiagonal) -> LDLt
	qr(A, pivot=Val(false)) -> F
	qr!(A, pivot=Val(false))
	QR <: Factorization
	QRCompactWY <: Factorization
	QRPivoted <: Factorization
	lq!(A) -> LQ
	lq(A) -> S::LQ
	bunchkaufman(A, rook::Bool=false; check = true) -> S::BunchKaufman
	bunchkaufman!(A, rook::Bool=false; check = true) -> BunchKaufman
	eigvals(A; permute::Bool=true, scale::Bool=true, sortby) -> values
	eigvals!(A; permute::Bool=true, scale::Bool=true, sortby) -> values
	eigmax(A; permute::Bool=true, scale::Bool=true)
	eigmin(A; permute::Bool=true, scale::Bool=true)
	eigvecs(A::SymTridiagonal[, eigvals]) -> Matrix
	eigen(A; permute::Bool=true, scale::Bool=true, sortby) -> Eigen
	eigen!(A, [B]; permute, scale, sortby)
	hessenberg(A) -> Hessenberg
	hessenberg!(A) -> Hessenberg
	schur!(A::StridedMatrix) -> F::Schur
	schur(A::StridedMatrix) -> F::Schur
	ordschur(F::Schur, select::Union{Vector{Bool},BitVector}) -> F::Schur
	ordschur!(F::Schur, select::Union{Vector{Bool},BitVector}) -> F::Schur
	svd(A; full::Bool = false) -> SVD
	svd(A; full::Bool = false) -> SVD
	svdvals(A)
	svdvals!(A)
	Givens(i1,i2,c,s) -> G
	givens(f::T, g::T, i1::Integer, i2::Integer) where {T} -> (G::Givens, r::T)
	triu(M)
	triu!(M)
	tril(M)
	tril!(M)
	diagind(M, k::Integer=0)
	diag(M, k::Integer=0)
	diagm(kv::Pair{<:Integer,<:AbstractVector}...)
	rank(A::AbstractMatrix; atol::Real=0, rtol::Real=atol>0 ? 0 : n*ϵ)
	norm(A, p::Real=2)
	opnorm(A::AbstractMatrix, p::Real=2)
	normalize!(v::AbstractVector, p::Real=2)
	normalize(v::AbstractVector, p::Real=2)
	cond(M, p::Real=2)
	condskeel(M, [x, p::Real=Inf])
	tr(M)
	det(M)
	logdet(M)
	logabsdet(M)
	inv(M)
	pinv(M; atol::Real=0, rtol::Real=atol>0 ? 0 : n*ϵ)
	nullspace(M; atol::Real=0, rtol::Rea=atol>0 ? 0 : n*ϵ)
	kron(A, B)
	lyap(A, C)
	sylvester(A, B, C)
	issuccess(F::Factorization)
	issymmetric(A) -> Bool
	isposdef(A) -> Bool
	isposdef!(A) -> Bool
	istril(A::AbstractMatrix, k::Integer = 0) -> Bool
	istriu(A::AbstractMatrix, k::Integer = 0) -> Bool
	isdiag(A) -> Bool
	ishermitian(A) -> Bool
	transpose!(dest,src)
	Transpose
	adjoint!(dest,src)
	Adjoint
	stride1(A) -> Int
	checksquare(A)
	peakflops(n::Integer=2000; parallel::Bool=false)

	# Low-level matrix operations
	mul!(Y, A, B) -> Y
	lmul!(a::Number, B::AbstractArray)
	rmul!(A::AbstractArray, b::Number)
	ldiv!(Y, A, B) -> Y
	rdiv!(A, B)

	# BLAS Functions
	# LAPACK Functions
end

# Logging
# https://docs.julialang.org/en/v1/stdlib/Logging/
module Base.CoreLogging
	@debug message  [key=value | value ...]
	@info  message  [key=value | value ...]
	@warn  message  [key=value | value ...]
	@error message  [key=value | value ...]
	@logmsg level message [key=value | value ...]
	LogLevel(level)
	AbstractLogger
	handle_message(logger, level, message, _module, group, id, file, line; key1=val1, ...)
	shouldlog(logger, level, _module, group, id)
	min_enabled_level(logger)
	catch_exceptions(logger)
	disable_logging(level)
	global_logger()
	with_logger(function, logger)
	current_logger()
	NullLogger()
	ConsoleLogger(stream=stderr, min_level=Info; meta_formatter=default_metafmt, show_limited=true, right_justify=0)
	SimpleLogger(stream=stderr, min_level=Info)
end

# Memory-mapped I/O
# https://docs.julialang.org/en/v1/stdlib/Mmap/
module Mmap
	Anonymous(name::AbstractString="", readonly::Bool=false, create::Bool=true)
	mmap(io::Union{IOStream,AbstractString,Mmap.AnonymousMmap}[, type::Type{Array{T,N}}, dims, offset]; grow::Bool=true, shared::Bool=true)
	sync!(array)
end

# Printf
# https://docs.julialang.org/en/v1/stdlib/Printf/
module Printf
	@printf([io::IOStream], "%Fmt", args...)
	@sprintf("%Fmt", args...)
end

# Profiling
# https://docs.julialang.org/en/v1/stdlib/Profile/
module Profile
	@profile
	clear()
	init(; n::Integer, delay::Float64)
	fetch() -> data
	retrieve() -> data, lidict
	callers(funcname, [data, lidict], [filename=<filename>], [linerange=<start:stop>]) -> Vector{Tuple{count, lineinfo}}
	clear_malloc_data()
end

# Random Numbers
# https://docs.julialang.org/en/v1/stdlib/Random/
module Base
	rand([rng=GLOBAL_RNG], [S], [dims...])
	randn([rng=GLOBAL_RNG], [T=Float64], [dims...])
end

module Random
	Random
	rand!([rng=GLOBAL_RNG], A, [S=eltype(A)])
	bitrand([rng=GLOBAL_RNG], [dims...])
	randn!([rng=GLOBAL_RNG], A::AbstractArray) -> A
	randexp([rng=GLOBAL_RNG], [T=Float64], [dims...])
	randexp!([rng=GLOBAL_RNG], A::AbstractArray) -> A
	randstring([rng=GLOBAL_RNG], [chars], [len=8])

	# Subsequences, permutations and shuffling
	randsubseq([rng=GLOBAL_RNG,] A, p) -> Vector
	randsubseq!([rng=GLOBAL_RNG,] S, A, p)
	randperm([rng=GLOBAL_RNG,] n::Integer)
	randperm!([rng=GLOBAL_RNG,] A::Array{<:Integer})
	randcycle([rng=GLOBAL_RNG,] n::Integer)
	randcycle!([rng=GLOBAL_RNG,] A::Array{<:Integer})
	shuffle([rng=GLOBAL_RNG,] v::AbstractArray)
	shuffle!([rng=GLOBAL_RNG,] v::AbstractArray)

	# Generators (creation and seeding)
	seed!([rng=GLOBAL_RNG], seed) -> rng
	seed!([rng=GLOBAL_RNG]) -> rng
	AbstractRNG
	MersenneTwister(seed)
	RandomDevice()
end

# Serialization
# https://docs.julialang.org/en/v1/stdlib/Serialization/
module Serialization
	serialize(stream::IO, value)
	deserialize(stream)
	Serialization.writeheader(s::AbstractSerializer)
end

# Shared Arrays
# https://docs.julialang.org/en/v1/stdlib/SharedArrays/
module SharedArrays
	SharedArray{T}(dims::NTuple; init=false, pids=Int[])
	SharedVector
	SharedMatrix
	sdata(S::SharedArray)
	indexpids(S::SharedArray)
	localindices(S::SharedArray)
end

module Distributed
	procs(S::SharedArray)
end

# Sockets
# https://docs.julialang.org/en/v1/stdlib/Sockets/
module Base
	bind(chnl::Channel, task::Task)
end

module Sockets
	connect([host], port::Integer) -> TCPSocket
	listen([addr, ]port::Integer; backlog::Integer=BACKLOG_DEFAULT) -> TCPServer
	getalladdrinfo(host::AbstractString, IPAddr=IPv4) -> IPAddr
	getipaddr() -> IPAddr
	getipaddrs(; loopback::Bool=false) -> Vector{IPAddr}
	getalladdrinfo(host::AbstractString) -> Vector{IPAddr}
	getnameinfo(host::IPAddr) -> String
	getsockname(sock::Union{TCPServer, TCPSocket}) -> (IPAddr, UInt16)
	getpeername(sock::TCPSocket) -> (IPAddr, UInt16)
	IPAddr
	IPv4(host::Integer) -> IPv4
	IPv6(host::Integer) -> IPv6
	TCPSocket(; delay=true)
	UDPSocket()
	accept(server[,client])
	send(socket::UDPSocket, host, port::Integer, msg)
	recv(socket::UDPSocket)
	recvfrom(socket::UDPSocket) -> (address, data)
	setopt(sock::UDPSocket; multicast_loop = nothing, multicast_ttl=nothing, enable_broadcast=nothing, ttl=nothing)
end

# Sparse Arrays
# https://docs.julialang.org/en/v1/stdlib/SparseArrays/
module SparseArrays
	AbstractSparseArray{Tv,Ti,N}
	AbstractSparseVector{Tv,Ti}
	AbstractSparseMatrix{Tv,Ti}
	SparseVector{Tv,Ti<:Integer} <: AbstractSparseVector{Tv,Ti}
	SparseMatrixCSC{Tv,Ti<:Integer} <: AbstractSparseMatrix{Tv,Ti}
	sparse(A)
	sparsevec(I, V, [m, combine])
	issparse(S)
	nnz(A)
	findnz(A)
	spzeros([type,]m[,n])
	spdiagm(kv::Pair{<:Integer,<:AbstractVector}...)
	blockdiag(A...)
	sprand([rng],[type],m,[n],p::AbstractFloat,[rfn])
	sprandn([rng][,Type],m[,n],p::AbstractFloat)
	nonzeros(A)
	rowvals(A::SparseMatrixCSC)
	nzrange(A::SparseMatrixCSC, col::Integer)
	droptol!(A::SparseMatrixCSC, tol; trim::Bool = true)
	dropzeros!(A::SparseMatrixCSC; trim::Bool = true)
	dropzeros(A::SparseMatrixCSC; trim::Bool = true)
	permute(A::SparseMatrixCSC{Tv,Ti}, p::AbstractVector{<:Integer}, q::AbstractVector{<:Integer}) where {Tv,Ti}
	permute!(X::SparseMatrixCSC{Tv,Ti}, A::SparseMatrixCSC{Tv,Ti}, p::AbstractVector{<:Integer}, q::AbstractVector{<:Integer}, [C::SparseMatrixCSC{Tv,Ti}]) where {Tv,Ti}
end

# Statistics
# https://docs.julialang.org/en/v1/stdlib/Statistics/
module Statistics
	std(itr; corrected::Bool=true, mean=nothing[, dims])
	stdm(itr, m; corrected::Bool=true)
	var(itr; dims, corrected::Bool=true, mean=nothing)
	varm(itr, m; dims, corrected::Bool=true)
	cor(x::AbstractVector)
	cov(x::AbstractVector; corrected::Bool=true)
	mean!(r, v)
	mean(itr)
	median!(v)
	median(itr)
	middle(x)
	quantile!([q::AbstractArray, ] v::AbstractVector, p; sorted=false)
	quantile(itr, p; sorted=false)
end

# Unit Testing
# https://docs.julialang.org/en/v1/stdlib/Test/
module Base
	runtests(tests=["all"]; ncores=ceil(Int, Sys.CPU_THREADS / 2), exit_on_error=false, [seed])
end

module Test
	@test ex
	@test_throws exception expr
	@testset [CustomTestSet] [option=val  ...] ["description"] begin ... end
	@inferred [AllowedType] f(x)
	@test_logs [log_patterns...] [keywords] expression
	@test_deprecated [pattern] expression
	@test_warn msg expr
	@test_nowarn expr
	@test_broken ex
	@test_skip ex
	record(ts::AbstractTestSet, res::Result)
	finish(ts::AbstractTestSet)
	get_testset()
	get_testset_depth()
end

# UUIDs
# https://docs.julialang.org/en/v1/stdlib/UUIDs/
module UUIDs
	uuid1([rng::AbstractRNG=GLOBAL_RNG]) -> UUID
	uuid4([rng::AbstractRNG=GLOBAL_RNG]) -> UUID
	uuid5(ns::UUID, name::String) -> UUID
	uuid_version(u::UUID) -> Int
end

# Unicode
# https://docs.julialang.org/en/v1/stdlib/Unicode/
module Unicode
	isassigned(c) -> Bool
	normalize(s::AbstractString, normalform::Symbol)
	graphemes(s::AbstractString) -> GraphemeIterator
end
