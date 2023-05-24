// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Lexical_grammar
// https://www.ecma-international.org/publications/standards/Ecma-262.htm
// 2020 https://tc39.es/ecma262/2020/
// 2021 https://tc39.es/ecma262/2021/
// 2022 https://tc39.es/ecma262/2022/
// 2023 https://tc39.es/ecma262/2023/
// 2024 https://tc39.es/ecma262/

//! keywords
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Lexical_grammar#keywords
async await
break
case catch class const continue
debugger default delete do
else export extends
finally for function
if import in instanceof
new
return
super switch
this throw try typeof
var void
while with
yield
let
static

null true false
globalThis
Infinity
NaN
undefined
arguments
get set of

//! future reserved words
enum implements interface package private protected public
// ECMAScript 1 till 3
// abstract boolean byte char double final float goto int long native short synchronized throws transient volatile

// typeof Operator Results
// undefined object boolean number string symbol bigint function

// Meta Properties
// new.target // 2020
// import.meta // 2020

//! directive		===========================================================
// https://www.ecma-international.org/ecma-262/11.0/index.html#sec-imports
// https://www.ecma-international.org/ecma-262/11.0/index.html#sec-exports
import as from
import(path) // 2020
export as
// export default

// https://nodejs.org/api/globals.html
__dirname
__filename
exports
module
require(path)

//! decorators
// https://github.com/tc39/proposal-decorators

//! JSDoc			===========================================================
// https://jsdoc.app/
/**
 * @abstract @virtual
 * @access
 * @alias
 * @async
 * @augments @extends
 * @author
 * @borrows
 * @callback
 * @class @constructor
 * @classdesc
 * @constant @const
 * @constructs
 * @copyright
 * @default @defaultvalue
 * @deprecated
 * @description @desc
 * @enum
 * @event
 * @example
 * @exports
 * @external @host
 * @file @fileoverview @overview
 * @fires @emits
 * @function @func @method
 * @generator
 * @global
 * @hideconstructor
 * @ignore
 * @implements
 * @inheritdoc
 * @inner
 * @instance
 * @interface
 * @kind
 * @lends
 * @license
 * @listens
 * @member @var
 * @memberof
 * @mixes
 * @mixin
 * @module
 * @name
 * @namespace
 * @override
 * @package
 * @param @arg @argument
 * @private
 * @property @prop
 * @protected
 * @public
 * @readonly
 * @requires
 * @returns @return
 * @see
 * @since
 * @static
 * @summary
 * @this
 * @throws @exception
 * @todo
 * @tutorial
 * @type
 * @typedef
 * @variation
 * @version
 * @yields @yield
 * {@link } {@linkcode } {@linkplain }
 * {@tutorial }
 */

//! API				===========================================================
eval(x)
isFinite(number)
isNaN(number)
parseFloat(string)
parseInt(string, radix)
decodeURI(encodedURI)
decodeURIComponent(encodedURIComponent)
encodeURI(uri)
encodeURIComponent(uriComponent)
escape(string)
unescape(string)

Object([value]) {
	assign(target, ...sources)
	create(O, Properties)
	defineProperties(O, Properties)
	defineProperty(O, P, Attributes)
	entries(O) // 2017
	freeze(O)
	fromEntries(iterable) // 2019
	getOwnPropertyDescriptor(O, P)
	getOwnPropertyDescriptors(O) // 2017
	getOwnPropertyNames(O)
	getOwnPropertySymbols(O)
	getPrototypeOf(O)
	hasOwn(O, P) // 2022
	is(value1, value2)
	isExtensible(O)
	isFrozen(O)
	isSealed(O)
	keys(O)
	preventExtensions(O)
	seal(O)
	setPrototypeOf(O, proto)
	values(O) // 2017
	prototype:
		constructor
		hasOwnProperty(V)
		isPrototypeOf(V)
		propertyIsEnumerable(V)
		toLocaleString([reserved1 [, reserved2]])
		toString()
		valueOf()

		__proto__
		__defineGetter__(P, getter)
		__defineSetter__(P, setter)
		__lookupGetter__(P)
		__lookupSetter__(P)
}

Function(p1, p2, … , pn, body) {
	length
	name
	prototype:
		apply(thisArg, argArray)
		bind(thisArg, ...args)
		call(thisArg, ...args)
		toString()
}

Boolean(value)

