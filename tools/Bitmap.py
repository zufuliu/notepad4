import os.path
import struct
from enum import IntEnum
from PIL import Image

__all__ = ['Bitmap', 'Image', 'ResizeMethod', 'QuantizeMethod']

# https://en.wikipedia.org/wiki/BMP_file_format
# https://learn.microsoft.com/en-us/windows/win32/gdi/bitmaps
class BitmapFileHeader:
	StructureSize = 14

	def __init__(self):
		self.size = 0
		self.reserved1 = 0
		self.reserved2 = 0
		self.offset = 54

	def read(self, fd):
		magic = fd.read(2)
		assert magic == b'BM'
		self.size = struct.unpack('<I', fd.read(4))[0]
		self.reserved1, self.reserved2 = struct.unpack('<HH', fd.read(2*2))
		self.offset = struct.unpack('<I', fd.read(4))[0]
		assert self.offset >= 54, self.offset

	def write(self, fd):
		fd.write(b'BM')
		fd.write(struct.pack('<I', self.size))
		fd.write(struct.pack('<HH', self.reserved1, self.reserved2))
		fd.write(struct.pack('<I', self.offset))

	def __str__(self):
		return f'''BitmapFileHeader {{
	size: {self.size :08X} {self.size}
	reserved: {self.reserved1 :04X} {self.reserved2 :04X}
	offset: {self.offset :08X} {self.offset}
}}'''

_InchesPerMetre = 0.0254
_TransparentColor = (0, 0, 0, 0)
_SupportedColorDepth = (1, 4, 8, 24, 32)

def _find_color_index_naive(table, color):
	result = 0
	diff = 1 << 18 # 4*255*255
	red, green, blue, alpha = color
	for index, item in enumerate(table):
		R, G, B, A = item
		distance = (red - R)**2 + (green - G)**2 + (blue - B)**2 + (alpha - A)**2
		if distance < diff:
			diff = distance
			result = index
	return result

class ResizeMethod(IntEnum):
	Nearest = Image.Resampling.NEAREST
	Bilinear = Image.Resampling.BILINEAR
	Bicubic = Image.Resampling.BICUBIC
	Lanczos = Image.Resampling.LANCZOS
	Box = Image.Resampling.BOX
	Hamming = Image.Resampling.HAMMING

class QuantizeMethod(IntEnum):
	MedianCut = Image.Quantize.MEDIANCUT
	MaxCoverage = Image.Quantize.MAXCOVERAGE
	FastOctree = Image.Quantize.FASTOCTREE
	LibImageQuant = Image.Quantize.LIBIMAGEQUANT
	Naive = 100
	PngQuant = 101
	ImageMagick = 102

class CompressionMethod(IntEnum):
	BI_RGB = 0
	BI_RLE8 = 1
	BI_RLE4 = 2
	BI_BITFIELDS = 3
	BI_JPEG = 4
	BI_PNG = 5
	BI_ALPHABITFIELDS = 6
	BI_CMYK = 11
	BI_CMYKRLE8 = 12
	BI_CMYKRLE4 = 13

	@staticmethod
	def getName(value):
		try:
			return CompressionMethod(value).name
		except ValueError:
			return f'Unknown-{value}'

