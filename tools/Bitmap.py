import os.path
import struct
import io
from enum import IntEnum
import ctypes
from PIL import Image

__all__ = ['Bitmap', 'Color', 'Icon', 'Image', 'ResizeMethod', 'QuantizeMethod']

class Color:
	Transparent = (0, 0, 0, 0)
	Black = (0, 0, 0, 255)
	White = (255, 255, 255, 255)

	@staticmethod
	def find_color_index_naive(table, color):
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

	# https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getsyscolor
	@staticmethod
	def GetSysColor(index):
		value = ctypes.windll.user32.GetSysColor(index)
		red, green, blue = value & 0xff, (value >> 8) & 0xff, (value >> 16) & 0xff
		return (red, green, blue, 0xff)

def _drawMaskRow(maskRow):
	result = ['o']*8*len(maskRow)
	index = 0
	for mask in maskRow:
		for i in range(8):
			if mask & 0x80:
				result[index + i] = '.'
			mask <<= 1
		index += 8
	return result

def _diffMaskData(name, referData, maskData, rowSize, saveLog=False):
	if not referData or referData == maskData:
		return 0
	if not saveLog:
		total = 0
		diff = 0
		count = 0
		for i, refer in enumerate(referData):
			mask = maskData[i]
			diff <<= 8
			diff |= refer ^ mask
			count += 1
			if count == 8:
				count = 0
				total += diff.bit_count()
				diff = 0
		print(f'{name} mask diff: {total} {len(referData)*8}')
		return total

	total = 0
	output = []
	referList = [referData[i:i+rowSize] for i in range(0, len(referData), rowSize)]
	maskList = [maskData[i:i+rowSize] for i in range(0, len(maskData), rowSize)]
	referList.reverse()
	maskList.reverse()
	for i, row in enumerate(referList):
		left = _drawMaskRow(row)
		right = _drawMaskRow(maskList[i])
		for j, ch in enumerate(right):
			if ch != left[j]:
				total += 1
				ch = '=' if ch == '.' else '*'
				right[j] = ch
		left = ''.join(left)
		right = ''.join(right)
		output.append(f'{left} | {right}\n')
	print(f'{name} mask diff: {total} {len(referData)*8}')
	doc = ''.join(output)
	with open(f'{name}.log', 'wb') as fd:
		fd.write(doc.encode('utf-8'))
	return total

# https://pillow.readthedocs.io/en/stable/index.html
def _showMaskImage(maskList, rowSize, height, title=None):
	rows = [maskList[i:i+rowSize] for i in range(0, len(maskList), rowSize)]
	maskList = []
	for row in reversed(rows):
		maskList.extend(row)
	image = Image.frombuffer('1', (rowSize*8, height), bytes(maskList), "raw", '1', 0, 1)
	image.show(title)

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

