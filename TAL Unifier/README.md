# TAL Unifier

Patch files for [TAL Software](https://tal-software.com/) plug-ins all use a straightforward XML format, wrapped with the [same binary header/trailer as we use for Unify patch files](../Utilities/README.md). Furthermore, all of these plug-ins also use essentially the same format for saving/restoring their state (which is what gets saved in [Unify patch files](../Documents/Unify-patch-structure.md)).

This leads to the notion of creating "unified" libraries for TAL plug-ins by direct processing of the factory patch files, rather than the usual manual method of loading and saving patches using Unify itself.

Because each set of instrument-specific patch files differs in many ways, I have structured this program not as a single, finished app, but rather a *template* for creating similar apps.

## Basic approach

This *TAL Unifier* app is intended to be a template illustrating this basic approach:

- The generic algorithm goes into an abstract base class *TAL_Unifier*, which has one pure-virtual function called *makeUnifiedPatch()* which must be overridden to do the actual patch processing.
- Concrete derived classes are written for specific instruments:
  - *Sampler_Unifier* for [TAL-Sampler](https://tal-software.com/products/tal-sampler).
  - *BassLine_Unifier* for [TAL-BassLine-101](https://tal-software.com/products/tal-bassline-101)
  - *J8_Unifier* for [TAL-J-8](https://tal-software.com/products/tal-j-8)
  - *UnoLX_Unifier* for [TAL-U-NO-LX](https://tal-software.com/products/tal-u-no-lx)

The *Main.cpp* file is modified to select which of these concrete subclasses is instantiated.

### Manual editing is anticipated

Automatic Unify patch generation is only intended as the *first step of a multi-step process*, which I expect will be followed by multiple phases of manual editing/renaming, and possibly other automated batch operations.

### Use of a reference patch

Automatic generation of complete Unify patch files is simplified by starting with a *reference patch*. This will normally consist of a single INST layer, with an instance the target instrument plug-in. Whether or not any additional layers, macro definitions, etc. are used is up to the person using the code, but note that the existing [PatchReformer](../PatchReformer) app can be used to add such details later.

### One output library per *group* of source patches

Factory patches for TAL instruments tend to be divided into separate groups or collections, usually as these are contributed by separate authors. For *Sampler_Unifier* I tried processing all of these at once, to generate a single *Unified - TAL-Sampler* library, but this proved difficult, as the various patch collections differed in so many ways. This led to the subclass approach which I used for the other instruments, and I realized that it would be more practical to generate a separate unified library for each collection, to keep them separate during further manual editing steps, because the existing [LibraryRenamer](../LibraryRenamer) app could be used to change them all to a common name later.

### Table-based methods to generate patch-name prefixes

Each of the concrete classes implements a table-driven technique for generating preliminary patch-name prefixes (e.g., "KEY - ", "PAD - ", etc.) In most cases, there are two tables: First, the name of each patch's enclosing folder is used, as in many libraries this is named for the basic category. Second, if this first pass fails (as it always will for libraries without sub-folders), a second pass is made, based on looking for key words in each patch's name. If both fail, a generic prefix such as "UNKNOWN - " is used, indicating that the patch must be auditioned and renamed manually.

Once the basic algorithm was finalized, making these folder-name and key-word tables proved to be by far the biggest part of the work.

### Equalizing patch volume levels

All of the TAL plug-in patch formats are XML-based, and include a "volume" attribute. The *TAL_Unifier* class includes a *volumeScaling* variable, which is initialized to 1.0 but can be set in subclasses, which is used to scale the original values, in an attempt to equalize the volume levels of different patch collections.

## Thoughts on unifying audio-effect plug-ins

As I write, the *TAL Unifier* only creates unified libraries for TAL *instrument* plug-ins. However, TAL also offers a handful of nice *audio effect* plug-ins, each of which comes with a quite small number of factory presets, and it occurs to me that these could also be unified in the form of *AUX Layer presets* for Unify. The basic processing technique would be very similar, with a slightly simpler output step because these are plain XML files.