Symbol([description]) {
	asyncIterator
	hasInstance
	isConcatSpreadable
	iterator
	match
	matchAll // 2020
	replace
	search
	species
	split
	toPrimitive
	toStringTag
	unscopables
	prototype
	for(key)
	keyFor(sym)
}

Error(message) {
	cause // 2022
	message
	name

	AggregateError // 2021
	// Native Error Types
	EvalError
	RangeError
	ReferenceError
	SyntaxError
	TypeError
	URIError
}

Number(value) {
	EPSILON
	MAX_SAFE_INTEGER
	MAX_VALUE
	MIN_SAFE_INTEGER
	MIN_VALUE
	NaN
	NEGATIVE_INFINITY
	POSITIVE_INFINITY

	isFinite(number)
	isInteger(number)
	isNaN(number)
	isSafeInteger(number)
	parseFloat(string)
	parseInt(string, radix)
	prototype:
		toExponential(fractionDigits)
		toFixed(fractionDigits)
		toPrecision(precision)
}

BigInt(value) {
	asIntN(bits, bigint)
	asUintN(bits, bigint)
}

Math {
	E
	LN10
	LN2
	LOG10E
	LOG2E
	PI
	SQRT1_2
	SQRT2
	toStringTag

	abs(x)
	acos(x)
	acosh(x)
	asin(x)
	asinh(x)
	atan(x)
	atanh(x)
	atan2(y, x)
	cbrt(x)
	ceil(x)
	clz32(x)
	cos(x)
	cosh(x)
	exp(x)
	expm1(x)
	floor(x)
	fround(x)
	hypot(value1, value2, ...values)
	imul(x, y)
	log(x)
	log1p(x)
	log10(x)
	log2(x)
	max(value1, value2, ...values)
	min(value1, value2, ...values)
	pow(base, exponent)
	random()
	round(x)
	sin(x)
	sinh(x)
	sqrt(x)
	tan(x)
	tanh(x)
	trunc(x)
}

Date(year, month [, date [, hours [, minutes [, seconds [, ms]]]]]) {
	Date(value)
	Date()
	UTC(year [, month [, date [, hours [, minutes [, seconds [, ms]]]]]])
	now()
	parse(string)
	prototype:
		getDate()
		getDay()
		getFullYear()
		getHours()
		getMilliseconds()
		getMinutes()
		getMonth()
		getSeconds()
		getTime()
		getTimezoneOffset()
		getUTCDate()
		getUTCDay()
		getUTCFullYear()
		getUTCHours()
		getUTCMilliseconds()
		getUTCMinutes()
		getUTCMonth()
		getUTCSeconds()
		setDate(date)
		setFullYear(year [, month [, date]])
		setHours(hour [, min [, sec [, ms]]])
		setMilliseconds(ms)
		setMinutes(min [, sec [, ms]])
		setMonth(month [, date])
		setSeconds(sec [, ms])
		setTime(time)
		setUTCDate(date)
		setUTCFullYear(year [, month [, date]])
		setUTCHours(hour [, min [, sec [, ms]]])
		setUTCMilliseconds(ms)
		setUTCMinutes(min [, sec [, ms]])
		setUTCMonth(month [, date])
		setUTCSeconds(sec [, ms])
		toDateString()
		toISOString()
		toJSON(key)
		toLocaleDateString([reserved1 [, reserved2]])
		toLocaleTimeString([reserved1 [, reserved2]])
		toTimeString()
		toUTCString()

		getYear()
		setYear(year)
		toGMTString()
}

String(value) {
	fromCharCode(...codeUnits)
	fromCodePoint(...codePoints)
	raw(template, ...substitutions)
	length
	prototype:
		at(index) // 2022
		charAt(pos)
		charCodeAt(pos)
		codePointAt(pos)
		concat(...args)
		endsWith(searchString [, endPosition])
		includes(searchString [, position])
		indexOf(searchString [, position])
		lastIndexOf(searchString [, position])
		localeCompare(that [, reserved1 [, reserved2]])
		match(regexp)
		matchAll(regexp) // 2020
		normalize([form])
		padEnd(maxLength [, fillString])
		padStart(maxLength [, fillString])
		repeat(count)
		replace(searchValue, replaceValue)
		replaceAll(searchValue, replaceValue) // 2021
		search(regexp)
		slice(start, end)
		split(separator, limit)
		startsWith(searchString [, position])
		substring(start, end)
		toLocaleLowerCase([reserved1 [, reserved2]])
		toLocaleUpperCase([reserved1 [, reserved2]])
		toLowerCase()
		toUpperCase()
		trim()
		trimEnd() // 2019
		trimStart() // 2019
		iterator
			next()

		substr(start, length)
		anchor(name)
		big()
		blink()
		bold()
		fixed()
		fontcolor(color)
		fontsize(size)
		italics()
		link(url)
		small()
		strike()
		sub()
		sup()
		trimLeft()
		trimRight()

}

