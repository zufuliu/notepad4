" Vim Reference Manual 8.1
" https://vimhelp.org/
" Expression evaluation
" https://vimhelp.org/eval.txt.html#eval.txt
" Write a Vim script
" https://vimhelp.org/usr_41.txt.html#usr_41.txt

"! keywords			===========================================================
" Expression syntax
is isnot
END
" Defining functions
function
endfunction
delfunction
delfun
return
call
func
endfunc
fun
endfun endf
" Commands
let
unlet
const
lockvar
unlockvar
if
elseif
else
endif
if
end
while
endwhile endw
for in
endfor
continue
break
try
catch
finally
throw
endtry
" Automatic commands
au autocmd

"! commands			===========================================================
echo
echon
echoh echohl
echom echomsg
echoe echoerr
exe exec execute
finish

" Setting options
" https://vimhelp.org/options.txt.html#options.txt
set
setl setlocal
setg setglobal
setf setfiletype

" Key mapping
" https://vimhelp.org/map.txt.html#map.txt
map
unmap

" Automatic commands
" https://vimhelp.org/autocmd.txt.html#autocmd.txt
aug augroup
do doautocmd
doautoall
noautocmd

" Various commands
" https://vimhelp.org/various.txt.html#various.txt
ascii
print
normal
version
redir
filter
silent
unsilent
verbose

" Syntax highlighting
" https://vimhelp.org/syntax.txt.html#syntax.txt
syn syntax
