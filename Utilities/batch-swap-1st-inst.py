import os

# hand-edited file contains only <Instrument...> ... </Instrument> tag
newInst = open('PigmentsInst.txt').read()

for f in os.listdir('XML'):
    xml = open(os.path.join('XML', f)).read()
    
    istart = xml.find('<Instrument', offset)
    iend = xml.find('</Instrument>', istart) + 13
    
    outf = open(os.path.join('XML', f), 'w');
    outf.write(xml[ : istart] + newInst + xml[iend : ])
    outf.close()
