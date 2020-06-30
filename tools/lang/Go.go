// Go 1.14 https://golang.org/
// https://github.com/golang/go/blob/master/doc/go_spec.html

//! Keywords
break
case chan const continue
default defer
else
fallthrough for func
go goto
if import interface
map
package
range return
select struct switch
type
var

true false iota nil

//! Types
bool byte complex64 complex128 error float32 float64
int int8 int16 int32 int64 rune string
uint uint8 uint16 uint32 uint64 uintptr

//! builtin
func append(slice []Type, elems ...Type) []Type
func cap(v Type) int
func close(c chan<- Type)
func complex(r, i FloatType) ComplexType
func copy(dst, src []Type) int
func delete(m map[Type]Type1, key Type)
func imag(c ComplexType) FloatType
func len(v Type) int
func make(t Type, size ...IntegerType) Type
func new(Type) *Type
func panic(v interface{})
func print(args ...Type)
func println(args ...Type)
func real(c ComplexType) FloatType
func recover() interface{}

//! packages
// run godoc on go/src folder

package bytes {
	func Compare(a, b []byte) int
	func Contains(b, subslice []byte) bool
	func ContainsAny(b []byte, chars string) bool
	func ContainsRune(b []byte, r rune) bool
	func Count(s, sep []byte) int
	func Equal(a, b []byte) bool
	func EqualFold(s, t []byte) bool
	func Fields(s []byte) [][]byte
	func FieldsFunc(s []byte, f func(rune) bool) [][]byte
	func HasPrefix(s, prefix []byte) bool
	func HasSuffix(s, suffix []byte) bool
	func Index(s, sep []byte) int
	func IndexAny(s []byte, chars string) int
	func IndexByte(b []byte, c byte) int
	func IndexFunc(s []byte, f func(r rune) bool) int
	func IndexRune(s []byte, r rune) int
	func Join(s [][]byte, sep []byte) []byte
	func LastIndex(s, sep []byte) int
	func LastIndexAny(s []byte, chars string) int
	func LastIndexByte(s []byte, c byte) int
	func LastIndexFunc(s []byte, f func(r rune) bool) int
	func Map(mapping func(r rune) rune, s []byte) []byte
	func Repeat(b []byte, count int) []byte
	func Replace(s, old, new []byte, n int) []byte
	func ReplaceAll(s, old, new []byte) []byte
	func Runes(s []byte) []rune
	func Split(s, sep []byte) [][]byte
	func SplitAfter(s, sep []byte) [][]byte
	func SplitAfterN(s, sep []byte, n int) [][]byte
	func SplitN(s, sep []byte, n int) [][]byte
	func Title(s []byte) []byte
	func ToLower(s []byte) []byte
	func ToLowerSpecial(c unicode.SpecialCase, s []byte) []byte
	func ToTitle(s []byte) []byte
	func ToTitleSpecial(c unicode.SpecialCase, s []byte) []byte
	func ToUpper(s []byte) []byte
	func ToUpperSpecial(c unicode.SpecialCase, s []byte) []byte
	func ToValidUTF8(s, replacement []byte) []byte
	func Trim(s []byte, cutset string) []byte
	func TrimFunc(s []byte, f func(r rune) bool) []byte
	func TrimLeft(s []byte, cutset string) []byte
	func TrimLeftFunc(s []byte, f func(r rune) bool) []byte
	func TrimPrefix(s, prefix []byte) []byte
	func TrimRight(s []byte, cutset string) []byte
	func TrimRightFunc(s []byte, f func(r rune) bool) []byte
	func TrimSpace(s []byte) []byte
	func TrimSuffix(s, suffix []byte) []byte

	type Buffer struct {
		func NewBuffer(buf []byte) *Buffer
		func NewBufferString(s string) *Buffer
		func (b *Buffer) Bytes() []byte
		func (b *Buffer) Cap() int
		func (b *Buffer) Grow(n int)
		func (b *Buffer) Len() int
		func (b *Buffer) Next(n int) []byte
		func (b *Buffer) Read(p []byte) (n int, err error)
		func (b *Buffer) ReadByte() (byte, error)
		func (b *Buffer) ReadBytes(delim byte) (line []byte, err error)
		func (b *Buffer) ReadFrom(r io.Reader) (n int64, err error)
		func (b *Buffer) ReadRune() (r rune, size int, err error)
		func (b *Buffer) ReadString(delim byte) (line string, err error)
		func (b *Buffer) Reset()
		func (b *Buffer) String() string
		func (b *Buffer) Truncate(n int)
		func (b *Buffer) UnreadByte() error
		func (b *Buffer) UnreadRune() error
		func (b *Buffer) Write(p []byte) (n int, err error)
		func (b *Buffer) WriteByte(c byte) error
		func (b *Buffer) WriteRune(r rune) (n int, err error)
		func (b *Buffer) WriteString(s string) (n int, err error)
		func (b *Buffer) WriteTo(w io.Writer) (n int64, err error)
	}
	type Reader struct {
		func NewReader(b []byte) *Reader
		func (r *Reader) Len() int
		func (r *Reader) Read(b []byte) (n int, err error)
		func (r *Reader) ReadAt(b []byte, off int64) (n int, err error)
		func (r *Reader) ReadByte() (byte, error)
		func (r *Reader) ReadRune() (ch rune, size int, err error)
		func (r *Reader) Reset(b []byte)
		func (r *Reader) Seek(offset int64, whence int) (int64, error)
		func (r *Reader) Size() int64
		func (r *Reader) UnreadByte() error
		func (r *Reader) UnreadRune() error
		func (r *Reader) WriteTo(w io.Writer) (n int64, err error)
	}
}

