// C# 10.0 https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/
// https://wiki.gnome.org/Projects/Vala
// https://wiki.gnome.org/Projects/Vala/Manual

//! keywords		===========================================================
// https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/
abstract as
base break
case catch checked class const continue
default delegate do
else enum event explicit extern
false finally fixed for foreach
goto
if implicit in interface internal is
lock
namespace new null
operator out override
params private protected public
readonly ref return
sealed sizeof stackalloc static struct switch
this throw true try typeof
unchecked unsafe using
virtual volatile
while

// Contextual keywords
add and alias ascending async await
by descending
equals
from
get global group
init into
join
let
nameof notnull
on or orderby
partial
record remove
select set
unmanaged managed
value var
when where with
yield

//! types			===========================================================
// https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/builtin-types/built-in-types
bool byte char decimal double dynamic float int long
object sbyte short string uint ulong ushort void
nint nuint

//! Vala types		===========================================================
// https://wiki.gnome.org/Projects/Vala/Manual/Types
uchar size_t ssize_t unichar
int8 uint8 int16 uint16 int32 uint32 int64 uint64

//! preprocessor	===========================================================
// https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/preprocessor-directives
#nullable disable
#nullable enable
#nullable restore
#if DEBUG
#elif DEBUG
#else
#endif
#define DEBUG
#undef DEBUG
#region region
#endregion
#error error
#warning warning
#line 200 "path"
#line default
#line hidden
#pragma warning disable warning-list
#pragma warning restore warning-list
#pragma checksum "filename" "{guid}" "checksum bytes"
// C# Script
#r "path"
#load "path"

//! attributes		===========================================================
// https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/attributes/general
[assembly:]
[module:]
[field:]
[event:]
[method:]
[param:]
[property:]
[return:]
[type:]