# https://en.wikipedia.org/wiki/BMP_file_format
# https://learn.microsoft.com/en-us/windows/win32/gdi/windows-gdi
_InchesPerMetre = 0.0254
_SupportedColorDepth = (1, 4, 8, 24, 32, 'png')
_PngMagic = b'\x89PNG'

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

	def read(self, fd, iconFile=False):
		magic, self.width, self.height = struct.unpack('<III', fd.read(4*3))
		if iconFile:
			self.height //= 2
		assert magic == BitmapInfoHeader.StructureSize
		self.planes, self.bitsPerPixel = struct.unpack('<HH', fd.read(2*2))
		assert self.planes == 1
		assert self.bitsPerPixel in _SupportedColorDepth, self.bitsPerPixel
		self.compression, self.sizeImage = struct.unpack('<II', fd.read(4*2))
		assert self.compression == CompressionMethod.BI_RGB
		self._resolutionX, self._resolutionY = struct.unpack('<II', fd.read(4*2))
		self.colorUsed, self.colorImportant = struct.unpack('<II', fd.read(4*2))

	def write(self, fd, iconFile=False):
		height = self.height
		if iconFile:
			height *= 2
		fd.write(struct.pack('<III', BitmapInfoHeader.StructureSize, self.width, height))
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
	def maskRowSize(self):
		# 4 byte aligned
		aligned = (self.width + 31) & ~31
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
		self.maskData = None

		if width and height:
			row = [Color.Transparent] * width
			for y in range(height):
				self.rows.append(row[:])

	def read(self, fd, iconFile=False):
		start = fd.tell()
		if not iconFile:
			self.fileHeader.read(fd)
		self.infoHeader.read(fd, iconFile)
		self.palette = None
		# color palette after header
		if self.infoHeader.bitsPerPixel < 24:
			paletteSize = 4 << self.infoHeader.bitsPerPixel
			self.palette = fd.read(paletteSize)

		if not iconFile:
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

	def write(self, fd, iconFile=False):
		self.encode()
		maskData = self.maskData
		#if self.bitsPerPixel == 32:
		#	maskData = None # mask is not required
		if iconFile:
			if maskData:
				self.infoHeader.sizeImage += len(maskData)
		else:
			self.fileHeader.write(fd)
		self.infoHeader.write(fd, iconFile)
		if self.palette:
			fd.write(self.palette)
		fd.write(self.data)
		if iconFile and maskData:
			fd.write(maskData)

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

	def decode_palette(self):
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

	def build_palette(self, method=None):
		colorCount = 1 << self.bitsPerPixel
		palette = self.palette
		colorMap = {}
		if palette and len(palette) == 4*colorCount:
			table = self.decode_palette()
			for index, color in enumerate(table):
				if color not in colorMap:
					colorMap[color] = index
		else:
			counter = self.countColor()
			if len(counter) > colorCount and (method is None or method < QuantizeMethod.Naive):
				reduced, result, count, bmp = self.reduce_color(colorCount, method)
				name = 'Default' if method is None else method.name
				print(f'{name} reduce {bmp.width}x{bmp.height} bitmap color: {len(counter)} => {reduced}, {(count, result)}')
				bmp.bitsPerPixel = self.bitsPerPixel
				return bmp.build_palette(QuantizeMethod.Naive)

			table = [item[0] for item in sorted(counter.items(), key=lambda m: (m[1], m[0]), reverse=True)][:colorCount]
			for index, color in enumerate(table):
				colorMap[color] = index
			if len(counter) > colorCount:
				for color in counter:
					if color not in colorMap:
						index = Color.find_color_index_naive(table, color)
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
		table = self.decode_palette()
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
		palette, indexData = self.build_palette()
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
		table = self.decode_palette()
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
		palette, indexData = self.build_palette()
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
		table = self.decode_palette()
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
		palette, indexData = self.build_palette()
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

	def apply_alpha_mask(self, maskData):
		self.maskData = maskData
		if self.bitsPerPixel == 32:
			return
		width = self.width
		rowSize = self.infoHeader.maskRowSize
		maskList = [maskData[i:i+rowSize] for i in range(0, len(maskData), rowSize)]
		index = 0
		for row in reversed(self.rows):
			maskRow = maskList[index]
			index += 1
			offset = 0
			for mask in maskRow:
				for i in range(8):
					red, green, blue, alpha = row[offset]
					alpha = 0 if (mask & 0x80) else 0xff
					row[offset] = (red, green, blue, alpha)
					mask <<= 1
					offset += 1
					if offset == width:
						break
				if offset == width:
					break

	def build_alpha_mask(self, threshold=0):
		rowSize = self.infoHeader.maskRowSize
		maskList = []
		for row in reversed(self.rows):
			maskRow = [0] * rowSize
			mask = 0
			offset = 0
			count = 0
			for color in row:
				mask <<= 1
				mask |= color[3] <= threshold
				count += 1
				if count == 8:
					count = 0
					maskRow[offset] = mask
					mask = 0
					offset += 1
			if count:
				count = 8 - count
				mask <<= count
				maskRow[offset] = mask
			maskList.extend(maskRow)
		#_showMaskImage(maskList, rowSize, self.height, f'threshold {threshold}')
		return maskList

	def make_transparent(self, maskData):
		width = self.width
		rowSize = self.infoHeader.maskRowSize
		maskList = [maskData[i:i+rowSize] for i in range(0, len(maskData), rowSize)]
		index = 0
		for row in reversed(self.rows):
			maskRow = maskList[index]
			index += 1
			offset = 0
			for mask in maskRow:
				for i in range(8):
					if mask & 0x80:
						row[offset] = Color.Transparent
					mask <<= 1
					offset += 1
					if offset == width:
						break
				if offset == width:
					break

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

	def save(self, path, colorDepth=None, backColor=Color.White):
		if colorDepth and colorDepth != 32 and not self.isOpaque():
			bmp = self.asOpaque(backColor)
			bmp.save(path, colorDepth)
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
						bmp[x, y] = Color.Transparent
					else:
						bmp[x, y] = color
		return bmp

	def asOpaque(self, backColor=Color.White):
		image = self.toImage(32, False)
		if backColor is None:
			image = image.convert('RGB')
		else:
			background = Image.new('RGBA', image.size, color=backColor)
			image = Image.alpha_composite(background, image)
		bmp = Bitmap.fromImage(image)
		bmp.bitsPerPixel = 24
		return bmp

	def reduce_icon_color(self, colorDepth, maskData, method=None):
		colorCount = 1 << colorDepth
		counter = self.countColor()
		current = len(counter)
		if current < colorCount:
			self.make_transparent(maskData)
			return

		if current == colorCount:
			rows = []
			for row in self.rows:
				rows.append(row[:])
			self.make_transparent(maskData)
			counter = self.countColor()
			if len(counter) <= colorCount:
				return
			self.rows = rows

		reduced, result, count, bmp = self.reduce_color(colorCount, method, maskData)
		self.rows = bmp.rows
		name = 'Default' if method is None else method.name
		print(f'{name} reduce {bmp.width}x{bmp.height} icon color: {current} => {reduced}, {(count, result)}')

	def reduce_color(self, colorCount, method=None, maskData=None):
		image = self.toImage()
		reduced = {}
		best = None
		count = colorCount
		while count and count <= 256:
			img = image.quantize(count, method=method)
			bmp = Bitmap.fromImage(img)
			if maskData:
				bmp.make_transparent(maskData)
			counter = bmp.countColor()
			result = len(counter)
			reduced[count] = (result, count, bmp)
			if result == colorCount:
				best = (result, count, bmp)
				break
			if result > colorCount:
				count -= 1
			else:
				count += 1
			if count in reduced:
				break

		if not best:
			best = sorted(value for value in reduced.values() if value[0] < colorCount)[-1]
		reduced = dict((key, value[0]) for key, value in reduced.items())
		return reduced, *best

	def asIcon(self, colorDepth=None, method=None, threshold=0, backColor=Color.White):
		maskData = self.build_alpha_mask(threshold)
		maskData = bytes(maskData)
		bmp = self
		colorDepth = colorDepth or self.bitsPerPixel
		if colorDepth != 32 and not self.isOpaque():
			bmp = self.asOpaque(backColor)
		if colorDepth < 24:
			bmp.reduce_icon_color(colorDepth, maskData, method)
		elif colorDepth == 24:
			bmp.make_transparent(maskData)
		bmp.bitsPerPixel = colorDepth
		bmp.maskData = maskData
		return bmp

	def isOpaque(self, colorDepth=None):
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
				if color == Color.Transparent:
					transparent += 1
		total = self.width*self.height
		return transparent != total and (count == 0 or count == total)

	def toImage(self, colorDepth=None, check=True):
		if check and self.isOpaque(colorDepth):
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
		result = self.reduce_color(colorCount, method)
		return result[-1]

	@staticmethod
	def fromFileEx(path):
		ext = os.path.splitext(path)[0].lower()
		if ext in ('.bmp', '.dib'):
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