package context {
	func WithCancel(parent Context) (ctx Context, cancel CancelFunc)
	func WithDeadline(parent Context, d time.Time) (Context, CancelFunc)
	func WithTimeout(parent Context, timeout time.Duration) (Context, CancelFunc)

	type Context interface {
		Deadline() (deadline time.Time, ok bool)
		Done() <-chan struct{}
		Err() error
		Value(key interface{}) interface{}
		func WithValue(parent Context, key, val interface{}) Context
	}
}

package encoding/json {
	func Compact(dst *bytes.Buffer, src []byte) error
	func HTMLEscape(dst *bytes.Buffer, src []byte)
	func Indent(dst *bytes.Buffer, src []byte, prefix, indent string) error
	func Marshal(v interface{}) ([]byte, error)
	func MarshalIndent(v interface{}, prefix, indent string) ([]byte, error)
	func Unmarshal(data []byte, v interface{}) error
	func Valid(data []byte) bool
	type Decoder struct {
		func NewDecoder(r io.Reader) *Decoder
		func (dec *Decoder) Buffered() io.Reader
		func (dec *Decoder) Decode(v interface{}) error
		func (dec *Decoder) DisallowUnknownFields()
		func (dec *Decoder) InputOffset() int64
		func (dec *Decoder) More() bool
		func (dec *Decoder) Token() (Token, error)
		func (dec *Decoder) UseNumber()
	}
	type Encoder struct {
		func NewEncoder(w io.Writer) *Encoder
		func (enc *Encoder) Encode(v interface{}) error
		func (enc *Encoder) SetEscapeHTML(on bool)
		func (enc *Encoder) SetIndent(prefix, indent string)
	}
}

package errors {
	func As(err error, target interface{}) bool
	func Is(err, target error) bool
	func New(text string) error
	func Unwrap(err error) error
}

package fmt {
	func Errorf(format string, a ...interface{}) error
	func Fprint(w io.Writer, a ...interface{}) (n int, err error)
	func Fprintf(w io.Writer, format string, a ...interface{}) (n int, err error)
	func Fprintln(w io.Writer, a ...interface{}) (n int, err error)
	func Fscan(r io.Reader, a ...interface{}) (n int, err error)
	func Fscanf(r io.Reader, format string, a ...interface{}) (n int, err error)
	func Fscanln(r io.Reader, a ...interface{}) (n int, err error)
	func Print(a ...interface{}) (n int, err error)
	func Printf(format string, a ...interface{}) (n int, err error)
	func Println(a ...interface{}) (n int, err error)
	func Scan(a ...interface{}) (n int, err error)
	func Scanf(format string, a ...interface{}) (n int, err error)
	func Scanln(a ...interface{}) (n int, err error)
	func Sprint(a ...interface{}) string
	func Sprintf(format string, a ...interface{}) string
	func Sprintln(a ...interface{}) string
	func Sscan(str string, a ...interface{}) (n int, err error)
	func Sscanf(str string, format string, a ...interface{}) (n int, err error)
	func Sscanln(str string, a ...interface{}) (n int, err error)
}