//!api				===========================================================
// .NET 5 https://docs.microsoft.com/en-us/dotnet/api/
namespace System {
	// Classes
	public static class Activator
	public abstract class Array : ICloneable, IList, IStructuralComparable, IStructuralEquatable
	public abstract class Attribute
	[AttributeUsage()]
	public static class BitConverter
	public static class Buffer
	public static class Console
	[CLSCompliant(false)]
	[ContextStatic]
	public static class Convert
	public sealed class DBNull : IConvertible, ISerializable
	public abstract class Delegate : ICloneable, ISerializable
	public abstract class Enum : ValueType, IComparable, IConvertible, IFormattable
	public static class Environment
	public class EventArgs
	public class Exception : ISerializable
	[Flags]
	public class Lazy<T>
	public abstract class MarshalByRefObject
	public static class Math
	[MTAThread]
	[NonSerialized()]
	public class Object
	[Obsolete()]
	[ParamArray]
	public class Random
	[Serializable()]
	[STAThread]
	public sealed class String
	public abstract class StringComparer : IComparer<string>, IEqualityComparer<string>, IComparer, IEqualityComparer
	[ThreadStatic]
	public sealed class TimeZoneInfo : IEquatable<TimeZoneInfo>, IDeserializationCallback, ISerializable
	public class Tuple<T1,T2> : IComparable, IStructuralComparable, IStructuralEquatable, ITuple
	public abstract class Type : MemberInfo, IReflect
	public class Uri : ISerializable
	public class UriBuilder
	public abstract class UriParser
	public abstract class ValueType
	public sealed class Version : ICloneable, IComparable, IComparable<Version>, IEquatable<Version>
	public sealed class WeakReference<T> : ISerializable where T : class
	// Structs
	public struct ArgIterator
	public struct ArraySegment<T> : ICollection<T>, IEnumerable<T>, IList<T>, IReadOnlyCollection<T>, IReadOnlyList<T>
	public struct Boolean : IComparable, IComparable<bool>, IConvertible, IEquatable<bool>
	public struct Byte : IComparable, IComparable<byte>, IConvertible, IEquatable<byte>, IFormattable
	public struct Char : IComparable, IComparable<char>, IConvertible, IEquatable<char>
	public struct DateTime : IComparable, IComparable<DateTime>, IConvertible, IEquatable<DateTime>, IFormattable, ISerializable
	public struct DateTimeOffset : IComparable, IComparable<DateTimeOffset>, IEquatable<DateTimeOffset>, IFormattable, IDeserializationCallback, ISerializable
	public struct Decimal : IComparable, IComparable<decimal>, IConvertible, IEquatable<decimal>, IFormattable, IDeserializationCallback, ISerializable
	public struct Double : IComparable, IComparable<double>, IConvertible, IEquatable<double>, IFormattable
	public struct Guid : IComparable, IComparable<Guid>, IEquatable<Guid>, IFormattable
	public struct Index : IEquatable<Index>
	public struct Int16 : IComparable, IComparable<short>, IConvertible, IEquatable<short>, IFormattable
	public struct Int32 : IComparable, IComparable<int>, IConvertible, IEquatable<int>, IFormattable
	public struct Int64 : IComparable, IComparable<long>, IConvertible, IEquatable<long>, IFormattable
	public struct IntPtr : IComparable, IComparable<IntPtr>, IEquatable<IntPtr>, IFormattable, ISerializable
	public struct Memory<T> : IEquatable<Memory<T>>
	public struct ModuleHandle
	public struct Nullable<T> where T : struct
	public struct Range : IEquatable<Range>
	public struct SByte : IComparable, IComparable<sbyte>, IConvertible, IEquatable<sbyte>, IFormattable
	public struct Single : IComparable, IComparable<float>, IConvertible, IEquatable<float>, IFormattable
	public struct Span<T>
	public struct TimeSpan : IComparable, IComparable<TimeSpan>, IEquatable<TimeSpan>, IFormattable
	public struct TypedReference
	public struct UInt16 : IComparable, IComparable<ushort>, IConvertible, IEquatable<ushort>, IFormattable
	public struct UInt32 : IComparable, IComparable<uint>, IConvertible, IEquatable<uint>, IFormattable
	public struct UInt64 : IComparable, IComparable<ulong>, IConvertible, IEquatable<ulong>, IFormattable
	public struct UIntPtr : IComparable, IComparable<UIntPtr>, IEquatable<UIntPtr>, IFormattable, ISerializable
	public struct ValueTuple<T1,T2> : IComparable, IComparable<(T1,T2)>, IEquatable<(T1,T2)>, IStructuralComparable, IStructuralEquatable, ITuple
	public struct Void
	// Interfaces
	public interface IAsyncDisposable
	public interface IAsyncResult
	public interface ICloneable
	public interface IComparable<in T>
	public interface IConvertible
	public interface ICustomFormatter
	public interface IDisposable
	public interface IEquatable<T>
	public interface IFormatProvider
	public interface IFormattable
	public interface IObservable<out T>
	public interface IObserver<in T>
	public interface IServiceProvider
	// Enums
	public enum AttributeTargets
	public enum ConsoleColor
	public enum ConsoleKey
	public enum DateTimeKind
	public enum DayOfWeek
	public enum StringComparison
	public enum StringSplitOptions
	public enum TypeCode
	// Delegates
	public delegate void Action<in T>(T obj);
	public delegate TResult Func<out TResult>();
	public delegate void EventHandler<TEventArgs>(object? sender, TEventArgs e);
}

namespace System.Buffers.Text {
	public static class Base64
}

namespace System.ComponentModel {
	// Classes
	[Browsable(true)]
	public class Component : MarshalByRefObject, IDisposable, IComponent
	// Interfaces
	public interface IComponent : IDisposable
}

namespace System.Collections {
	// Classes
	public class ArrayList : ICloneable, IList
	public sealed class BitArray : ICloneable, ICollection
	public class Hashtable : ICloneable, IDictionary, IDeserializationCallback, ISerializable
}

namespace System.Collections.Concurrent  {
	public class BlockingCollection<T> : IDisposable, IEnumerable<T>, IReadOnlyCollection<T>, ICollection
	public class ConcurrentBag<T> : IProducerConsumerCollection<T>, IEnumerable<T>, IReadOnlyCollection<T>, ICollection
	public class ConcurrentDictionary<TKey,TValue> : ICollection<KeyValuePair<TKey,TValue>>, IDictionary<TKey,TValue>, IEnumerable<KeyValuePair<TKey,TValue>>, IReadOnlyCollection<KeyValuePair<TKey,TValue>>, IReadOnlyDictionary<TKey,TValue>, IDictionary
	public class ConcurrentQueue<T> : IProducerConsumerCollection<T>, IEnumerable<T>, IReadOnlyCollection<T>, ICollection
	public class ConcurrentStack<T> : IProducerConsumerCollection<T>, IEnumerable<T>, IReadOnlyCollection<T>, ICollection
}