RegExp(pattern, flags) {
	compile(pattern, flags)
	lastIndex
	prototype:
		exec(string)
		test(S)
		dotAll // 2018
		flags
		global
		ignoreCase
		multiline
		sticky
		source
		unicode
}

Array(...items) {
	Array(len)
	Array()
	from(items [, mapfn [, thisArg]])
	isArray(arg)
	of(...items)
	length
	prototype:
		at(index) // 2022
		concat(...arguments)
		copyWithin(target, start [, end])
		entries()
		every(callbackfn [, thisArg])
		fill(value [, start [, end]])
		filter(callbackfn [, thisArg])
		find(predicate [, thisArg])
		findIndex(predicate [, thisArg])
		findLast(predicate [, thisArg]) // 2023
		findLastIndex(predicate [, thisArg]) // 2023
		flat([depth]) // 2019
		flatMap(mapperFunction [, thisArg]) // 2019
		forEach(callbackfn [, thisArg])
		includes(searchElement [, fromIndex]) // 2016
		indexOf(searchElement [, fromIndex])
		join(separator)
		keys()
		lastIndexOf(searchElement [, fromIndex])
		map(callbackfn [, thisArg])
		pop()
		push(...items)
		reduce(callbackfn [, initialValue])
		reduceRight(callbackfn [, initialValue])
		reverse()
		shift()
		slice(start, end)
		some(callbackfn [, thisArg])
		sort(comparefn)
		splice(start, deleteCount, ...items)
		toReversed() // 2023
		toSorted(comparefn) // 2023
		toSpliced(start, skipCount, ...items) // 2023
		unshift(...items)
		values()
		with(index, value) // 2023
		iterator
		unscopables

	// TypedArray
		Int8Array
		Uint8Array
		Uint8ClampedArray
		Int16Array
		Uint16Array
		Int32Array
		Uint32Array
		BigInt64Array // 2020
		BigUint64Array // 2020
		Float32Array
		Float64Array
		prototype:
			BYTES_PER_ELEMENT
			buffer
			byteLength
			byteOffset
			set(overloaded [, offset ])
			subarray(begin, end)
}

Map([iterable]) {
	prototype:
		clear()
		delete(key)
		entries()
		forEach(callbackfn [, thisArg])
		get(key)
		has(key)
		keys()
		set(key, value)
		values()
		size
		iterator
		toStringTag
}

Set([iterable]) {
	prototype:
		add(value)
		clear()
		delete(value)
		entries()
		forEach(callbackfn [, thisArg])
		has(value)
		keys()
		values()
		size
}

WeakMap([iterable]) {
	prototype:
		delete(key)
		get(key)
		has(key)
		set(key, value)
}

WeakSet([iterable]) {
	prototype:
		add(value)
		delete(value)
		has(value)
}

ArrayBuffer(length) {
	isView(arg)
	prototype:
		byteLength
		slice(start, end)
}

SharedArrayBuffer(length) {
	prototype:
		byteLength
		slice(start, end)
}

DataView(buffer [, byteOffset [, byteLength]]) {
	prototype:
		buffer
		byteLength
		byteOffset
		getBigInt64(byteOffset [, littleEndian]) // 2020
		getBigUint64(byteOffset [, littleEndian]) // 2020
		getFloat32(byteOffset [, littleEndian])
		getFloat64(byteOffset [, littleEndian])
		getInt8(byteOffset)
		getInt16(byteOffset [, littleEndian])
		getInt16(byteOffset [, littleEndian])
		getInt32(byteOffset [, littleEndian])
		getUint8(byteOffset)
		getUint16(byteOffset [, littleEndian])
		getUint32(byteOffset [, littleEndian])
		setBigInt64(byteOffset, value [, littleEndian])
		setBigUint64(byteOffset, value [, littleEndian])
		setFloat32(byteOffset, value [, littleEndian])
		setFloat64(byteOffset, value [, littleEndian])
		setInt8(byteOffset, value)
		setInt16(byteOffset, value [, littleEndian])
		setInt32(byteOffset, value [, littleEndian])
		setUint8(byteOffset, value)
		setUint16(byteOffset, value [, littleEndian])
		setUint32(byteOffset, value [, littleEndian])
}