package io {
	const (
		SeekStart
		SeekCurrent
		SeekEnd
	)
	var (
		EOF
	)

	func Copy(dst Writer, src Reader) (written int64, err error)
	func CopyBuffer(dst Writer, src Reader, buf []byte) (written int64, err error)
	func CopyN(dst Writer, src Reader, n int64) (written int64, err error)
	func Pipe() (*PipeReader, *PipeWriter)
	func ReadAtLeast(r Reader, buf []byte, min int) (n int, err error)
	func ReadFull(r Reader, buf []byte) (n int, err error)
	func WriteString(w Writer, s string) (n int, err error)

	type ByteReader interface {
		ReadByte() (byte, error)
	}
	type ByteScanner interface {
		ByteReader
		UnreadByte() error
	}
	type ByteWriter interface {
		WriteByte(c byte) error
	}
	type Closer interface {
		Close() error
	}
	type LimitedReader struct
	type PipeReader struct {
		func (r *PipeReader) Close() error
		func (r *PipeReader) CloseWithError(err error) error
		func (r *PipeReader) Read(data []byte) (n int, err error)
	}
	type PipeWriter struct {
		func (w *PipeWriter) Close() error
		func (w *PipeWriter) CloseWithError(err error) error
		func (w *PipeWriter) Write(data []byte) (n int, err error)
	}
	type ReadCloser interface
	type ReadSeeker interface
	type ReadWriteCloser interface
	type ReadWriteSeeker interface
	type ReadWriter interface
	type Reader interface  {
		Read(p []byte) (n int, err error)
		func LimitReader(r Reader, n int64) Reader
		func MultiReader(readers ...Reader) Reader
		func TeeReader(r Reader, w Writer) Reader
	}
	type ReaderAt interface {
		ReadAt(p []byte, off int64) (n int, err error)
	}
	type ReaderFrom interface {
		ReadFrom(r Reader) (n int64, err error)
	}
	type RuneReader interface {
		ReadRune() (r rune, size int, err error)
	}
	type RuneScanner interface {
		UnreadRune() error
	}
	type SectionReader struct {
		func NewSectionReader(r ReaderAt, off int64, n int64) *SectionReader
		func (s *SectionReader) Read(p []byte) (n int, err error)
		func (s *SectionReader) ReadAt(p []byte, off int64) (n int, err error)
		func (s *SectionReader) Seek(offset int64, whence int) (int64, error)
		func (s *SectionReader) Size() int64
	}
	type Seeker interface {
		Seek(offset int64, whence int) (int64, error)
	}
	type StringWriter interface {
		WriteString(s string) (n int, err error)
	}
	type WriteCloser interface
	type WriteSeeker interface
	type Writer interface {
		Write(p []byte) (n int, err error)
		func MultiWriter(writers ...Writer) Writer
	}
	type WriterAt interface {
		WriteAt(p []byte, off int64) (n int, err error)
	}
	type WriterTo interface {
		WriteTo(w Writer) (n int64, err error)
	}
}

package io/ioutil {
	var Discard io.Writer = devNull(0)
	func NopCloser(r io.Reader) io.ReadCloser
	func ReadAll(r io.Reader) ([]byte, error)
	func ReadDir(dirname string) ([]os.FileInfo, error)
	func ReadFile(filename string) ([]byte, error)
	func TempDir(dir, pattern string) (name string, err error)
	func TempFile(dir, pattern string) (f *os.File, err error)
	func WriteFile(filename string, data []byte, perm os.FileMode) error
}

