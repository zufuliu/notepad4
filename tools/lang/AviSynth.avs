# 2.6.0 http://avisynth.nl/index.php/Main_Page
# 3.6.1 https://github.com/AviSynth/AviSynthPlus
# http://avisynth.nl/index.php/AviSynth_Syntax
# http://avisynth.nl/index.php/The_full_AviSynth_grammar
# https://github.com/nekopanda/AviSynthPlus/wiki/Language-New-Features

#! keywords				=======================================================
# http://avisynth.nl/index.php/The_full_AviSynth_grammar#Keywords
__END__
function global return try catch
last true false yes no
clip int float string bool val func
# 1.1 http://avisynth.nl/index.php/GScript
if else while for

#! functions			=======================================================
# http://avisynth.nl/index.php/Internal_functions
# Boolean functions
IsBool(var)
IsClip(var)
IsFloat(var)
IsInt(var)
IsString(var)
Exist(filename)
Defined(var)
FunctionExists(name)				# AVS+
InternalFunctionExists(name)		# AVS+
VarExist(name)						# AVS+

# Control functions
Apply(string func_string [, arg1 [, arg2 [, ... [, argn]]]])
Eval(expression [, string name])
Import(filename)
Import(filename [, ...] [, bool utf8])							# AVS+
Select(index, item0 [, item1 [, ...[, itemn]]])
Default(x, d)
Assert(condition [, err_msg])
NOP()
Undefined()														# v2.60

# Global Options
SetMemoryMax(amount)
SetCacheMode(mode)												# AVS+
SetMaxCPU([string feature1, string feature2, ...])				# AVS+
SetWorkingDir(path)
SetPlanarLegacyAlignment(mode)
global OPT_AllowFloatAudio = true ## default false
global OPT_UseWaveExtensible = true ## default false
global OPT_dwChannelMask(int v)									# v2.60
global OPT_AVIPadScanlines = true ## default false				v2.60
global OPT_VDubPlanarHack = true ## default false				v2.60
global OPT_Enable_V210 = true ## default false					AVS+
global OPT_Enable_Y3_10_10 = true ## default false				AVS+
global OPT_Enable_Y3_10_16 = true ## default false				AVS+
global OPT_Enable_b64a = true ## default false					AVS+
global OPT_Enable_PlanarToPackedRGB = true ## default false		AVS+

# Conversion functions
Value(string)
HexValue(string)
HexValue(string [, int pos])	# AVS+
Hex(int)						# v2.60
Hex(int [, int width])			# AVS+
String(var [, string format_string])

# Numeric functions
Max(float, float [, ...])
Min(float, float [, ...])
MulDiv(int, int, int)
Floor(float)
Ceil(float)
Round(float)
Int(float)
Float(int)
Fmod(float, float)				# v2.60
Pi()
Exp(float)
Log(float)
Log10(float)					# v2.60
Pow(float base, float power)
Sqrt(float)
Abs(float or int)
Sign(float)
Frac(float)
Rand([int max] [, bool scale] [, bool seed])
Spline(float X, x1, y1, x2, y2, .... [, bool cubic])
ContinuedNumerator(float, int limit)				# v2.60
ContinuedNumerator(int, int, int limit)				# v2.60
ContinuedDenominator(float, int limit)				# v2.60
ContinuedDenominator(int, int, int limit)			# v2.60

# Trigonometry functions
Sin(float)
Cos(float)
Tan(float)						# v2.60
Asin(float)						# v2.60
Acos(float)						# v2.60
Atan(float)						# v2.60
Atan2(float, float)				# v2.60
Sinh(float)						# v2.60
Cosh(float)						# v2.60
Tanh(float)						# v2.60

# Bit functions
BitAnd(int, int)				# v2.60
BitNot(int)						# v2.60
BitOr(int, int)					# v2.60
BitXor(int, int)				# v2.60
BitLShift(int, int)				# v2.60
BitShl(int, int)				# v2.60
BitSal(int, int)				# v2.60
BitRShiftA(int, int)			# v2.60
BitRShiftS(int, int)			# v2.60
BitSar(int, int)				# v2.60
BitRShiftL(int, int)			# v2.60
BitRShiftU(int, int)			# v2.60
BitShr(int, int)				# v2.60
BitLRotate(int, int)			# v2.60
BitRol(int, int)				# v2.60
BitRRotateL(int, int)			# v2.60
BitRor(int, int)				# v2.60
BitTest(int, int)				# v2.60
BitTst(int, int)				# v2.60
BitSet(int, int)				# v2.60
BitSetCount(int [, int...])		# AVS+
BitClear(int, int)				# v2.60
BitClr(int, int)				# v2.60
BitChange(int, int)				# v2.60
BitChg(int, int)				# v2.60

# Runtime functions
AverageLuma(clip [, int offset = 0])
AverageChromaU(clip [, int offset = 0])
AverageChromaV(clip [, int offset = 0])
AverageB(clip [, int offset = 0])				# AVS+
AverageG(clip [, int offset = 0])				# AVS+
AverageR(clip [, int offset = 0])				# AVS+
LumaDifference(clip1, clip2)
ChromaUDifference(clip1, clip2)
ChromaVDifference(clip1, clip2)
RGBDifference(clip1, clip2)
BDifference(clip1, clip2)						# AVS+
GDifference(clip1, clip2)						# AVS+
RDifference(clip1, clip2)						# AVS+
YDifferenceFromPrevious(clip)
UDifferenceFromPrevious(clip)
VDifferenceFromPrevious(clip)
RGBDifferenceFromPrevious(clip)
BDifferenceFromPrevious(clip)					# AVS+
GDifferenceFromPrevious(clip)					# AVS+
RDifferenceFromPrevious(clip)					# AVS+
YDifferenceToNext(clip [, int offset = 1])
UDifferenceToNext(clip [, int offset = 1])
VDifferenceToNext(clip [, int offset = 1])
RGBDifferenceToNext(clip [, int offset = 1])
BDifferenceToNext(clip [, int offset = 1])		# AVS+
GDifferenceToNext(clip [, int offset = 1])		# AVS+
RDifferenceToNext(clip [, int offset = 1])		# AVS+
YPlaneMedian(clip [, int offset = 0])
UPlaneMedian(clip [, int offset = 0])
VPlaneMedian(clip [, int offset = 0])
BPlaneMedian(clip [, int offset = 0])			# AVS+
GPlaneMedian(clip [, int offset = 0])			# AVS+
RPlaneMedian(clip [, int offset = 0])			# AVS+
YPlaneMin(clip [, float threshold = 0, int offset = 0])
UPlaneMin(clip [, float threshold = 0, int offset = 0])
VPlaneMin(clip [, float threshold = 0, int offset = 0])
BPlaneMin(clip [, float threshold = 0, int offset = 0])				# AVS+
GPlaneMin(clip [, float threshold = 0, int offset = 0])				# AVS+
RPlaneMin(clip [, float threshold = 0, int offset = 0])				# AVS+
YPlaneMax(clip [, float threshold = 0, int offset = 0])
UPlaneMax(clip [, float threshold = 0, int offset = 0])
VPlaneMax(clip [, float threshold = 0, int offset = 0])
BPlaneMax(clip [, float threshold = 0, int offset = 0])				# AVS+
GPlaneMax(clip [, float threshold = 0, int offset = 0])				# AVS+
RPlaneMax(clip [, float threshold = 0, int offset = 0])				# AVS+
YPlaneMinMaxDifference(clip [, float threshold, int offset = 0])
UPlaneMinMaxDifference(clip [, float threshold, int offset = 0])
VPlaneMinMaxDifference(clip [, float threshold, int offset = 0])
BPlaneMinMaxDifference(clip [, float threshold, int offset = 0])	# AVS+
GPlaneMinMaxDifference(clip [, float threshold, int offset = 0])	# AVS+
RPlaneMinMaxDifference(clip [, float threshold, int offset = 0])	# AVS+

# Runtime functions for frame properties
# VapourSynth
# Property set
propSet(clip, string key_name, function func_obj [, integer mode])		# AVS+
propSet(clip, string key_name, integer value [, integer mode])			# AVS+
propSet(clip, string key_name, float value [, integer mode])				# AVS+
propSet(clip, string key_name, string value [, integer mode])				# AVS+
propSet(clip, string key_name, array value)									# AVS+
propSetInt(clip, string key_name, function func_obj [, integer mode])		# AVS+
propSetFloat(clip, string key_name, function func_obj [, integer mode])	# AVS+
propSetString(clip, string key_name, function func_obj [, integer mode])	# AVS+
propSetArray(clip, string key_name, function func_obj [, integer mode])	# AVS+
# Property get
propGetAny(clip, string key_name[, integer index, integer offset])		# AVS+
propGetInt(clip, string key_name[, integer index, integer offset])		# AVS+
propGetFloat(clip, string key_name[, integer index, integer offset])	# AVS+
propGetString(clip, string key_name[, integer index, integer offset])	# AVS+
propGetAsArray(clip, string key_name[, integer index, integer offset])	# AVS+
propGetAsArray(clip [, integer offset])									# AVS+
# Deleting properties
propDelete(clip, string)													# AVS+
# Other property functions
propShow(clip, integer size, bool showtype)								# AVS+
propGetDataSize(clip, string key_name [, integer index, integer offset])# AVS+
propNumElements(clip, string key_name [, integer offset])					# AVS+
propNumKeys(clip, [, integer offset])										# AVS+
propGetKeyByIndex(clip, integer index [, integer offset])				# AVS+
propGetType(clip, string key_name [, integer offset])						# AVS+

# Script functions
ScriptName()								# v2.60
ScriptNameUtf8()							# AVS+
ScriptFile()								# v2.60
ScriptFileUtf8()							# AVS+
ScriptDir()									# v2.60
ScriptDirUtf8()								# AVS+
SetLogParams([string target, int level])	# AVS+
LogMsg(string, int)							# AVS+
GetProcessInfo(int)							# AVS+

# String functions
LCase(string)
UCase(string)
StrToUtf8(string)			# AVS+
StrFromUtf8(string)			# AVS+
StrLen(string)
RevStr(string)
LeftStr(string, int)
RightStr(string, int)
MidStr(string, int pos [, int length])
FindStr(string, substring)
ReplaceStr(string, substring, replacement_string [, bool sig])	# AVS+
Format(formatstring [, value1, value2, ...])					# AVS+
FillStr(int [, string])			# v2.60
StrCmp(string, string)			# v2.60
StrCmpi(string, string)			# v2.60
TrimLeft(string)
TrimRight(string)
TrimAll(string)					# AVS+
Chr(int)
Ord(string)						# v2.60
Time(string)

# Version functions
VersionNumber()
VersionString()
bool IsVersionOrGreater(int majorVersion, int minorVersion [,int bugfixVersion]) # AVS+

# Other helper functions
string BuildPixelType(string family, int bits, int chroma, bool compat, bool oldnames, clip sample_clip) # AVS+
int ColorSpaceNameToPixelType(string)	# AVS+