namespace System.Collections.Generic {
	// Classes
	public abstract class Comparer<T> : IComparer<T>, IComparer
	public class Dictionary<TKey,TValue> : ICollection<KeyValuePair<TKey,TValue>>, IDictionary<TKey,TValue>, IEnumerable<KeyValuePair<TKey,TValue>>, IReadOnlyCollection<KeyValuePair<TKey,TValue>>, IReadOnlyDictionary<TKey,TValue>, IDictionary, IDeserializationCallback, ISerializable
	public class HashSet<T> : ICollection<T>, IEnumerable<T>, IReadOnlyCollection<T>, IReadOnlySet<T>, ISet<T>, IDeserializationCallback, ISerializable
	public static class KeyValuePair
	public class LinkedList<T> : ICollection<T>, IEnumerable<T>, IReadOnlyCollection<T>, ICollection, IDeserializationCallback, ISerializable
	public sealed class LinkedListNode<T>
	public class List<T> : ICollection<T>, IEnumerable<T>, IList<T>, IReadOnlyCollection<T>, IReadOnlyList<T>, IList
	public class Queue<T> : IEnumerable<T>, IReadOnlyCollection<T>, ICollection
	public class SortedDictionary<TKey,TValue> : ICollection<KeyValuePair<TKey,TValue>>, IDictionary<TKey,TValue>, IEnumerable<KeyValuePair<TKey,TValue>>, IReadOnlyCollection<KeyValuePair<TKey,TValue>>, IReadOnlyDictionary<TKey,TValue>, IDictionary
	public class SortedList<TKey,TValue> : ICollection<KeyValuePair<TKey,TValue>>, IDictionary<TKey,TValue>, IEnumerable<KeyValuePair<TKey,TValue>>, IReadOnlyCollection<KeyValuePair<TKey,TValue>>, IReadOnlyDictionary<TKey,TValue>, IDictionary
	public class SortedSet<T> : ICollection<T>, IEnumerable<T>, IReadOnlyCollection<T>, IReadOnlySet<T>, ISet<T>, ICollection, IDeserializationCallback, ISerializable
	public class Stack<T> : IEnumerable<T>, IReadOnlyCollection<T>, ICollection
	// Interfaces
	public interface ICollection<T> : IEnumerable<T>
	public interface IComparer<in T>
	public interface IDictionary<TKey,TValue> : ICollection<KeyValuePair<TKey,TValue>>, IEnumerable<KeyValuePair<TKey,TValue>>
	public interface IEnumerable<out T> : IEnumerable
	public interface IEnumerator<out T> : IDisposable, IEnumerator
	public interface IList<T> : ICollection<T>, IEnumerable<T>
	public interface ISet<T> : ICollection<T>, IEnumerable<T>
}

namespace System.Collections.ObjectModel {
	// Classes
	public class Collection<T> : ICollection<T>, IEnumerable<T>, IList<T>, IReadOnlyCollection<T>, IReadOnlyList<T>, IList
	public abstract class KeyedCollection<TKey,TItem> : Collection<TItem>
	public class ObservableCollection<T> : Collection<T>, INotifyCollectionChanged,INotifyPropertyChanged
}

namespace System.Collections.Specialized {
	public class OrderedDictionary : IDictionary, IOrderedDictionary, IDeserializationCallback, ISerializable
}

namespace System.Data {
	// Classes
	public class DataColumn : MarshalByValueComponent
	public class DataRow
	public class DataSet : MarshalByValueComponent, IListSource, ISupportInitialize, ISupportInitializeNotification, ISerializable, IXmlSerializable
	public class DataTable : MarshalByValueComponent, IListSource, ISupportInitialize, ISupportInitializeNotification, ISerializable, IXmlSerializable
	public class DataView : MarshalByValueComponent, IList, IBindingListView, ISupportInitialize, ISupportInitializeNotification, ITypedList
}