package log {
	func Fatal(v ...interface{})
	func Fatalf(format string, v ...interface{})
	func Fatalln(v ...interface{})
	func Flags() int
	func Output(calldepth int, s string) error
	func Panic(v ...interface{})
	func Panicf(format string, v ...interface{})
	func Panicln(v ...interface{})
	func Prefix() string
	func Print(v ...interface{})
	func Printf(format string, v ...interface{})
	func Println(v ...interface{})
	func SetFlags(flag int)
	func SetOutput(w io.Writer)
	func SetPrefix(prefix string)
	func Writer() io.Writer
	type Logger struct {
		func New(out io.Writer, prefix string, flag int) *Logger
		func (l *Logger) Fatal(v ...interface{})
		func (l *Logger) Fatalf(format string, v ...interface{})
		func (l *Logger) Fatalln(v ...interface{})
		func (l *Logger) Flags() int
		func (l *Logger) Output(calldepth int, s string) error
		func (l *Logger) Panic(v ...interface{})
		func (l *Logger) Panicf(format string, v ...interface{})
		func (l *Logger) Panicln(v ...interface{})
		func (l *Logger) Prefix() string
		func (l *Logger) Print(v ...interface{})
		func (l *Logger) Printf(format string, v ...interface{})
		func (l *Logger) Println(v ...interface{})
		func (l *Logger) SetFlags(flag int)
		func (l *Logger) SetOutput(w io.Writer)
		func (l *Logger) SetPrefix(prefix string)
		func (l *Logger) Writer() io.Writer
	}
}

package math {
	const (
		E
		Pi
		Phi

		Sqrt2
		SqrtE
		SqrtPi
		SqrtPhi

		Ln2
		Log2E
		Ln10
		Log10E
	)
	func Abs(x float64) float64
	func Acos(x float64) float64
	func Acosh(x float64) float64
	func Asin(x float64) float64
	func Asinh(x float64) float64
	func Atan(x float64) float64
	func Atan2(y, x float64) float64
	func Atanh(x float64) float64
	func Cbrt(x float64) float64
	func Ceil(x float64) float64
	func Copysign(x, y float64) float64
	func Cos(x float64) float64
	func Cosh(x float64) float64
	func Dim(x, y float64) float64
	func Erf(x float64) float64
	func Erfc(x float64) float64
	func Erfcinv(x float64) float64
	func Erfinv(x float64) float64
	func Exp(x float64) float64
	func Exp2(x float64) float64
	func Expm1(x float64) float64
	func FMA(x, y, z float64) float64
	func Float32bits(f float32) uint32
	func Float32frombits(b uint32) float32
	func Float64bits(f float64) uint64
	func Float64frombits(b uint64) float64
	func Floor(x float64) float64
	func Frexp(f float64) (frac float64, exp int)
	func Gamma(x float64) float64
	func Hypot(p, q float64) float64
	func Ilogb(x float64) int
	func Inf(sign int) float64
	func IsInf(f float64, sign int) bool
	func IsNaN(f float64) (is bool)
	func J0(x float64) float64
	func J1(x float64) float64
	func Jn(n int, x float64) float64
	func Ldexp(frac float64, exp int) float64
	func Lgamma(x float64) (lgamma float64, sign int)
	func Log(x float64) float64
	func Log10(x float64) float64
	func Log1p(x float64) float64
	func Log2(x float64) float64
	func Logb(x float64) float64
	func Max(x, y float64) float64
	func Min(x, y float64) float64
	func Mod(x, y float64) float64
	func Modf(f float64) (int float64, frac float64)
	func NaN() float64
	func Nextafter(x, y float64) (r float64)
	func Nextafter32(x, y float32) (r float32)
	func Pow(x, y float64) float64
	func Pow10(n int) float64
	func Remainder(x, y float64) float64
	func Round(x float64) float64
	func RoundToEven(x float64) float64
	func Signbit(x float64) bool
	func Sin(x float64) float64
	func Sincos(x float64) (sin, cos float64)
	func Sinh(x float64) float64
	func Sqrt(x float64) float64
	func Tan(x float64) float64
	func Tanh(x float64) float64
	func Trunc(x float64) float64
	func Y0(x float64) float64
	func Y1(x float64) float64
	func Yn(n int, x float64) float64
}

