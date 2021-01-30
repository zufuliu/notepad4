// Java SE 15 https://docs.oracle.com/en/java/javase/index.html
// JDK 16 http://openjdk.java.net/projects/jdk/16/
// https://docs.oracle.com/javase/specs/index.html
// https://github.com/beanshell/beanshell/wiki/Basic-syntax

//! keywords		===========================================================
// https://docs.oracle.com/javase/specs/jls/se15/html/jls-3.html#jls-3.9
abstract assert
break
case catch
class const continue
default do
else enum extends
final finally
for goto
if implements import instanceof interface
native new
package private protected public
return
static strictfp super switch synchronized
this throw throws transient try
volatile
while

// others
false true null

// Blocks and Statements
// https://docs.oracle.com/javase/specs/jls/se15/html/jls-14.html
var yield

// Sealed Classes
// https://docs.oracle.com/javase/specs/jls/se15/preview/specs/sealed-classes-jls.html
sealed non-sealed

// Records
// https://docs.oracle.com/javase/specs/jls/se15/preview/specs/records-jls.html
record

// Android IDL
parcelable

//! types			===========================================================
boolean byte char double float int long short void

//! directive		===========================================================
// Packages and Modules
// https://docs.oracle.com/javase/specs/jls/se15/html/jls-7.html
open module
requires
exports to
opens to
uses
provides with
transitive static

//! API				===========================================================
// https://docs.oracle.com/en/java/javase/15/docs/api/index.html