# http://avisynth.nl/index.php/GScript
GScript(string script)						# AVS+
GImport(string file [, ...])				# AVS+
GEval(string expression [, string name])	# AVS+

# http://avisynth.nl/index.php/Plugins
LoadPlugin(string filename [, string filename...])
LoadCPlugin(string filename)
LoadCPlugin(string filename [, string filename...])				# AVS+
Load_Stdcall_Plugin(string filename)
Load_Stdcall_Plugin(string filename [, string filename...])		# AVS+
LoadVirtualDubPlugin(string filename, string filtername [, int preroll])
LoadVFAPIPlugin(string filename, string filtername)

#! filters		=======================================================
# http://avisynth.nl/index.php/Internal_filters
# Source filters
AviSource(string filename [, ...] [, bool audio, string pixel_type, string fourCC, int vtrack, int atrack, AVS+ bool utf8])
AviFileSource(string filename [, ...] [, bool audio, string pixel_type, string fourCC, int vtrack, int atrack, AVS+ bool utf8])
OpenDMLSource(string filename [, ...] [, bool audio, string pixel_type, string fourCC, int vtrack, int atrack, AVS+ bool utf8])
WavSource(string filename [, ...][, AVS+ bool utf8])
DirectShowSource(string filename [, float fps, bool seek, bool audio, bool video, bool convertfps, bool seekzero, int timeout, string pixel_type, int framecount, string logfile, int logmask])
ImageReader(string file, int start, int end, float fps, bool use_DevIL, bool info, string pixel_type)
ImageSource(string file, int start, int end, float fps, bool use_DevIL, bool info, string pixel_type)
ImageSourceAnim(string file, float fps, bool info, string pixel_type)
SegmentedAviSource(string base_filename [, ...] [, bool audio, string pixel_type, int vtrack, int atrack])
SegmentedDirectShowSource(string base_filename [, ...] [, float fps, bool seek, bool audio, bool video] [,bool convertfps, bool seekzero, int timeout, string pixel_type])

# Color conversion and adjustment filters
ColorYUV(clip [, float gain_y, float off_y, float gamma_y, float cont_y, float gain_u, float off_u, float gamma_u, float cont_u, float gain_v, float off_v, float gamma_v, float cont_v, string levels, string opt, bool showyuv, bool analyze, bool autowhite, bool autogain, bool conditional])
ColorYUV(clip [, float gain_y, float off_y, float gamma_y, float cont_y, float gain_u, float off_u, float gamma_u, float cont_u, float gain_v, float off_v, float gamma_v, float cont_v, string levels, string opt, bool showyuv, bool analyze, bool autowhite, bool autogain, bool conditional, int bits, bool showyuv_fullrange, bool f2c]) # AVS+
# RGB interleaved
ConvertToRGB(clip [, string matrix, bool interlaced, string ChromaInPlacement, string chromaresample])
ConvertToRGB24(clip [, string matrix, bool interlaced, string ChromaInPlacement, string chromaresample])
ConvertToRGB32(clip [, string matrix, bool interlaced, string ChromaInPlacement, string chromaresample])
ConvertToRGB48(clip, [string matrix, bool interlaced, string ChromaInPlacement, string chromaresample]) # AVS+
ConvertToRGB64(clip, [string matrix, bool interlaced, string ChromaInPlacement, string chromaresample]) # AVS+
# RGB planar
ConvertToPlanarRGB(clip, [string matrix, bool interlaced, string ChromaInPlacement, string chromaresample]) # AVS+
ConvertToPlanarRGBA(clip, [string matrix, bool interlaced, string ChromaInPlacement, string chromaresample]) # AVS+
# YUV444
ConvertToYV24(clip [, bool interlaced, string matrix, string ChromaInPlacement, string chromaresample])
ConvertToYUV444(clip, [string matrix, bool interlaced, string ChromaInPlacement, string chromaresample])
# YUV422
ConvertToYV16(clip [, bool interlaced, string matrix, string ChromaInPlacement, string chromaresample])
ConvertToYUY2(clip [, bool interlaced, string matrix, string ChromaInPlacement, string chromaresample])
ConvertToYUV422(clip, [string matrix, bool interlaced, string ChromaInPlacement, string chromaresample]) # AVS+
ConvertBackToYUY2(clip [, string matrix])
# YUV420
ConvertToYV12(clip [, bool interlaced, string matrix, string ChromaInPlacement, string chromaresample, string ChromaOutPlacement])
ConvertToYUV420(clip, [string matrix, bool interlaced, string ChromaInPlacement, string chromaresample, string ChromaOutPlacement]) #AVS+
# YUV411
ConvertToYV411(clip [, bool interlaced, string matrix, string ChromaInPlacement, string chromaresample])
# Y-only
ConvertToY8(clip [, string matrix])
ConvertToY(clip, [string matrix]) # AVS+
FixLuminance(clip clip, int intercept, int slope)
Greyscale(clip clip [, string matrix])
Grayscale(clip clip [, string matrix])
Invert(clip clip [, string channels])
Levels(clip input, int input_low, float gamma, int input_high, int output_low, int output_high [, bool coring, bool dither])
Levels(clip input, float input_low, float gamma, float input_high, float output_low, float output_high [, bool coring, bool dither]) # AVS+
Limiter(clip clip [, int min_luma, int max_luma, int min_chroma, int max_chroma, string show])
Limiter(clip clip [, float min_luma, float max_luma, float min_chroma, float max_chroma, string show, bool paramscale]) # AVS+
MergeARGB(clip clipA, clip clipR, clip clipG, clip clipB)
MergeRGB(clip clipR, clip clipG, clip clipB [, string pixel_type])
Merge(clip clip1, clip clip2 [, float weight])
MergeChroma(clip clip1, clip clip2 [, float chromaweight])(depreciated since v2.60)
MergeChroma(clip clip1, clip clip2 [, float weight])(v2.60 only)
MergeLuma(clip clip1, clip clip2 [, float lumaweight])(depreciated since v2.60)
MergeLuma(clip clip1, clip clip2 [, float weight])(v2.60 only)
RGBAdjust(clip clip [, float r, float g, float b, float a, float rb, float gb, float bb, float ab, float rg, float gg, float bg, float ag, bool analyze, bool dither, AVS+ bool conditional])
ShowAlpha(clip clip [, string pixel_type])
ShowRed(clip clip [, string pixel_type])
ShowGreen(clip clip [, string pixel_type])
ShowBlue(clip clip [, string pixel_type])
SwapUV(clip clip)
UToY(clip clip)
VToY(clip clip)
UToY8(clip clip)
VToY8(clip clip)
YToUV(clip clipU, clip clipV [, clip clipY])
Tweak(clip clip [, float hue, float sat, float bright, float cont, bool coring, bool sse, float startHue, float endHue, float maxSat, float minSat, float interp, bool dither])
Tweak(clip clip [, float hue, float sat, float bright, float cont, bool coring, bool sse, float startHue, float endHue, float maxSat, float minSat, float interp, bool dither, bool realcalc, float dither_strength]) # AVS+

# Overlay and Mask filters
Layer(clip base_clip, clip overlay_clip [, string op, int level, int x, int y, int threshold, bool use_chroma])
Layer(clip base_clip, clip overlay_clip [, string op, int level, int x, int y, int threshold, bool use_chroma, float opacity, string placement]) # AVS+
Mask(clip clip, clip mask_clip)
ResetMask(clip clip)
ResetMask(clip clip, float mask) # AVS+
ColorKeyMask(clip clip, int color [, int tolB, int tolG, int tolR])
MaskHS(clip [, int startHue, int endHue, int maxSat, int minSat, bool coring])
MaskHS(clip [, int startHue, int endHue, int maxSat, int minSat, bool coring, bool realcalc]) # AVS+
Overlay(clip clip, clip overlay [, int x, int y, clip mask, float opacity, string mode, bool greymask, string output, bool ignore_conditional, bool pc_range])
Overlay(clip clip, clip overlay [, int x, int y, clip mask, float opacity, string mode, bool greymask, string output, bool ignore_conditional, bool pc_range, bool use444]) AVS+
Subtract(clip clip1, clip clip2)

# Geometric deformation filters
AddBorders(clip clip, int left, int top, int right, int bottom [, int color] [, int color_yuv AVS+])
Crop(clip clip, int left, int top, int width, int height [, bool align])
Crop(clip clip, int left, int top, int -right, int -bottom [, bool align])
CropBottom(clip clip, int count)
FlipHorizontal(clip clip)
FlipVertical(clip clip)
Letterbox(clip clip, int top, int bottom [, int x1, int x2, int color] [, int color_yuv AVS+])
HorizontalReduceBy2(clip clip)
VerticalReduceBy2(clip clip)
ReduceBy2(clip clip)
BilinearResize(clip clip, int target_width, int target_height [, float src_left, float src_top, float src_width, float src_height])
BicubicResize(clip clip, int target_width, int target_height [, float b, float c, float src_left, float src_top, float src_width, float src_height])
BlackmanResize(clip clip, int target_width, int target_height [, float src_left, float src_top, float src_width, float src_height, int taps])
GaussResize(clip clip, int target_width, int target_height [, float src_left, float src_top, float src_width, float src_height, float p])
LanczosResize(clip clip, int target_width, int target_height [, float src_left, float src_top, float src_width, float src_height, int taps])
Lanczos4Resize(clip clip, int target_width, int target_height [, float src_left, float src_top, float src_width, float src_height])
PointResize(clip clip, int target_width, int target_height [, float src_left, float src_top, float src_width, float src_height])
Spline16Resize(clip clip, int target_width, int target_height [, float src_left, float src_top, float src_width, float src_height])
Spline36Resize(clip clip, int target_width, int target_height [, float src_left, float src_top, float src_width, float src_height])
Spline64Resize(clip clip, int target_width, int target_height [, float src_left, float src_top, float src_width, float src_height])
SincResize(clip clip, int target_width, int target_height [, float src_left, float src_top, float src_width, float src_height, int taps])
SkewRows(clip clip, int skew)
TurnLeft(clip clip)
TurnRight(clip clip)
Turn180(clip clip)

# Pixel restoration filters
Blur(clip clip, float amount, bool MMX)
Blur(clip, float amountH, float amountV, bool MMX)
Sharpen(clip clip, float amount, bool MMX)
Sharpen(clip, float amountH, float amountV, bool MMX)
GeneralConvolution(clip clip, [int bias, string matrix, float divisor, bool auto])
GeneralConvolution(clip clip, [float bias, string matrix, float divisor, bool auto, bool luma, bool chroma, bool alpha]) # AVS+
SpatialSoften(clip clip, int radius, int luma_threshold, int chroma_threshold)
TemporalSoften(clip clip, int radius, int luma_threshold, int chroma_threshold [, int scenechange] [, int mode])
FixBrokenChromaUpsampling(clip clip)

