# LibraryRenamer
This program was originally developed simply for renaming Unify patch libraries; hence the name.

Changing the name of a Unify patch is not a simple thing, because several plug-ins save the library name in their state-data blobs. Hence *LibraryRenamer* has to parse, update, and re-save these data blobs. Also, both *Unify* and the built-in *ComboBox* plug-in can contain other plug-ins, so the process must be recursive.

To understand this code, you need to learn about [the structure of Unify patch files](../Documents/Unify-patch-structure.md).