Atomics {
	add(typedArray, index, value)
	and(typedArray, index, value)
	compareExchange(typedArray, index, expectedValue, replacementValue)
	exchange(typedArray, index, value)
	isLockFree(size)
	load(typedArray, index)
	or(typedArray, index, value)
	store(typedArray, index, value)
	sub(typedArray, index, value)
	wait(typedArray, index, value, timeout)
	wake(typedArray, index, count)
	xor(typedArray, index, value)
}

JSON {
	parse(text [, reviver])
	stringify(value [, replacer [, space]])
}

WeakRef(target) { // 2021
	prototype:
		deref()
}

FinalizationRegistry(cleanupCallback) { // 2021
	prototype:
		register(target, heldValue [, unregisterToken])
		unregister(unregisterToken)
}

GeneratorFunction(p1, p2, … , pn, body) {
}
AsyncGeneratorFunction(p1, p2, … , pn, body) {
}

// Generator, AsyncGenerator
{
	prototype:
		next(value)
		return(value)
		throw(exception)
}

Promise(executor) {
	all(iterable)
	allSettled(iterable) // 2020
	race(iterable)
	any(iterable) // 2021
	reject(r)
	resolve(x)
	prototype:
		catch(onRejected)
		finally(onFinally)
		then(onFulfilled, onRejected)
}

AsyncFunction(p1, p2, … , pn, body) {
}

Reflect {
	apply(target, thisArgument, argumentsList)
	construct(target, argumentsList [, newTarget])
	defineProperty(target, propertyKey, attributes)
	deleteProperty(target, propertyKey)
	get(target, propertyKey [, receiver])
	getOwnPropertyDescriptor(target, propertyKey)
	getPrototypeOf(target)
	has(target, propertyKey)
	isExtensible(target)
	ownKeys(target)
	preventExtensions(target)
	set(target, propertyKey, V [, receiver])
	setPrototypeOf(target, proto)
}

Proxy(target, handler) {
	revocable(target, handler)
}

// JScript
// https://docs.microsoft.com/en-us/previous-versions/ms950396(v=msdn.10)
// https://developer.mozilla.org/en-US/docs/Archive/Web/JavaScript/Microsoft_Extensions
ActiveXObject(servername.typename[, location])
Debug {
	write([str1 [, str2 [, ... [, strN]]]])
	writeln([str1 [, str2 [, ... [, strN]]]])
}
Enumerator([collection]) {
	atEnd()
	item()
	moveFirst()
	moveNext()
}
VBArray {
	dimensions()
	getItem(dimension1[, dimension2, ...], dimensionN)
	lbound(dimension)
	toArray()
	ubound(dimension)
}
// Windows Script Host
// https://msdn.microsoft.com/en-us/library/9bbdkx3k(v=msdn.10)
WScript {
	//CreateObject(strProgID[,strPrefix])
	//Echo()
	//Exec(strCommand)
	//GetObject(strPathname [,strProgID], [strPrefix])
	//Quit()
}

// https://developer.mozilla.org/en-US/docs/Web/API
// https://developer.mozilla.org/en-US/docs/Web/API/Console
{
	assert(assertion, obj1 [, obj2, ..., objN])
	assert(assertion, msg [, subst1, ..., substN]) // C-like message formatting
	clear()
	count([label])
	countReset([label])
	debug(obj1 [, obj2, ..., objN])
	debug(msg [, subst1, ..., substN])
	dir(object)
	dirxml(object)
	error(obj1 [, obj2, ..., objN])
	error(msg [, subst1, ..., substN])
	exception(obj1 [, obj2, ..., objN])
	exception(msg [, subst1, ..., substN])
	group([label])
	groupCollapsed([label])
	groupEnd()
	info(obj1 [, obj2, ..., objN])
	info(msg [, subst1, ..., substN])
	log(obj1 [, obj2, ..., objN])
	log(msg [, subst1, ..., substN])
	table(data [, columns])
	time(label)
	timeEnd(label)
	timeLog(label)
	trace( [...any, ...data ])
	warn(obj1 [, obj2, ..., objN])
	warn(msg [, subst1, ..., substN])
}