package os {
	const (
		O_RDONLY
		O_WRONLY
		O_RDWR
		O_APPEND
		O_CREATE
		O_EXCL
		O_SYNC
		O_TRUNC
	)
	const (
		PathSeparator
		PathListSeparator
	)
	const DevNull
	var (
		Stdin
		Stdout
		Stderr
	)
	var Args []string

	func Chdir(dir string) error
	func Chmod(name string, mode FileMode) error
	func Chown(name string, uid, gid int) error
	func Chtimes(name string, atime time.Time, mtime time.Time) error
	func Clearenv()
	func Environ() []string
	func Executable() (string, error)
	func Exit(code int)
	func Expand(s string, mapping func(string) string) string
	func ExpandEnv(s string) string
	func Getegid() int
	func Getenv(key string) string
	func Geteuid() int
	func Getgid() int
	func Getgroups() ([]int, error)
	func Getpagesize() int
	func Getpid() int
	func Getppid() int
	func Getuid() int
	func Getwd() (dir string, err error)
	func Hostname() (name string, err error)
	func IsExist(err error) bool
	func IsNotExist(err error) bool
	func IsPathSeparator(c uint8) bool
	func IsPermission(err error) bool
	func IsTimeout(err error) bool
	func Lchown(name string, uid, gid int) error
	func Link(oldname, newname string) error
	func LookupEnv(key string) (string, bool)
	func Mkdir(name string, perm FileMode) error
	func MkdirAll(path string, perm FileMode) error
	func NewSyscallError(syscall string, err error) error
	func Pipe() (r *File, w *File, err error)
	func Readlink(name string) (string, error)
	func Remove(name string) error
	func RemoveAll(path string) error
	func Rename(oldpath, newpath string) error
	func SameFile(fi1, fi2 FileInfo) bool
	func Setenv(key, value string) error
	func Symlink(oldname, newname string) error
	func TempDir() string
	func Truncate(name string, size int64) error
	func Unsetenv(key string) error
	func UserCacheDir() (string, error)
	func UserConfigDir() (string, error)
	func UserHomeDir() (string, error)

	type File struct {
		func Create(name string) (*File, error)
		func NewFile(fd uintptr, name string) *File
		func Open(name string) (*File, error)
		func OpenFile(name string, flag int, perm FileMode) (*File, error)
		func (f *File) Chdir() error
		func (f *File) Chmod(mode FileMode) error
		func (f *File) Chown(uid, gid int) error
		func (file *File) Close() error
		func (file *File) Fd() uintptr
		func (f *File) Name() string
		func (f *File) Read(b []byte) (n int, err error)
		func (f *File) ReadAt(b []byte, off int64) (n int, err error)
		func (f *File) Readdir(n int) ([]FileInfo, error)
		func (f *File) Readdirnames(n int) (names []string, err error)
		func (f *File) Seek(offset int64, whence int) (ret int64, err error)
		func (f *File) SetDeadline(t time.Time) error
		func (f *File) SetReadDeadline(t time.Time) error
		func (f *File) SetWriteDeadline(t time.Time) error
		func (file *File) Stat() (FileInfo, error)
		func (f *File) Sync() error
		func (f *File) Truncate(size int64) error
		func (f *File) Write(b []byte) (n int, err error)
		func (f *File) WriteAt(b []byte, off int64) (n int, err error)
		func (f *File) WriteString(s string) (n int, err error)
	}
	type FileInfo interface {
		Name() string
		Size() int64
		Mode() FileMode
		ModTime() time.Time
		IsDir() bool
		Sys() interface{}
		func Lstat(name string) (FileInfo, error)
		func Stat(name string) (FileInfo, error)
	}
	type FileMode uint32
	const (
		ModeDir
		ModeAppend
		ModeExclusive
		ModeTemporary
		ModeSymlink
		ModeDevice
		ModeNamedPipe
		ModeSocket
		ModeSetuid
		ModeSetgid
		ModeCharDevice
		ModeSticky
		ModeIrregular

		ModeType
		ModePerm
	)
}

package path {
	func Base(path string) string
	func Clean(path string) string
	func Dir(path string) string
	func Ext(path string) string
	func IsAbs(path string) bool
	func Join(elem ...string) string
	func Match(pattern, name string) (matched bool, err error)
	func Split(path string) (dir, file string)
}

package path/filepath {
	func Abs(path string) (string, error)
	func Base(path string) string
	func Clean(path string) string
	func Dir(path string) string
	func EvalSymlinks(path string) (string, error)
	func Ext(path string) string
	func FromSlash(path string) string
	func Glob(pattern string) (matches []string, err error)
	func HasPrefix(p, prefix string) bool
	func IsAbs(path string) (b bool)
	func Join(elem ...string) string
	func Match(pattern, name string) (matched bool, err error)
	func Rel(basepath, targpath string) (string, error)
	func Split(path string) (dir, file string)
	func SplitList(path string) []string
	func ToSlash(path string) string
	func VolumeName(path string) string
	func Walk(root string, walkFn WalkFunc) error
}

