import sys

def unpack(unifyFile, xmlFile):
    infile = open(unifyFile, 'rb')
    data = infile.read()
    infile.close()
    
    outfile = open(xmlFile, 'wb')
    outfile.write(data[8:-1])
    outfile.close()
        
if __name__ == "__main__":
    if (len(sys.argv) != 3 or not sys.argv[1].endswith('.unify') or not sys.argv[2].endswith('.xml')):
        print('Usage: py %s <infile>.unify <outfile>.xml' % sys.argv[0])
    else:
        unpack(sys.argv[1], sys.argv[2])
