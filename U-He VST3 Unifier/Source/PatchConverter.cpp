#pragma once
#include "PatchConverter.h"

static struct
{
    String presetPrefix;
    String patchPrefix;
    String patchCategory;
}
prefixPrefixCategory[] =
{
    { "BS - ", "BASS", "Bass" },
    { "CH - ", "CHORD", "Chord" },
    { "DR - ", "DRUM", "Drum" },
    { "FX - ", "SFX", "SFX" },
    { "KY - ", "KEY", "Keyboard" },
    { "LD - ", "LEAD", "Lead" },
    { "PD - ", "PAD", "Pad" },
    { "PL - ", "PLUCK", "Pluck" },
    { "SQ - ", "BPM SEQ", "Sequence" },
    { "SY - ", "SYNTH", "Synth" },
};

static struct
{
    String word;
    String prefix;
    String category;
}
wordPrefixCategory[] =
{
    { "Other:Templates", "TEMPLATE", "Template"},
    { "Seq+Arp:Bass", "BPM BASS", "BPM Bass" },
    { "Seq+Arp:Plucks", "BPM PLUCK", "BPM Pluck"},
    { "Seq+Arp:Melodic", "BPM SEQ", "BPM Sequence" },
    { "Drums:Percussion", "PERC", "Percussion" },

    { "Drums", "DRUM", "Drum" },
    { "Guitars", "GTR", "Guitar" },
    { "Bass", "BASS", "Bass" },
    { "Brass", "BRASS", "Brass" },
    { "Hits", "HIT", "Hit" },
    { "Mallets", "PERC", "Percussion" },
    { "Plucks", "PLUCK", "Pluck" },
    { "Strings", "STRING", "String" },

    { "FX", "FX", "FX" },
    { "Pads", "PAD", "Pad" },
    { "Leads", "LEAD", "Lead" },
    { "Keys", "KEY", "Keyboard" },
};

PatchConverter::PatchConverter()
{
    unifyPatchXml = parseXML(BinaryData::One_Diva_Layer_xml);

    //test();
}

void PatchConverter::test()
{
    File assetsFolder("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\U-He VST3 Unifier\\Assets");
#if 1
    auto layerXml = unifyPatchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");

    MemoryBlock mb;
    bool b64ok = mb.fromBase64Encoding(stateInfoString);

    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());

    //File outFile = assetsFolder.getChildFile("test.xml");
    //vst3stateXml->writeTo(outFile);

    auto icDataString = vst3stateXml->getChildByName("IComponent")->getAllSubText();
    b64ok = mb.fromBase64Encoding(icDataString);
    File icDataFile = assetsFolder.getChildFile("ic.txt");
    icDataFile.replaceWithData(mb.getData(), mb.getSize());

    auto ecDataString = vst3stateXml->getChildByName("IEditController")->getAllSubText();
    b64ok = mb.fromBase64Encoding(ecDataString);
    File ecDataFile = assetsFolder.getChildFile("ec.txt");
    ecDataFile.replaceWithData(mb.getData(), mb.getSize());
#endif

#if 0
    MemoryBlock mb;
    File unifyPatchFile("E:\\PlugInGuru\\Unify\\Libraries\\Unified - PA Derailer\\Patches\\ATMO - Gazer.unify");
    unifyPatchFile.loadFileAsData(mb);
    auto patchXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());

    auto layerXml = patchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");
    bool b64ok = mb.fromBase64Encoding(stateInfoString);
    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    auto icXml = vst3stateXml->getChildByName("IComponent");
    auto icDataString = icXml->getAllSubText();
    mb.setSize(0);
    b64ok = mb.fromBase64Encoding(icDataString);
    auto presetXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    DBG(presetXml->toString());
#endif
}

int PatchConverter::processFiles(const StringArray& filePaths)
{
    if (outputFolderPath.isNotEmpty())
    {
        outputFolder = File(outputFolderPath);
        outputFolder.createDirectory();
    }

    int fileCount = 0;
    for (auto filePath : filePaths)
    {
        File file(filePath);
        processFile(file, fileCount);
    }
    return fileCount;
}

void PatchConverter::processFile(File file, int& fileCount)
{
    DBG(file.getFileName());

    if (file.isDirectory())
    {
        for (DirectoryEntry entry : RangedDirectoryIterator(file, false, "*", File::findFilesAndDirectories))
            processFile(entry.getFile(), fileCount);
    }
    else if (file.getFileExtension() == ".h2p")
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processPresetFile(file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
        else
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);

        fileCount++;
    }
}