# Timeline editing filters
AlignedSplice(clip clip1, clip clip2 [,...])
UnalignedSplice(clip clip1, clip clip2 [,...])
AssumeFPS(clip clip, float fps [, bool sync_audio])
AssumeFPS(clip clip, int numerator [, int denominator, bool sync_audio])
AssumeFPS(clip clip1, clip clip2 [, bool sync_audio])
AssumeFPS(clip clip1, string preset [, bool sync_audio])
AssumeScaledFPS(clip [, int multiplier, int divisor, bool sync_audio])
ChangeFPS(clip clip, float fps [, bool linear])
ChangeFPS(clip clip, int numerator [, int denominator, bool linear])
ChangeFPS(clip clip1, clip clip2, bool linear)
ChangeFPS(clip clip1, string preset [, bool linear])
ConvertFPS(clip clip, float new_rate [, int zone, int vbi])
ConvertFPS(clip clip, int numerator [, int denominator, int zone, int vbi])
ConvertFPS(clip clip1, clip clip2 [, int zone, int vbi])
ConvertFPS(clip clip1, string preset [, int zone, int vbi])
DeleteFrame(clip clip, int frame_num [, ...])
Dissolve(clip clip1, clip clip2 [,...], int overlap [, float fps])
DuplicateFrame(clip clip, int frame_num [, ...])
FadeIn(clip clip, int num_frames [, int color, float fps])
FadeOut(clip clip, int num_frames [, int color, float fps])
FadeIO(clip clip, int num_frames [, int color, float fps])
FadeIn0(clip clip, int num_frames [, int color, float fps])
FadeOut0(clip clip, int num_frames [, int color, float fps])
FadeIO0(clip clip, int num_frames [, int color, float fps])
FadeIn2(clip clip, int num_frames [, int color, float fps])
FadeOut2(clip clip, int num_frames [, int color, float fps])
FadeIO2(clip clip, int num_frames [, int color, float fps])
FreezeFrame(clip clip, int first-frame, int last-frame, int source-frame)
Interleave(clip1, clip2 [, ...])
Loop(clip clip [, int times = -1] [, int start_frame = 0] [, int end_frame = inf])
Reverse(clip clip)
SelectEven(clip clip)
SelectOdd(clip clip)
SelectEvery(clip clip, int step-size, int offset1 [, int offset2, ...])
SelectRangeEvery(clip clip [, int every] [, int length] [, int offset] [, bool audio])
Trim(clip, int first_frame, int last_frame [, bool pad])
Trim(clip, int first_frame, int -num_frames [, bool pad])
Trim(clip, int first_frame, [int end, bool pad])
Trim(clip, int first_frame, [int length, bool pad])

# Interlace filters
AssumeFieldBased(clip clip)
AssumeFrameBased(clip clip)
AssumeTFF(clip clip)
AssumeBFF(clip clip)
ComplementParity(clip clip)
Bob(clip clip [, float b, float c, int height])
DoubleWeave(clip clip)
PeculiarBlend(clip clip, int cutoff)
Pulldown(clip, int a, int b)
SeparateFields(clip clip)
SeparateColumns(clip, int interval)
SeparateRows(clip, int interval)
SwapFields(clip)
Weave(clip clip)
WeaveColumns(clip clip, int period)
WeaveRows(clip clip, int period)

# Audio processing filters
Amplify(clip, float amount [, float amount]...)
AmplifyDB(clip, float amount [, float amount]...)
AssumeSampleRate(clip clip, int samplerate)
AudioDub(video_clip, audio_clip)
AudioDubEx(video_clip, audio_clip)
AudioTrim(clip, float start_time, float end_time)
AudioTrim(clip, float start_time, float -duration)
AudioTrim(clip, float start_time [, float end])
AudioTrim(clip, float start_time [, float length])
ConvertAudioTo8bit(clip clip)
ConvertAudioTo16bit(clip clip)
ConvertAudioTo24bit(clip clip)
ConvertAudioTo32bit(clip clip)
ConvertAudioToFloat(clip clip)
ConvertToMono(clip clip)
DelayAudio(clip, float seconds)
EnsureVBRMP3Sync(clip clip)
GetChannel(clip clip, int ch1 [, int ch2, ...])
GetChannels(clip clip, int ch1 [, int ch2, ...])
GetLeftChannel(clip clip)
GetRightChannel(clip clip)
KillAudio(clip)
KillVideo(clip)
MergeChannels(clip clip1, clip clip2 [, clip clip3, ...])
MixAudio(clip clip1, clip clip2, float clip1_factor, float clip2_factor)
MonoToStereo(clip left_channel_clip, clip right_channel_clip)
Normalize(clip clip [, float volume, bool show])
ResampleAudio(clip clip, int new_rate_numerator [, int new_rate_denominator])
SuperEQ(clip, string filename)
SuperEQ(clip, int band1 [, int band2, ..., int band18])
SSRC(clip, int samplerate [, bool fast])
TimeStretch(clip clip [, float tempo, float rate, float pitch, int sequence, int seekwindow, int overlap, bool quickseek, int aa])
TimeStretch(clip clip [, int tempo_n, int tempo_d, int rate_n, int rate_d, int pitch_n, int pitch_d, int sequence, int seekwindow, int overlap, bool quickseek, int aa])

# Conditional and other meta filters
ConditionalFilter(clip testclip, clip source1, clip source2, string expression1, string operator, string expression2 [, bool show]) AVS+ConditionalFilter(clip testclip, clip source1, clip source2, string expression1 [, bool show])
ConditionalSelect(clip testclip, string expression, clip source0 [, clip source1...] [, bool show])
ScriptClip(clip clip, string filter [, bool show, bool after_frame])
FrameEvaluate(clip clip, string filter [, bool show, bool after_frame])
ConditionalReader(clip clip, string filename, string variablename [, bool show])
WriteFile(clip clip, string filename, string expression1 [, string expression2 [, ...]] [, bool append, bool flush])
WriteFileIf(clip clip, string filename, string expression1 [, string expression2 [, ...]] [, bool append, bool flush])
WriteFileStart(clip clip, string filename, string expression1 [, string expression2 [, ...]] [, bool append])
WriteFileEnd(clip clip, string filename, string expression1 [, string expression2 [, ...]] [, bool append])
Animate(clip clip, int start_frame, int end_frame, string filtername, var start_args, var end_args)
ApplyRange(clip clip, int start_frame, int end_frame, string filtername, var args)
TCPServer(clip clip [, int port])
TCPSource(string hostname [, int port, string compression])

# Export filters
ImageWriter(clip clip, string file, int start, int [-]end, string type, bool info)

# Debug filters
BlankClip([clip clip, int length, int width, int height, string pixel_type, float fps, int fps_denominator, int audio_rate, int channels, string sample_type, int color, int color_yuv])
Blackness(...same parameters as BlankClip)
ColorBars([int width, int height, string pixel_type])
ColorBars([int width, int height, string pixel_type, bool staticframes]) # AVS+
ColorBarsHD([int width, int height, string pixel_type])
ColorBarsHD([int width, int height, string pixel_type, bool staticframes]) # AVS+
Compare(clip clip_filtered, clip clip_original [, string channels, string logfile, bool show_graph])
Echo(clip clip1, clip clip2 [, ...])
Histogram(clip clip [, string mode, float factor])
Histogram(clip clip [, string mode, float factor, int bits, bool keepsource, bool markers]) # AVS+
Info(clip clip)
Info(clip clip, [string font, float size, int text_color, int halo_color]) # AVS+
MessageClip(string message [, int width, int height, bool shrink, int text_color, int halo_color, int bg_color])
Preroll(clip clip, int video, float audio)
ShowFiveVersions(clip clip1, clip clip2, clip clip3, clip clip4, clip clip5)
ShowFrameNumber(clip clip [, bool scroll, int offset, float x, float y, string font, int size, int text_color, int halo_color, float font_width, float font_angle])
ShowSMPTE(clip clip [, float fps, string offset, int offset_f, float x, float y, string font, int size, int text_color, int halo_color, float font_width, float font_angle])
ShowTime(clip clip [int offset_f, float x, float y, string font, int size, int text_color, int halo_color, float font_width, float font_angle])
StackHorizontal(clip clip1, clip clip2 [, ...])
StackVertical(clip clip1, clip clip2 [, ...])
Subtitle(clip clip, string text, float x, float y, int first_frame, int last_frame, string font, float size, int text_color, int halo_color, int align, int spc, int lsp, float font_width, float font_angle, bool interlaced)
Subtitle(clip clip, string text, float x, float y, int first_frame, int last_frame, string font, float size, int text_color, int halo_color, int align, int spc, int lsp, float font_width, float font_angle, bool interlaced, string font_filename, bool utf8) # AVS+
Tone([float length, float frequency, int samplerate, int channels, string type, float level])
Version()

#! properties		=======================================================
# Reserved frame property names
int _ChromaLocation
int _ColorRange
int _Primaries
int _Matrix
int _Transfer
int _FieldBased
float _AbsoluteTime
int _DurationNum
int _DurationDen
int _Combed(boolean)
int _Field
string _PictType
int _SARNum
int _SARDen
int _SceneChangeNext(boolean)
int _SceneChangePrev(boolean)

# http://avisynth.nl/index.php/Clip_properties
# Content Properties
bool clip.HasAudio
bool clip.HasVideo
# Video: Resolution
int clip.Width
int clip.Height
# Video: Framerate and Duration
int clip.FrameCount
float clip.FrameRate
int clip.FrameRateNumerator
int clip.FrameRateDenominator
# Video: Interlacing
bool clip.IsFieldBased
bool clip.IsFrameBased
bool clip.GetParity([int f])
# Video: Color Format
string clip.PixelType			# v2.60
bool clip.IsPlanar
bool clip.IsInterleaved
bool clip.IsRGB
bool clip.IsRGB24
bool clip.IsRGB32
bool clip.IsYUV
bool clip.IsYUY2
bool clip.IsY8					# v2.60
bool clip.IsYV12
bool clip.IsYV16				# v2.60
bool clip.IsYV24				# v2.60
bool clip.IsYV411				# v2.60
bool clip.Is420					# AVS+
bool clip.Is422					# AVS+
bool clip.Is444					# AVS+
bool clip.IsY					# AVS+
bool clip.IsYUVA				# AVS+
bool clip.IsRGB48				# AVS+
bool clip.IsRGB64				# AVS+
bool clip.IsPackedRGB			# AVS+
bool clip.IsPlanarRGB			# AVS+
bool clip.IsPlanarRGBA			# AVS+
bool clip.HasAlpha				# AVS+
int clip.ComponentSize			# AVS+
int clip.NumComponents			# AVS+
int clip.BitsPerComponent		# AVS+
# Audio
int clip.AudioRate
float clip.AudioDuration			# v2.60
int clip.AudioLength
float clip.AudioLengthF
string clip.AudioLengthS			# v2.60
int clip.AudioLengthLo([int m])	# v2.60
int clip.AudioLengthHi([int d])	# v2.60
int clip.AudioChannels
int clip.AudioBits
bool clip.IsAudioFloat
bool clip.IsAudioInt

