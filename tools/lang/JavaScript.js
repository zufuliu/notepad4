// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Lexical_grammar
// 2020 11.0 https://www.ecma-international.org/publications/standards/Ecma-262.htm

//! keywords
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

//! future reserved words
enum implements interface package private protected public
// ECMAScript 1 till 3
// abstract boolean byte char double final float goto int long native short synchronized throws transient volatile

// typeof Operator Results
// undefined object boolean number string symbol bigint function

// Meta Properties
// new.target // 2020
// import.meta // 2020

//! module
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

//! API
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
	message
	name

	AggregateError // MDN
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
		concat(...arguments)
		copyWithin(target, start [, end])
		entries()
		every(callbackfn [, thisArg])
		fill(value [, start [, end]])
		filter(callbackfn [, thisArg])
		find(predicate [, thisArg])
		findIndex(predicate [, thisArg])
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
		unshift(...items)
		values()
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
	parentNode
	parentElement
	previousSibling
	textContent
	appendChild(aChild)
	cloneNode([deep])
	compareDocumentPosition(otherNode)
	contains(otherNode)
	getRootNode(options)
	hasChildNodes()
	insertBefore(newNode, referenceNode)
	isDefaultNamespace(namespaceURI)
	isEqualNode(otherNode)
	isSameNode(otherNode)
	lookupPrefix()
	lookupNamespaceURI(prefix)
	normalize()
	removeChild(child)
	replaceChild(newChild, oldChild)
}

// https://developer.mozilla.org/en-US/docs/Web/API/Element
{
	attributes
	classList
	className
	clientHeight
	clientLeft
	clientTop
	clientWidth
	id
	innerHTML
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
	assignedSlot
	attachShadow(shadowRootInit)
	getAttribute(attributeName)
	getAttributeNames()
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
	querySelector(selectors)
	querySelectorAll(selectors)
	releasePointerCapture(pointerId)
	removeAttribute(attrName)
	removeAttributeNS(namespace, attrName)
	scroll(x-coord, y-coord)
	scroll(options)
	scrollBy(x-coord, y-coord)
	scrollBy(options)
	scrollTo(x-coord, y-coord)
	scrollTo(options)
	setAttribute(name, value)
	setAttributeNS(namespace, name, value)
	setPointerCapture(pointerId)
	toggleAttribute(name [, force])
}

// https://developer.mozilla.org/en-US/docs/Web/API/Document
Document() {
	body
	characterSet
	doctype
	documentElement
	documentURI
	embeds
	fonts
	forms
	head
	hidden
	images
	implementation
	links
	plugins
	scripts
	scrollingElement
	visibilityState
	childElementCount
	children
	firstElementChild
	lastElementChild
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
	activeElement
	fullscreenElement
	// Methods
	adoptNode(externalNode)
	createAttribute(name)
	createCDATASection(data)
	createComment(data)
	createDocumentFragment()
	createElement(tagName[, options])
	createElementNS(namespaceURI, qualifiedName[, options])
	createNodeIterator(root[, whatToShow[, filter]])
	createProcessingInstruction(target, data)
	createRange()
	createTextNode(data)
	createTreeWalker(root, whatToShow[, filter[, entityReferenceExpansion]])
	getElementsByClassName(names)
	getElementsByTagName(name)
	getElementsByTagNameNS(namespace, name)
	importNode(externalNode [, deep])
	getElementById(id)
	querySelector(selectors)
	querySelectorAll(selectors)
	createExpression(xpathText, namespaceURLMapper)
	createNSResolver(node)
	evaluate(xpathExpression, contextNode, namespaceResolver, resultType, result)
	// Extension
	close()
	getElementsByName(name)
	hasFocus()
	open()
	write(markup)
	writeln(line)
	elementFromPoint(x, y)
}

// https://developer.mozilla.org/en-US/docs/Web/API/Window
{
	console
	customElements
	crypto
	document
	event
	frameElement
	frames
	history
	innerHeight
	innerWidth
	length
	location
	locationbar
	localStorage
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

// https://developer.mozilla.org/en-US/docs/Web/API/WindowOrWorkerGlobalScope
{
	indexedDB
	atob(encodedData)
	btoa(stringToEncode)
	clearInterval(intervalID)
	clearTimeout(timeoutID)
	createImageBitmap(image[, sx, sy, sw, sh[, options]]).then(function(response) { ... })
	fetch(input[, init]) -> Promise<Response>
	queueMicrotask(function)
	setInterval(func, delay[, param1, param2, ...])
	setTimeout(function[, delay, param1, param2, ...])
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

// https://api.jquery.com/
jQuery
