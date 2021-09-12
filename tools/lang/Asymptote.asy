// 2.70 https://asymptote.sourceforge.io/
// https://github.com/vectorgraphics/asymptote

//! keywords		===========================================================
// asy-keywords.el, asy.vim
access and as atleast
break
continue controls curl cycle
do default
else explicit
for from
if import include
new newframe
operator
private public
quote
readable restricted return
static struct
tension this typedef
unravel
var
while

//! types			===========================================================
// https://asymptote.sourceforge.io/doc/Data-types.html#Data-types
void bool int real pair triple string
path path3 guide guide3 pen transform transform3 frame file
curlSpecifier tensionSpecifier
arrowbar arrowbar3 suffix

//! structs			===========================================================
// plain.asy
align arrowhead
bool3 bounds
coord coords2 coords3
filltype
Label Legend light
margin marker
object
picture position projection
scaling side  slice
transformation
// three.asy
arrowhead3
control
flatguide3
interaction
margin3 material
patch
render
scene surface
Tension tube

//! constants		===========================================================
// plain_constants.asy
true false null
nullpath
currentpen nullpen invisible defaultpen
nullframe currentpicture currentprojection currentlight
undefined inches inch cm bp pt mm cm
identity zeroTransform
pi infinity

//! functions		===========================================================
// https://asymptote.sourceforge.io/doc/Drawing-commands.html#Drawing-commands
// draw
void draw(picture pic=currentpicture, Label L="", path g, align align=NoAlign, pen p=currentpen, arrowbar arrow=None, arrowbar bar=None, margin margin=NoMargin, Label legend="", marker marker=nomarker);
frame legend(picture pic=currentpicture, int perline=1, real xmargin=legendmargin, real ymargin=xmargin, real linelength=legendlinelength, real hskip=legendhskip, real vskip=legendvskip, real maxwidth=0, real maxheight=0, bool hstretch=false, bool vstretch=false, pen p=currentpen);
void dot(frame f, pair z, pen p=currentpen, filltype filltype=dotfilltype);
void dot(picture pic=currentpicture, pair z, pen p=currentpen, filltype filltype=dotfilltype);
void dot(picture pic=currentpicture, Label L, pair z, align align=NoAlign, string format=defaultformat, pen p=currentpen, filltype filltype=dotfilltype);
void dot(picture pic=currentpicture, Label[] L=new Label[], pair[] z, align align=NoAlign, string format=defaultformat, pen p=currentpen, filltype filltype=dotfilltype);
void dot(picture pic=currentpicture, path[] g, pen p=currentpen, filltype filltype=dotfilltype);
void dot(picture pic=currentpicture, Label L, pen p=currentpen, filltype filltype=dotfilltype);
void dot(picture pic=currentpicture, Label[] L=new Label[], explicit path g, align align=RightSide, string format=defaultformat, pen p=currentpen, filltype filltype=dotfilltype);
void draw(pair origin, picture pic=currentpicture, Label L="", path g, align align=NoAlign, pen p=currentpen, arrowbar arrow=None, arrowbar bar=None, margin margin=NoMargin, Label legend="", marker marker=nomarker);

// fill
void filldraw(picture pic=currentpicture, path g, pen fillpen=currentpen, pen drawpen=currentpen);
void fill(pair origin, picture pic=currentpicture, path g, pen p=currentpen);
void filloutside(picture pic=currentpicture, path g, pen p=currentpen);
void latticeshade(picture pic=currentpicture, path g, bool stroke=false, pen fillrule=currentpen, pen[][] p)
void axialshade(picture pic=currentpicture, path g, bool stroke=false, pen pena, pair a, bool extenda=true, pen penb, pair b, bool extendb=true);
void radialshade(picture pic=currentpicture, path g, bool stroke=false, pen pena, pair a, real ra, bool extenda=true, pen penb, pair b, real rb, bool extendb=true);
void gouraudshade(picture pic=currentpicture, path g, bool stroke=false, pen fillrule=currentpen, pen[] p, pair[] z, int[] edges);
void gouraudshade(picture pic=currentpicture, path g, bool stroke=false, pen fillrule=currentpen, pen[] p, int[] edges);
void tensorshade(picture pic=currentpicture, path[] g, bool stroke=false, pen fillrule=currentpen, pen[][] p, path[] b=g, pair[][] z=new pair[][]);
void tensorshade(picture pic=currentpicture, path g, bool stroke=false, pen fillrule=currentpen, pen[] p, path b=g, pair[] z=new pair[]);
void draw(picture pic=currentpicture, pen fillrule=currentpen, path[] g, pen[] p);
void functionshade(picture pic=currentpicture, path[] g, bool stroke=false, pen fillrule=currentpen, string shader);
void unfill(picture pic=currentpicture, path g);