#! plugins				=======================================================
# http://avisynth.nl/index.php/External_filters
# Source Filters
BestAudioSource(string source, int track, int adjustdelay, bool exactsamples, string varprefix)
DSS2(string filename, float fps, int cache, int seekthr, int preroll, int subsm, string lavs, string lavd, string lavf_path, string dvs_path, bool flipv, bool fliph, string pixel_type, int timeout)
FFmpegSource2(string source [, int vtrack, int atrack, bool cache, string cachefile, int fpsnum, int fpsden, int threads, string timecodes, int seekmode, bool overwrite, int width, int height, string resizer, string colorspace, int rffmode, int adjustdelay, bool utf8, string varprefix])
CoronaSequence(string file, int start, int stop, int sort, bool gapless, int textmode, int x, int y, string font, int size, int text_color, int halo_color, string expression)
RawSequence(string file, int start, int stop, int sort, bool gapless, int textmode, int x, int y, string font, int size, int text_color, int halo_color, string pixel_type, int width, int height)
JpegSource(string file, int rec, int channel, int length, float fps_num, int fps_den)
MPEG2Source(string d2v, int cpu, int idct, bool iPP, int moderate_h, int moderate_v, string cpu2, int upConv, bool iCC, bool i420, int info, bool showQ, bool fastMC)
LumaYV12(clip, int lumoff, float lumgain)
NicAC3Source(string, int channels, int drc)
NicDTSSource(string, int channels, int drc)
NicMPG123Source(string, bool normalize)
NicLPCMSource(string, int samplerate, int samplebits, int channels)
RaWavSource(string, int samplerate, int samplebits, int channels)
QTInput(string file, int color, int quality, int audio, int mode, string raw, int info, int dither, string vfw, float gamma, float vfrFPS)
QTInput(string file, string format, int quality, int datarate, int keyframe, string raw, string settings, int audio)
RawSource(string file, int width, int height, string pixel_type, int fpsnum, int fpsden, string index, bool show)
VSImport(string source, bool stacked, int index, int prefetch)
VSEval(string source, bool stacked, int index, bool utf8, int prefetch)

