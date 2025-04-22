#include "KorgPatchFinalizer.h"

// I used DB Browser for Sqlite to export the SoundDatabase table from the Korg "inspiration.db" database
// to a tag-separated-values file called SoundDatabase.tsv, then stripped off the first line (which contains
// the field names, and manually converted them into the following enum declaration.
enum fieldNames
{
    ID = 0,
    UUID,
    DataType,
    Name,
    FilePath,
    ReferenceUUID,
    Genre,
    InstrumentMainCategory,
    InstrumentSubCategory,
    Author,
    SongTitle,
    SongArtist,
    CreatedDateTime,
    LastModifiedDateTime,
    LastAccessedDateTime,
    TotalPlayTime,
    Tag,
    Rating,
    Collection,
    IndexInCollection,
    PCMMemorySize,
    SystemMemorySize,
    Color,
    Note,
    IsInProgramBank,
    BankAndProgram,
    Product,
    AdditionalType,
    WriteProtected
};

KorgPatchFinalizer::KorgPatchFinalizer()
    : PatchConverter()
{
    File assetsFolder("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\Korg Unified Patch Finalizer\\Assets");
    File tsvFile = assetsFolder.getChildFile("MultiPoly.tsv");
    StringArray tsvLines;
    tsvFile.readLines(tsvLines);

    String curLine;
    for (auto line : tsvLines)
    {
        auto num = line.getIntValue();
        if (num != 0)
        {
            if (curLine.isNotEmpty())
            {
                auto split = StringArray::fromTokens(curLine, "\t", "\"");
                tsvData.add(new StringArray(split));
            }
            curLine = line;
        }
        else
        {
            curLine += "\n" + line;
        }
    }
    if (curLine.isNotEmpty())
    {
        auto split = StringArray::fromTokens(curLine, "\t", "\"");
        tsvData.add(new StringArray(split));
    }
    DBG(String(tsvData.size()) + " Adjusted lines in TSV");

#if 0
    StringArray categories;
    for (auto record : tsvData)
    {
        if (isPositiveAndBelow<int>(fieldNames::InstrumentMainCategory, record->size()))
        {
            String catStr(record->getReference(fieldNames::InstrumentMainCategory));
            auto catList = StringArray::fromTokens(catStr, ";", "");
            for (auto cat : catList)
                if (cat.isNotEmpty())
                    categories.addIfNotAlreadyThere(cat, true);
        }
    }

    categories.sort(true);
    for (auto cat : categories) DBG(cat);
#endif
}

StringArray* KorgPatchFinalizer::lookupPatchName(String& name)
{
    for (auto record : tsvData)
        if (name == record->getReference(fieldNames::Name)) return record;
    return nullptr;
}

struct { const String instMainCategory, prefix; }
prefixTable[] =
{
    { "Bass", "BASS" },
    { "Bell/Mallet", "BELL" },
    { "Brass", "BRASS" },
    { "Drums", "DRUM" },
    { "E.Piano", "KEY" },
    { "Fast Synth", "SYNTH" },
    { "Gtr/Plucked", "PLUCK" },
    { "Lead", "LEAD" },
    { "Orchestral", "ORCH" },
    { "Organ", "ORGAN" },
    { "Pad", "PAD" },
    { "Percussion", "PERC" },
    { "Piano/Keys", "KEY" },
    { "Seq", "BPM SEQ" },
    { "SFX", "SFX" },
    { "Soundscape", "AMB" },
    { "Strings", "STR" },
    { "Synth", "SYNTH" },
    { "Vocal/Airy", "VOCAL" },
    { "Woodwind", "WIND" },

    { "Arpeggio", "BPM ARP" },
    { "Rhythm Hard", "BPM SEQ" },
    { "Rhythm Soft", "BPM SEQ" },

    // MultiPoly only
    { "Centering", "CENT" },
    { "Eastern", "EAST" },
    { "None", "NONE" },
    { "Simple", "SMPL" }
};

String lookupPrefix(String& cat)
{
    for (auto& entry : prefixTable)
        if (entry.instMainCategory == cat) return entry.prefix;
    return String();
}

void KorgPatchFinalizer::processPatchXml(XmlElement* patchXml)
{
    XmlElement* pmXml = patchXml->getChildByName("PresetMetadata");
    String name = pmXml->getStringAttribute("name");

    String prefix = "UNKNOWN";
    String authStr, noteStr, tagsStr, catStr;
    auto record = lookupPatchName(name);
    if (record)
    {
        authStr = record->getReference(fieldNames::Author);
        noteStr = record->getReference(fieldNames::Note);
        tagsStr = record->getReference(fieldNames::InstrumentSubCategory);

        authStr = authStr.unquoted().replace("\"\"", "\"");
        noteStr = noteStr.unquoted().replace("\n\n", "\n");

        catStr = record->getReference(fieldNames::InstrumentMainCategory);
        auto catList = StringArray::fromTokens(catStr, ";", "");
        for (auto cat : catList)
        {
            if (cat.isNotEmpty())
            {
                String pfx = lookupPrefix(cat);
                if (pfx.isNotEmpty())
                {
                    prefix = pfx;
                    break;
                }
            }
        }

#if 0
        if (name.startsWith(" Init"))
        {
            name = name.trim();
            prefix = "_INIT";
        }
        else if (prefix == "UNKNOWN")
        {
            if (name == "Synth Brass")
                prefix = "BRASS";
            else
                DBG(prefix);
        }
#endif

        pmXml->setAttribute("library", "Unified - Korg MultiPoly");
        pmXml->setAttribute("name", prefix + " - " + name);
        pmXml->setAttribute("category", catStr);
        pmXml->setAttribute("tags", tagsStr);
        pmXml->setAttribute("author", authStr);
        pmXml->setAttribute("comment", noteStr);

        auto inst1Xml = patchXml->getChildByName("Layer");
        inst1Xml->setAttribute("layerTitle", name);
    }
}