// clip
void clip(picture pic=currentpicture, path g, stroke=false, pen fillrule=currentpen);

// https://asymptote.sourceforge.io/doc/Data-types.html#Data-types
// pair
pair conj(pair z)
real length(pair z)
real angle(pair z, bool warn=true)
real degrees(pair z, bool warn=true)
pair unit(pair z)
pair expi(real angle)
pair expi(real angle)
real xpart(pair z)
real ypart(pair z)
pair realmult(pair z, pair w)
real dot(explicit pair z, explicit pair w)
real cross(explicit pair z, explicit pair w)
real orient(pair a, pair b, pair c);
real incircle(pair a, pair b, pair c, pair d);
pair minbound(pair z, pair w)
pair maxbound(pair z, pair w)

// triple
real length(triple v)
real polar(triple v, bool warn=true)
real azimuth(triple v, bool warn=true)
real colatitude(triple v, bool warn=true)
real latitude(triple v, bool warn=true)
real longitude(triple v, bool warn=true)
triple unit(triple v)
triple expi(real polar, real azimuth)
triple dir(real colatitude, real longitude)
real xpart(triple v)
real ypart(triple v)
real zpart(triple v)
real dot(triple u, triple v)
triple cross(triple u, triple v)
triple minbound(triple u, triple v)
triple maxbound(triple u, triple v)

// string
int length(string s)
int find(string s, string t, int pos=0)
int rfind(string s, string t, int pos=-1)
string insert(string s, int pos, string t)
string erase(string s, int pos, int n)
string substr(string s, int pos, int n=-1)
string reverse(string s)
string replace(string s, string before, string after)
string replace(string s, string[][] table)
string[] split(string s, string delimiter="")
string[] array(string s)
string format(string s, int n, string locale="")
string format(string s=defaultformat, bool forcemath=false, string s=defaultseparator, real x, string locale="")
int hex(string s);
int ascii(string s);
string string(real x, int digits=realDigits)
string locale(string s="")
string time(string format="%a %b %d %T %Z %Y")
int seconds(string t="", string format="")
string time(int seconds, string format="%a %b %d %T %Z %Y")
int system(string s)
int system(string[] s)
void asy(string format, bool overwrite=false ... string[] s)
void abort(string s="")
void assert(bool b, string s="")
void exit()
void sleep(int seconds)
void usleep(int microseconds)
void beep()

// https://asymptote.sourceforge.io/doc/Paths-and-guides.html#Paths-and-guides
int length(path p);
int size(path p);
bool cyclic(path p);
bool straight(path p, int i);
bool piecewisestraight(path p)
pair point(path p, int t);
pair point(path p, real t);
pair dir(path p, int t, int sign=0, bool normalize=true);
pair dir(path p, real t, bool normalize=true);
pair dir(path p)
pair dir(path p, path q)
pair accel(path p, int t, int sign=0);
pair accel(path p, real t);
real radius(path p, real t);
pair precontrol(path p, int t);
pair precontrol(path p, real t);
pair postcontrol(path p, int t);
pair postcontrol(path p, real t);
real arclength(path p);
real arctime(path p, real L);
pair arcpoint(path p, real L);
real dirtime(path p, pair z);
real reltime(path p, real l);
pair relpoint(path p, real l);
pair midpoint(path p);
path reverse(path p);
path subpath(path p, int a, int b);
path subpath(path p, real a, real b);
real[] intersect(path p, path q, real fuzz=-1);
real[][] intersections(path p, path q, real fuzz=-1);
real[] intersections(path p, explicit pair a, explicit pair b, real fuzz=-1);
real[] times(path p, real x)
real[] times(path p, explicit pair z)
real[] mintimes(path p)
real[] maxtimes(path p)
pair intersectionpoint(path p, path q, real fuzz=-1);
pair[] intersectionpoints(path p, path q, real fuzz=-1);
pair extension(pair P, pair Q, pair p, pair q);
slice cut(path p, path knife, int n);
slice firstcut(path p, path knife);
slice lastcut(path p, path knife);
path buildcycle(... path[] p);
pair min(path p);
pair max(path p);
int windingnumber(path p, pair z);
bool interior(int windingnumber, pen fillrule)
bool inside(path p, pair z, pen fillrule=currentpen);
int inside(path p, path q, pen fillrule=currentpen);
pair inside(path p, pen fillrule=currentpen);
path[] strokepath(path g, pen p=currentpen);

