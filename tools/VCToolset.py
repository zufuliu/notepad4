#!/usr/bin/env python3
import glob

msvc = """
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='15.0'">v141</PlatformToolset>
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='16.0'">v142</PlatformToolset>
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='17.0'">v143</PlatformToolset>
""".strip('\r\n').splitlines()

llvm = """
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='15.0'">LLVM_v141</PlatformToolset>
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='16.0'">LLVM_v142</PlatformToolset>
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='17.0'">LLVM_v143</PlatformToolset>
""".strip('\r\n').splitlines()

def update_project_toolset(projectPath):
	doc = open(projectPath, encoding='utf-8').read()
	lines = []
	previous = False
	for line in doc.splitlines():
		current = '<PlatformToolset Condition=' in line
		if current:
			if not previous:
				if 'LLVM' in line:
					lines.extend(llvm)
				else:
					lines.extend(msvc)
		else:
			lines.append(line)
		previous = current

	updated = '\n'.join(lines)
	if updated != doc:
		print('update:', projectPath)
		with open(projectPath, 'w', encoding='utf-8') as fd:
			fd.write(updated)

def main():
	for path in glob.glob('../build/VS2017/*.vcxproj'):
		update_project_toolset(path)
	for path in glob.glob('../locale/*/*.vcxproj'):
		update_project_toolset(path)

main()