namespace System.Diagnostics {
	public class Activity : IDisposable
	[Conditional()]
	[Debuggable]
	[DebuggerHidden]
	public class Process : Component, IDisposable
	public sealed class ProcessStartInfo
	public class Stopwatch
}

namespace System.Drawing {
	// Classes
	public sealed class Bitmap : Image
	public abstract class Brush : MarshalByRefObject, ICloneable, IDisposable
	public static class Brushes
	public sealed class BufferedGraphics : IDisposable
	public sealed class Font : MarshalByRefObject, ICloneable, IDisposable, ISerializable
	public sealed class FontFamily : MarshalByRefObject, IDisposable
	public sealed class Graphics : MarshalByRefObject, IDisposable, IDeviceContext
	public sealed class Icon : MarshalByRefObject, ICloneable, IDisposable, ISerializable
	public abstract class Image : MarshalByRefObject, ICloneable, IDisposable, ISerializable
	public sealed class ImageAnimator
	public sealed class Pen : MarshalByRefObject, ICloneable, IDisposable
	public static class Pens
	public sealed class Region : MarshalByRefObject, IDisposable
	public sealed class SolidBrush : Brush
	public sealed class StringFormat : MarshalByRefObject, ICloneable, IDisposable
	public static class SystemBrushes
	public static class SystemColors
	public static class SystemFonts
	public static class SystemIcons
	public static class SystemPens
	public sealed class TextureBrush : Brush
	// Structs
	public struct CharacterRange
	public struct Color : IEquatable<Color>
	public struct Point : IEquatable<Point>
	public struct PointF : IEquatable<PointF>
	public struct Rectangle : IEquatable<Rectangle>
	public struct RectangleF : IEquatable<RectangleF>
	public struct Size : IEquatable<Size>
	public struct SizeF : IEquatable<SizeF>
	// Enums
	public enum FontStyle
	public enum GraphicsUnit
	public enum KnownColor
}

namespace System.Drawing.Drawing2D {
	// Classes
	public sealed class Blend
	public sealed class ColorBlend
	public sealed class GraphicsPath : MarshalByRefObject, ICloneable, IDisposable
	public sealed class LinearGradientBrush : Brush
	public sealed class Matrix : MarshalByRefObject, IDisposable
}

namespace System.IO {
	// Classes
	public class BinaryReader : IDisposable
	public class BinaryWriter : IAsyncDisposable, IDisposable
	public sealed class BufferedStream : Stream
	public static class Directory
	public sealed class DirectoryInfo : FileSystemInfo
	public sealed class DriveInfo : ISerializable
	public static class File
	public sealed class FileInfo : FileSystemInfo
	public class FileStream : Stream
	public abstract class FileSystemInfo : MarshalByRefObject, ISerializable
	public class FileSystemWatcher : Component, ISupportInitialize
	public class IOException : SystemException
	public class MemoryStream : Stream
	public static class Path
	public abstract class Stream : MarshalByRefObject, IAsyncDisposable, IDisposable
	public class StreamReader : TextReader
	public class StreamWriter : TextWriter
	public class StringReader : TextReader
	public class StringWriter : TextWriter
	public abstract class TextReader : MarshalByRefObject, IDisposable
	public abstract class TextWriter : MarshalByRefObject, IAsyncDisposable, IDisposable
	// Enums
	public enum DriveType
	public enum FileAccess
	public enum FileAttributes
	public enum FileMode
	public enum FileOptions
	public enum FileShare
	public enum SearchOption
	public enum SeekOrigin
}

namespace System.Net {
	// Classes
	public abstract class EndPoint
	public class FileWebRequest : WebRequest, ISerializable
	public class FileWebResponse : WebResponse, ISerializable
	public sealed class HttpListener : IDisposable
	public sealed class HttpListenerRequest
	public sealed class HttpListenerResponse : IDisposable
	public class HttpWebRequest : WebRequest, ISerializable
	public class HttpWebResponse : WebResponse, ISerializable
	public class IPAddress
	public class IPEndPoint : EndPoint
	public class SocketAddress
	public class WebClient : Component
	public class WebException : InvalidOperationException
	public abstract class WebRequest : MarshalByRefObject, ISerializable
	public abstract class WebResponse : MarshalByRefObject, IDisposable, ISerializable
	// Enums
	public enum HttpStatusCode
	public enum HttpRequestHeader
	public enum HttpResponseHeader
}