# Restoration Filters
# Anti-aliasing
maa2(clip c, int mask, bool chroma, float ss, int aa, int aac, int threads, int show)
santiag(clip c, int strh, int strv, string type, int nns, int aa, int aac, int threads, int nsize, int vcheck, int fw, int fh, bool halfres, string scaler_post, int maskt, string typeh, string typev)
TIsophote(clip, int iterations, float tStep, int type, bool chroma)
xaa(clip input, int ow, int oh, float ss, val ssw, val ssh, string mode, string uscl, string dscl, int csharp, float cstr, int mask, string mtype, float mthr, int chroma, string cplace, int nns, float eedimthr, float eediA, float eediB, float eediG, bool lsb_in, bool lsb, int threads)
# Chroma correction
caf(clip src, float rx, float ry, float bx, float by, int warp, bool clamp)
ChromaShift(clip, int C, int U, int V, int L, int R, int G, int B)
ChromaShiftSP(clip clp, float X, float Y)
ColorMatrix(clip, string mode, int source, int dest, int clamp, bool interlaced, bool inputFR, bool outputFR, bool hints, string d2v, bool debug, int threads, int thrdmthd, int opt)
FixChromaBleeding(clip input)
FixChromaBleedingMod(clip input, int cxShift, int cyShift, float thr, float strength, bool f, float opacity, bool n, bool xysh, bool Bic)
FixChromaticAberration(clip clip, val red, val green, val blue, val x, val y)
MoveChroma(clip, int Cb, int Cr)
ReInterpolate411(clip)
# Debanding
GradFun2DB(clip, float thr)
GradFun2DBmod(clip input, float thr, float thrC, int mode, float str, float strC, int temp, int adapt, string custom, bool mask, int radius, int range, bool show, int screenW, int screenH)
# Deblocking
Deblock(clip, int quant, int aOffset, int bOffset)
DeBlock_QED(clip clp, int quant1, int quant2, int aOff1, int bOff1, int aOff2, int bOff2, int uv)
DeblockPP7(clip, int qp, string mode, bool mmx)
FunkyDeBlock(clip c, int quant, int th, int radius, bool deblock, bool depump)
MDeblock(clip, string mode, int width, int height, int cwidth, int cheight, int leftcrop, int topcrop)
SmoothD(clip clip, int quant, int num_shift, int adaptive_shift, int zero_weight)
SmoothD2(clip clip, clip ZWmask, int quant, int num_shift, int Matrix, int Qtype, int ZW, int ZWce, int ZWlmDark, int ZWlmBright, int ncpu)
vsDeblockPP7(clip, float mthresh, int mode, int y, int u, int v)
# Dehaloing
abcxyz(clip clp, int rad, int ss)
BlindDeHalo3(clip clp, float rx, float ry, int strength, float lodamp, float hidamp, float sharpness, float tweaker, int PPmode, int PPlimit, bool interlaced)
DeHalo_alpha(clip clp, float rx, float ry, float darkstr, float brightstr, float lowsens, float highsens, float ss)
DeHaloHmod(clip input, int Radius, int Str, bool Maska, bool strong, int mode, int thr, string exdehalo, bool analog, bool dirty, bool smooth)
FineDehalo(clip src, float rx, float ry, int thmi, int thma, int thlimi, int thlima, float darkstr, float brightstr, int showmask, float contra, bool excl, float edgeproc)
YAHR(clip clp)
# Deringing & Mosquito Noise
aWarpSharpDering(clip source, int depth,int diffthresh,int lumathresh,int debug)
EdgeCleaner(clip c, int strength, bool rep, int rmode, int smode, bool hot)
HQDering(clip input, int strength, int overall, clip smoother)
HQDeringmod(clip input, clip smoothed, clip ringmask, int mrad, int msmooth, bool incedge, int mthr, int minp, int nrmode, int nrmodec, float sigma, float sigma2, int sbsize, int sosize, int sharp, int drrep, float thr, float elast, float darkthr, int Y, int U, int V, bool lsb_in, bool lsb, bool lsb_out, bool tv_range, int dither, bool show)
LazyDering(clip source, int depth,int diff,int thr)
MosquitoNR(clip, int strength, int restore, int radius, int threads)
# Deinterlacing
BWDIF(clip, int field, clip edeint, int opt)
EEDI2(clip, int mthresh, int lthresh, int vthresh, int estr, int dstr, int maxd, int field, int map, int nt, int pp)
FieldHint(clip, string ovr, bool show)
LeakKernelBob(clip, int order, int threshold, bool sharp, bool twoway, bool map, bool linked, bool debug, int forceCPU)
LeakKernelDeint(clip, int order, int threshold, bool sharp, bool twoway, bool map, bool linked, bool debug, int forceCPU)
nnedi3(clip, int field, bool dh, bool Y, bool U, bool V, int nsize, int nns, int qual, int etype, int pscrn, int threads, int opt, int fapprox)
nnedi3_rpow2(clip, int rfactor, int nsize, int nns, int qual, int etype, int pscrn, string cshift, int fwidth, int fheight, float ep0, float ep1, int threads, int opt, int fapprox)
nnedi3ocl(clip, int field, bool dh, bool Y, bool U, bool V, int nsize, int nns, int qual, int etype, int dw)
SangNom2(clip, int order, int aa, int aac, int threads, bool dh, bool luma, bool chroma, int opt)
TDeint(clip, int mode, int order, int field, int mthreshL, int mthreshC, int map, string ovr, int ovrDefault, int type, bool debug, int mtnmode, bool sharp, bool hints, PClip clip2, bool full, int cthresh, bool chroma, int MI, bool tryWeave, int link, bool denoise, int AP, int blockx, int blocky, int APType, PClip edeint, PClip emask, float blim, int metric, int expand, int slow, PClip emtn, bool tshints, int opt)
TelecideHints(clip, clip, clip cleanclip, clip unknownclip)
IsHinted(clip)
KillHints(clip)
TempGaussMC_beta2(clip clp, int tr0, int tr1, int tr2, int rep0, int rep1, int rep2, string EdiMode, int qual, int EEDI2maxd, int lossless, float sharpness, int Smode, int SLmode, int SLrad, float Sbb, float SVthin, int Sovs, int blocksize, int overlap, bool truemotion, bool globalmtn, int search, int searchparam, int pelsearch, int sharp, int lambda, int DCT, int pnew, int plevel, int lsad, int SCth1, int SCth2, int thSAD1, int thSAD2, float pel2hr, bool border, int draft, clip edeint)
Yadif(clip, int mode, int order, bool planar, int opt)
yadifmod(clip, int order, int field, int mode, clip edeint, int opt)
yadifmod2(clip, int order, int field, int mode, clip edeint, int opt)
# Duplicate Frame Detectors
ApparentFPS(clip clp, float DupeThresh, float FrameRate, int Samples, float ChromaWeight, string Prefix, bool Show, bool Verbose, bool Debug, int Mode, int Matrix, int BlkW, int BlkH, int oLapX, int oLapY)
Dup1(clip, float threshold, bool chroma, bool show, bool copy, int maxcopies, bool blend, bool debug, string log, int blksize)
DupStep(clip, float thresh = -1.0, int ifmcm = 0, int metric = 2, int blksize = -1, int planes = 2, int pco = -1, float Cweight = -1.0, bool ptp = true, string times = , float toff = 0.0, int show = 0, oclpi = -2, ocldi = 0)
ExactDedup(clip, bool firstpass, string dupinfo, string times, int maxdupcount, bool _keeplastframe, bool show)
# Fieldblending and Frameblending removal
Cdeblend(clip input, int omode, float bthresh, mthresh, float xr, float yr, bool fnr, clip dclip)
Cdeint(clip clp, clip bob, int bmode, float nlv, float hv)
DeBlend(clip Last, bool Debug)
Exblend(Clip,int mode,int pal,float lockthresh,float ithresh,int show,string ExBfile,bool ver,bool revip, string override,int lv,int dv,int disp,clip Dclip,int CompUB,bool Decimate,int DecompIx,int DecClpIx, bool Debug)
FixBlendIVTC(clip input, post, float bthresh, mthresh, bool sbd, clip dclip)
mrestore(clip clp, int numr, int denm, int mode, float bf, bool chroma, int dup, int cache, float rx, float ry, int mthr, clip dclip)
removeblend(clip, int threshold, float dthresh, int pixels, bool info, bool interlaced, bool tff, int mode, bool show, float cthresh, float bthresh, float othresh, bool decomb, int pt)
RestoreFPS(clip, float, float)
srestore(clip source, float frate, omode, float blocks, int mthresh, int bthresh, bool chroma, int cache, float rr, clip dclip)
# Film Damage correction
DePulse(clip, int h, int l, int d, bool debug)
descratch(clip, int mindif, int asym, int maxgap, int maxwidth, int minlen, int maxlen, float maxangle, int blurlen, int keep, int border, int modeY, int modeU, int modeV, int mindifUV, bool mark, int minwidth, int left, int right)
DeSpot(clip, int mthres, int mwidth, int mheight, int merode, bool interlaced, bool median, int p1, int p2, int pwidth, int pheight, bool ranked, int sign, int maxpts, int p1percent, int dilate, bool fitluma, int blur, int tsmooth, int show, int mark_v, bool show_chroma, bool motpn, int seg, bool color, int mscene, int minpts, clip extmask, bool planar, string outfile, bool mc, int spotmax1, int spotmax2)
deVCR(clip c, int threshold)
KillPulse(clip, int times, int motion, int complex, int complex2, int mode)
RestoreMotionBlocks(clip filtered, clip restore, clip neighbour, clip neighbour2, clip alternative, bool planar, bool show, bool debug, int gmthreshold, int mthreshold, int noise, int noisy, int dist, int tolerance, int dmode, int pthreshold, int cthreshold, bool grey)
# Frequency Interference removal
# IVTC & Decimation
AnimeIVTC(clip i, int mode, int aa, int precision, int killcomb, int cache, bool ifade, float sfthr, bool sfshow , bool chrfix , bool blend , bool normconv , int pattern , int pass, bool rendering , int bbob, int cbob, string edimode, int degrain, int omode , int cthresh , int blockx , int blocky , int MI , int tfmm , int pp , int metric , int micmatching , int i1, int i2, int e1, int e2, int e3, int p1, int p2 , bool dchr , bool palf , bool tcfv1 , bool nvfr , bool real30p , bool autoAssuf , int ediandnn , bool o3025cfr , int overlap, int pel, int search, bool nnedi3pel , string credconv , bool mode22 , clip ediext , string bob4p , clip extbob , string extbobf , bool repwithtdeint , float dark, int thin, int sharp, int smooth, bool stabilize, int tradius, int aapel, int aaov, int aablk, string aatype)
DecombUCF(clip s, float y1, float y2, float y3, float y4, float y5, float x1, float x2, float x3, float x4, float x5, float off_t, float off_b, int th_mode, float fd_thresh, float namax_thresh, int namax_diff, float nrt1y, float nrt2y, float nrt2x, float nrw, string nr, int chroma, int debug, string txt_file, int frame_cache)
FDecimate(clip, float rate, float threshold, bool protect, bool metrics, bool show, bool debug)
FDecimate2(clip, float rate, float threshold, bool metrics, bool show, bool debug, bool chroma)
IT(clip, int fps, int threshold, int pthreshold, string ref, bool blend, bool debug, string read, string write, string log, int dimode)
ivtc_txt60mc(clip src, int frame_ref, bool srcbob, bool draft)
MDEC2(clip c, float rate, bool show bool create, string dir, bool chroma, bool ver)
# Ghost Removal
Ghostbuster(clip, int offset, int strength)
LGhost(clip, int mode0, int shift0, int intensity0, ... ,int mode17, int shift17, int intensity17)
# Logo Removal
InpaintLogo(clip Clip, clip Mask, float Radius, float Sharpness, float PreBlur, float PostBlur, float ChromaWeight, float PreBlurSize, float PostBlurSize, bool ChromaTensor, float PixelAspect, int Steps)
DeblendLogo(clip Clip, clip Logo, clip Alpha)
AnalyzeLogo(clip Clip, clip Mask, bool ComputeAlpha, float DeviationWeight, float SubsamplingWeight)
DistanceFunction(clip Clip, float Scale, float PixelAspect)
DelogoHD(clip, string logofile, string logoname, int left, int top, int start, int end, int fadein, int fadeout, bool mono, int cutoff)
AddlogoHD(clip, string logofile, string logoname, int left, int top, int start, int end, int fadein, int fadeout, bool mono, int cutoff)
ExInpaint(clip, clip mask, int color, int dilate int xsize, int ysize, int radius, int steps)
InpaintFunc(clip clp, string mask, string loc, float AR, string mode,int speed, int pp, int ppmode, bool reset, float radius, float sharpness, float preblur, floatpostblur)
InpaintAssist(clip clp, string loc, string alignment)
rm_logo( clip clp, string logomask, string loc,float par, string mode,int percent,int deblendfalloff, int AlphaToRepair, int RepairRadius, float InpaintRadius, float InpaintSharpness, float InpaintPreBlur, float InpaintPostBlur, string cutsize, bool lmask, int pp, int cutwidth, int cutheight)
# Luma Equalization
Deflicker(clip, float percent, int lag, float noise, int scene, int lmin, int lmax, int border, bool info, bool debug, int opt)
EquLines(clip, int deltamax)
ReduceFlicker(clip, int strength, bool aggressive, bool grey, int opt, bool raccess, bool luma)
Vinverse(clip, float sstr int amnt, int uv, float scl)
Vinverse2(clip, float sstr int amnt, int uv, float scl)
# Rainbow & Dot Crawl Removal
Bifrost(clip, clip altclip, float luma_thresh, int variation, bool conservative_mask, bool interlaced, int blockx, int blocky)
cc(clip, int y1, int y2, int c1, int c2, bool interlaced, float yc, bool ylimit, bool climit)
Checkmate(clip, int thr, int max, int tthr2)
ChubbyRain(clip c, int th, int radius, bool show, bool interlaced)
ChubbyRain2(clip c, int th, int radius, bool show, int sft, bool interlaced)
DeCrawl(clip, int ythresht, int ythreshs, int cthresh, int temporal, int spatial, int spatialpasses)
DeCross(clip, int ThresholdY, int Noise, int Margin, bool Debug)
DeDot(clip, int luma2d, int lumaT, int chromaT1, int chromaT2)
DeRainbow(clip org, int thresh, bool interlaced)
DeRainbowYUY2(clip org, int thresh, bool interlaced)
DFMDeRainbow(clip org, int maskthresh, bool mask, bool interlaced)
LUTDeCrawl(clip input, int ythresh, int cthresh, int maxdiff, int scnchg, bool usemaxdiff, bool mask)
LUTDeRainbow(clip input, float cthresh, float ythresh, bool y, bool linkUV, bool mask)
mfRainbow(clip input, int scd, bool interlaced)
Rainbow_Smooth(clip orig, int radius, int lthresh, int hthresh)
SmartSSIQ(clip input, int strength, bool interlaced)
SSIQ(clip, int diameter, int strength, bool interlaced)
TComb(clip, int mode, int fthreshL, int fthreshC, int othreshL, int othreshC, bool map, float scthresh, bool debug, int opt)
YARK(clip c, int thr, int rad, int str, int scd, bool show)
ASTDR(clip input, int strength, int tempsoftth, int tempsoftrad, int tempsoftsc, float blstr, int tht, int FluxStv, int dcn, bool edgem, bool exmc, clip edgemprefil, bool nomask)
ASTDRmc(clip input, int strength, int tempsoftth, int tempsoftrad, int tempsoftsc, float blstr, int tht, int FluxStv, int dcn, bool edgem, int thSAD, clip prefil, bool chroma, clip edgemprefil, bool nomask)
# Stabilization
Depansafe( clip c, float dxmax, float dymax, float error, clip prefilter, bool info, string log)
Deshaker3D(clip, float fov, string log_file, int x, int y, int z)
Stab(clip clp, int range, int dxmax, int dymax, int mirror)
# Denoisers
AdaptiveMedian(clip, int sf, int ef, int maxgrid, bool yy, bool uu, bool vv)
Deathray(clip, float hY, float hUV, int tY, int tUV, float s, int x, bool l, bool c, bool z, bool b)
DeNoise(clip, int sf, int ef, int xgrid, int ygrid, bool getvar, bool clip, int lx, int ty, int rx, int by, int elx, int ety, int erx, int eby, int var, int evar, bool uv, bool usey, bool show)
DeSaltPepper(clip, string opt, int tol, bool uv, bool avg, int feedback)
ExtendedBilateral(bool sup, int preprocess, int estimator, int final, int kernelD, int kernelR, int diameterL, int diameterC, float sigmaDL, float sigmaDC, float sigmaRL, float sigmaRC, float cwL, float cwC, bool chroma)
F1Quiver(clip, int Array of values, bool morph, bool rescale, float gamma, int frad, bool uv)
TNLMeans(clip, int Ax, int Ay, int Az, int Sx, int Sy, int Bx, int By, bool ms, int rm, float a, float h, bool sse)
KNLMeansCL(clip, int d, int a, int s, float h, string channels, int wmode, float wref, clip rclip, string device_type, int device_id, int ocl_x, int ocl_y, int ocl_r, bool stacked, bool info)
# Spatial Denoisers
_2DCleanYUY2(clip, int interlaced, int thresholdY, int radiusX, int radiusY, int dmode, int thresholdU, int thresholdV)
DCTFilter8(clip, float, float, float, float, float, float, float, float, float x40, float x41, float x42, float x43, int chroma, int opt)
DCTFilter(clip, float, float, float, float, float, float, float, float, float x40, float x41, float x42, float x43, int chroma, int opt)
DCTFilter8D(clip, int diagonals_count, int x4, int chroma, int opt)
DCTFilterD(clip, int diagonals_count, int x4, int chroma, int opt)
DCTFilter4(clip, float, float, float, float, int chroma, int opt)
DCTFilter4D(clip, int diagonals_count, int chroma, int opt)
DCTFun4b(clip, float luma_thresh, float chroma_thresh)
DCTFun4c(clip, float luma_thresh, float chroma_thresh)
DCTFun5(clip, float luma_thresh, float chroma_thresh)
FrFun3b(clip, float T, float Tuv, int S)
FrFun3d(clip, float T, float Tuv, int S)
FrFun7(clip, float T, float Tuv, int S)
MiniDeen(clip, int radius, int thrY, int thrUV, int Y, int U, int V)
neo_vd(clip clip, float threshold, int method, int nsteps, float percent, int y, int u, int v, int opt)
SmoothUV(clip, int radius, int threshold, bool field)
SSHiQ(clip, int rY, int rC, int tY, int tC, bool HQY, bool HQC, bool field)
SmoothUV(clip, int radius, int threshold, bool interlaced)
SPresso(clip clp, int limit, int bias, int RGmode, int limitC, int biasC, int RGmodeC)
TBilateral(clip, int diameterL, int diameterC, float sDevL, float sDevC, float iDevL, float iDevC, float csL, float csC, bool d2, bool chroma, bool gui, clip ppClip, int kernS, int kernI, int resType)
UnDot(clip)
VagueDenoiser(clip, int threshold, int method, int nsteps, float chromaT, bool debug, bool interlaced, int wavelet, bool Wiener, float wratio, integer percent, clip auxclip)
VerticalCleaner(clip, int mode, bool planar)
vsMSmooth(clip, float threshold, float strength, bool mask, bool luma, bool chroma)
# Temporal Denoisers
Cnr2(clip, string mode, float scdthr, int ln, int lm, int un, int um, int vn, int vm, bool log, bool sceneChroma)
FluxSmoothT(clip, int temporal_threshold, bool luma, bool chroma, int opt)
FluxSmoothST(clip, int temporal_threshold, int spatial_threshold, bool luma, bool chroma, int opt)
TTempSmooth(clip, int maxr, int lthresh, int cthresh, int lmdiff, int cmdiff, int strength, float scthresh, bool fp, int vis_blur, bool debug, bool interlaced, PClip pfclip)