// https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model
// https://developer.mozilla.org/en-US/docs/Web/API/EventTarget
EventTarget() {
	addEventListener(type, listener [, options])
	addEventListener(type, listener [, useCapture])
	dispatchEvent(event)
	removeEventListener(type, listener[, options])
	removeEventListener(type, listener[, useCapture])
}

// https://developer.mozilla.org/en-US/docs/Web/API/Node
{
	baseURI
	childNodes
	firstChild
	isConnected
	lastChild
	nextSibling
	nodeName
	nodeType
	nodeValue
	ownerDocument
	parentElement
	parentNode
	previousSibling
	textContent
	appendChild(aChild)
	cloneNode([deep])
	compareDocumentPosition(otherNode)
	contains(otherNode)
	getRootNode([options])
	hasChildNodes()
	insertBefore(newNode, referenceNode)
	isDefaultNamespace(namespaceURI)
	isEqualNode(otherNode)
	isSameNode(otherNode)
	lookupNamespaceURI(prefix)
	lookupPrefix(namespace)
	normalize()
	removeChild(child)
	replaceChild(newChild, oldChild)
}

// https://developer.mozilla.org/en-US/docs/Web/API/Element
{
	assignedSlot
	attributes
	childElementCount
	children
	classList
	className
	clientHeight
	clientLeft
	clientTop
	clientWidth
	firstElementChild
	id
	innerHTML
	lastElementChild
	localName
	namespaceURI
	nextElementSibling
	outerHTML
	part
	prefix
	previousElementSibling
	scrollHeight
	scrollLeft
	scrollTop
	scrollWidth
	shadowRoot
	slot
	tagName
	after(node1, node2, /* ... ,*/ nodeN)
	animate(keyframes, options)
	append(param1, param2, /* ... ,*/ paramN)
	attachShadow(options)
	before(param1, param2, /* ... ,*/ paramN)
	closest(selectors)
	getAttribute(attributeName)
	getAttributeNames()
	getAttributeNode(attrName)
	getAttributeNodeNS(namespace, nodeName)
	getAttributeNS(namespace, name)
	getBoundingClientRect()
	getClientRects()
	getElementsByClassName(names)
	getElementsByTagName(tagName)
	getElementsByTagNameNS(namespaceURI, localName)
	hasAttribute(name)
	hasAttributeNS(namespace,localName)
	hasAttributes()
	hasPointerCapture(pointerId)
	insertAdjacentElement(position, element)
	insertAdjacentHTML(position, text)
	insertAdjacentText(position, element)
	matches(selectorString)
	prepend(param1, param2, /* ... ,*/ paramN)
	querySelector(selectors)
	querySelectorAll(selectors)
	releasePointerCapture(pointerId)
	remove()
	removeAttribute(attrName)
	removeAttributeNode(attributeNode)
	removeAttributeNS(namespace, attrName)
	replaceChildren(param1, param2, /* ... ,*/ paramN)
	replaceWith(param1, param2, /* ... ,*/ paramN)
	requestFullscreen([options])
	scroll(x-coord, y-coord)
	scroll(options)
	scrollBy(x-coord, y-coord)
	scrollBy(options)
	scrollTo(x-coord, y-coord)
	scrollTo(options)
	setAttribute(name, value)
	setAttributeNode(attribute)
	setAttributeNodeNS(attributeNode)
	setAttributeNS(namespace, name, value)
	setPointerCapture(pointerId)
	toggleAttribute(name [, force])
}

