import sys

vertices=[]
face_index=[]
normal_index=[]
normals=[]

def get_xyz(line):
	tokens = line.split(' ')
	if (len(tokens) < 4):
		return None
	x = float(tokens[1].strip())
	y = float(tokens[2].strip())
	z = float(tokens[3].strip())
	return (x, y, z)

def get_face(line):
	tokens = line.split(' ')
	fi = []
	ni = []
	if (len(tokens) < 4):
		return None
	for token in tokens:
		ab = token.split('//')
		if (ab[0].startswith("f")):
			continue
		if (len(ab) == 1):
			fi.append(int(ab[0].strip())-1)
		elif (len(ab) == 2):
			fi.append(int(ab[0].strip())-1)
			ni.append(int(ab[1].strip())-1)
		else:
			return None
	return (fi, ni)


def write_xyz(file, varname, sufix, list, isToPrintCounts):
	count = 0
	n = len(list)

	if isToPrintCounts:
		file.write("GLuint %s_numberOfVertices=%s;\n\n"%(varname, n))

	file.write("GLfloat %s_%s[] = {\n"%(varname, sufix))
	for v in list:
		if count < (n-1):
			file.write("\t%f, %f, %f,\n"%v)
		else:
			file.write("\t%f, %f, %f\n"%v)
		count += 1
	file.write("};\n\n")

def write_index(file, varname, sufix, list, isToPrintCounts):

	n = len(list)
	if isToPrintCounts:
		file.write("GLuint %s_numberOfFaces=%s;\n\n"%(varname, n))

	file.write("GLuint %s_%s[] = {\n"%(varname, sufix))
	count = 0
	verticesInFace = 0
	for v in list:
		tv = tuple(v)
		verticesInFace = len(tv)
		if count < (n-1):
			if (len(tv) == 3):
				file.write("\t%i, %i, %i,\n"%tv)
			elif (len(tv)==4):
				file.write("\t%i, %i, %i, %i,\n"%tv)
		else:
			if (len(tv) == 3):
				file.write("\t%i, %i, %i\n"%tv)
			elif (len(tv) == 4):
				file.write("\t%i, %i, %i, %i\n"%tv)
		count += 1
	file.write("};\n\n")

	if isToPrintCounts:
		file.write("GLuint %s_verticesInFace=%s;\n\n"%(varname,verticesInFace))


def generate(file, varname):
	write_xyz(file, varname, "vertices", vertices, True)
	write_xyz(file, varname, "normals", normals, False)
	write_index(file, varname, "faceindex", face_index, True)
	write_index(file, varname, "normalindex", normal_index, False)

def main():
	argv = sys.argv
	argc = len(argv)

	outfilename = "model%s"
	filename = "model%s"

	if (argc > 1):
		filename = argv[1]+"%s"

	if (argc > 2):
		outfilename = argv[2]+"%s"

	outcppfile = open(outfilename%(".cpp"), 'wt')
	outhppfile = open(outfilename%(".hpp"), 'wt')

	outhppfile.write("#pragma once\n\n")
	
	#lines = [line.rstrip('\n') for line in open(filename)]
	lines = open(filename%(".obj"))
	erros = 0
	countline = 0
	for line in lines:
		line = line.strip()
		if (line.startswith('v ')):
			v = get_xyz(line)
			if (v):
				vertices.append(v)
			else:
				print("Invalid wavefront file: error on line: %s"%(countline))
		elif (line.startswith('vn')):
			n = get_xyz(line)
			if (n):
				normals.append(n)
			else:
				print("Invalid wavefront file: error on line: %s"%(countline))
		elif (line.startswith('f')):
			r = get_face(line)
			if (r) :
				(f, n) = r
				face_index.append(f)
				normal_index.append(n)
			else:
				print("Invalid wavefront file: error on line: %s"%(countline))

		countline = countline + 1

	lines.close()

	varbasename = outfilename%("")

	outhppfile.write("extern GLfloat %s_vertices[];\n"%(varbasename))
	outhppfile.write("extern GLfloat %s_normals[];\n"%(varbasename))
	outhppfile.write("extern GLuint %s_faceindex[];\n"%(varbasename))
	outhppfile.write("extern GLuint %s_normalindex[];\n"%(varbasename))

	generate(outcppfile, varbasename)
	outcppfile.close()
	outhppfile.close()



if __name__ == '__main__':
	main()