TTempSmoothF(clip, int maxr, int lthresh, int cthresh, float scthresh, bool fp, int vis_blur, bool debug, bool interlaced, PClip pfclip)
TemporalDegrain(clip input, clip denoise, bool GPU, int sigma, int bw, int bh, int pel, int blksize, int ov, int degrain, int limit int SAD1, int SAD2, int HQ)
vsTTempSmooth(clip, int maxr, int ythresh, int uthresh, int vthresh, int ymdiff, int umdiff, int vmdiff, int strength, float scthresh, bool fp, bool y, bool u, bool v, clip pfclip)
# Spatio-Temporal Denoisers
Deen(clip, string mode, int rad, int thrY, int thrUV, int tthY, int tthUV, float min, float scd, string fcf, bool borderfix)
dftTest(clip clip [, bool Y, bool U, bool V, int ftype, float sigma, float sigma2, float pmin, float pmax, int sbsize, int smode, int sosize, int tbsize, int tmode, int tosize, int swin, int twin, float sbeta, float tbeta, bool zmean, string sfile, string sfile2, string pminfile, string pmaxfile, float f0beta, string nfile, int threads, int opt, string nstring, string sstring, string ssx, string ssy, string sst, int dither, bool lsb, bool lsb_in, bool quiet])
DeGrainMedian(clip, int limitY, int limitUV, int mode, bool interlaced, bool norow)
FFT3DFilter(clip, float sigma, float beta, int plane, int bw, int bh, int bt, int ow, int oh, float kratio, float sharpen, float scutoff, float svr, float smin, float smax, bool measure, bool interlaced, int wintype, int pframe, int px, int py, bool pshow, float pcutoff, float pfactor, float sigma2, float sigma3, float sigma4, float degrid, float dehalo, float hr, float ht, int ncpu)
FFT3DGPU(clip, float sigma, float beta, int bw, int bh, int bt, float sharpen, int plane, int mode, int bordersize, int precision, bool NVPerf, float degrid, float scutoff, float svr, float smin, float smax, float kratio, int ow, int oh, int wintype , int interlaced, float sigma2, float sigma3, float sigma4, bool oldfft)
HQdn3d(clip, float ls, float cs, float lt, float ct, int restart)
MC_Spuds( clip clp, int frames, int strength, int blocksize, int overlap, int thsad, int dct, int ml, bool chro, bool postprocess, bool preprocess, bool aggressive,int debug, int thSCD1, int thSCD2, bool edgeclean, bool focus, bool removeblocks, bool starfield, bool anime, float ss_x, int sharpp, bool addnoise, int shadow_l, int shadow_h, float thStar, int limit, int pnew, int edm_lo, int edm_hi, bool flow, int search, bool colorbleed, int quant1, int quant2, int uv, bool temporal, int pel, int sharp, bool premax, int lumathres, int lambda, bool truemotion, int lsad, int plevel, bool mvglobal, float fs1, float fs2, float fs3, float fs4, bool prefast, int prefilter, bool gpu, bool premc, string mode)
MCTemporalDenoise( clip i, int radius, int sigma, bool twopass, bool useTTmpSm, int limit, int limit2, bool fixFFT3D, bool chroma, bool GPU, bool MT, int idx, bool interlaced, int sharp, bool adapt, int strength, int SHmode, int SHmethod, int Slimit, int Sovershoot, bool Tlimit, int Tovershoot, bool protect, int cutoff, int threshold, int maxdiff, bool AA, bool useEEDI2, float reduc, int maxd, int AAthr, int method, bool deblock, bool useQED, int quant1, int quant2, bool edgeclean, int ECrad, int ECthr, string ECmode, bool stabilize, int maxr, int TTstr, bool flat, float GFthr, int AGstr, int bias, int bwbh, int owoh, int blksize, int overlap, bool truemotion, bool safe, bool MVglobal, int bt, int ncpu, int precision, int mode, int thSAD, int thSAD2, int thSCD1, int thSCD2, int pel, int pelsearch, int search, int searchparam, int MVsharp, int DCT, clip p, string settings, bool show, int screenW, int screenH)
BackwardClense(clip, bool grey)
Clense(clip, clip previous, clip next, bool grey)
ForwardClense(clip, bool grey)
Repair(clip, clip, int mode, int modeU, int modeV, bool planar)
Repair(clip, clip, int mode, int modeU, int modeV, bool planar)
RemoveGrain(clip, int mode, int modeU, int modeV, bool planar)
VerticalCleaner(clip, int mode, int modeU, int modeV, bool planar)
TemporalRepair(clip, clip, int mode, int smooth, bool grey, bool planar, int opt)
STMedianFilter(clip, int, int, int, int)
STPresso(clip clp, int limit, int bias, int RGmode, int tthr, int tlimit, int tbias, int back)

