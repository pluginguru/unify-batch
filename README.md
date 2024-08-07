# Batch processing of Unify patch files

[Unify](https://www.pluginguru.com/products/unify-standard) is a combination plug-in/host which allows creating combination sounds using multiple plug-ins, which can be saved as presets for quick recall, either in other DAW projects or in live performance. This repo documents Unify's patch format, and provides some open-source batch processing utilities for generating or modifying patch files automatically.

Unify's patch files (file type ".unify") are basically [XML](https://en.wikipedia.org/wiki/XML) files (with a binary header; see [Unify patch structure](Documents/Unify-patch-structure.md)), and there is nothing secret about the format. Hence these files are naturally amenable to being processed and even generated by programs. The code here provides some starting points.

There are currently two categories of programs here: *batch-processing utilities* and *automatic unified library generators*.

## Batch-processing utilities

### Utilities

The [Utilities](Utilities) folder contains Python tools for simple things like stripping and re-forming the binary header of .unify files.

### MetadataUpdater, LibraryRenamer

These are JUCE/C++ apps for changing *metadata* fields in patches--things like the patch name, author, tags and categories, etc. [MetadataUpdater](MetadataUpdater) allows changing the simpler items. [LibraryRenamer](LibraryRenamer) handles the much more intricate job of changing the library-name.

### PatchReformer, PatchRenamer

When preparing "unified" patch libraries, it's common to begin by making one very simple Unify patch for each "source patch" (preset of the plug-in being unified), then, as a separate step, update all those simple patches to have a common structure with added effects and macro definitions. [PatchReformer](PatchReformer) is a JUCE/C++ app for this second step. It "re-forms" a series of input patches, to conform to the structure of a single "reference patch".

### PatchRenamer

Another common issue which arises in preparing "unified" libraries is ensuring that all the Unify patches have meaningful prefixes (e.g. "STR - ", "GTR - ", "BPM DRUM - ", etc.). This can be done manually, but in some cases there may be enough clues available in e.g. the patch name, category, or tags (which may have been captured automatically by other means) to do at least a partial first-pass automatically.

[PatchRenamer](PatchRenamer) is intended as a *template* for creating multiple JUCE/C++ apps to do this, customized on an *ad-hoc* basis for each specific situation.

## Unified-library generators

"Unified" libraries are most commonly made using Unify itself, either fully manually, or using software tools like [AutoHotKey](https://www.autohotkey.com/) for partial automation. For some plug-ins, a completely different approach is feasible: processing the plug-in's own preset files to generate corresponding .unify files. [TAL Unifier](TAL%20Unifier/) is a template JUCE/C++ app for applying this approach for plug-ins from [TAL Software GmbH](https://tal-software.com).

To learn more about creating such "unifier" programs, see [this document](Documents/About-unifiers.md).
