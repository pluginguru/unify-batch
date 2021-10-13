import os, sys
import struct

def repack(xmlFile, unifyFile):
    infile = open(xmlFile, 'rb')
    data = infile.read()
    infile.close()
    
    outfile = open(unifyFile, 'wb')
    outfile.write('VC2!'.encode())
    outfile.write(struct.pack('<I', len(data)))
    outfile.write(data)
    outfile.write(b'\0')
    outfile.close()

if not os.path.exists('Patches'):
    os.mkdir('Patches')

for xf in os.listdir('XML'):
    if xf.endswith('.xml'):
        repack(os.path.join('XML', xf), os.path.join('Patches', xf[:-4] + '.unify'))