# Adjustment Filters
# Averaging/Layering/Masking
Average(clip, float, clip, float, clip, float ...)
SoftLight(clip, int mode)
ClipBlend(clip, int delay)
ClipBlend16(clip, int delay)
ColorKeyMask(clip, string keymode, string keycolor, int gth, int grm, int ggm, int gbm, int contx, int conty, int cth, int crm, int cgm, int cbm, int pixv)
ColourMask(clip, int y, int u, int v, int lumathreshold, int chromathreshold)
ColourStabilise(clip, int y, int u, int v, int lumathreshold, int chromathreshold)
RGBColourStabilise(clip, int colour, int lumathreshold, int chromathreshold)
CombMask2(clip, int cthresh, int mthresh, bool chroma, bool expand, int metric, int opt)
MaskedMerge2(clip base, clip alt, clip mask, int MI, int blockx, int blocky, bool chroma, int opt)
IsCombed2(clip, int cthresh, int mthresh, int MI, int blockx, int blocky, int metric, int opt)
GraMaMa(clip, int mode, int a, int b, int rad, int rad2, bool binarize)
Median(clip, clip, clip, ..., bool chroma, int sync, int samples, bool debug)
Median(clip, clip, clip, ..., int low, int high, bool chroma, int sync, int samples, bool debug)
Median(clip, int radius, bool chroma, bool debug)
CombMask(clip, int thY1, int thY2, int Y, int U, int V, bool usemmx)
RAverageM(clip1, mask1, clip2, mask2, ... clipn,maskn, float bias, int y, int u, int v, int sse, bool lsb_in, bool lsb_out)
RAverageW(clip1, weight1, clip2, weight2, ... clipn, weightn, float bias, int y, int u, int v, int sse, bool lsb_in, bool lsb_out, int mode, int errlevel, bool silent)
RMerge(clip1, clip2, mask clip, int y, int u, int v, int sse, bool lsb_in, bool lsb_out, int mode)
RMerge(clip1, clip2, mask clip, float bias, int y, int u, int v, int sse, bool lsb_in, bool lsb_out, int mode)
TColorMask(clip, string colors, int tolerance, bool bt601, bool gray, int lutthr, bool mt)
TCombMask(clip, int athreshL, int athreshC, int mthreshL, int mthreshC, int edgethreshL, int edgethreshC,bool denoise, bool lcLinked, int map, int mtnChk, int ametric, int field, int low, int high, bool chkPrg, int athreshLP, int athreshCP, int mthreshLP, int mthreshCP, int edgethreshLP, int edgethreshCP, bool denoiseP, int mtnChkP, int ametricP, bool chroma, int MI, bool debug)
TempLinearApproximate(clip, int radius, int plane, bool inLsb, bool outLsb)
TMaskCleaner(clip, int length, int thresh, int fade)
TMM(clip, int mode, int order, int field, int length, int mtype, int ttype, int mtqL, int mthL, int mtqC, int mthC, int nt, int minthresh, int maxthresh, int cstr)
TMM2(clip, int mode, int order, int field, int length, int mtype, int ttype, int mtqL, int mthL, int mtqC, int mthC, int nt, int minthresh, int maxthresh, int cstr, int opt)
Unpremultiply(clip)
uu_mt_blend(clip C, clip D, float opacity, string mode, int cmode, bool showargs)
Watermark2(clip clip, clip watermark, int displace, int light, int depth, bool softEdge, string lightFrom)
mt_YRangeMask(clip , int min_y, int fade_min_y, int max_y, int fade_max_y, bool invert)
# Blurring
BucketMedian(clip, int radius, int thresh, int min, int max)
FastBlur(clip, float blur, float y_blur, int iterations, bool dither, bool gamma)
GBlur(clip, int size, float sd, bool u, bool v)
MedianBlur(clip, int radiusy, int radiusu, int radiusv)
MedianBlurTemporal(clip, int radiusy, int radiusu, int radiusv, int temporalradius)
AverageBlur(clip, int radY, int radC, int Y, int U, int V)
BinomialBlur(clip, float varY, float varC, int Y, int U, int V, bool useMMX)
GaussianBlur(clip, float varY, float varC, int border, bool integrate, int Y, int U, int V, int gfunc, int gfuncc, int pcr, int pcrc, int nthreads)
Unsharp(clip, float varY, float varC, float strength, int border, bool integrate, int Y, int U, int V, int gfunc, int gfuncc, int pcr, int pcrc, int nthreads)
# Borders and Cropping
BorderControl(clip, int YBB, int YBF, itn YBS, int YBSF, int YTB, int YTF, int YTS, int YTSF, int XLB, int XLF, int XLS, int XLSF, int XRB, int XRF int XRS, int XRSF)
ContinuityFixer(clip clip, int left, int top, int right, int bottom, int radius)
ReferenceFixerFixer(clip clip, clip ref, int left, int top, int right, int bottom, int radius)
FillBorders(clip, int left, int top, int right, int bottom, int mode, int y, int u, int v)
FillMargins(clip, int left, int top, int right, int bottom, int y, int u, int v)
FillMargins(clip, int, int, int, int)
AutoCrop(clip, int mode, int wMultOf, int hMultOf, int leftAdd, int topAdd, int rightAdd, int bottomAdd, int threshold, int samples, int samplestartframe, int sampleendframe, float aspect)
RoboCrop(clip, int Samples, float Thresh, bool Laced, int WMod, int HMod, int RLBT, bool Debug, float Ignore, int Matrix, int Baffle, bool ScaleAutoThreshRGB, bool ScaleAutoThreshYUV, int CropMode, bool Blank, bool BlankPC, bool Align, bool Show, string LogFn, bool LogAppend, float ATM, int Start, int End, int LeftAdd, int TopAdd, int RightAdd, int BotAdd, int LeftSkip, int TopSkip, int RightSkip, int BotSkip)
# Colourspace Conversion
PackedToPlanar(clip clip)
PackedToPlanar(clip clip, int plane)
PlanarToRGB32(clip base, clip alpha)
PlanarToRGB32(clip green, clip blue, clip red, clip alpha)
RGBToRGB(clip clip)
YV12To422(clip, bool interlaced, int itype, int cplace, bool lshift, bool yuy2, bool avx2, bool threads, float b, float c)
# Effects
AddGrain(clip, float var, float hcorr, float vcorr, float uvar, int seed, bool constant, bool sse2)
AddGrainC(clip, float var, float uvar, float hcorr, float vcorr, int seed, bool constant, bool sse2)
Colorize(clip, int color, bool scale)
Posterize(clip, int pbits)
Sepia(clip, int color, string mode)
Technicolor(clip, string channels)
EffectsMany(clip, int sf, int ef, int Effect_specific_parameters)
SpecificEffectName(clip, int sf, int ef, .......... effect_specific_parameters)
F3kGrain(clip input, int luma, int chroma, int mode, int temp, int adapt, float sigma, bool lsb)
Fingerprint(clip)
GNoise(clip, float sigma, int seed, bool color, bool temporal, bool info)
GrainFactory3(clip clp, int g1str, int g2str, int g3str, int g1shrp, int g2shrp, int g3shrp, float g1size, float g2size, float g3size, int g1tex, int g2tex, int g3tex, int temp_avg, int ontop_grain, int th1, int th2, int th3, int th4)
MPlayerNoise(clip, int strength_y, int strength_uv)
NoiseGenerator(clip, bool Gaussian, int Amount, bool LumaOnly)
Scanlines(clip, int STRENGTH)
StaticNoiseC(clip, int var, int seed, bool Y, bool UV)
TurnsTile(clip c, clip tilesheet, int tilew, int tileh, int res, int mode, string levels, int lotile, int hitile, bool interlaced)
# Field Order
# Frame Rate Conversion
Convert60ito24p(clip video, int mode, int offset)
FixFPS(clip c, string times, int frames, int div, int mul)
FrameDbl(clip, int, int)
FrameRateConverter(clip C, int NewNum, int NewDen, string Preset, int BlkSize, int BlkSizeV, bool FrameDouble, string Output, bool Debug, clip Prefilter, int MaskThr, int MaskOcc, int SkipThr, int BlendOver, int SkipOver, bool Stp, int Dct, int DctRe, int BlendRatio)
InterpolateDoubles(clip C, float Thr, bool Show, string Preset, int BlkSize, int BlkSizeV, int MaskThr, int MaskOcc, int SkipThr, int BlendOver, int SkipOver, bool Stp, int Dct, int DctRe)
StripeMask(clip, int blksize, int blksizev, int overlap, int overlapv, int thr, int Comp, int CompV, int str, int strf, bool lines)
ContinuousMask(clip, int radius)
ConvertFpsLimit(clip, int numerator, int denominator, int zone, int vbi, int ratio)
ConditionalFilterMT(clip testclip, clip source1, clip source2, string expression1, string expression2, string expression3, bool show)
salFPS3(clip input, float FPS, int mode, int protection, int protection2, int iterate, int reset, int initialise)
SickJumps(clip c, int first_frame, int last_frame, float start_multiplier, float full_multiplier, float up_seconds, float down_seconds, string script_variable, float end_multiplier)
TimecodeFPS(clip, string timecodes, int fpsnum, int fpsden, bool report, float threshone, float threshmore, bool start)
VFRtoCFR(clip, string times, int numfps, int denfps, bool dropped)
VfrToCfr(clip, string timecodes, int fpsnum, int fpsden, bool debug)
YFRC(clip clp_Input, int BlockH, int BlockV, int OverlayType, int MaskExpand)
# Frame Replacement/Range Processing
ApplyEvery(clip c, int period, string thunk)
DeleteEvery(clip c, int period [, int offset1 [, int offset2 [, ...]]])
DeleteFrames(clip c, int frame1 [, int frame2 [, ...]])
InterleaveEvery(clip baseClip, clip shimClip, int period [, int offset1 [, int offset2 [, ...]]])
RepeatEveryFrame(clip c, int count)
LengthenClip(clip c, int minLength, bool copy, int color)
WhileEval(string condition, string loopBody)
ClipClop(clip source, clip R1, ... , clip R255, string Cmd,string SCmd, bool Show, bool Ver,int dv, bool NoErr,string Nickname,bool Purge)
FrameRepeat(Clip,int default,string Cmd, bool Show, bool Ver)
FrameSel(Clip, int F1, ... , int Fn, string scmd,string cmd, bool show, bool ver,bool reject,bool ordered, bool debug, int Extract)
RemapFrames(clip baseClip, string filename, string mappings, clip sourceClip)
RemapFramesSimple(clip c, string filename, string mappings)
ReplaceFramesSimple(clip baseClip, clip sourceClip, string filename, string mappings)
Prune(Clip R0, clip R1, ... , clip R255, string Cmd,string SCmd, bool Show, bool Ver,int dv, bool NoErr, float Fade, bool FadeIn, bool FadeSplice, bool FadeOut, String Nickname)
# Levels and Chroma
Cube(clip, string cube, int cpu, bool fullrange)
ChanMix(clip, float r, float g, float b)
ChannelMixer(clip, float RR, float RG, float RB, float GR, float GG, float GB, float BR, float BG, float BB)
ColorBalance(clip, float rs, float gs, float bs, float rm, float gm, float bm, float rh, float gh, float bh, bool keep_luma, bool clone_gimp, bool highcolor)
ColourWarp(clip, integers, clips, integers, bool debug, bool invert)
DGHDRtoSDR(clip, int white, int black, float gamma, float hue, float r, float g, float b, float tm, float roll, bool fulldepth, string impl, string mode)
DGPQtoHLG(clip, float light, string impl)
DGReinhard(clip, float contrast, float bright)
DGHable(clip, float exposure, float a, float b, float c, float d, float e, float f, float w)
GiCoCu(clip, string f, bool alpha, bool photoshop, bool hsv)
ApplyGradationCurves(clip, string lumaPoints, string redPoints, string greenPoints, string bluePoints, string curvesFile, string plotsPath)
HDRAGC(clip, float max_gain, float min_gain, float coef_gain, float max_sat, float min_sat, float coef_sat, int avg_window, int response, int debug, int mode, int protect, int passes, int shift, bool shadows, int shift_u, int shift_v, float corrector, float reducer, int corrector_mode, int avg_lum, float black_clip, int freezer)
TweakHist(clip, int*, int type, clip mclip, int mf, int limit, int wsize, int rgb)
HistogramRGBLevels(clip input, bool range, float factor)
HistogramCMYLevels(clip input, bool range, float factor)
HSVAdjust(clip, float hue, float sat, float bright, float cont, string brightness)
HSLAdjust(clip, float hue, float sat, float bright, float cont)

HSIAdjust(clip, float hue, float sat, float bright, float cont)
Hue(clip, float hue, float sat, float intensity, string channels)
MatchHistogram(clip clip1 , clip clip2, clip clip3, bool raw, bool show, bool debug, int smoothing_window, bool y, bool u, bool v)
OutRange(clip c, float thr, int plane, string log)
SelectiveColour(clip, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, bool)
ShowOverRange(clip)
TweakColor(clip, float hue, float sat, float bright, float cont, bool coring, int startHue, int endHue, int maxSat, int minSat, int smooth)
VideoScope(clip, string DrawMode, bool TickMarks, string HistoTypeSide, string HistoTypeBottom, string FrameType)
WhiteBalance(clip, int r1, int g1, int b1, int y1, int u1, int v1, int r2, int g2, int b2, int y2, int u2, int v2, bool interlaced, bool split)
# Line Darkening
Hysteria(clip orig, float strength, bool usemask, int lowthresh, int highthresh, int luma_cap, int maxchg, int minchg, bool showmask)
mfToon2(clip orig, int twidth, int theight, int ssw, int ssh, int xstren, int xthresh, bool cwarp, bool sharpen, int strength, float wdepth, int wblur, float wthresh, int drange, float dboost, int dlimit, bool debug, bool doutput, string dclip, bool show, int scolor)
mfToonLite(clip orig, int twidth, int theight, int strength, int dstren, int drange, float dboost, int dlimit, string mask)
proToon(clip input, int strength, int luma_cap, int threshold, int thinning, bool sharpen, bool mask, bool show, string showclip, int ssw, int ssh, int xstren, int xthresh)
SuperToon(clip input, float power, int mode, int Nthr, int Nstr, int Ncap, int Lthr, int Hthr, int Lcap, int cont, bool show)
Toon(clip, float strength)
ToonLite(clip, float strength)
vmToon(clip input, int strength, int luma_cap, int threshold, int thinning, bool sharpen, bool mask, bool show, string showclip, int ssw, int ssh, int xstren, int xthresh)
# Resizers
Anime4KCPP(clip src, int passes, int pushColorCount, float strengthColor, float strengthGradient, int zoomFactor, bool ACNet, bool GPUMode, bool HDN, int HDNlevel, int platformID, int deviceID)
listGPUs()
z_ConvertFormat(clip clip, int width, int height, str pixel_type, string colorspace_op, string chromaloc_op, bool interlaced, float src_left, float src_top, float src_width, float src_height, string resample_filter, float filter_param_a, float filter_param_b, string resample_filter_uv, float filter_param_a_uv, float filter_param_b_uv, string dither_type, string cpu_type, float nominal_luminance, bool approximate_gamma)
z_PointResize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, string chromaloc_op, string dither)
z_BilinearResize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, string chromaloc_op, string dither)
z_BicubicResize(clip, float b, float c, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, string chromaloc_op, string dither)
z_LanczosResize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, int taps, string chromaloc_op, string dither)
z_Lanczos4Resize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, int taps, string chromaloc_op, string dither)
z_Spline16Resize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, string chromaloc_op, string dither)
z_Spline36Resize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, string chromaloc_op, string dither)
z_Spline64Resize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, string chromaloc_op, string dither)
AreaResize(clip, int width, int height)
Debicubic(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, bool lsb_inout, float b, float c)
DebicubicY(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, bool lsb_inout, float b, float c)
Debilinear(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, bool lsb_inout)