class BitmapInfoHeader:
	StructureSize = 40

	def __init__(self, width=0, height=0, bitsPerPixel=32):
		self.width = width
		self.height = height
		self.planes = 1
		self.bitsPerPixel = bitsPerPixel
		self.compression = int(CompressionMethod.BI_RGB)
		self.sizeImage = 0
		self.resolution = 96
		self.colorUsed = 0
		self.colorImportant = 0

	def read(self, fd):
		magic, self.width, self.height = struct.unpack('<III', fd.read(4*3))
		assert magic == BitmapInfoHeader.StructureSize
		self.planes, self.bitsPerPixel = struct.unpack('<HH', fd.read(2*2))
		assert self.planes == 1
		assert self.bitsPerPixel in _SupportedColorDepth, self.bitsPerPixel
		self.compression, self.sizeImage = struct.unpack('<II', fd.read(4*2))
		assert self.compression == CompressionMethod.BI_RGB
		assert self.sizeImage == 0 or self.sizeImage == self.height*self.rowSize
		self._resolutionX, self._resolutionY = struct.unpack('<II', fd.read(4*2))
		self.colorUsed, self.colorImportant = struct.unpack('<II', fd.read(4*2))

	def write(self, fd):
		fd.write(struct.pack('<III', BitmapInfoHeader.StructureSize, self.width, self.height))
		fd.write(struct.pack('<HH', self.planes, self.bitsPerPixel))
		fd.write(struct.pack('<II', self.compression, self.sizeImage))
		fd.write(struct.pack('<II', self._resolutionX, self._resolutionY))
		fd.write(struct.pack('<II', self.colorUsed, self.colorImportant))

	@property
	def size(self):
		return (self.width, self.height)

	@property
	def resolution(self):
		return (round(self._resolutionX * _InchesPerMetre), round(self._resolutionY * _InchesPerMetre))

	@resolution.setter
	def resolution(self, value):
		if isinstance(value, (int, float)):
			x, y = value, value
		else:
			x, y = value
		self._resolutionX = round(x / _InchesPerMetre)
		self._resolutionY = round(y / _InchesPerMetre)

	@property
	def rowSize(self):
		value = self.bitsPerPixel*self.width
		# 4 byte aligned
		aligned = (value + 31) & ~31
		return aligned >> 3

	@property
	def rowPadding(self):
		value = self.bitsPerPixel*self.width
		# 4 byte aligned
		aligned = (value + 31) & ~31
		return (aligned - value) >> 3

	def __str__(self):
		resolution = self.resolution
		return f'''BitmapInfoHeader {{
	width: {self.width :08X} {self.width}
	height: {self.height :08X} {self.height}
	planes: {self.planes :04X} {self.planes}
	bitsPerPixel: {self.bitsPerPixel :04X} {self.bitsPerPixel}
	compression: {self.compression :08X} {self.compression} {CompressionMethod.getName(self.compression)}
	sizeImage: {self.sizeImage :08X} {self.sizeImage}
	resolutionX: {self._resolutionX :08X} {self._resolutionX} {resolution[0]}
	resolutionY: {self._resolutionY :08X} {self._resolutionY} {resolution[1]}
	colorUsed: {self.colorUsed :08X} {self.colorUsed}
	colorImportant: {self.colorImportant :08X} {self.colorImportant}
}}'''