namespace System.Net.Http {
	// Classes
	public class HttpClient : HttpMessageInvoker
	public class HttpMethod : IEquatable<HttpMethod>
}

namespace System.Net.Sockets {
	// Classes
	public class NetworkStream : Stream
	public class Socket : IDisposable
	public class SocketException : Win32Exception
	public class TcpClient : IDisposable
	public class TcpListener
	public class UdpClient : IDisposable
	// Enums
	public enum SocketError
	public enum SocketFlags
	public enum SocketType
}

namespace System.Numerics {
	// Structs
	public struct BigInteger : IComparable, IComparable<BigInteger>, IEquatable<BigInteger>, IFormattable
	public struct Complex : IEquatable<Complex>, IFormattable
}

namespace System.Reflection {
	public abstract class Assembly : ICustomAttributeProvider, ISerializable
	public abstract class EventInfo : MemberInfo
	public abstract class FieldInfo : MemberInfo
	public abstract class MemberInfo : ICustomAttributeProvider
	public abstract class MethodInfo : MethodBase
	public abstract class Module : ICustomAttributeProvider, ISerializable
	public class ParameterInfo : ICustomAttributeProvider, IObjectReference
	public abstract class PropertyInfo : MemberInfo
	public abstract class TypeInfo : Type, IReflectableType
}

namespace System.Resources {
	// Classes
	public class ResourceManager
	public sealed class ResourceReader : IResourceReader
	public class ResourceSet : IDisposable, IEnumerable
	public sealed class ResourceWriter : IDisposable, IResourceWriter
	public sealed class ResXDataNode : ISerializable
	public class ResXFileRef
	public class ResXResourceReader : IResourceReader
	public class ResXResourceSet : ResourceSet
	public class ResXResourceWriter : IDisposable, IResourceWriter
	// Interfaces
	public interface IResourceReader : IDisposable, IEnumerable
	public interface IResourceWriter : IDisposable
}

namespace System.Runtime.Serialization {
	// Interfaces
	public interface IFormatter
	public interface IFormatterConverter
	public interface ISerializable
}

namespace System.Runtime.CompilerServices {
	// Classes
	[MethodImpl(MethodImplOptions.NoInlining)]
	[ModuleInitializer]
	[SkipLocalsInit]
}

namespace System.Runtime.InteropServices {
	// Classes
	public class COMException : System.Runtime.InteropServices.ExternalException
	[ComImport]
	[ComVisible(false)]
	[DefaultCharSet(CharSet.Auto)]
	[DllImport("user32.dll", CharSet = CharSet.Unicode)]
	[Guid("")]
	public static class Marshal
	[MarshalAs(UnmanagedType.LPWStr)]
	public abstract class SafeHandle : System.Runtime.ConstrainedExecution.CriticalFinalizerObject, IDisposable
	[StructLayout(LayoutKind.Explicit)]
	// Structs
	public struct GCHandle
	public struct HandleRef
	// Enums
	public enum CallingConvention
	public enum CharSet
	public enum LayoutKind
}

namespace System.Text {
	// Classes
	public abstract class Encoding : ICloneable
	public sealed class StringBuilder : ISerializable
	public class UTF8Encoding : Encoding
}

namespace System.Text.Json {
	// Classes
	public sealed class JsonDocument : IDisposable
	public static class JsonSerializer
	// Structs
	public struct JsonElement
	public struct JsonProperty
}

namespace System.Text.RegularExpressions {
	// Classes
	public class Regex : ISerializable
	// Classes
	public enum RegexOptions
}