DebilinearY(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, bool lsb_inout)
FCBI(clip, bool ed, int tm)
JincResize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, int quant_x, int quant_y, int tap, float blur, int opt)
Jinc36Resize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, int quant_x, int quant_y)
Jinc64Resize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, int quant_x, int quant_y)
Jinc144Resize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, int quant_x, int quant_y)
Jinc256Resize(clip, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, int quant_x, int quant_y)
Resize8(clip input, int target_width, int target_height, float src_left, float src_top, float src_width, float src_height, string kernel, string kernel_c, float a1, float a2, float a1_c, float a2_c, val noring, val noring_c, string cplace, bool Y, bool U, bool V, bool Alpha)
SimpleResize(clip, int, int)
InterlacedResize(clip, int, int)
WarpedResize(clip, int, int, float, float)
InterlacedWarpedResize(clip, int, int, float, float)
# Sharpeners
ASharp(clip, float T, float D, float B, bool hqbf)
CAS(clip, float sharpness, int y, int u, int v, int opt)
FineSharp(clip c, int mode, float sstr, float cstr, float xstr, float lstr, float pstr, float ldmp)
LimitedSharpenFaster(clip clp, float ss_x, float ss_y, int dest_x, int dest_y, int Smode , int strength, int radius, int Lmode, bool wide, int overshoot, int undershoot, int soft, int edgemode, bool special, int exborder)
LSFmod(clip input, int strength, int Smode, int Smethod, int kernel, string preblur, bool secure, clip source, int Szrp, int Spwr, int SdmpLo, int SdmpHi, int Lmode, int overshoot, int undershoot, int overshoot2, int undershoot2, int soft, bool soothe, int keep, int edgemode, bool edgemaskHQ, float ss_x, float ss_y, int dest_x, int dest_y, bool show, int screenW, int screenH, string defaults)
MSharpen(clip, int threshold, int strength, bool highq, bool mask)
NonlinUSM(clip o, float z, float pow, float str, float rad, float ldmp)
pSharpen(clip clp, int strength, int threshold, float ss_x, float ss_y, int dest_x, int dest_y)
ReCon(clip c, float str, float strv, float strh, float rad, float radh, float radv, int lmode, int thresh)
SeeSaw(clip clp, clip denoised, int NRlimit,int NRlimit2, float Sstr, int Slimit, float Spower, float SdampLo, float SdampHi, float Szp, float bias, int Smode, int sootheT, int sootheS, float ssx, float ssy)
SSSharp(clip c, float rad, bool ssw, float strength, int iter, bool ss, int denoise)
TUnsharp(clip, int strength, int thresholdL, int thresholdU, int type, int map, int lim, int radius, bool gui)
UnFilter(clip, int, int)
UnsharpHQ(clip, int threshold, float sharpstr, float smooth, bool show)
vsMSharpen(clip, float threshold, float strength, bool mask, bool luma, bool chroma)
# Rotation/Shear/Skew/Perspective
Barrel(clip, float a, float b, float c, bool pin, float vh, bool test, float ea, float eb, float ec, int nsteps, Int frame, Int color, bool ind, bool ypin, float ya, float yb, float yc, float yea, float yeb, float yec)
Defish(clip, float fov, float scale, float aspect, string direction, string scaling, float a, float b, float c, bool pin, int threads)
Deskew(clip, clip, string resize, float ltopx, float ltopy, float lbotx, float lboty, float rtopx, float rtopy, float rbotx, float rboty, float leftx, float rightx, float ytop, float ybot, float eltopx, float eltopy, float elbotx, float elboty, float ertopx, float ertopy, float erbotx, float erboty, float eleftx, float erightx, float eytop, float eybot)
Skew(clip, clip, string resize, float ltopx, float ltopy, float lbotx, float lboty, float rtopx, float rtopy, float rbotx, float rboty, float leftx, float rightx, float ytop, float ybot, float eltopx, float eltopy, float elbotx, float elboty, float ertopx, float ertopy, float erbotx, float erboty, float eleftx, float erightx, float eytop, float eybot)
Rotate(clip, float angle, int color, int start, int end, float endangle, int width, int height, float aspect)
HShear(clip, float angle, int color, int start, int end, float endangle, int width, int height, float aspect)
VShear(clip, float angle, int color, int start, int end, float endangle, int width, int height, float aspect)
Zoom(clip, string srcx, string srcy, string dstx, string dsty, string factorX, string factorY, string angle, int width, int height, bool show)
# Subtitling
AssRender(clip, string file, string vfr, int hinting, float scale, float line_spacing, float dar, float sar, int top, int bottom, int left, int right, string charset, int debuglevel, string fontdir, string srt_font, string colorspace)
FreeSub(clip, string text, int x, int y, int first_frame, int last_frame, string font, int size, int text_color, int halo_color, int align, int lsp, int font_width, int halo_width, int halo_height)
SubtitleEx(clip, string, int x, int y, int firstframe, int lastframe, string font, string effects, int size, int textcolor, int halocolor, int spc, int linemargin)
SupTitle(clip, string filename, bool forcedOnly, bool swapCbCr, bool relocate, string relocOffset)
TextSubMod(clip, string file, int charset, float fps, string vfr)
VobSub(clip, string)
MaskSubMod(string file, int width, int height, float fps, int length, int charset, string vfr)
TextSubModSwapUV(bool)
TextSub(clip, string file, int charset, float fps, string vfr)
VobSub(clip, string)
MaskSub(string file, int width, int height, float fps, int length, int charset, string vfr, string pixel_type)
TextSubSwapUV(bool)
# Transitions
Transition(clip, clip clip2, int frames, string pattern, int smooth, int a_offset, int a_frames)
Transition(clip, clip, string Effect, int PARAM, int CX, int CY)

# Debugging/Diagnostic Filters
Butteraugli(clip clip1, clip clip2, float hf_asymmetry, bool heatmap, bool linput)
ColorDiff(clip, int c1, int c2, int c3, float w1, float w2, float w3, int threshold, string colorspace)
DumpPixelValues(clip, string, string, string filename)
FFTSpectrum(clip, bool grid)
Grid(clip, int sf, int ef, int lineint, int bold, int vbold, int color, int bcolor, int vbcolor, bool grid, bool axis)
Info2(clip, int size, string font, int background, bool verbose, bool glyph)
MaxCLLFind(clip, int maxFallAlgorithm)
PixelScope(clip, int scale, bool hex, int darken, bool grid, bool interlaced, string matrix, bool nocrop, bool right)
SSIM(clip, clip, string, string, int lumimask, int scaled)
SSIM_FRAME(clip, clip)
SSIM_Y_FRAME(clip, clip)
SSIM_U_FRAME(clip, clip)
SSIM_V_FRAME(clip, clip)
ShowPixelValues(clip, int Left, int Top, bool ShowValues, bool ShowArea, int Bright, int Dim, int Threshold, bool ShowPixels)
VMAF(clip reference, clip distorted, int model, string log_path, int log_fmt, bool ssim, bool ms_ssim, int pool, bool ci)

# Edge Detection
aSobel(clip, int thresh, int chroma)
mt_edge(clip, string mode, int thY1, int thY2, int thC1, int thC2, int Y, int U, int V, string chroma, int offX, int offY int w, int h)
scharr(clip c)
tcanny(clip, float sigma, float t_h, float t_l, int nms, int mode, int plane, float gmmax)
TCannyMod(clip, int mode, float sigma, float t_h, float t_l, bool sobel, int chroma, float gmmax, int opt)
TEdgeMask(clip, int Y, int U, int V, float threshY, float threshU, float threshV, int link, int type, int valOn, int valOff, bool preBlur)
TEMmod(clip, float threshY, float threshC, int type, int link, int chroma, bool invert, bool preblur, float scale)

# Export Filters
# Import Filters
# Meta-Filters
MT(clip clip, string filter, int threads, int overlap, bool splitvertical)
MTi(clip clip, string filter)
MTsource(string filter, int delta, int threads, int max_fetch)
GetMTMode(bool threads)
SetMTmode(int mode,int threads)

# Multipurpose Filters
ConvertToShader(clip, int Precision, bool lsb, bool planar, int opt)
ConvertFromShader(clip, int Precision, string Format, bool lsb, int opt)
Shader(clip, string Path, string EntryPoint, string ShaderModel, string Param0, ..., string Param8, clip Clip1, ..., clip Clip9, int Output, int Width, int Height, int Precision, string Defines)
ExecuteShader(clip, clip Clip1-9, int Clip1Precision, ..., int Clip9Precision, int Precision, int OutputPrecision, bool PlanarOut, int Engines, bool Resource)
cl_expr(clip, string expr, string yExpr, string uExpr, string vExpr, int Y, int U, int V, string chroma, bool lsb)
cl_exprxy(clip, clip, string expr, string yExpr, string uExpr, string vExpr, int Y, int U, int V, string chroma, bool lsb)
cl_exprxyz(clip, clip, clip, string expr, string yExpr, string uExpr, string vExpr, int Y, int U, int V, string chroma, bool lsb)

# Scene Change Detection
MSCDetection(clip, clip, int Ysc, int thSCD1, int thSCD2, bool isse)
SCXvid(clip, string log)
SCXvidMask(clip, string path, int offset, bool strict)

# Support filters
ApplyInterlacedFilter(clip v1, string filter)
AvsMotion(clip c, string file, int offset, bool mirror, int pad_color)
CallCmd(clip, string Command, string Frames,string Insert,int Digits,bool Once, int Offset, bool Hide,bool Debug, string Open, string Close, int Synchronous)
FileSize(string)
GetProgramName2(bool full_path)
LoadDLL(string)

# Deepcolor Filters
# 3D Filters
# Libraries
# Audio Filters
AddAudio(clip v1)
AudioGraph(clip, int frames_either_side, int graph_scale, int middle_colour, int side_colour)
ExpotencialLimiter(clip, float factor)
LinearLimiter(clip, float factor)
WavGainLimiter(clip, float factor)
ExpotencialLimiter(clip, float curve)
AudioTimeline(clip, int frames, float mixdown, int height, int dBHeadroom, int dBRange, int loFreq)
Befa(clip, float sec, int freq, int bandwidth, int freq2, int bandwidth2, int dftsize, int fade, int window, int mode, int show, int scale, int plevel)
AudioMin(clip, int channel)
AudioMax(clip, int channel)
AudioRMS(clip, int channel)
Normalize2(clip, string cache_file, float targetPeak)
SoxFilter(clip, string effect1 [, string effect 2, string effect 3...])
Spectrogram(clip, int width, int height, bool transpose)
ViewAudio(clip, int frames, int top, int height, bool fill, int channel)
CacheAudio(clip, int MaxFrame, int BlockFrame)
Waveform(clip, int window, float height, bool under, float zoom, bool marks)

# As Yet Unclassified
OverlayConfig(float minOverlayArea, float minSourceArea, float aspectRatio1, float aspectRatio2, float angle1, float angle2, int minSampleArea, int requiredSampleArea, float maxSampleDifference, int subpixel, float scaleBase, int branches, float acceptableDiff, int correction, int minX, int maxX, int minY, int maxY, int minArea, int maxArea, bool fixedAspectRatio, bool debug)
OverlayEngine(clip, clip, string statFile, int backwardFrames, int forwardFrames, clip sourceMask, clip overlayMask, float maxDiff, float maxDiffIncrease, float maxDeviation, bool stabilize, clip configs, string downsize, string upsize, string rotate, bool editor, string mode, bool debug)
OverlayRender(clip, clip, clip, clip sourceMask, clip overlayMask, bool lumaOnly, int width, int height, int gradient, int noise, bool dynamicNoise, bool mode, float opacity, int colorAdjust, string matrix, string upsize, string downsize, string rotate, bool debug)
OverlayCompare(clip, clip, clip, string sourceText, string overlayText, int sourceColor, int overlayColor, int borderSize, float opacity, int width, int height, bool info)
ColorRangeMask(clip, int low, int high)
ffdshow(clip, string preset, string options)
ffdshowAudio(clip, string preset, string options)
SegmentedAmp(clip, int sh, int sm, bool c4, bool useclip, clip sclip, int smu, int smv)
Soothe(clip sharp, clip orig, int keep)