XmlElement* PatchConverter::processPresetFile(File inFile, String& newPatchNameOrErrorMessage)
{
#if 0
    return nullptr;
#else
    String presetName = inFile.getFileNameWithoutExtension();

    // load preset file as text and parse the metadata
    String presetStr = inFile.loadFileAsString();
    String author = presetStr.fromFirstOccurrenceOf("Author:\n'", false, false).upToFirstOccurrenceOf("'", false, false);
    String comment = presetStr.fromFirstOccurrenceOf("Description:\n'", false, false).upToFirstOccurrenceOf("'", false, false);
    String usage = presetStr.fromFirstOccurrenceOf("Usage:\n'", false, false).upToFirstOccurrenceOf("'", false, false);
    if (usage.isNotEmpty()) comment += "\n" + usage;
    comment = comment.replace("\\r\\n", "\n");
    String categories = presetStr.fromFirstOccurrenceOf("Categories:\n'", false, false).upToFirstOccurrenceOf("'", false, false);
    String tags = categories;
    String features = presetStr.fromFirstOccurrenceOf("Features:\n'", false, false).upToFirstOccurrenceOf("'", false, false);
    if (features.isNotEmpty()) tags += ", " + features;
    String character = presetStr.fromFirstOccurrenceOf("Character:\n'", false, false).upToFirstOccurrenceOf("'", false, false);
    if (character.isNotEmpty()) tags += ", " + character;
    String prefix = "UNKNOWN";
    String category = "UNKNOWN";
#if 1
    // Saif Sameer uses 2-character prefixes for categories
    int numPrefixCharsToRemove = 0;
    for (auto& ppc : prefixPrefixCategory)
    {
        if (presetName.startsWith(ppc.presetPrefix))
        {
            prefix = ppc.patchPrefix;
            category = ppc.patchCategory;
            numPrefixCharsToRemove = ppc.presetPrefix.length();
            break;
        }
    }
    newPatchNameOrErrorMessage = prefix + " - " + presetName.substring(numPrefixCharsToRemove);
#else
    for (auto& wpc : wordPrefixCategory)
    {
        if (categories.containsIgnoreCase(wpc.word))
        {
            prefix = wpc.prefix;
            category = wpc.category;
            break;
        }
    }
    newPatchNameOrErrorMessage = prefix + " - " + presetName;
#endif

    // load preset file as data and convert to base64
    MemoryBlock mb;
#if 1
    // Diva requires a prefix identifying the preset file
    String prefixString = "#pgm=" + inFile.getFileName() + "\n";
    MemoryBlock prefixBlock(prefixString.toUTF8(), prefixString.getNumBytesAsUTF8());
    MemoryBlock presetBlock;
    inFile.loadFileAsData(presetBlock);
    mb.append(prefixBlock.getData(), prefixBlock.getSize());
    mb.append(presetBlock.getData(), presetBlock.getSize());
#else
    inFile.loadFileAsData(mb);
#endif
    auto b64state = mb.toBase64Encoding();

    // assemble a VST3PluginState XML structure, with this base64 state-string
    std::unique_ptr<XmlElement> vst3stateXml(new XmlElement("VST3PluginState"));
    auto iComponentXml = new XmlElement("IComponent");
    vst3stateXml->addChildElement(iComponentXml);
    iComponentXml->addTextElement(b64state);
    //auto eComponentXml = new XmlElement("IEditController");
    //vst3stateXml->addChildElement(eComponentXml);
    //eComponentXml->addTextElement(b64state);

    // now base64 encode that entire XML string, as new binary state
    AudioProcessor::copyXmlToBinary(*vst3stateXml, mb);
    b64state = mb.toBase64Encoding();

    // assemble new Unify patch XML
    auto patchXml = new XmlElement(*unifyPatchXml); // deep copy
    auto layerXml = patchXml->getChildByName("Layer");
    layerXml->setAttribute("layerTitle", presetName);
    auto instXml = layerXml->getChildByName("Instrument");
    instXml->setAttribute("stateInformation", b64state);
    auto pmXml = patchXml->getChildByName("PresetMetadata");
    pmXml->setAttribute("name", newPatchNameOrErrorMessage);
    pmXml->setAttribute("author", author.isEmpty() ? "U-He" : author);
    pmXml->setAttribute("category", category);
    pmXml->setAttribute("tags", tags);
    pmXml->setAttribute("comment", comment);
    if (libraryName.isNotEmpty()) pmXml->setAttribute("library", libraryName);

    return patchXml;
#endif
}

void PatchConverter::saveUnifyPatch(File inFile, String patchName, XmlElement* patchXml)
{
    MemoryBlock outBlock;
    AudioProcessor::copyXmlToBinary(*patchXml, outBlock);

    if (outputFolderPath.isNotEmpty())
    {
        // Save to output folder
        File outFile = File(outputFolderPath).getChildFile(patchName + ".unify");
        outFile.create();
        outFile.replaceWithData(outBlock.getData(), outBlock.getSize());
    }
}