module java.base {

package java.lang {
	// Interface
	public interface Appendable
	public interface AutoCloseable
	public interface CharSequence
	public interface Cloneable
	public interface Comparable<T>
	public interface Iterable<T>
	public interface Readable
	@FunctionalInterface
	public interface Runnable

	// Class
	public final class Boolean implements Serializable, Comparable<Boolean>, Constable
	public final class Character implements Serializable, Comparable<Character>, Constable
	public final class Class<T> implements Serializable, GenericDeclaration, Type, AnnotatedElement, TypeDescriptor.OfField<Class<?>>, Constable
	public abstract class ClassLoader
	public abstract class Enum<E extends Enum<E>> implements Constable, Comparable<E>, Serializable
	public final class Math
	public final class Module implements AnnotatedElement
	public abstract class Number implements Serializable
		public final class Byte implements Comparable<Byte>, Constable
		public final class Double implements Comparable<Double>, Constable, ConstantDesc
		public final class Float implements Comparable<Float>, Constable, ConstantDesc
		public final class Integer implements Comparable<Integer>, Constable, ConstantDesc
		public final class Long implements Comparable<Long>, Constable, ConstantDesc
		public final class Short implements Comparable<Short>, Constable
	public class Object
	public class Package implements AnnotatedElement
	public abstract class Process
	public final class ProcessBuilder
	public abstract class Record
	public class Runtime
	public final class String implements Serializable, Comparable<String>, CharSequence, Constable, ConstantDesc
	public final class StringBuffer implements Serializable, Comparable<StringBuffer>, CharSequence
	public final class StringBuilder implements Serializable, Comparable<StringBuilder>, CharSequence
	public final class System
	public class SecurityManager
	public class Thread implements Runnable
	public class ThreadGroup implements Thread.UncaughtExceptionHandler
	public class ThreadLocal<T>
	public class Throwable implements Serializable
	public final class Void

	// Exception
	public class Exception extends Throwable
		public class RuntimeException
	// Error
	public class Error extends Throwable

	// Annotation
	@interface // for auto-completion
	@Deprecated
	@FunctionalInterface
	@Override
	@SafeVarargs
	@SuppressWarnings
}

package java.lang.annotation {
	public interface Annotation
	public enum ElementType
	public enum RetentionPolicy

	// Annotation
	@Documented
	@Inherited
	@Native
	@Repeatable
	@Retention
	@Target
}

package java.lang.ref {
	public abstract class Reference<T>
		public class PhantomReference<T>
		public class SoftReference<T>
		public class WeakReference<T>
	public class ReferenceQueue<T>
}

package java.lang.reflect {
	public final class Array
	public final class Constructor<T> extends Executable
	public final class Field extends AccessibleObject implements Member
	public final class Method extends Executable
	public class Modifier
}

package java.io {
	// Interface
	public interface Closeable extends AutoCloseable
	public interface DataInput
	public interface DataOutput
	public interface Flushable
	public interface ObjectInput extends DataInput, AutoCloseable
	public interface ObjectOutput extends DataOutput, AutoCloseable
	public interface Serializable

	// Class
	public class BufferedInputStream extends FilterOutputStream
	public class BufferedOutputStream
	public class BufferedReader extends Reader
	public class BufferedWriter extends Writer
	public class ByteArrayInputStream extends InputStream
	public class ByteArrayOutputStream extends OutputStream
	public class CharArrayReader extends Reader
	public class CharArrayWriter extends Writer
	public final class Console implements Flushable
	public class DataInputStream extends FilterInputStream implements DataInput
	public class DataOutputStream extends FilterOutputStream implements DataOutput
	public class File implements Serializable, Comparable<File>
	public final class FileDescriptor
	public class FileInputStream extends InputStream
	public class FileOutputStream extends OutputStream
	public final class FilePermission extends Permission implements Serializable
	public class FileReader extends InputStreamReader
	public class FileWriter extends OutputStreamWriter
	public class FilterInputStream extends InputStream
	public class FilterOutputStream extends OutputStream
	public abstract class FilterReader extends Reader
	public abstract class FilterWriter extends Writer
	public abstract class InputStream implements Closeable
	public class InputStreamReader extends Reader
	public class ObjectInputStream extends InputStream implements ObjectInput, ObjectStreamConstants
	public class ObjectOutputStream extends OutputStream implements ObjectOutput, ObjectStreamConstants
	public abstract class OutputStream implements Closeable, Flushable
	public class OutputStreamWriter extends Writer
	public class PipedInputStream extends InputStream
	public class PipedOutputStream extends OutputStream
	public class PipedReader extends Reader
	public class PipedWriter extends Writer
	public class PrintStream extends FilterOutputStream implements Appendable, Closeable
	public class PrintWriter extends Writer
	public abstract class Reader implements Readable, Closeable
	public class StringReader extends Reader
	public class StringWriter extends Writer
	public abstract class Writer implements Appendable, Closeable, Flushable

	// Exception
	public class IOException extends Exception
		public class EOFException
	// Error
	public class IOError extends Error

	// Annotation
	@Serial
}

package java.math {
	public class BigDecimal extends Number implements Comparable<BigDecimal>
	public class BigInteger extends Number implements Comparable<BigInteger>
}

package java.net {
	public class DatagramSocket implements Closeable
		public class MulticastSocket
	public final class DatagramPacket
	public final class HttpCookie implements Cloneable
	public abstract class HttpURLConnection extends URLConnection
	public class InetAddress implements Serializable
		public final class Inet4Address
		public final class Inet6Address
	public class ServerSocket implements Closeable
	public class Socket implements Closeable
	public abstract class SocketAddress implements Serializable
		public class InetSocketAddress
	public final class URI implements Comparable<URI>, Serializable
	public final class URL implements Serializable
	public abstract class URLConnection
	public class URLDecoder
	public class URLEncoder
}

package java.nio {
	public abstract class Buffer
		public abstract class ByteBuffer
			public abstract class MappedByteBuffer
		public abstract class CharBuffer implements Comparable<CharBuffer>, Appendable, CharSequence, Readable
		public abstract class DoubleBuffer implements Comparable<DoubleBuffer>
		public abstract class FloatBuffer implements Comparable<FloatBuffer>
		public abstract class IntBuffer implements Comparable<IntBuffer>
		public abstract class LongBuffer implements Comparable<LongBuffer>
		public abstract class ShortBuffer implements Comparable<ShortBuffer>
	public final class ByteOrder
}

package java.nio.channels {
	public interface Channel extends Closeable
	public interface ByteChannel extends ReadableByteChannel, WritableByteChannel
}

package java.nio.charset {
	public abstract class Charset implements Comparable<Charset>
}

package java.nio.file {
	public interface Path extends Comparable<Path>, Iterable<Path>, Watchable
	public abstract class FileSystem implements Closeable
}

package java.text {
	public abstract class Format implements Serializable, Cloneable
		public class MessageFormat
		public abstract class DateFormat
			public class SimpleDateFormat
		public abstract class NumberFormat
			public class ChoiceFormat
			public class DecimalFormat
}

package java.util {
	// Interface
	public interface Collection<E> extends Iterable<E>
		public interface List<E>
		public interface Queue<E>
			public interface Deque<E>
		public interface Set<E>
			public interface SortedSet<E>
				public interface NavigableSet<E>
	@FunctionalInterface
	public interface Comparator<T>
	public interface Enumeration<E>
	public interface EventListener
	public interface Formattable
	public interface Iterator<E>
		public interface ListIterator<E>
	public interface Map<K,​V>
		public interface SortedMap<K,​V>
			public interface NavigableMap<K,​V>
	public interface RandomAccess
	public interface Spliterator<T>

	// Class
	public abstract class AbstractCollection<E> implements Collection<E>
		public abstract class AbstractList<E> implements List<E>
			public class ArrayList<E> implements List<E>, RandomAccess, Cloneable, Serializable
			public abstract class AbstractSequentialList<E>
				public class LinkedList<E> implements List<E>, Deque<E>, Cloneable, Serializable
			public abstract class AbstractQueue<E> implements Queue<E>
				public class PriorityQueue<E> implements Serializable
			public class Vector<E> implements List<E>, RandomAccess, Cloneable, Serializable
				public class Stack<E>
		public abstract class AbstractSet<E> implements Set<E>
			public abstract class EnumSet<E extends Enum<E>> implements Cloneable, Serializable
			public class HashSet<E> implements Set<E>, Cloneable, Serializable
				public class LinkedHashSet<E>
			public class TreeSet<E> implements NavigableSet<E>, Cloneable, Serializable
		public class ArrayDeque<E> implements Deque<E>, Cloneable, Serializable
	public abstract class AbstractMap<K,​V> implements Map<K,​V>
		public class EnumMap<K extends Enum<K>,​V> implements Serializable, Cloneable
		public class HashMap<K,​V> implements Map<K,​V>, Cloneable, Serializable
			public class LinkedHashMap<K,​V>
		public class TreeMap<K,​V> implements NavigableMap<K,​V>, Cloneable, Serializable
		public class WeakHashMap<K,​V>
	public class Arrays
	public class Base64
	public class BitSet implements Cloneable, Serializable
	public abstract class Calendar implements Serializable, Cloneable, Comparable<Calendar>
	public class Collections
	public final class Currency implements Serializable
	public class Date implements Serializable, Cloneable, Comparable<Date>
	public abstract class Dictionary<K,​V>
		public class Hashtable<K,​V> implements Map<K,​V>, Cloneable, Serializable
			public class Properties
	public class EventObject implements Serializable
	public final class Formatter implements Closeable, Flushable
	public class GregorianCalendar extends Calendar
	public final class Locale implements Cloneable, Serializable
	public final class Optional<T>
	public class Random implements Serializable
	public final class Scanner implements Iterator<String>, Closeable
	public final class ServiceLoader<S> implements Iterable<S>
	public final class StringJoiner
	public class StringTokenizer implements Enumeration<Object>
	public class Timer
	public abstract class TimerTask implements Runnable
	public abstract class TimeZone implements Serializable, Cloneable
	public final class UUID implements Serializable, Comparable<UUID>
}

package java.util.concurrent {
	public interface BlockingQueue<E> extends Queue<E>
		public interface BlockingDeque<E> extends Deque<E>
		public class ArrayBlockingQueue<E> extends AbstractQueue<E> implements Serializable
	@FunctionalInterface
	public interface Callable<V>
	public interface ConcurrentMap<K,​V> extends Map<K,​V>
		public class ConcurrentHashMap<K,​V> extends AbstractMap<K,​V> implements Serializable
	public interface Executor
	public interface Future<V>
	public class ConcurrentLinkedDeque<E> extends AbstractCollection<E> implements Deque<E>, Serializable
	public class ConcurrentLinkedQueue<E> extends AbstractQueue<E> implements Queue<E>, Serializable
	public class CountDownLatch
	public class Semaphore implements Serializable
	public class ThreadPoolExecutor extends AbstractExecutorService
	public enum TimeUnit extends Enum<TimeUnit>
}

package java.util.concurrent.atomic {
	public class AtomicBoolean implements Serializable
	public class AtomicInteger extends Number implements Serializable
	public class AtomicLong extends Number implements Serializable
	public class AtomicReference<V> implements Serializable
}

package java.util.concurrent.locks {
	public interface Condition
	public interface Lock
	public interface ReadWriteLock
}

package java.util.function {
	@FunctionalInterface
	public interface Function<T,​R>
	@FunctionalInterface
	public interface Predicate<T>
}

package java.util.jar {
	public class JarEntry extends ZipEntry
	public class JarFile extends ZipFile
	public class JarInputStream extends ZipInputStream
	public class JarOutputStream extends ZipOutputStream
	public class JarException extends ZipException
}

package java.util.regex {
	public interface MatchResult
	public final class Matcher implements MatchResult
	public final class Pattern implements Serializable
}

package java.util.stream {
	public interface BaseStream<T,​S extends BaseStream<T,​S>> extends AutoCloseable
		public interface Stream<T>
		public interface DoubleStream
		public interface IntStream
		public interface LongStream
	public interface Collector<T,​A,​R>
	public final class Collectors
}

package java.util.zip {
	public class DeflaterOutputStream extends FilterOutputStream
	public class InflaterInputStream extends FilterInputStream
	public class GZIPInputStream extends InflaterInputStream
	public class GZIPOutputStream extends DeflaterOutputStream
	public class ZipInputStream extends InflaterInputStream
	public class ZipEntry implements Cloneable
	public class ZipFile implements Closeable
	public class ZipOutputStream extends DeflaterOutputStream
	public class ZipException extends IOException
}

}

