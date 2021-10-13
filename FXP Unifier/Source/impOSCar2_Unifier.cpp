#include "impOSCar2_Unifier.h"

impOSCar2_Unifier::impOSCar2_Unifier()
{
    presetsRootFolder = File("C:\\Program Files\\GForce\\impOSCar2\\Patches\\Instrument");
    presetFileType = "fxp";
    refPatch = File("D:\\PlugInGuru\\Content Development\\GForce Software\\impOSCar2 Template.unify");

    libraryName = "Unified - GF impOSCar2";
    authorName = "GForce Software";
    sourceFolder = presetsRootFolder;
    patchComment = "impOSCar2 factory patches by GForce Software";
    outputFolder = unifyLibrariesFolder.getChildFile(libraryName).getChildFile("Patches");
}

static struct
{
    String folder;
    String prefix;
}
folderPrefix[] =
{
    { "Arpeggios", "BPM ARP" },
    { "Arpeggios Unison", "BPM ARP" },
    { "Bass", "BASS" },
    { "Bass Unison", "BASS" },
    { "Bells", "BELL" },
    { "Chords", "CHORD" },
    { "Duophonic", "" },
    { "Effects", "SFX" },
    { "Leads", "LEAD" },
    { "Leads Unison", "LEAD" },
    { "Mono", "" },
    { "Mono Unison", "" },
    { "Organ", "ORG" },
    { "OSCar Bank", "" },
    { "OSCar Bank Redux", "" },
    { "Pads", "PAD" },
    { "Pads Unison", "PAD" },
    { "Poly", "KEY" },
    { "Poly Unison", "KEY" },
    { "Sequences", "BPM SEQ" },
};

static struct
{
    String word;
    String prefix;
}
wordPrefix[] =
{
    { "Performance", "KEY" },
    { "Piano", "KEY" },
    { "Clav", "KEY" },
    { "Lead", "LEAD" },
    { "LEA", "LEAD" },
    { "ARP", "ARP" },
    { "Synth", "SYNTH" },
    { "SY", "SYNTH" },
    { "KEY", "KEY" },
    { "Pad", "PAD" },
    { "PAD", "PAD" },
    { "PD", "PAD" },
    { "Bass", "BASS" },
    { "BAS", "BASS" },
    { "Slap", "BASS" },
    { "Brass", "BRASS" },
    { "String", "STR" },
    { "Bell", "BELL" },
    { "Choir", "VOX" },
    { "Vox", "VOX" },
    { "Kalimba", "PLUCK" },
    { "Harp", "PLUCK" },
    { "PL", "PLUCK" },

    { "DDD-1", "DRUM KIT" },
    { "Drumulator", "DRUM KIT" },
    { "Guitar", "GTR" },
    { "Clarinet", "WIND" },
    { "Shakuhachi", "WIND" },
    { "Horn", "Brass" },
    { "Flute", "WIND" },
    { "Fuzz", "BASS" },
    { "Oboe", "WIND" },
    { "Trombone", "BRASS" },
    { "Trumpet", "BRASS" },
    { "Angels", "VOX" },
    { "Voice", "VOX" },
    { "Hammond", "ORG" },
    { "Organ", "ORG" },
    { "Drums", "DRUM KIT" },
    { "Violin", "STR" },
};

void impOSCar2_Unifier::getPatchDetailsAndVstState(File& sourcePresetFile, MemoryBlock& vstState)
{
    patchName = sourcePresetFile.getFileNameWithoutExtension();
    sourcePresetFile.loadFileAsData(vstState);

    prefix.clear();
    String folderName = sourcePresetFile.getParentDirectory().getFileName();
    for (auto fp : folderPrefix)
    {
        if (fp.folder == folderName)
            prefix = fp.prefix;
    }
    if (prefix.isEmpty()) for (auto fp : wordPrefix)
    {
        if (patchName.contains(fp.word))
            prefix = fp.prefix;
    }
    if (prefix.isEmpty())
    {
        prefix = "UNKNOWN";
        DBG(patchName);
    }
}
