import os, sys

def unpack(unifyFile, xmlFile):
    infile = open(unifyFile, 'rb')
    data = infile.read()
    infile.close()
    
    outfile = open(xmlFile, 'wb')
    outfile.write(data[8:-1])
    outfile.close()

if not os.path.exists('XML'):
    os.mkdir('XML')

for uf in os.listdir('Patches'):
    if uf.endswith('.unify'):
        unpack(os.path.join('Patches', uf), os.path.join('XML', uf[:-6] + '.xml'))
