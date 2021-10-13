# PatchRenamer
This program was split off from an earlier version of *LibraryRenamer*. Its purpose is to update the names of batches of Unify patches, adding PlugInGuru-standard name prefixes like "STR - ", "GTR - ", "BPM DRUM - ", etc. by applying some (typically table-driven) algorithm.

Because there are no standards for patch category-coding, every source patch-library is different, and so there is no universal algorithm for determining the right category for every patch automatically. Hence *this program is intended to serve as a template* for creating your own methods and expressing them in code.

## Analysis techniques

The hard part of the renaming task is analyzing whatever information is available in each source patch, in order to guess the most appropriate category (which is then used to select the prefix to prepend to the name).

In some cases, all that is available is the original patch name. If the sound designer had already used some kind of category-prefix system, the original prefixes can be used, although it's worth noting that different sound designers often have very different notions for what constitutes e.g. a "pad", "lead", or "synth" sound.

If category prefixes were not used, it is often possible to make a preliminary categorization guess by looking for key words such as "pad" in the patch name itself.

Recent versions of Unify have some built-in "metadata import" capability, to parse useful data (patch name, library name, author, etc.) out of the saved plug-in state. As this is written, this is mainly for *Omnisphere 2*. The imported metadata will get put directly into string attributes of the the *PresetMetadata* element in the Unify patch, and can be analyzed for clues about categorization.

When creating "unified" libraries by direct processing of a plug-in's own patch-files (see e.g. [TAL Unifier](../TAL Unifier/README.md)), contextual cues such as *patch-file name* and *parent folder name* may be available, in addition to whatever metadata can be parsed from the source patch. Hence, programs that process source patch-files this way will usually have their own integrated analysis/renaming code.

For some source patches, there is simply no way to guess the right category based on the only available information. In such cases, the analysis/renaming algorithm must be allowed to fail, leaving the original patch unchanged so it can be checked manually in Unify and saved under a new, appropriate name.

