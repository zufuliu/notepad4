// 5.2 https://www.typescriptlang.org/docs/
// 1.8 https://github.com/microsoft/TypeScript/blob/main/doc/

//! keywords		===========================================================
// textToKeywordObj https://github.com/microsoft/TypeScript/tree/main/src/compiler/scanner.ts
abstract accessor as assert asserts async await
break
case catch class const constructor continue
debugger declare default delete do
else enum extends
false finally for function
get global
if implements in infer instanceof interface intrinsic is
keyof
let
new null
of out override
package private protected public
readonly return
satisfies set static super switch
this throw true try type typeof
undefined unique using
var
while with
yield

//! directive		===========================================================
namespace module export import require from

//! types			===========================================================
any bigint boolean never number object string symbol unknown void

//! decorators		===========================================================
// https://www.typescriptlang.org/docs/handbook/decorators.html


//! API				===========================================================
// use what in JavaScript.js
// https://github.com/microsoft/TypeScript/tree/main/lib/

//! TSDoc			===========================================================
// https://www.typescriptlang.org/docs/handbook/triple-slash-directives.html
/// <reference path="" />
/// <reference types="" />
/// <reference lib="" />
/// <reference no-default-lib="true" />
/// <amd-module name="" />
/// <amd-dependency path="" name="" />

// https://tsdoc.org/
/**
 * @alpha
 * @beta
 * @decorator
 * @deprecated
 * @defaultValue
 * @eventProperty
 * @example
 * @experimental
 * {@inheritDoc }
 * @internal
 * @label
 * {@link }
 * @override
 * @packageDocumentation
 * @param
 * @privateRemarks
 * @public
 * @readonly
 * @remarks
 * @returns
 * @sealed
 * @see
 * @throws
 * @typeParam
 * @virtual
 */
