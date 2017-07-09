#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/C_Sharp_%28programming_language%29
// https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/
// https://wiki.gnome.org/Projects/Vala

static KEYWORDLIST Keywords_CSharp = {
"abstract as base break case catch checked class const continue default delegate do else enum event explicit "
"extern false finally fixed for foreach goto if implicit in interface internal is lock namespace new null "
"operator out override params private protected public readonly ref return sealed sizeof stackalloc static "
"struct switch this throw true try typeof unchecked unsafe using virtual void volatile while "
// contextual keywords
"add alias async await get global nameof partial remove set value when where yield "
, // type keyword
"dynamic var bool byte char decimal double float int long object sbyte short string uint ulong ushort"
, // preprocessor
"if else elif endif define undef warning error line region endregion pragma "
, // directive
// LINQ
"from where select group into orderby join let in on equals by ascending descending "
, // attribute
// attribute target
"assembly module field event method param property return type "
"AttributeUsage Flags Serializable STAThread MTAThread ThreadStatic CLSCompliant ComVisible DllImport Guid MarshalAs StructLayout ComImport DefaultCharSet NonSerialized Obsolete ParamArray "
, // class
"Delegate Enum Void Assembly Guid Clsid Type ValueType Array Marshal "
"Object String Boolean Byte SByte Char Decimal Double Single Int16 UInt16 Int32 UInt32 Int64 UInt64 Complex BigInteger "
"IntPtr UIntPtr HandleRef GCHandle "
"Activator ArgIterator BitConverter Buffer Console Convert DBNull Environment GC Lazy MarshalByRefObject Math ModuleHandle Nullable Random StringComparer TimeSpan TimeZone TimeZoneInfo Tuple TypedReference WeakReference Version Uri UriBuilder UriParser "
"Attribute EventInfo FieldInfo MemberInfo MethodInfo ParameterInfo PropertyInfo "
"DateTime StringBuilder Regex Encoding UTF8Encoding ArrayList BitArray Hashtable Queue Stack List Dictionary Collection SortedList SortedSet SortedDictionary LinkedList KeyValuePair HashSet "
"Process ProcessStartInfo Mutex Thread Timer "
"BinaryReader BinaryWriter BufferedStream FileStream MemoryStream Stream StreamReader StreamWriter StringReader StringWriter TextReader TextWriter "
"Directory DirectoryInfo DriveInfo File FileInfo FileSystemInfo Path "
"Exception IOException EventArgs EventHandler KeyEventArgs MouseEventArgs"
"NetworkStream Socket SocketException TcpClient TcpListener "
"EndPoint FileWebRequest FileWebResponse FtpWebRequest FtpWebResponse HttpListener HttpListenerRequest HttpListenerResponse HttpWebRequest HttpWebResponse IPAddress IPEndPoint SocketAddress WebClient WebException WebRequest WebResponse "
"Registry RegistryKey SerialPort COMException "
"Bitmap Brush Brushes BufferedGraphics CharacterRange Color Font Graphics Icon Image ImageAnimator Pen Pens Point PointF Rectangle RectangleF Size SizeF SolidBrush SystemBrushes SystemColors SystemFonts SystemIcons SystemPens TextureBrush Blend ColorBlend Matrix "
"Application Clipboard ContainerControl Control Cursor Cursors Form Message NativeWindow Screen UserControl "
"Button CheckBox CheckedListBox ComboBox ContextMenu ContextMenuStrip DataGridView DataGridViewCell DataGridViewColumn DataGridViewRow GroupBox HtmlDocument HtmlWindow ImageList Label LinkLabel ListBox ListControl ListView ListViewGroup ListViewItem MainMenu MaskedTextBox Menu MenuItem MenuStrip NotifyIcon NumericUpDown Panel PictureBox ProgressBar RadioButton RichTextBox ScrollBar SplitContainer Splitter StatusBar StatusBarPanel StatusStrip TabControl TableLayoutPanel TabPage TextBox ToolBar ToolStrip ToolStripItem ToolTip TrackBar TreeNode TreeView VScrollBar WebBrowser "
"MessageBox CommonDialog FileDialog ColorDialog FolderBrowserDialog FontDialog OpenFileDialog PageSetupDialog PrintDialog PrintPreviewDialog SaveFileDialog "
"ResourceManager ResourceReader ResourceSet ResourceWriter ResXDataNode ResXFileRef ResXResourceReader ResXResourceSet ResXResourceWriter "
, // interface
"IAsyncResult ICloneable IComparable IConvertible IDisposable IEquatable IFormattable IComparer IEnumerable IEnumerator ISerializable ICollection IDictionary IList ISet IFormatProvider IObservable IObserver IServiceProvider IFormatter IFormatterConverter "
"IResourceReader IResourceWriter "
, // enumeration
"ConsoleColor ConsoleKey StringComparison StringSplitOptions TypeCode LayoutKind AttributeTargets DialogResult Keys CharSet "
"DriveType FileAccess FileAttributes FileMode FileOptions FileShare SearchOption SeekOrigin "
"SocketError SocketFlags SocketType FtpStatusCode HttpRequestHeader HttpResponseHeader HttpStatusCode Parity "
"FontStyle GraphicsUnit KnownColor "
, // constant
""

#if NUMKEYWORD == 16
, "", "", "","","",""
,
"for^() if^() switch^() while^() catch^() else^if^() foreach^() sizeof() typeof() using^() fixed^() lock^() "
"base() this() checked() default() delegate() nameof()"
#endif
};

EDITLEXER lexCSharp = { SCLEX_CPP, NP2LEX_CSHARP, L"C# Source", L"cs", L"", &Keywords_CSharp,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_DIRECTIVE, 63391, L"LINQ Keyword", L"fore:#FF8000", L"" },
	{ SCE_C_PREPROCESSOR, NP2STYLE_Preprocessor, L"Preprocessor", L"fore:#FF8000", L"" },
	{ SCE_C_ATTRIBUTE, NP2STYLE_Attribute, L"Attribute", L"fore:#FF8000", L""},
	{ SCE_C_CLASS, NP2STYLE_Class, L"Class", L"fore:#0080FF", L"" },
	{ SCE_C_STRUCT, NP2STYLE_Struct, L"Struct", L"fore:#0080FF", L"" },
	{ SCE_C_INTERFACE, NP2STYLE_Interface, L"Interface", L"bold; fore:#1E90FF", L""},
	{ SCE_C_FUNCTION, NP2STYLE_Method, L"Method", L"fore:#B000B0", L"" },
	{ SCE_C_ENUMERATION, NP2STYLE_Enumeration, L"Enumeration", L"fore:#FF8000", L""},
	{ SCE_C_CONSTANT, NP2STYLE_Constant, L"Constant", L"fore:#B000B0", L""},
	{ MULTI_STYLE(SCE_C_COMMENT,SCE_C_COMMENTLINE,0,0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_C_COMMENTDOC_TAG_XML, NP2STYLE_DocCommentTag, L"Doc Comment Tag", L"bold; fore:#008000F", L"" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC,SCE_C_COMMENTLINEDOC,SCE_C_COMMENTDOC_TAG,0), NP2STYLE_DocComment, L"Doc Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_STRING,SCE_C_CHARACTER,SCE_C_STRINGEOL,0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_VERBATIM, NP2STYLE_VerbatimString, L"Verbatim String", L"fore:#008080", L"" },
	{ SCE_C_LABEL, NP2STYLE_Label, L"Label", L"fore:#000000; back:#FFC040", L""},
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