module java.net.http {

package java.net.http {
	public interface HttpResponse<T>
	public interface WebSocket
	public abstract class HttpClient
	public abstract class HttpRequest
}

}

// https://docs.oracle.com/javaee/7/
package javax.annotation {
	@Generated
	@ManagedBean
	@PostConstruct
	@PreDestroy
	@Priority
	@Resource
	@Resources
}

package javax.persistence {
	@Basic
	@Column
	@Entity
	@Id
	@OrderBy
	@OrderColumn
	@Table
	@Transient
	@Version
}

// JSR 308 https://jcp.org/en/jsr/detail?id=308
@Readonly
@NonNull
@NonEmpty

// https://developer.android.com/reference/
module android {

package android.os {
	public interface IBinder
	public interface IInterface
	public interface Parcelable

	public class Binder implements IBinder
	public class Build
	public final class Bundle extends BaseBundle implements Cloneable, Parcelable
	public class Environment
	public class Handler
	public final class Looper
	public final class Message implements Parcelable
	public final class Parcel
	public class RemoteException extends AndroidException
}

package android.app {
	public abstract class ActionBar
	public class Activity extends ContextThemeWrapper implements LayoutInflater.Factory2, Window.Callback, KeyEvent.Callback, View.OnCreateContextMenuListener, ComponentCallbacks2
	public class Application extends ContextWrapper implements ComponentCallbacks2
	public class Dialog implements DialogInterface, Window.Callback, KeyEvent.Callback, View.OnCreateContextMenuListener
		public class AlertDialog implements DialogInterface
	public class Notification implements Parcelable
	public final class PendingIntent implements Parcelable
	public abstract class Service extends ContextWrapper implements ComponentCallbacks2
}

package android.content {
	public abstract class Context
		public class ContextWrapper
	public class Intent implements Parcelable, Cloneable
	public class IntentFilter implements Parcelable
}

package android.graphics {
	public final class Bitmap implements Parcelable
	public class Canvas
	public class Color
	public class Matrix
	public class Paint
	public class Point implements Parcelable
	public class PointF implements Parcelable
	public final class Rect implements Parcelable
	public final class RectF implements Parcelable
	public final class Region implements Parcelable
}

package android.graphics.drawable {
	public abstract class Drawable
}

package android.media {
	public final class AudioFormat implements Parcelable
	public class AudioManager
	public class AudioRecord implements AudioRouting, MicrophoneDirection, AudioRecordingMonitor
	public class AudioTrack implements AudioRouting, VolumeAutomation
	public abstract class Image implements AutoCloseable
}

package android.view {
	public interface Menu
	public interface MenuItem

	public final class Display
	public final class MotionEvent extends InputEvent implements Parcelable
	public abstract class LayoutInflater
	public class KeyEvent extends InputEvent implements Parcelable
	public class View implements Drawable.Callback, KeyEvent.Callback, AccessibilityEventSource {
		public static interface OnClickListener
	}
	public abstract class ViewGroup extends View implements ViewParent, ViewManager
	public abstract class Window
}

package android.widget {
	public interface Adapter
		public interface ListAdapter
	public abstract class AdapterView extends ViewGroup
	public class ArrayAdapter extends BaseAdapter implements Filterable, ThemedSpinnerAdapter
	public abstract class BaseAdapter extends Object implements ListAdapter, SpinnerAdapter
	public class Button extends TextView
	public class CheckBox extends CompoundButton
	public class EditText extends TextView
	public class GridView extends AbsListView
	public class ImageButton extends ImageView
	public class ImageView extends View
	public class LinearLayout extends ViewGroup
	public class ListView extends AbsListView
	public class ProgressBar extends View
	public class RadioButton extends CompoundButton
	public class RadioGroup extends LinearLayout
	public class RelativeLayout extends ViewGroup
	public class ScrollView extends FrameLayout
	public class SearchView extends LinearLayout implements CollapsibleActionView
	public class SeekBar extends AbsSeekBar
	public class Spinner extends AbsSpinner implements DialogInterface.OnClickListener
	public class StackView extends AdapterViewAnimator
	public class TableLayout extends LinearLayout
	public class TextView extends View implements ViewTreeObserver.OnPreDrawListener
	public class Toast
	public class ToggleButton extends CompoundButton
}

}

//! Javadoc			===========================================================
// https://docs.oracle.com/javase/8/docs/technotes/tools/windows/javadoc.html#CHDJGIJB
/**
 * @author name-text
 * {@code text}
 * @deprecated deprecated-text
 * {@docRoot}
 * @exception class-name description
 * {@inheritDoc}
 * {@link package.class#member label}
 * {@linkplain package.class#member label}
 * {@literal text}
 * @param parameter-name description
 * @return description
 * @see reference
 * @serial field-description | include | exclude
 * @serialData data-description
 * @serialField field-name field-type field-description
 * @since since-text
 * @throws class-name description
 * {@value package.class#field}
 * @version version-text
 *
 * used by Android
 * @hide
 */
