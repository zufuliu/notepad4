-- GHC 9.6 https://www.haskell.org/
-- https://www.haskell.org/documentation/
-- https://www.haskell.org/onlinereport/haskell2010/
-- https://downloads.haskell.org/ghc/latest/docs/users_guide/

--! keywords		===========================================================
-- https://wiki.haskell.org/Keywords
as
case class
data
default deriving do
else
family forall foreign
hiding
if import in infix infixl infixr instance
let
mdo module
newtype
of
proc
qualified
rec
then type
where

True False Nothing

--! class				===========================================================
-- https://www.haskell.org/onlinereport/haskell2010/haskellch6.html
Bool
Char String
Maybe Either Ordering
Eq Ord Read Show
Enum
Functor
IO
Monad
Bounded
Num Real
Integral Int Integer
Fractional RealFrac
Floating RealFloat Float  Double Complex
Rational Ratio

-- https://www.haskell.org/onlinereport/haskell2010/haskellpa2.html
Array
Word

-- https://downloads.haskell.org/ghc/latest/docs/users_guide/exts/pragmas.html

-- https://hackage.haskell.org/package/cpphs
-- use C++ preprocessor
