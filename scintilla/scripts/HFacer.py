# HFacer.py - regenerate the Scintilla.h file from the Scintilla.iface interface
# definition file.
# Implemented 2000 by Neil Hodgson neilh@scintilla.org

import pathlib
import Face

import FileGenerator

def printHFile(f):
	out = []
	previousCategory = ""
	anyProvisional = False
	for name in f.order:
		v = f.features[name]
		currentCategory = v["Category"]
		if currentCategory != "Deprecated":
			if currentCategory == "Provisional" and previousCategory != "Provisional":
				out.append("#ifndef SCI_DISABLE_PROVISIONAL")
				anyProvisional = True
			previousCategory = currentCategory
			featureType = v["FeatureType"]
			if featureType in ["fun", "get", "set"]:
				featureDefineName = "SCI_" + name.upper()
				out.append("#define " + featureDefineName + " " + v["Value"])
			elif featureType == "evt":
				featureDefineName = "SCN_" + name.upper()
				out.append("#define " + featureDefineName + " " + v["Value"])
			elif featureType == "val":
				out.append("#define " + name + " " + v["Value"])
	if anyProvisional:
		out.append("#endif")
	return out

def RegenerateAll(root, showMaxID):
	f = Face.Face()
	f.ReadFromFile(root / "include/Scintilla.iface")
	FileGenerator.Regenerate(root / "include/Scintilla.h", "/* ", printHFile(f))
	if showMaxID:
		valueSet = set(int(x) for x in f.values if int(x) < 3000)
		maximumID = max(valueSet)
		print("Maximum ID is", maximumID)
		#~ valuesUnused = sorted(x for x in range(2001, maximumID) if x not in valueSet)
		#~ print("\nUnused values")
		#~ valueToName = {}
		#~ for name, feature in f.features.items():
			#~ try:
				#~ value = int(feature["Value"])
				#~ valueToName[value] = name
			#~ except ValueError:
				#~ pass
		#~ for v in valuesUnused:
			#~ prev = valueToName.get(v-1, "")
			#~ print(v, prev)

if __name__ == "__main__":
	RegenerateAll(pathlib.Path(__file__).resolve().parent.parent, True)
