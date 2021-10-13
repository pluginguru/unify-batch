import os

def fixPatchName(xmlFile):
    infile = open(xmlFile, 'r')
    xml = infile.read()
    infile.close()
    
    # isolate the patch name
    nameStart = xml.find("name=") + 6
    nameEnd = xml.find('"', nameStart)
    
    # compute the new name, in this case by lopping off the last 4 characters
    patchName = xml[nameStart : nameEnd - 4]
    
    # rewrite the XML patch, substituting the modified name
    outfile = open(xmlFile, 'w')
    outfile.write(xml[:nameStart] + patchName + xml[nameEnd:])
    outfile.close()

for xf in os.listdir('XML'):
    if xf.endswith('.xml'):
        fixPatchName(os.path.join('XML', xf))