# https://en.wikipedia.org/wiki/ICO_(file_format)
class IconCursorType(IntEnum):
	Icon = 1
	Cursor = 2

class IconDirectoryEntry:
	StructureSize = 16

	def __init__(self, width=0, height=0, bitsPerPixel=32):
		self.width = width
		self.height = height
		self.colorCount = (1 << bitsPerPixel) & 0xff
		self.reserved = 0
		self.planes = 1
		self.bitsPerPixel = bitsPerPixel
		self.sizeImage = 0
		self.imageOffset = 0

	def read(self, fd):
		self.width, self.height = fd.read(2)
		if self.width == 0:
			self.width = 256
		if self.height == 0:
			self.height = 256
		self.colorCount, self.reserved = fd.read(2)
		self.planes, self.bitsPerPixel = struct.unpack('<HH', fd.read(2*2))
		self.sizeImage, self.imageOffset = struct.unpack('<II', fd.read(4*2))

	def write(self, fd):
		fd.write(bytes((self.width & 255, self.height & 255)))
		fd.write(bytes((self.colorCount, self.reserved)))
		fd.write(struct.pack('<HH', self.planes, self.bitsPerPixel))
		fd.write(struct.pack('<II', self.sizeImage, self.imageOffset))

	@property
	def hotspot(self):
		return (self.planes, self.bitsPerPixel)

	@hotspot.setter
	def hotspot(self, value):
		self.planes, self.bitsPerPixel = value

	def __str__(self):
		return f'''IconDirectoryEntry {{
	width: {self.width :02X} {self.width}
	height: {self.height :02X} {self.height}
	colorCount: {self.colorCount :02X} {self.colorCount}
	reserved: {self.reserved :02X} {self.reserved}
	planes/X: {self.planes :04X} {self.planes}
	bitsPerPixel/Y: {self.bitsPerPixel :04X} {self.bitsPerPixel}
	sizeImage: {self.sizeImage :08X} {self.sizeImage}
	imageOffset: {self.imageOffset :08X} {self.imageOffset}
}}'''

