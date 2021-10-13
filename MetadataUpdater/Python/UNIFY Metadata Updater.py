import os
import glob
from tkinter import *
from tkinter import filedialog
import xml.etree.ElementTree as Et


def getfolder():
    global folder_selected, nopatches
    
    files = filedialog.askopenfilenames(parent=window, title='Select files')
    if files:
        folder_selected = os.path.dirname(os.path.abspath(files[0]))
        nopatches = len(files)
        unpack(files)
        folderPath.set(folder_selected + ' (' + str(len(files)) + ')')
        btnSubmit.config(state=NORMAL)
        eAuthor.config(state=DISABLED)
        eCategories.config(state=DISABLED)
        eTags.config(state=DISABLED)
        eComment.config(state=DISABLED)
        chkAuthor.config(state=NORMAL)
        chkCategories.config(state=NORMAL)
        chkTags.config(state=NORMAL)
        chkComment.config(state=NORMAL)
        chkRenameLayer.config(state=NORMAL)


def getvalue():
    global nopatches
    if upd_Author.get() == 1 or upd_Categories.get() == 1 or upd_Tags.get() == 1 or \
            upd_Comment.get() == 1 or ren_Layer.get() == 1:
        list_files()
        repack_all()
        btnSubmit.config(state=DISABLED)
        lblResult.config(text=str(nopatches) + ' patches in ' + folder_selected + ' updated!')
    else:
        lblResult.config(text='No fields selected for update!')


def chkrenamelayer_status():
    if ren_Layer.get() == 0:
        chkRenameLayer.config(state=DISABLED)
    else:
        chkRenameLayer.config(state=NORMAL)


def chkauthor_status():
    if upd_Author.get() == 0:
        eAuthor.delete(0, END)
        eAuthor.config(state=DISABLED)
    else:
        eAuthor.config(state=NORMAL)


def chkcategories_status():
    if upd_Categories.get() == 0:
        eCategories.delete(0, END)
        eCategories.config(state=DISABLED)
    else:
        eCategories.config(state=NORMAL)


def chktags_status():
    if upd_Tags.get() == 0:
        eTags.delete(0, END)
        eTags.config(state=DISABLED)
    else:
        eTags.config(state=NORMAL)


def chkcomment_status():
    if upd_Comment.get() == 0:
        eComment.delete(1.0, END)
        eComment.config(state=DISABLED)
    else:
        eComment.config(state=NORMAL)


def cancel_run():
    if folder_selected:
        for xml_file in glob.glob(folder_selected + '/**/*.xml', recursive=True):
            os.remove(xml_file)
    window.destroy()
    sys.exit()


def unpack(filelist):
    patchlist = []
    # noPatches = 0
    for file in filelist:
        infile = open(file, 'rb')
        data = infile.read()
        infile.close()
        patchlist.append(file[:-6])
        xmlfile = file[:-6] + '.xml'
        outfile = open(xmlfile, 'wb')
        outfile.write(data[8:-1])
        outfile.close()


def repack_all():
    #    library_repacked = folder_selected
    for filename in os.listdir(folder_selected):
        if filename.endswith(".xml"):
            (file_out, ext) = os.path.splitext(filename)
            file_out = file_out + '.unify'
            repack(os.path.join(folder_selected, filename), os.path.join(folder_selected, file_out))
    
    for xml_file in glob.glob(folder_selected + '/**/*.xml', recursive=True):
        os.remove(xml_file)


def repack(xmlfile, unifyfile):
    infile = open(xmlfile, 'rb')
    data = infile.read()
    infile.close()
    
    outfile = open(unifyfile, 'wb')
    outfile.write('VC2!'.encode())
    outfile.write((len(data)).to_bytes(4, byteorder='little'))
    outfile.write(data)
    outfile.write(b'\0')
    outfile.close()


def list_files():
    data_folder = folder_selected
    
    filecount = 0
    for xml_file in glob.glob(data_folder + '/**/*.xml', recursive=True):
        xml_doc = Et.parse(xml_file)
        root = xml_doc.getroot()
        patchname = xml_doc.find('./PresetMetadata').attrib['name']
        
        for elem in root.iter('PresetMetadata'):
            if upd_Author.get() == 1:
                elem.set('author', new_Author.get())
            if upd_Categories.get() == 1:
                elem.set('category', new_Categories.get())
            if upd_Tags.get() == 1:
                elem.set('tags', new_Tags.get())
            if upd_Comment.get() == 1:
                elem.set('comment', eComment.get(1.0, END))
        
        if ren_Layer.get() == 1:
            inx = 0
            for elem in root.iter('Layer'):
                if inx == 0:
                    print("Elem: " + str(elem))
                    elem.set('layerTitle', patchname)
                    inx = 1
        
        filecount = filecount + 1
        xml_doc.write(xml_file)


