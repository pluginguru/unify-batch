# LibraryRenamer
This program was originally developed simply for renaming Unify patch libraries; hence the name.

Changing the name of a Unify patch is not a simple thing, because several plug-ins save the library name in their state-data blobs. Hence *LibraryRenamer* has to parse, update, and re-save these data blobs. Also, both *Unify* and the built-in *ComboBox* plug-in can contain other plug-ins, so the process must be recursive.

To understand this code, you need to learn about [the structure of Unify patch files](../Documents/Unify-patch-structure.md).

### GUI and usage

![LibraryRenamer](LibraryRenamer.png)

Enter the new library name you want in the "Library Name" text box.

If you would like the updated patch files saved to a different folder, click the "Output Folder" button and select the target folder. If you don't do this, the input files will be overwritten, so it's wise to make a safe copy first.

Unify patch files contain embedded references to sample files (for *Guru Sampler*) and MIDI files (for *MIDIBox*). *LibraryRenamer* normally updates these to be relative to your new library name, with one exception: any references to files in the *Unify Standard Library* are always left unchanged.

When renaming a library you have built for yourself, you may wish to disable these reference updates, because your patches may refer to files in arbitrary locations. The two checkboxes at the bottom of the GUI allow for this.
