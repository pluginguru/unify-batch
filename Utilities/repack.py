import sys
import struct

def repack(xmlFile, unifyFile):
    infile = open(xmlFile, 'rb')
    data = infile.read()
    infile.close()
    
    outfile = open(unifyFile, 'wb')
    outfile.write('VC2!'.encode())
    #outfile.write((len(data)).to_bytes(4, byteorder='little'))
    outfile.write(struct.pack('<I', len(data)))
    outfile.write(data)
    outfile.write(b'\0')
    outfile.close()

if __name__ == "__main__":
    if (len(sys.argv) != 3 or not sys.argv[1].endswith('.xml') or not sys.argv[2].endswith('.unify')):
        print('Usage: py %s input.xml output.unify' % sys.argv[0])
    else:
        repack(sys.argv[1], sys.argv[2])