folder_selected = ''
workPath = ''
nopatches = 0
window = Tk()
window.title("UNIFY Patch Editor - Batch")
w = 900
h = 500
ws = window.winfo_screenwidth()
hs = window.winfo_screenheight()
x = (ws / 2) - (w / 2)
y = (hs / 2) - (h / 2)
window.geometry('%dx%d+%d+%d' % (w, h, x, y))

# Init vars
new_Author = StringVar()
new_Categories = StringVar()
new_Tags = StringVar()
old_Library = ''
old_Author = ''
old_Categories = ''
old_Tags = ''
old_Comment = ''
upd_Author = IntVar()
upd_Categories = IntVar()
upd_Tags = IntVar()
upd_Comment = IntVar()
ren_Layer = IntVar()
folderPath = StringVar()

lEmpty = Label(window, pady=10, text='   ', width=100, anchor=W)
lEmpty.place(relx=0.0, rely=0.0)

btn_getFolder = Button(window, text='Get folder', fg='White', bg='Grey', bd=2, width=12, command=getfolder)
btn_getFolder.place(relx=0.05, rely=0.03)
lFolder = Label(window, textvariable=folderPath, pady=10, width=100, anchor=W)
lFolder.place(relx=0.2, rely=0.03)

# Rename Layer 1
chkRenameLayer = Checkbutton(window, variable=ren_Layer, onvalue=1, offvalue=0,
                             state=DISABLED, command=chkrenamelayer_status)
chkRenameLayer.place(relx=0.05, rely=0.1)
lRenameLayer = Label(window, text='Rename Layer 1 to Patch name')
lRenameLayer.place(relx=0.1, rely=0.1)

# Author
chkAuthor = Checkbutton(window, variable=upd_Author, onvalue=1, offvalue=0,
                        state=DISABLED, command=chkauthor_status)
chkAuthor.place(relx=0.05, rely=0.15)
lAuthor = Label(window, text='Author:')
lAuthor.place(relx=0.1, rely=0.15)
eAuthor = Entry(window, text=old_Author, textvariable=new_Author, width=100)
eAuthor.place(relx=0.2, rely=0.15)
eAuthor.delete(0, END)
eAuthor.insert(0, '')
eAuthor.config(state=DISABLED)

# Categories
chkCategories = Checkbutton(window, variable=upd_Categories, onvalue=1, offvalue=0, state=DISABLED,
                            command=chkcategories_status)
chkCategories.place(relx=0.05, rely=0.2)
lCategories = Label(window, text='Categories:')
lCategories.place(relx=0.1, rely=0.2)
eCategories = Entry(window, text=old_Categories, textvariable=new_Categories, width=100)
eCategories.place(relx=0.2, rely=0.2)
eCategories.delete(0, END)
eCategories.insert(0, '')
eCategories.config(state=DISABLED)

# Tags
chkTags = Checkbutton(window, variable=upd_Tags, onvalue=1, offvalue=0, state=DISABLED, command=chktags_status)
chkTags.place(relx=0.05, rely=0.25)
lTags = Label(window, text='Tags:')
lTags.place(relx=0.1, rely=0.25)
eTags = Entry(window, text=old_Tags, textvariable=new_Tags, width=100)
eTags.place(relx=0.2, rely=0.25)
eTags.delete(0, END)
eTags.insert(0, '')
eTags.config(state=DISABLED)

# Comment
chkComment = Checkbutton(window, variable=upd_Comment, onvalue=1, offvalue=0, state=DISABLED, command=chkcomment_status)
chkComment.place(relx=0.05, rely=0.3)
lComment = Label(window, text='Comment:')
lComment.place(relx=0.1, rely=0.3)
eComment = Text(window, height=8, width=70, state=DISABLED)
eComment.insert(END, old_Comment)
eComment.place(relx=0.2, rely=0.3)

lblResult = Label(window, text='', width=100, anchor=W)
lblResult.place(relx=0.15, rely=0.75)

btnSubmit = Button(window, text='Submit', fg='White', bg='Grey', bd=2, justify=CENTER, state=DISABLED, command=getvalue)
btnSubmit.place(rely=0.9, relx=0.3)
btnCancel = Button(window, text='Cancel', fg='White', bg='Grey', bd=2, justify=CENTER, command=cancel_run)
btnCancel.place(rely=0.9, relx=0.7)

window.mainloop()