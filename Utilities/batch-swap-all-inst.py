import os

# hand-edited file contains only <Instrument...> ... </Instrument> tag
newInst = open('PigmentsInst.txt').read()

for f in os.listdir('XML'):
    xml = open(os.path.join('XML', f)).read()
    
    offset = 0
    while True:
        istart = xml.find('<Instrument', offset)
        if istart < 0:
            break
        iend = xml.find('</Instrument>', istart) + 13
        xml = xml[ : istart] + newInst + xml[iend : ]
        offset = istart + len(newInst)
    
    outf = open(os.path.join('XML', f), 'w');
    outf.write(xml)
    outf.close()