namespace System.Threading {
	// Classes
	public sealed class AutoResetEvent : EventWaitHandle
	public class CountdownEvent : IDisposable
	public class EventWaitHandle : WaitHandle
	public sealed class ExecutionContext : IDisposable, ISerializable
	public static class Interlocked
	public sealed class ManualResetEvent : EventWaitHandle
	public sealed class Mutex : WaitHandle
	public sealed class ReaderWriterLock : CriticalFinalizerObject
	public class SynchronizationContext
	public sealed class Semaphore : WaitHandle
	public sealed class Thread : CriticalFinalizerObject
	public class ThreadLocal<T> : IDisposable
	public static class ThreadPool
	public sealed class Timer : MarshalByRefObject, IAsyncDisposable, IDisposable
	public abstract class WaitHandle : MarshalByRefObject, IDisposable
	// Structs
	public struct CancellationToken
	public struct SpinLock
	public struct SpinWait
}

namespace System.Threading.Tasks {
	// Classes
	public class Task<TResult> : Task
	public class TaskCompletionSource<TResult>
	public class TaskFactory<TResult>
	public abstract class TaskScheduler
	// Enums
	public enum TaskStatus
}

namespace System.Windows {
	public class Application : DispatcherObject, IQueryAmbient
	public static class Clipboard
	public class ContentElement : DependencyObject, IInputElement, IAnimatable
	public sealed class DataObject : IDataObject, IDataObject
	public class UIElement : Visual, IInputElement, IAnimatable
	public class Window : ContentControl
}

namespace System.Windows.Forms {
	// Classes
	public sealed class Application
	public class Button : System.Windows.Forms.ButtonBase, System.Windows.Forms.IButtonControl
	public class CheckBox : System.Windows.Forms.ButtonBase
	public class CheckedListBox : System.Windows.Forms.ListBox
	public static class Clipboard
	public class ColorDialog : System.Windows.Forms.CommonDialog
	public class ComboBox : System.Windows.Forms.ListControl
	public abstract class CommonDialog : System.ComponentModel.Component
	public class ContainerControl : System.Windows.Forms.ScrollableControl, System.Windows.Forms.IContainerControl
	public class ContextMenuStrip : System.Windows.Forms.ToolStripDropDownMenu
	public class Control : System.ComponentModel.Component, IDisposable, System.ComponentModel.ISynchronizeInvoke, System.Windows.Forms.IBindableComponent, System.Windows.Forms.IDropTarget, System.Windows.Forms.IWin32Window
	public sealed class Cursor : IDisposable, ISerializable
	public static class Cursors
	public class DataGridView : System.Windows.Forms.Control, System.ComponentModel.ISupportInitialize
	public abstract class DataGridViewCell : System.Windows.Forms.DataGridViewElement, ICloneable, IDisposable
	public class DataGridViewColumn : System.Windows.Forms.DataGridViewBand, IDisposable, System.ComponentModel.IComponent
	public class DataGridViewRow : System.Windows.Forms.DataGridViewBand
	public class DataObject : System.Runtime.InteropServices.ComTypes.IDataObject, System.Windows.Forms.IDataObject
	public class DateTimePicker : System.Windows.Forms.Control
	public abstract class FileDialog : System.Windows.Forms.CommonDialog
	public class FlowLayoutPanel : System.Windows.Forms.Panel, System.ComponentModel.IExtenderProvider
	public sealed class FolderBrowserDialog : System.Windows.Forms.CommonDialog
	public class FontDialog : System.Windows.Forms.CommonDialog
	public class Form : System.Windows.Forms.ContainerControl
	public class GroupBox : System.Windows.Forms.Control
	public sealed class HtmlDocument
	public sealed class HtmlElement
	public sealed class HtmlWindow
	public sealed class ImageList : System.ComponentModel.Component
	public class KeyEventArgs : EventArgs
	public class Label : System.Windows.Forms.Control, System.Windows.Forms.Automation.IAutomationLiveRegion
	public class LinkLabel : System.Windows.Forms.Label, System.Windows.Forms.IButtonControl
	public class ListBox : System.Windows.Forms.ListControl
	public abstract class ListControl : System.Windows.Forms.Control
	public class ListView : System.Windows.Forms.Control
	public sealed class ListViewGroup : ISerializable
	public class ListViewItem : ICloneable, ISerializable
	public class MaskedTextBox : System.Windows.Forms.TextBoxBase
	public class MenuStrip : System.Windows.Forms.ToolStrip
	public class MessageBox
	public class MonthCalendar : System.Windows.Forms.Control
	public class MouseEventArgs : EventArgs
	public class NativeWindow : MarshalByRefObject, System.Windows.Forms.IWin32Window
	public sealed class NotifyIcon : System.ComponentModel.Component
	public class NumericUpDown : System.Windows.Forms.UpDownBase, System.ComponentModel.ISupportInitialize
	public sealed class OpenFileDialog : System.Windows.Forms.FileDialog
	public sealed class PageSetupDialog : System.Windows.Forms.CommonDialog
	public class Panel : System.Windows.Forms.ScrollableControl
	public class PictureBox : System.Windows.Forms.Control, System.ComponentModel.ISupportInitialize
	public sealed class PrintDialog : System.Windows.Forms.CommonDialog
	public class PrintPreviewDialog : System.Windows.Forms.Form
	public class ProgressBar : System.Windows.Forms.Control
	public class PropertyGrid : System.Windows.Forms.ContainerControl, System.Windows.Forms.ComponentModel.Com2Interop.IComPropertyBrowser
	public class PropertyManager : System.Windows.Forms.BindingManagerBase
	public class RadioButton : System.Windows.Forms.ButtonBase
	public class RichTextBox : System.Windows.Forms.TextBoxBase
	public sealed class SaveFileDialog : System.Windows.Forms.FileDialog
	public class Screen
	public abstract class ScrollBar : System.Windows.Forms.Control
	public class SplitContainer : System.Windows.Forms.ContainerControl, System.ComponentModel.ISupportInitialize
	public class Splitter : System.Windows.Forms.Control
	public sealed class SplitterPanel : System.Windows.Forms.Panel
	public class StatusStrip : System.Windows.Forms.ToolStrip
	public class TabControl : System.Windows.Forms.Control
	public class TableLayoutPanel : System.Windows.Forms.Panel, System.ComponentModel.IExtenderProvider
	public class TabPage : System.Windows.Forms.Panel
	public class TaskDialog : System.Windows.Forms.IWin32Window
	public class TextBox : System.Windows.Forms.TextBoxBase
	public class Timer : System.ComponentModel.Component
	public class ToolStrip : System.Windows.Forms.ScrollableControl, IDisposable
	public abstract class ToolStripItem : System.ComponentModel.Component, IDisposable, System.Windows.Forms.IDropTarget
	public class ToolTip : System.ComponentModel.Component, System.ComponentModel.IExtenderProvider
	public class TrackBar : System.Windows.Forms.Control, System.ComponentModel.ISupportInitialize
	public class TreeNode : MarshalByRefObject, ICloneable, ISerializable
	public class TreeView : System.Windows.Forms.Control
	public class UserControl : System.Windows.Forms.ContainerControl
	public class WebBrowser : System.Windows.Forms.WebBrowserBase
	// Structs
	public struct Message
	// Interfaces
	public interface IDataObject
	public interface IDropTarget
	// Enums
	public enum DialogResult
	public enum Keys
}

