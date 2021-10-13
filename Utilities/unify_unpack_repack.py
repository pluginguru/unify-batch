import sys
import os
import os.path
from os import path
import glob

def unpack(unifyFile, xmlFile):
    infile = open(unifyFile, 'rb')
    data = infile.read()
    infile.close()

    outfile = open(xmlFile, 'wb')
    outfile.write(data[8:-1])
    outfile.close()

def repack(xmlFile, unifyFile):
    infile = open(xmlFile, 'rb')
    data = infile.read()
    infile.close()

    outfile = open(unifyFile, 'wb')
    outfile.write('VC2!'.encode())
    outfile.write((len(data)).to_bytes(4, byteorder='little'))
    outfile.write(data)
    outfile.write(b'\0')
    outfile.close()

# Prompt for folder 

unifyfolder =""
while not path.exists(unifyfolder):
	unifyfolder = input("Folder with UNIFY patches as 'C:\Folder\Folder' or ctrl+c to quit: ")
	if not path.exists(unifyfolder):
		print("No such directory!")

print(unifyfolder + " contains " + str(len(glob.glob1(unifyfolder, "*.unify"))) + " UNIFY files")



# Prompt for option for run
runalternative = input('Enter option U = Unpack, P = Pack or Q = Quit: ')
runalternative = str.lower(runalternative)

# Unpack unify files
if (runalternative == "u"): 
	for filename in os.listdir(unifyfolder):
		if filename.endswith(".unify"):
			#print(Unify directory)
			(file_out, ext) = os.path.splitext(filename)
			file_out = file_out + '.xml'
			unpack(os.path.join(unifyfolder, filename), os.path.join(unifyfolder, file_out))
		else:
			continue
			
	print("Unpack ready!")

# Pack unify files
if (runalternative == "p"):
	for filename in os.listdir(unifyfolder):
		if filename.endswith(".xml"):
			(file_out, ext) = os.path.splitext(filename)
			file_out = file_out + '.unify'
			repack(os.path.join(unifyfolder, filename), os.path.join(unifyfolder, file_out))

	print("Repack ready")

# Prompt for removal of xml-files in folder?
removexml = input("Delete xml-files? (Y): ")
removexml = str.lower(removexml)
if removexml == "y":
	for item in os.listdir(unifyfolder):
		if item.endswith(".xml"):
			os.remove(os.path.join(unifyfolder, item))

print("Done!")