package sort {
	func Float64s(a []float64)
	func Float64sAreSorted(a []float64) bool
	func Ints(a []int)
	func IntsAreSorted(a []int) bool
	func IsSorted(data Interface) bool
	func Search(n int, f func(int) bool) int
	func SearchFloat64s(a []float64, x float64) int
	func SearchInts(a []int, x int) int
	func SearchStrings(a []string, x string) int
	func Slice(slice interface{}, less func(i, j int) bool)
	func SliceIsSorted(slice interface{}, less func(i, j int) bool) bool
	func SliceStable(slice interface{}, less func(i, j int) bool)
	func Sort(data Interface)
	func Stable(data Interface)
	func Strings(a []string)
	func StringsAreSorted(a []string) bool
}

package strconv {
	func AppendBool(dst []byte, b bool) []byte
	func AppendFloat(dst []byte, f float64, fmt byte, prec, bitSize int) []byte
	func AppendInt(dst []byte, i int64, base int) []byte
	func AppendQuote(dst []byte, s string) []byte
	func AppendQuoteRune(dst []byte, r rune) []byte
	func AppendQuoteRuneToASCII(dst []byte, r rune) []byte
	func AppendQuoteRuneToGraphic(dst []byte, r rune) []byte
	func AppendQuoteToASCII(dst []byte, s string) []byte
	func AppendQuoteToGraphic(dst []byte, s string) []byte
	func AppendUint(dst []byte, i uint64, base int) []byte
	func Atoi(s string) (int, error)
	func CanBackquote(s string) bool
	func FormatBool(b bool) string
	func FormatFloat(f float64, fmt byte, prec, bitSize int) string
	func FormatInt(i int64, base int) string
	func FormatUint(i uint64, base int) string
	func IsGraphic(r rune) bool
	func IsPrint(r rune) bool
	func Itoa(i int) string
	func ParseBool(str string) (bool, error)
	func ParseFloat(s string, bitSize int) (float64, error)
	func ParseInt(s string, base int, bitSize int) (i int64, err error)
	func ParseUint(s string, base int, bitSize int) (uint64, error)
	func Quote(s string) string
	func QuoteRune(r rune) string
	func QuoteRuneToASCII(r rune) string
	func QuoteRuneToGraphic(r rune) string
	func QuoteToASCII(s string) string
	func QuoteToGraphic(s string) string
	func Unquote(s string) (string, error)
	func UnquoteChar(s string, quote byte) (value rune, multibyte bool, tail string, err error)
}

package strings {
	func Compare(a, b string) int
	func Contains(s, substr string) bool
	func ContainsAny(s, chars string) bool
	func ContainsRune(s string, r rune) bool
	func Count(s, substr string) int
	func EqualFold(s, t string) bool
	func Fields(s string) []string
	func FieldsFunc(s string, f func(rune) bool) []string
	func HasPrefix(s, prefix string) bool
	func HasSuffix(s, suffix string) bool
	func Index(s, substr string) int
	func IndexAny(s, chars string) int
	func IndexByte(s string, c byte) int
	func IndexFunc(s string, f func(rune) bool) int
	func IndexRune(s string, r rune) int
	func Join(elems []string, sep string) string
	func LastIndex(s, substr string) int
	func LastIndexAny(s, chars string) int
	func LastIndexByte(s string, c byte) int
	func LastIndexFunc(s string, f func(rune) bool) int
	func Map(mapping func(rune) rune, s string) string
	func Repeat(s string, count int) string
	func Replace(s, old, new string, n int) string
	func ReplaceAll(s, old, new string) string
	func Split(s, sep string) []string
	func SplitAfter(s, sep string) []string
	func SplitAfterN(s, sep string, n int) []string
	func SplitN(s, sep string, n int) []string
	func Title(s string) string
	func ToLower(s string) string
	func ToLowerSpecial(c unicode.SpecialCase, s string) string
	func ToTitle(s string) string
	func ToTitleSpecial(c unicode.SpecialCase, s string) string
	func ToUpper(s string) string
	func ToUpperSpecial(c unicode.SpecialCase, s string) string
	func ToValidUTF8(s, replacement string) string
	func Trim(s string, cutset string) string
	func TrimFunc(s string, f func(rune) bool) string
	func TrimLeft(s string, cutset string) string
	func TrimLeftFunc(s string, f func(rune) bool) string
	func TrimPrefix(s, prefix string) string
	func TrimRight(s string, cutset string) string
	func TrimRightFunc(s string, f func(rune) bool) string
	func TrimSpace(s string) string
	func TrimSuffix(s, suffix string) string

	type Builder struct {
		func (b *Builder) Cap() int
		func (b *Builder) Grow(n int)
		func (b *Builder) Len() int
		func (b *Builder) Reset()
		func (b *Builder) String() string
		func (b *Builder) Write(p []byte) (int, error)
		func (b *Builder) WriteByte(c byte) error
		func (b *Builder) WriteRune(r rune) (int, error)
		func (b *Builder) WriteString(s string) (int, error)
	}
	type Reader struct  {
		func NewReader(s string) *Reader
		func (r *Reader) Len() int
		func (r *Reader) Read(b []byte) (n int, err error)
		func (r *Reader) ReadAt(b []byte, off int64) (n int, err error)
		func (r *Reader) ReadByte() (byte, error)
		func (r *Reader) ReadRune() (ch rune, size int, err error)
		func (r *Reader) Reset(s string)
		func (r *Reader) Seek(offset int64, whence int) (int64, error)
		func (r *Reader) Size() int64
		func (r *Reader) UnreadByte() error
		func (r *Reader) UnreadRune() error
		func (r *Reader) WriteTo(w io.Writer) (n int64, err error)
	}
}