// guide
int size(guide g);
int length(guide g);
bool cyclic(path p);
pair point(guide g, int t);
guide reverse(guide g);
pair[] dirSpecifier(guide g, int i);
pair[] controlSpecifier(guide g, int i);
tensionSpecifier tensionSpecifier(guide g, int i);
real[] curlSpecifier(guide g);

// https://asymptote.sourceforge.io/doc/Pens.html#Pens
pen gray(real g);
pen rgb(real r, real g, real b);
pen cmyk(real c, real m, real y, real k);
defaultpen(pen)
colorspace(pen p)
colorless(pen=currentpen)
pen rgb(string s);
string hex(pen p);
pen linetype(real[] a, real offset=0, bool scale=true, bool adjust=true)
pen linewidth(real)
real linewidth(pen p=currentpen)
int linecap(pen p=currentpen)
int linejoin(pen p=currentpen)
real miterlimit(pen p=currentpen)
int fillrule(pen p=currentpen)
int basealign(pen p=currentpen)
pen fontsize(real size, real lineskip=1.2*size)
real fontsize(pen p=currentpen)
real lineskip(pen p=currentpen)
pen font(string encoding, string family, string series, string shape)
string font(pen p=currentpen)
pen opacity(real opacity=1, string blend="Compatible")
picture hatch(real H=5mm, pair dir=NE, pen p=currentpen)
picture crosshatch(real H=5mm, pen p=currentpen)
pen makepen(path)
path nib(pen)

// https://asymptote.sourceforge.io/doc/Transforms.html#Transforms
transform shift(pair z);
transform shift(real x, real y);
transform xscale(real x);
transform yscale(real y);
transform scale(real s);
transform scale(real x, real y);
transform slant(real s);
transform rotate(real angle, pair z=(0,0));
transform reflect(pair a, pair b);

// https://asymptote.sourceforge.io/doc/Frames-and-pictures.html#Frames-and-pictures
// frame
void add(frame dest, frame src);
void prepend(frame dest, frame src);
frame align(frame f, pair align);
path box(frame f, Label L="", real xmargin=0, real ymargin=xmargin, pen p=currentpen, filltype filltype=NoFill, bool above=true);
path roundbox(frame f, Label L="", real xmargin=0, real ymargin=xmargin, pen p=currentpen, filltype filltype=NoFill, bool above=true);
path ellipse(frame f, Label L="", real xmargin=0, real ymargin=xmargin, pen p=currentpen, filltype filltype=NoFill, bool above=true);

