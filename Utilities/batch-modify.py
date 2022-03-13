import os, sys
import struct

def getModifiedXml(xml):
    return xml.replace('followHost="0"', 'followHost="1"')

def unpack_modify_repack(patchFile):
    infile = open(patchFile, 'rb')
    xml = infile.read()[8:-1]
    infile.close()
    
    xml = getModifiedXml(xml)
    
    outfile = open(patchFile, 'wb')
    outfile.write('VC2!'.encode())
    outfile.write(struct.pack('<I', len(xml)))
    outfile.write(xml)
    outfile.write(b'\0')
    outfile.close()

for filename in os.listdir('.'):
    if filename.endswith('.unify'):
        unpack_modify_repack(filename)