package sync {
	type Cond struct {
		func NewCond(l Locker) *Cond
		func (c *Cond) Broadcast()
		func (c *Cond) Signal()
		func (c *Cond) Wait()
	}
	type Locker interface {
		Lock()
		Unlock()
	}
	type Map struct {
		func (m *Map) Delete(key interface{})
		func (m *Map) Load(key interface{}) (value interface{}, ok bool)
		func (m *Map) LoadOrStore(key, value interface{}) (actual interface{}, loaded bool)
		func (m *Map) Range(f func(key, value interface{}) bool)
		func (m *Map) Store(key, value interface{})
	}
	type Mutex struct {
		func (m *Mutex) Lock()
		func (m *Mutex) Unlock()
	}
	type Once struct {
		func (o *Once) Do(f func())

	}
	type Pool struct {
		func (p *Pool) Get() interface{}
		func (p *Pool) Put(x interface{})
	}
	type RWMutex struct {
		func (rw *RWMutex) Lock()
		func (rw *RWMutex) RLock()
		func (rw *RWMutex) RLocker() Locker
		func (rw *RWMutex) RUnlock()
		func (rw *RWMutex) Unlock()
	}
	type WaitGroup struct {
		func (wg *WaitGroup) Add(delta int)
		func (wg *WaitGroup) Done()
		func (wg *WaitGroup) Wait()
	}
}

package sync/atomic {
	func AddInt32(addr *int32, delta int32) (new int32)
	func AddInt64(addr *int64, delta int64) (new int64)
	func AddUint32(addr *uint32, delta uint32) (new uint32)
	func AddUint64(addr *uint64, delta uint64) (new uint64)
	func AddUintptr(addr *uintptr, delta uintptr) (new uintptr)
	func CompareAndSwapInt32(addr *int32, old, new int32) (swapped bool)
	func CompareAndSwapInt64(addr *int64, old, new int64) (swapped bool)
	func CompareAndSwapPointer(addr *unsafe.Pointer, old, new unsafe.Pointer) (swapped bool)
	func CompareAndSwapUint32(addr *uint32, old, new uint32) (swapped bool)
	func CompareAndSwapUint64(addr *uint64, old, new uint64) (swapped bool)
	func CompareAndSwapUintptr(addr *uintptr, old, new uintptr) (swapped bool)
	func LoadInt32(addr *int32) (val int32)
	func LoadInt64(addr *int64) (val int64)
	func LoadPointer(addr *unsafe.Pointer) (val unsafe.Pointer)
	func LoadUint32(addr *uint32) (val uint32)
	func LoadUint64(addr *uint64) (val uint64)
	func LoadUintptr(addr *uintptr) (val uintptr)
	func StoreInt32(addr *int32, val int32)
	func StoreInt64(addr *int64, val int64)
	func StorePointer(addr *unsafe.Pointer, val unsafe.Pointer)
	func StoreUint32(addr *uint32, val uint32)
	func StoreUint64(addr *uint64, val uint64)
	func StoreUintptr(addr *uintptr, val uintptr)
	func SwapInt32(addr *int32, new int32) (old int32)
	func SwapInt64(addr *int64, new int64) (old int64)
	func SwapPointer(addr *unsafe.Pointer, new unsafe.Pointer) (old unsafe.Pointer)
	func SwapUint32(addr *uint32, new uint32) (old uint32)
	func SwapUint64(addr *uint64, new uint64) (old uint64)
	func SwapUintptr(addr *uintptr, new uintptr) (old uintptr)
}

