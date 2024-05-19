# Face.py - module for reading and parsing Scintilla.iface file
# Implemented 2000 by Neil Hodgson neilh@scintilla.org
# Released to the public domain.

def sanitiseLine(line):
	index = line.find("##")
	if index >= 0:
		line = line[:index]
	line = line.strip()
	return line

def decodeFunction(featureVal):
	retType, rest = featureVal.split(" ", 1)
	nameIdent, params = rest.split("(")
	name, value = nameIdent.split("=")
	params, rest = params.split(")")
	param1, param2 = params.split(",")
	return retType, name, value, param1, param2

def decodeEvent(featureVal):
	retType, rest = featureVal.split(" ", 1)
	nameIdent = rest.split("(")[0]
	name, value = nameIdent.split("=")
	return retType, name, value

def decodeParam(p):
	param = p.strip()
	paramType = ""
	name = ""
	value = ""
	if " " in param:
		paramType, nv = param.split(" ")
		if "=" in nv:
			name, value = nv.split("=")
		else:
			name = nv
	return paramType, name, value

def IsEnumeration(t):
	return t[:1].isupper()

def PascalCase(s):
	capitalized = s.title()
	# Remove '_' except between digits
	pascalCase = ""
	characterPrevious = " "
	# Loop until penultimate character
	for i in range(len(capitalized)-1):
		character = capitalized[i]
		characterNext = capitalized[i+1]
		if character != "_" or (
			characterPrevious.isnumeric() and characterNext.isnumeric()):
			pascalCase += character
		characterPrevious = character
	# Add last character - not between digits so no special treatment
	pascalCase += capitalized[-1]
	return pascalCase

class Face:

	def __init__(self):
		self.order = []
		self.features = {}
		self.values = {}
		self.events = {}
		self.aliases = {}

	def ReadFromFile(self, name):
		currentCategory = ""
		currentComment = []
		currentCommentFinished = False
		maxInt = (1 << 31) - 1
		lineno = 0
		autoValue = 0
		valueMap = None
		with open(name, encoding='utf-8') as fd:
			lines = fd.readlines()
		for line in lines:
			lineno += 1
			line = sanitiseLine(line)
			if line:
				if line[0] == "#":
					if line[1] == " ":
						if currentCommentFinished:
							currentCommentFinished = False
							currentComment = []
						currentComment.append(line[2:])
				else:
					currentCommentFinished = True
					featureType, featureVal = line.split(" ", 1)
					if featureType in ("fun", "get", "set"):
						try:
							retType, name, value, param1, param2 = decodeFunction(featureVal)
						except ValueError:
							print(f"Failed to decode line {lineno}: {line}")
							raise
						p1 = decodeParam(param1)
						p2 = decodeParam(param2)
						self.features[name] = {
							"FeatureType": featureType,
							"ReturnType": retType,
							"Value": value,
							"Param1Type": p1[0], "Param1Name": p1[1], "Param1Value": p1[2],
							"Param2Type": p2[0], "Param2Name": p2[1], "Param2Value": p2[2],
							"Category": currentCategory, "Comment": currentComment
						}
						if currentCategory != 'Deprecated' and value in self.values:
							raise ValueError(f"Duplicate value {value} {name} on line: {lineno}")
						self.values[value] = 1
						self.order.append(name)
						currentComment = []
					elif featureType == "evt":
						retType, name, value = decodeEvent(featureVal)
						self.features[name] = {
							"FeatureType": featureType,
							"ReturnType": retType,
							"Value": value,
							"Category": currentCategory, "Comment": currentComment
						}
						if value in self.events:
							raise ValueError(f"Duplicate event {value} {name} on line: {lineno}")
						self.events[value] = 1
						self.order.append(name)
					elif featureType == "cat":
						currentCategory = featureVal
					elif featureType == "val":
						try:
							name, value = featureVal.split("=", 1)
							val = 0
							if value:
								val = int(value, 0)
								if val < 0:
									# add parenthesis for negative value
									value = f'({value})'
								elif val > maxInt:
									# unsigned value
									value = value + 'U'
							else:
								value = 'auto'
							if name.startswith('SC_'):
								if value == 'auto':
									val = autoValue
									value = str(val)
								if valueMap is not None:
									if val in valueMap:
										raise ValueError(f"Duplicate Value: {name} = {val}, {valueMap[val]} on line: {lineno}")
									autoValue = val + 1
									valueMap[val] = name
						except ValueError:
							print(f"Failure line {lineno}: {featureVal}")
							raise
						self.features[name] = {
							"FeatureType": featureType,
							"Category": currentCategory,
							"Value": value }
						self.order.append(name)
					elif featureType in ("enu", "lex"):
						autoValue = 0
						valueMap = {}
						name, value = featureVal.split("=", 1)
						self.features[name] = {
							"FeatureType": featureType,
							"Category": currentCategory,
							"Value": value,
							"Comment": currentComment }
						self.order.append(name)
						currentComment = []
					elif featureType == "ali":
						# Enumeration alias
						name, value = featureVal.split("=", 1)
						self.aliases[name] = value
						currentComment = []
