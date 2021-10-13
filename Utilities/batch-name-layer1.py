import os, sys
import struct

def name2Layer1Title(xmlFile):
    infile = open(xmlFile, 'r')
    xml = infile.read()
    infile.close()
    
    nameStart = xml.find("name=") + 6
    nameEnd = xml.find('"', nameStart)
    patchName = xml[nameStart : nameEnd]
    
    layerTitleStart = xml.find("layerTitle=") + 12
    layerTitleEnd = xml.find('"', layerTitleStart)
    
    outfile = open(xmlFile, 'w')
    outfile.write(xml[:layerTitleStart] + patchName + xml[layerTitleEnd:])
    outfile.close()

for xf in os.listdir('XML'):
    if xf.endswith('.xml'):
        name2Layer1Title(os.path.join('XML', xf))