package time {
	const (
		Nanosecond
		Microsecond
		Millisecond
		Second
		Minute
		Hour
	)
	func After(d Duration) <-chan Time
	func Sleep(d Duration)
	func Tick(d Duration) <-chan Time
	type Duration int64 {
		func ParseDuration(s string) (Duration, error)
		func Since(t Time) Duration
		func Until(t Time) Duration
		func (d Duration) Hours() float64
		func (d Duration) Microseconds() int64
		func (d Duration) Milliseconds() int64
		func (d Duration) Minutes() float64
		func (d Duration) Nanoseconds() int64
		func (d Duration) Round(m Duration) Duration
		func (d Duration) Seconds() float64
		func (d Duration) String() string
		func (d Duration) Truncate(m Duration) Duration
	}
	type Location struct {
		func FixedZone(name string, offset int) *Location
		func LoadLocation(name string) (*Location, error)
		func LoadLocationFromTZData(name string, data []byte) (*Location, error)
		func (l *Location) String() string
	}
	type Month int
	type Ticker struct {
		func NewTicker(d Duration) *Ticker
		func (t *Ticker) Stop()
	}
	type Time struct {
		func Date(year int, month Month, day, hour, min, sec, nsec int, loc *Location) Time
		func Now() Time
		func Parse(layout, value string) (Time, error)
		func ParseInLocation(layout, value string, loc *Location) (Time, error)
		func Unix(sec int64, nsec int64) Time
		func (t Time) Add(d Duration) Time
		func (t Time) AddDate(years int, months int, days int) Time
		func (t Time) After(u Time) bool
		func (t Time) AppendFormat(b []byte, layout string) []byte
		func (t Time) Before(u Time) bool
		func (t Time) Clock() (hour, min, sec int)
		func (t Time) Date() (year int, month Month, day int)
		func (t Time) Day() int
		func (t Time) Equal(u Time) bool
		func (t Time) Format(layout string) string
		func (t *Time) GobDecode(data []byte) error
		func (t Time) GobEncode() ([]byte, error)
		func (t Time) Hour() int
		func (t Time) ISOWeek() (year, week int)
		func (t Time) In(loc *Location) Time
		func (t Time) IsZero() bool
		func (t Time) Local() Time
		func (t Time) Location() *Location
		func (t Time) MarshalBinary() ([]byte, error)
		func (t Time) MarshalJSON() ([]byte, error)
		func (t Time) MarshalText() ([]byte, error)
		func (t Time) Minute() int
		func (t Time) Month() Month
		func (t Time) Nanosecond() int
		func (t Time) Round(d Duration) Time
		func (t Time) Second() int
		func (t Time) String() string
		func (t Time) Sub(u Time) Duration
		func (t Time) Truncate(d Duration) Time
		func (t Time) UTC() Time
		func (t Time) Unix() int64
		func (t Time) UnixNano() int64
		func (t *Time) UnmarshalBinary(data []byte) error
		func (t *Time) UnmarshalJSON(data []byte) error
		func (t *Time) UnmarshalText(data []byte) error
		func (t Time) Weekday() Weekday
		func (t Time) Year() int
		func (t Time) YearDay() int
		func (t Time) Zone() (name string, offset int)
	}
	type Timer struct {
		func NewTimer(d Duration) *Timer
		func (t *Timer) Reset(d Duration) bool
		func (t *Timer) Stop() bool
	}
	type Weekday int
	const (
		Sunday
		Monday
		Tuesday
		Wednesday
		Thursday
		Friday
		Saturday
	)
}

package unsafe {
	func Alignof(x ArbitraryType) uintptr
	func Offsetof(x ArbitraryType) uintptr
	func Sizeof(x ArbitraryType) uintptr
	type Pointer *ArbitraryType
}