// https://developer.mozilla.org/en-US/docs/Web/API/Document
Document() {
	activeElement
	body
	characterSet
	childElementCount
	children
	currentScript
	defaultView
	designMode
	doctype
	documentElement
	documentURI
	embeds
	firstElementChild
	fonts
	forms
	fullscreenElement
	head
	hidden
	images
	implementation
	lastElementChild
	links
	pictureInPictureElement
	pictureInPictureEnabled
	plugins
	pointerLockElement
	scripts
	scrollingElement
	styleSheets
	visibilityState

	// Extensions
	cookie
	defaultView
	designMode
	dir
	domain
	lastModified
	location
	readyState
	referrer
	title
	URL
	// Methods
	adoptNode(externalNode)
	append(param1, param2, /* ... ,*/ paramN)
	createAttribute(name)
	createAttributeNS(namespaceURI, qualifiedName)
	createCDATASection(data)
	createComment(data)
	createDocumentFragment()
	createElement(tagName[, options])
	createElementNS(namespaceURI, qualifiedName[, options])
	createExpression(xpathText, namespaceURLMapper)
	createNodeIterator(root[, whatToShow[, filter]])
	createNSResolver(node)
	createProcessingInstruction(target, data)
	createRange()
	createTextNode(data)
	createTreeWalker(root, whatToShow, filter)
	evaluate(xpathExpression, contextNode, namespaceResolver, resultType, result)
	elementFromPoint(x, y)
	elementsFromPoint(x, y)
	exitFullscreen()
	exitPictureInPicture()
	getAnimations()
	getElementById(id)
	getElementsByClassName(names)
	getElementsByName(name)
	getElementsByTagName(name)
	getElementsByTagNameNS(namespace, name)
	getSelection()
	importNode(externalNode [, deep])
	prepend(param1, param2, /* ... ,*/ paramN)
	querySelector(selectors)
	querySelectorAll(selectors)
	replaceChildren(param1, param2, /* ... ,*/ paramN)
	// Extension
	close()
	hasFocus()
	open()
	write(markup)
	writeln(line)
}

// https://developer.mozilla.org/en-US/docs/Web/API/Window
{
	clientInformation
	closed
	console
	customElements
	crypto
	devicePixelRatio
	document
	event
	frameElement
	fullScreen
	frames
	history
	innerHeight
	innerWidth
	length
	localStorage
	location
	locationbar
	menubar
	name
	navigator
	opener
	outerHeight
	outerWidth
	pageXOffset
	pageYOffset
	parent
	performance
	personalbar
	screen
	screenX
	screenLeft
	screenY
	screenTop
	scrollbars
	scrollX
	scrollY
	self
	sessionStorage
	status
	statusbar
	toolbar
	top
	visualViewport
	window
	alert(message)
	blur()
	close()
	confirm(message)
	focus()
	getComputedStyle(element [, pseudoElt])
	getSelection()
	matchMedia(mediaQueryString)
	moveBy(deltaX, deltaY)
	moveTo(x, y)
	open(url, windowName, [windowFeatures])
	postMessage(message, targetOrigin, [transfer])
	print()
	prompt(message, default)
	requestAnimationFrame(callback)
	resizeBy(xDelta, yDelta)
	resizeTo(width, height)
	scroll(x-coord, y-coord)
	scroll(options)
	scrollBy(x-coord, y-coord)
	scrollBy(options)
	scrollTo(x-coord, y-coord)
	scrollTo(options)
	showOpenFilePicker()
	showSaveFilePicker()
	stop()
}

// https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest
XMLHttpRequest() {
	UNSENT
	OPENED
	HEADERS_RECEIVED
	LOADING
	DONE

	onreadystatechange
	readyState
	response
	responseText
	responseType
	responseURL
	responseXML
	status
	statusText
	timeout
	upload
	withCredentials
	abort()
	getAllResponseHeaders()
	getResponseHeader(headerName)
	open(method, url [, async [, user[, password]]])
	overrideMimeType(mimeType)
	send(body)
	setRequestHeader(header, value)
}

// https://developer.mozilla.org/en-US/docs/Web/API/FormData
FormData([form]) {
	append(name, value [, filename])
	delete(name)
	entries()
	get(name)
	getAll(name)
	has(name)
	keys()
	set(name, value [, filename])
	values()
}

Worker() {
	postMessage(message, transfer)
	terminate()
}

// https://developer.mozilla.org/en-US/docs/Web/API/WorkerGlobalScope
{
	caches
	indexedDB
	origin
	scheduler
	importScripts(path0, path1, /* ... ,*/ pathN)
	atob(encodedData)
	btoa(stringToEncode)
	clearInterval(intervalID)
	clearTimeout(timeoutID)
	createImageBitmap(image[, sx, sy, sw, sh[, options]]).then(function(response) { ... })
	fetch(input[, init]) -> Promise<Response>
	queueMicrotask(function)
	setInterval(func, delay[, param1, param2, ...])
	setTimeout(function[, delay, param1, param2, ...])
	reportError(throwable)
}

// https://developer.mozilla.org/en-US/docs/Web/API/Storage
{
	length
	clear()
	getItem(keyName)
	key(index)
	removeItem(keyName)
	setItem(keyName, keyValue)
}