// picture
void size(picture pic=currentpicture, real x, real y=x, bool keepAspect=Aspect);
void unitsize(picture pic=currentpicture, real x, real y=x);
transform fixedscaling(picture pic=currentpicture, pair min, pair max, pen p=nullpen, bool warn=false);
void shipout(string prefix=defaultfilename, picture pic=currentpicture, orientation orientation=orientation, string format="", bool wait=false, bool view=true, string options="", string script="", light light=currentlight, projection P=currentprojection)
fit(real xsize=pic.xsize, real ysize=pic.ysize, bool keepAspect=pic.keepAspect);
scale(real xsize=this.xsize, real ysize=this.ysize, bool keepAspect=this.keepAspect);
frame bbox(picture pic=currentpicture, real xmargin=0, filltype filltype=NoFill);
filldraw(real xmargin=0, real ymargin=xmargin, pen fillpen=nullpen, pen drawpen=nullpen)
fill(real xmargin=0, real ymargin=xmargin, pen p=nullpen)
draw(real xmargin=0, real ymargin=xmargin, pen p=nullpen)
unfill(real xmargin=0, real ymargin=xmargin)
radialshade(pen penc, pen penr)
radialshadedraw(real xmargin=0, real ymargin=xmargin, pen penc, pen penr, pen drawpen=nullpen)
frame pad(picture pic=currentpicture, real xsize=pic.xsize, real ysize=pic.ysize, filltype filltype=NoFill);
pair min(picture pic, user=false);
pair max(picture pic, user=false);
pair size(picture pic, user=false);
pair point(picture pic=currentpicture, pair dir, bool user=true);
pair truepoint(picture pic=currentpicture, pair dir, bool user=true);
void add(picture src, bool group=true, filltype filltype=NoFill, bool above=true);
void add(picture dest, picture src, bool group=true, filltype filltype=NoFill, bool above=true);
void add(picture src, pair position, bool group=true, filltype filltype=NoFill, bool above=true);
void add(picture dest, picture src, pair position, bool group=true, filltype filltype=NoFill, bool above=true);
void add(picture dest=currentpicture, frame src, pair position=0, bool group=true, filltype filltype=NoFill, bool above=true);
void add(picture dest=currentpicture, frame src, pair position, pair align, bool group=true, filltype filltype=NoFill, bool above=true);
void attach(picture dest=currentpicture, frame src, pair position=0, bool group=true, filltype filltype=NoFill, bool above=true);
void attach(picture dest=currentpicture, frame src, pair position, pair align, bool group=true, filltype filltype=NoFill, bool above=true);
void erase(picture pic=currentpicture);
void postscript(picture pic=currentpicture, string s);
void postscript(picture pic=currentpicture, string s, pair min, pair max)
void tex(picture pic=currentpicture, string s);
void tex(picture pic=currentpicture, string s, pair min, pair max)
void texpreamble(string s);
void texreset();
void usepackage(string s, string options="");

// https://asymptote.sourceforge.io/doc/Files.html#Files
input(string name="", bool check=true, string comment="#", string mode="");
file output(string name="", bool update=false, string comment="#", string mode="");
write(string s="", T x, suffix suffix=endl ... T[]);
write(file file, string s="", T x, suffix suffix=none ... T[]);
write(file file=stdout, string s="", explicit T[] x ... T[][]);
write(file file=stdout, T[][]);
write(file file=stdout, T[][][]);
write(suffix suffix=endl);
write(file file, suffix suffix=none);
eof(file)
eol(file)
error(file)
flush(file)
clear(file)
close(file)
int precision(file file=stdout, int digits=0)
int tell(file)
seek(file file, int pos)
seekeof(file)
string getstring(string name="", string default="", string prompt="", bool store=true);
int getint(string name="", int default=0, string prompt="", bool store=true);
real getreal(string name="", real default=0, string prompt="", bool store=true);
pair getpair(string name="", pair default=0, string prompt="", bool store=true);
triple gettriple(string name="", triple default=(0,0,0), string prompt="", bool store=true);
string readline(string prompt="", string name="", bool tabcompletion=false);
void saveline(string name, string value, bool store=true);
int delete(string s)
int rename(string from, string to)
int convert(string args="", string file="", string format="");
int animate(string args="", string file="", string format="");

// https://asymptote.sourceforge.io/doc/Arrays.html#Arrays
T push(T x),
void append(T[] a),
T pop(),
void insert(int i ... T[] x),
void delete(int i, int j=i),
void delete(), and
bool initialized(int n).

T[] array(int n, T value, int depth=intMax)
int[] sequence(int n)
T[] map(T f(T), T[] a)
int[] reverse(int n)
int[] complement(int[] a, int n)
real[] uniform(real a, real b, int n)
int find(bool[] a, int n=1)
int[] findall(bool[] a)
int search(T[] a, T key)
T[] copy(T[] a)
T[] concat(... T[][] a)
bool alias(T[] a, T[] b)
T[] sort(T[] a)
T[][] transpose(T[][] a)
T sum(T[] a)
T min(T[] a)
T max(T[] a)
pair[] pairs(real[] x, real[] y);
pair[] fft(pair[] a, int sign=1)
real dot(real[] a, real[] b)
real[] tridiagonal(real[] a, real[] b, real[] c, real[] f);
real[] solve(real[][] a, real[] b, bool warn=true)
real[][] identity(int n);
real[][] diagonal(... real[] a)
real[][] inverse(real[][] a)
real[] quadraticroots(real a, real b, real c);
real[] cubicroots(real a, real b, real c, real d);