class Bitmap:
	def __init__(self, width=0, height=0, bitsPerPixel=32):
		assert bitsPerPixel in _SupportedColorDepth, bitsPerPixel
		self.fileHeader = BitmapFileHeader()
		self.infoHeader = BitmapInfoHeader(width, height, bitsPerPixel)
		self.rows = [] # RGBA tuple
		self.palette = None
		self.data = None

		if width and height:
			for y in range(height):
				row = [_TransparentColor] * width
				self.rows.append(row)

	def read(self, fd):
		start = fd.tell()
		self.fileHeader.read(fd)
		self.infoHeader.read(fd)
		self.palette = None
		# color palette after header
		if self.infoHeader.bitsPerPixel < 24:
			paletteSize = (4 << self.infoHeader.bitsPerPixel)
			self.palette = fd.read(paletteSize)

		current = fd.tell()
		# enable reading from stream
		offset = self.fileHeader.offset - (current - start)
		if offset != 0:
			fd.seek(offset, os.SEEK_CUR)
		# infoHeader.sizeImage maybe zero
		sizeImage = self.height * self.rowSize
		self.data = fd.read(sizeImage)
		assert len(self.data) == sizeImage
		self.decode()

	def write(self, fd):
		self.encode()
		self.fileHeader.write(fd)
		self.infoHeader.write(fd)
		if self.palette:
			fd.write(self.palette)
		fd.write(self.data)

	def decode(self):
		getattr(self, f'_decode_{self.bitsPerPixel}bit')()

	def encode(self):
		getattr(self, f'_encode_{self.bitsPerPixel}bit')()

	def _set_data(self, buf, palette=None):
		sizeImage = len(buf)
		assert sizeImage == self.height*self.rowSize
		if False and self.data:
			count = 0
			for i, value in enumerate(self.data):
				if value != buf[i]:
					count += 1
			print('diff color:', count)

		bitsPerPixel = self.bitsPerPixel
		if palette:
			paletteSize = len(palette)
			self.palette = bytes(palette)
			assert paletteSize == (4 << bitsPerPixel), (bitsPerPixel, paletteSize)
		else:
			paletteSize = 0
			self.palette = None
			assert bitsPerPixel >= 24, bitsPerPixel

		offset = paletteSize + BitmapFileHeader.StructureSize + BitmapInfoHeader.StructureSize
		self.infoHeader.sizeImage = sizeImage
		self.data = bytes(buf)
		self.fileHeader.offset = offset
		self.fileHeader.size = sizeImage + offset

	def _decode_32bit(self):
		width, height = self.size
		self.rows.clear()

		offset = 0
		buf = self.data
		rows = []
		for y in range(height):
			row = []
			for x in range(width):
				blue = buf[offset]
				green = buf[offset + 1]
				red = buf[offset + 2]
				alpha = buf[offset + 3]
				offset += 4
				row.append((red, green, blue, alpha))
			rows.append(row)
		self.rows.extend(reversed(rows))

	def _encode_32bit(self):
		buf = []
		for row in reversed(self.rows):
			for red, green, blue, alpha in row:
				buf.append(blue)
				buf.append(green)
				buf.append(red)
				buf.append(alpha)
		self._set_data(buf)

	def _decode_24bit(self):
		width, height = self.size
		self.rows.clear()

		offset = 0
		buf = self.data
		padding = self.rowPadding
		rows = []
		for y in range(height):
			row = []
			for x in range(width):
				blue = buf[offset]
				green = buf[offset + 1]
				red = buf[offset + 2]
				offset += 3
				row.append((red, green, blue, 0xFF))

			offset += padding
			rows.append(row)
		self.rows.extend(reversed(rows))

	def _encode_24bit(self):
		padding = [0] * self.rowPadding
		buf = []
		for row in reversed(self.rows):
			for red, green, blue, _ in row:
				buf.append(blue)
				buf.append(green)
				buf.append(red)
			if padding:
				buf.extend(padding)
		self._set_data(buf)

	def _decode_palette(self):
		palette = self.palette
		table = []
		for i in range(0, len(palette), 4):
			blue = palette[i]
			green = palette[i + 1]
			red = palette[i + 2]
			alpha = palette[i + 3]
			table.append((red, green, blue, alpha))
		return table

	def countColor(self):
		counter = {}
		for row in self.rows:
			for color in row:
				if color in counter:
					counter[color] += 1
				else:
					counter[color] = 1
		return counter

	def _build_palette(self, method=QuantizeMethod.Naive):
		colorCount = 1 << self.bitsPerPixel
		palette = self.palette
		colorMap = {}
		if palette and len(palette) == 4*colorCount:
			table = self._decode_palette()
			for index, color in enumerate(table):
				if color not in colorMap:
					colorMap[color] = index
		else:
			counter = self.countColor()
			if len(counter) > colorCount and (method == None or method < QuantizeMethod.Naive):
				bmp = self.quantize(colorCount, method, False)
				print(f'quantize {len(counter)} => {bmp.colorUsed}')
				return bmp._build_palette(QuantizeMethod.Naive)

			table = [item[0] for item in sorted(counter.items(), key=lambda m: (m[1], m[0]), reverse=True)][:colorCount]
			for index, color in enumerate(table):
				colorMap[color] = index
			if len(counter) > colorCount:
				for color in counter:
					if color not in colorMap:
						index = _find_color_index_naive(table, color)
						colorMap[color] = index

			palette = []
			for red, green, blue, alpha in table:
				palette.append(blue)
				palette.append(green)
				palette.append(red)
				palette.append(alpha)
			colorCount = 4*colorCount - len(palette)
			if colorCount != 0:
				palette.extend([0]*colorCount)

		indexData = []
		for row in self.rows:
			buf = []
			for color in row:
				index = colorMap[color]
				buf.append(index)
			indexData.append(buf)
		return palette, indexData

	def _decode_8bit(self):
		width, height = self.size
		self.rows.clear()

		offset = 0
		buf = self.data
		padding = self.rowPadding
		rows = []
		table = self._decode_palette()
		for y in range(height):
			row = []
			for x in range(width):
				index = buf[offset]
				offset += 1
				row.append(table[index])

			offset += padding
			rows.append(row)
		self.rows.extend(reversed(rows))

	def _encode_8bit(self):
		padding = [0] * self.rowPadding
		buf = []
		palette, indexData = self._build_palette()
		for row in reversed(indexData):
			buf.extend(row)
			if padding:
				buf.extend(padding)
		self._set_data(buf, palette)

	def _decode_4bit(self):
		width, height = self.size
		self.rows.clear()

		offset = 0
		buf = self.data
		padding = self.rowPadding
		rows = []
		padded = width & 1
		octet = (width + padded) >> 1
		table = self._decode_palette()
		for y in range(height):
			row = []
			for x in range(octet):
				index = buf[offset]
				offset += 1
				row.append(table[index >> 4])
				row.append(table[index & 15])

			offset += padding
			if padded:
				del row[-1]
			rows.append(row)
		self.rows.extend(reversed(rows))

	def _encode_4bit(self):
		width = self.width
		padded = width & 1
		padding = [0] * (2*self.rowPadding + padded)
		octet = width + len(padding)
		buf = []
		palette, indexData = self._build_palette()
		for row in reversed(indexData):
			if padding:
				row.extend(padding)
			for x in range(0, octet, 2):
				value = (row[x] << 4) | row[x + 1]
				buf.append(value)
		self._set_data(buf, palette)

	def _decode_1bit(self):
		width, height = self.size
		self.rows.clear()

		offset = 0
		buf = self.data
		padding = self.rowPadding
		rows = []
		padded = (8 - (width & 7)) & 7
		octet = (width + padded) >> 3
		table = self._decode_palette()
		for y in range(height):
			row = []
			for x in range(octet):
				index = buf[offset]
				offset += 1
				row.append(table[index >> 7])
				row.append(table[(index >> 6) & 1])
				row.append(table[(index >> 5) & 1])
				row.append(table[(index >> 4) & 1])
				row.append(table[(index >> 3) & 1])
				row.append(table[(index >> 2) & 1])
				row.append(table[(index >> 1) & 1])
				row.append(table[index & 1])

			offset += padding
			if padded:
				del row[-padded:]
			rows.append(row)
		self.rows.extend(reversed(rows))

	def _encode_1bit(self):
		width = self.width
		padded = (8 - (width & 7)) & 7
		padding = [0] * (8*self.rowPadding + padded)
		octet = width + len(padding)
		buf = []
		palette, indexData = self._build_palette()
		for row in reversed(indexData):
			if padding:
				row.extend(padding)
			for x in range(0, octet, 8):
				value = (row[x] << 7) \
					| (row[x + 1] << 6) \
					| (row[x + 2] << 5) \
					| (row[x + 3] << 4) \
					| (row[x + 4] << 3) \
					| (row[x + 5] << 2) \
					| (row[x + 6] << 1) \
					| row[x + 7]
				buf.append(value)
		self._set_data(buf, palette)

	@property
	def width(self):
		return self.infoHeader.width

	@property
	def height(self):
		return self.infoHeader.height

	@property
	def size(self):
		return self.infoHeader.size

	@property
	def rowSize(self):
		return self.infoHeader.rowSize

	@property
	def rowPadding(self):
		return self.infoHeader.rowPadding

	@property
	def resolution(self):
		return self.infoHeader.resolution

	@resolution.setter
	def resolution(self, value):
		self.infoHeader.resolution = value

	@property
	def bitsPerPixel(self):
		return self.infoHeader.bitsPerPixel

	@bitsPerPixel.setter
	def bitsPerPixel(self, value):
		if value and self.infoHeader.bitsPerPixel != value:
			assert value in _SupportedColorDepth, value
			self.infoHeader = BitmapInfoHeader(self.width, self.height, value)
			self.palette = None

	@property
	def colorUsed(self):
		return len(self.countColor())

	def __getitem__(self, key):
		x, y = key
		return self.rows[y][x]

	def __setitem__(self, key, value):
		x, y = key
		self.setColor(x, y, value)

	def getColor(self, x, y):
		return self.rows[y][x]

	def setColor(self, x, y, color):
		if len(color) == 3:
			color = (*color, 0xFF)
		self.palette = None
		self.rows[y][x] = color

	def save(self, path, colorDepth=None):
		if colorDepth == 24 and not self.opaque():
			bmp = Bitmap.fromImage(self.toImage(24, False))
			bmp.save(path)
			return

		infoHeader = self.infoHeader
		self.bitsPerPixel = colorDepth
		if hasattr(path, 'write'):
			self.write(path)
		else:
			with open(path, 'wb') as fd:
				self.write(fd)
		self.infoHeader = infoHeader

	@staticmethod
	def fromFile(path):
		bmp = Bitmap()
		if hasattr(path, 'read'):
			bmp.read(path)
		else:
			with open(path, 'rb') as fd:
				bmp.read(fd)
		return bmp

	@staticmethod
	def fromImage(image):
		if image.mode not in ('RGB', 'RGBA'):
			image = image.convert('RGBA')

		width, height = image.size
		data = image.load()
		if image.mode == 'RGB':
			bmp = Bitmap(width, height, 24)
			for y in range(height):
				for x in range(width):
					bmp[x, y] = data[x, y]
		else:
			bmp = Bitmap(width, height, 32)
			for y in range(height):
				for x in range(width):
					color = data[x, y]
					# TODO: fix transparent color
					if color[3] == 0:
						bmp[x, y] = _TransparentColor
					else:
						bmp[x, y] = color
		return bmp

	def opaque(self, colorDepth=None):
		colorDepth = colorDepth or self.bitsPerPixel
		if colorDepth == 32:
			return False
		if colorDepth == 24 and self.bitsPerPixel == 24:
			return True
		transparent = 0
		count = 0
		for row in self.rows:
			for color in row:
				alpha = color[3]
				count += alpha & 1
				if alpha not in (0, 0xFF):
					return False
				if color == _TransparentColor:
					transparent += 1
		total = self.width*self.height
		return transparent != total and (count == 0 or count == total)

	def toImage(self, colorDepth=None, check=True):
		if check and self.opaque(colorDepth):
			image = Image.new('RGB', self.size)
			data = []
			for row in self.rows:
				for red, green, blue, _ in row:
					data.append((red, green, blue))
			image.putdata(data)
		else:
			image = Image.new('RGBA', self.size)
			data = []
			for row in self.rows:
				data.extend(row)
			image.putdata(data)
			if colorDepth == 24:
				image = image.convert('RGB')
		return image

	def resize(self, size, method=ResizeMethod.Lanczos):
		image = self.toImage()
		image = image.resize(size, resample=method)
		return Bitmap.fromImage(image)

	def quantize(self, colorCount, method=None, check=True):
		if check and self.colorUsed <= colorCount:
			return self
		if method and method >= QuantizeMethod.Naive:
			return self
		image = self.toImage().quantize(colorCount, method=method)
		return Bitmap.fromImage(image)

	@staticmethod
	def fromFileEx(path):
		ext = os.path.splitext(path)[0].lower()
		if ext in ['.bmp', '.dib']:
			try:
				return Bitmap.fromFile(path)
			except Exception:
				pass
		image = Image.open(path)
		return Bitmap.fromImage(image)

	@staticmethod
	def concatHorizontal(bmps):
		width = 0
		height = 0
		for bmp in bmps:
			width += bmp.width
			if height == 0:
				height = bmp.height
			elif height != bmp.height:
				raise ValueError(f'Invalid image height {bmp.height}, requres {height}!')

		out_bmp = Bitmap(width, height)
		rows = out_bmp.rows
		for y in range(height):
			rows[y].clear()
		for bmp in bmps:
			for y in range(height):
				rows[y].extend(bmp.rows[y])

		return out_bmp

	def splitHorizontal(self, dims=None):
		width, height = self.size
		if not dims:
			dims = [height] * (width // height)
			w = width % height
			if w:
				dims.append(w)
		else:
			used = []
			total = 0
			for w in dims:
				total += w
				if total > width:
					total -= w
					break
				used.append(w)
			w = width - total
			if w:
				used.append(w)
			dims = used

		total = 0
		bmps = []
		for w in dims:
			bmp = Bitmap(w, height)
			for y in range(height):
				bmp.rows[y].clear()
				bmp.rows[y].extend(self.rows[y][total:total + w])
			total += w
			bmps.append(bmp)

		return bmps

	@staticmethod
	def concatVertical(bmps):
		width = 0
		height = 0
		for bmp in bmps:
			height += bmp.height
			if width == 0:
				width = bmp.width
			elif width != bmp.width:
				raise ValueError(f'Invalid image width {bmp.width}, requres {width}!')

		out_bmp = Bitmap(width, height)
		rows = out_bmp.rows
		rows.clear()
		for bmp in bmps:
			for row in bmp.rows:
				rows.append(row[:])

		return out_bmp

	def splitVertical(self, dims=None):
		width, height = self.size
		if not dims:
			dims = [width] * (height // width)
			h = height % width
			if h:
				dims.append(h)
		else:
			used = []
			total = 0
			for h in dims:
				total += h
				if total > height:
					total -= h
					break
				used.append(h)
			h = height - total
			if h:
				used.append(h)
			dims = used

		total = 0
		bmps = []
		for h in dims:
			bmp = Bitmap(width, h)
			bmp.rows.clear()
			for row in self.rows[total:total + h]:
				bmp.rows.append(row[:])
			total += h
			bmps.append(bmp)
		return bmps

	def flipHorizontal(self):
		width, height = self.size
		bmp = Bitmap(width, height)
		bmp.rows.clear()
		for row in self.rows:
			copy = row[:]
			copy.reverse()
			bmp.rows.append(copy)
		return bmp

	def flipVertical(self):
		width, height = self.size
		bmp = Bitmap(width, height)
		bmp.rows.clear()
		for row in reversed(self.rows):
			bmp.rows.append(row[:])
		return bmp