class IconDirectory:
	StructureSize = 6

	def __init__(self, imageType=IconCursorType.Icon):
		self.reserved = 0
		self.imageType = imageType
		self.entryList = []

	def read(self, fd):
		self.reserved, imageType, count = struct.unpack('<HHH', fd.read(2*3))
		assert imageType in (IconCursorType.Icon, IconCursorType.Cursor)
		self.imageType = IconCursorType(imageType)
		self.entryList = [None] * count
		for i in range(count):
			entry = IconDirectoryEntry()
			entry.read(fd)
			self.entryList[i] = entry

	def write(self, fd):
		fd.write(struct.pack('<HHH', self.reserved, self.imageType, len(self.entryList)))
		for entry in self.entryList:
			entry.write(fd)

	def __str__(self):
		entry = '\n'.join(str(entry) for entry in self.entryList)
		return f'''{{
	reserved: {self.reserved :04X} {self.reserved}
	imageType: {self.imageType :04X} {self.imageType} {self.imageType.name}
	entryCount:{len(self.entryList):04X} {len(self.entryList)}
	entryList:
{entry}
}}'''

class Icon:
	def __init__(self, imageType=IconCursorType.Icon):
		self.directory = IconDirectory(imageType)
		self.imageList = []

	def read(self, fd):
		self.directory.read(fd)
		self.imageList = []
		for entry in self.directory.entryList:
			#print(entry)
			fd.seek(entry.imageOffset, os.SEEK_SET)
			current = fd.tell()
			magic = fd.read(4)
			fd.seek(current, os.SEEK_SET)
			raw = fd.read(entry.sizeImage)
			assert len(raw) == entry.sizeImage
			stream = io.BytesIO(raw)
			if magic == _PngMagic:
				image = Image.open(stream)
				#print(f'{image.width}x{image.height} png')
				self.imageList.append((image, 'png', raw))
				continue

			bmp = Bitmap()
			bmp.read(stream, iconFile=True)
			current = stream.tell()
			remain = entry.sizeImage - current
			if remain == 0:
				assert bmp.bitsPerPixel == 32
			else:
				maskSize = bmp.height * bmp.infoHeader.maskRowSize
				assert remain == maskSize, (remain, maskSize)
				maskData = stream.read(maskSize)
				assert len(maskData) == maskSize
				bmp.apply_alpha_mask(maskData)
			#print(bmp.infoHeader, bmp.colorUsed)
			if False:
				mask0 = bmp.build_alpha_mask(0)
				mask1 = bmp.build_alpha_mask(127)
				rowSize = bmp.infoHeader.maskRowSize
				_diffMaskData(f'{bmp.width}x{bmp.height}@{bmp.bitsPerPixel}-0', bmp.maskData, mask0, rowSize)
				_diffMaskData(f'{bmp.width}x{bmp.height}@{bmp.bitsPerPixel}-1', bmp.maskData, mask1, rowSize)
			self.imageList.append((bmp, bmp.bitsPerPixel, raw))

	def write(self, fd):
		self.directory.write(fd)
		for _, _, data in self.imageList:
			fd.write(data)

	def save(self, path):
		if hasattr(path, 'write'):
			self.write(path)
		else:
			with open(path, 'wb') as fd:
				self.write(fd)

	def build(self, args, method=None, threshold=0, backColor=Color.White):
		imageList = {}
		for path, colorDepth, hotspot in args:
			index = _SupportedColorDepth.index(colorDepth)
			if colorDepth == 'png':
				with open(path, 'rb') as fd:
					raw = fd.read()
				stream = io.BytesIO(raw)
				image = Image.open(stream)
				if image.getpalette():
					print(f'ERROR: {image.width}x{image.height} icon png with palette: {path}')
				assert image.width == image.height, (path, image.width, image.height)
				imageList[(image.width, image.height, index)] = (image, colorDepth, raw, hotspot)
			else:
				bmp = Bitmap.fromFileEx(path)
				assert bmp.width == bmp.height and bmp.width < 256 and bmp.height < 256, (path, bmp.width, bmp.height)
				bmp = bmp.asIcon(colorDepth, method, threshold, backColor)
				stream = io.BytesIO()
				bmp.write(stream, iconFile=True)
				raw = stream.getvalue()
				imageList[(bmp.width, bmp.height, index)] = (bmp, colorDepth, raw, hotspot)

		imageList = [item[1] for item in sorted(imageList.items(), reverse=True)]
		imageOffset = IconDirectory.StructureSize + IconDirectoryEntry.StructureSize*len(imageList)
		imageType = self.imageType
		for image, colorDepth, raw, hotspot in imageList:
			self.imageList.append((image, colorDepth, raw))
			bitsPerPixel = 32 if colorDepth == 'png' else colorDepth
			entry = IconDirectoryEntry(image.width, image.height, bitsPerPixel)
			sizeImage = len(raw)
			entry.imageOffset = imageOffset
			entry.sizeImage = sizeImage
			imageOffset += sizeImage
			if imageType == IconCursorType.Cursor:
				entry.hotspot = hotspot
			self.directory.entryList.append(entry)

	def extract(self, folder='.', prefix='', dims=None):
		if not os.path.exists(folder):
			os.makedirs(folder)
		for image, fmt, data in self.imageList:
			dim = f'{image.width}x{image.height}'
			if dims and dim not in dims:
				continue
			if fmt == 'png':
				path = f'{folder}/{prefix}{dim}.png'
			else:
				path = f'{folder}/{prefix}{dim}@{fmt}.png'
			if data and fmt == 'png':
				with open(path, 'wb') as fd:
					fd.write(data)
			else:
				if isinstance(image, Bitmap):
					image = image.toImage(32)
				image.save(path)

	@property
	def imageType(self):
		return self.directory.imageType

	@staticmethod
	def fromFile(path):
		icon = Icon()
		if hasattr(path, 'read'):
			icon.read(path)
		else:
			with open(path, 'rb') as fd:
				icon.read(fd)
		return icon

	@staticmethod
	def makeIcon(args, path=None, method=None, threshold=0, backColor=Color.White):
		icon = Icon()
		args = [(*arg, None) for arg in args]
		if path:
			print('make icon:', path)
		icon.build(args, method, threshold, backColor)
		if path:
			icon.save(path)
		return icon

	@staticmethod
	def makeCursor(args, path=None, method=None, threshold=0, backColor=Color.White):
		icon = Icon(IconCursorType.Cursor)
		if path:
			print('make cursor:', path)
		icon.build(args, method, threshold, backColor)
		if path:
			icon.save(path)
		return icon