namespace System.Xml {
	// Classes
	public class XmlDocument : XmlNode
	public class XmlElement : XmlLinkedNode
	public abstract class XmlNode : ICloneable, IEnumerable, IXPathNavigable
}

namespace Microsoft.VisualBasic.FileIO {
	public class TextFieldParser : IDisposable
}

namespace Microsoft.Win32 {
	// Classes
	public static class Registry
	public sealed class RegistryKey : MarshalByRefObject, IDisposable
}

//! comment			===========================================================
// https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/xmldoc/recommended-tags
/**
<c>text</c>
<code>text</code>
<description>description</description>
<example>
<exception cref="member">description</exception>
<include file='filename' path='tagpath[@name="id"]' />
<inheritdoc [cref=""] [path=""]/>
<list type="bullet|number|table">
<listheader>
<para>
<param name="name">description</param>
<paramref name="name"/>
<permission cref="member">description</permission>
<remarks>description</remarks>
<returns>description</returns>
<see cref="member"/>
<see cref="member">Link text</see>
<see href="link">Link Text</see>
<see langword="keyword"/>
<seealso cref="member"/>
<seealso href="link">Link Text</seealso>
<summary>description</summary>
<term>term</term>
<typeparam name="TResult">The type returned from this method</typeparam>
<typeparamref name="TKey"/>
<value>property-description</value>
<event>
<note>
*/
