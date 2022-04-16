import os

def clearComment(xmlFile):
    infile = open(xmlFile, 'r')
    xml = infile.read()
    infile.close()
    
    # isolate the patch comment text
    commentStart = xml.find("comment=") + 9
    commentEnd = xml.find('"', commentStart)
    
    # rewrite the XML patch, substituting the modified comment tag
    outfile = open(xmlFile, 'w')
    outfile.write(xml[:commentStart] + xml[commentEnd:])
    outfile.close()

for xf in os.listdir('XML'):
    if xf.endswith('.xml'):
        clearComment(os.path.join('XML', xf))
