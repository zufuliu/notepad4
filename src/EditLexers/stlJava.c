#include "EditLexer.h"
#include "EditStyle.h"

// https://docs.oracle.com/javase/specs/

static KEYWORDLIST Keywords_Java = {{
"@interface abstract assert break case catch class const continue default do else "
"enum extends final finally for goto if implements import instanceof interface "
"native new package private protected public return static strictfp super switch "
"synchronized this throw throws transient try volatile while "
"false true null parcelable "
, // type keyword
"boolean byte char double float int long short void "
, // preprocessor
""
, // annotation
"interface Deprecated Override SuppressWarnings Documented Retention Target Inherited SafeVarargs "
"Native Repeatable FunctionalInterface "
// javax.annotation.*
"Generated PostConstruct PreDestroy "
// JSR 305 javax.annotation.*
"Nonnull "
// Java Persistence javax.persistence.*
"Entity Table Id Column Basic OrderBy Version Transient "
, // attribute
""
, // class
"Void Class Enum Package Object Boolean Number Byte Short Integer Long Float Double Character String "
"BigDecimal BigInteger "
// java.lang
"ClassLoader Error Exception Process ProcessBuilder SecurityManager StringBuffer StringBuilder Thread ThreadGroup ThreadLocal Throwable "
"Math System "
"ClassNotFoundException IllegalArgumentException IndexOutOfBoundsException NullPointerException RuntimeException InterruptedException "
// java.lang.reflect
"Array Constructor Field Method Modifier "
// java.lang.ref
"Finalizer Reference ReferenceQueue WeakReference SoftReference PhantomReference "
// java.util
"Collections AttributeSet Arrays ArrayDeque ArrayList BitSet EnumMap EnumSet HashMap HashSet Hashtable LinkedHashMap LinkedHashSet LinkedList PriorityQueue Stack TreeMap TreeSet WeakHashMap Vector " "Dictionary Properties "
"Calendar Currency Date EventObject Formatter Locale Observable Random Scanner ServiceLoader StringTokenizer Timer TimerTask TimeZone UUID "
// java.util.zip
"GZIPInputStream GZIPOutputStream ZipInputStream ZipOutputStream "
// java.io
"File IOError IOException FileNotFoundException InputStream OutputStream FilterInputStream FilterOutputStream ObjectInputStream ObjectOutputStream FileInputStream FileOutputStream DataInputStream DataOutputStream ByteArrayInputStream ByteArrayOutputStream "
"BufferedInputStream BufferedOutputStream BufferedReader BufferedWriter "
// java.nio
" Buffer ByteOrder Charset ByteBuffer CharBuffer ShortBuffer IntBuffer LongBuffer FloatBuffer DoubleBuffer "
// java.net
"InetAddress Proxy Socket URI URL "
// java.text
"SimpleDateFormat "

// android.os
"AsyncTask AsyncResult Build Bundle Environment Handler Looper Message Binder Parcel "
// android.app
"ActionBar Activity AlertDialog Application Dialog PendingIntent Service "
// android.content
"BroadcastReceiver Context Intent IntentFilter "
// android.view
"KeyEvent MotionEvent View ViewGroup ImageView Menu MenuItem GridView "
// android.widget
"AdapterView Button CheckBox EditText ImageButton RadioButton RadioGroup Spinner TextView Toast SpinnerAdapter TabHost TabWidget "
"RelativeLayout ArrayAdapter "
// android.graphics
"Bitmap Canvas Drawable Rect RectF Region Paint Matrix "
// android.media
"AudioFormat AudioManager AudioRecord AudioTrack "

, // interface
// java.lang
"Annotation Appendable CharSequence Cloneable Comparable Iterable Readable Runnable "
// java.util
"Collection Comparator Deque Enumeration EventListener Formattable Iterator List ListIterator Map NavigableMap NavigableSet Observer Queue RandomAccess Set SortedMap SortedSet "
// java.io
"Closeable DataInput DataOutput Flushable ObjectInput ObjectOutput Serializable "



// android.os
"IBinder Parcelable "
// android.view
"OnClickListener "
// android.widget
"Adapter "
// android.graphics
" "
, // enumeration
""
/*
"ElementType RetentionPolicy "
*/
, // constant
""
/*
"TRUE FALSE MIN_VALUE MAX_VALUE MIN_RADIX MAX_RADIX POSITIVE_INFINITY NEGATIVE_INFINITY NaN MIN_NORMAL MAX_EXPONENT MIN_EXPONENT MAXIMUM_CAPACITY"
*/

#if NUMKEYWORD == 16
,
"java.util "
"android.widget "
, "", "", "", "", "",
"for^() if^() switch^() while^() catch^() else^if^() synchronized^() try^() "
#endif
}};

EDITLEXER lexJava = { SCLEX_CPP, NP2LEX_JAVA, L"Java Source", L"java", L"", &Keywords_Java,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_DIRECTIVE, NP2STYLE_Annotation, L"Annotation", L"fore:#FF8000", L""},
	{ SCE_C_CLASS, NP2STYLE_Class, L"Class", L"fore:#0080FF", L"" },
	{ SCE_C_INTERFACE, NP2STYLE_Interface, L"Interface", L"bold; fore:#1E90FF", L""},
	{ SCE_C_FUNCTION, NP2STYLE_Method, L"Method", L"fore:#A46000", L"" },
	{ SCE_C_ENUMERATION, NP2STYLE_Enumeration, L"Enumeration", L"fore:#FF8000", L""},
	{ SCE_C_CONSTANT, NP2STYLE_Constant, L"Constant", L"bold; fore:#B000B0", L""},
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, L"Doc Comment Tag", L"bold; fore:#008000F", L"" },
	{ SCE_C_COMMENTDOC_TAG_XML, NP2STYLE_DocCommentTagHTML, L"Doc Comment HTML Tag", L"bold; fore:#008000F", L"" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, 0, 0), NP2STYLE_DocComment, L"Doc Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_LABEL, NP2STYLE_Label, L"Label", L"fore:#000000; back:#FFC040", L""},
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
