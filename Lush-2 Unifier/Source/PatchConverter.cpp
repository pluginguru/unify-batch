#pragma once
#include "PatchConverter.h"

static struct
{
    String tag;
    String prefix;
    String category;
}
tagPrefixCategory[] =
{
    { "Keys", "KEY", "Keyboard" },
    { "Lead", "LEAD", "Lead" },
    { "Bass", "BASS", "Bass" },
    { "Guitar", "GTR", "Guitar" },
    { "Organ", "ORG", "Organ" },
    { "Pad", "PAD", "Pad" },
    { "String", "STR", "String" },
    { "Brass", "BRASS", "Brass" },
    { "Chord", "CHORD", "Chord" },
    { "Drum Kit", "DRUM KIT", "Drum" },
    { "Drums", "DRUM", "Drum" },
    { "Flute", "WIND", "Wind" },
    { "Fx", "FX", "FX" },
    { "Pluck", "PLUCK", "Pluck" },
    { "Vocal", "VOX", "Vocal" },
    { "Bell", "BELL", "Bell" },
    { "Analog", "SYNTH", "Synth" },
};

PatchConverter::PatchConverter()
{
    unifyPatchXml = parseXML(BinaryData::One_Lush2_Layer_xml);
    lush2stateXml = parseXML(BinaryData::Lush2state_xml);

    //test();
}

void PatchConverter::test()
{
    File assetsDir("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\Lush-2 Unifier\\Assets");

#if 0
    // tear the sample patch apart to create the Lush2-state.xml plugin-state XML file
    auto layerXml = unifyPatchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");

    MemoryBlock mb;
    bool b64ok = mb.fromBase64Encoding(stateInfoString);

    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    //vst3stateXml->writeTo(assetsDir.getChildFile("test.xml"));

    auto icDataString = vst3stateXml->getChildByName("IComponent")->getAllSubText();
    b64ok = mb.fromBase64Encoding(icDataString);
    //File icDataFile = assetsDir.getChildFile("ic.txt");
    //icDataFile.replaceWithData(mb.getData(), mb.getSize());

    auto presetXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    presetXml->writeTo(assetsDir.getChildFile("Lush2-state.xml"));
#endif

#if 1
    // test modifying the plugin-state XML to match a specific factory preset
    File inFile = assetsDir.getChildFile("Global Presets").getChildFile("3-Part Fugue 1.sh2glb");
    String presetName = inFile.getFileNameWithoutExtension();
    auto presetXml = parseXML(inFile);

    // add name attribute to preset XML
    presetXml->setAttribute("name", presetName);

    // get a fresh copy of lush2stateXml and swap in the modified preset XML
    std::unique_ptr<XmlElement> stateXml(new XmlElement(*lush2stateXml));
    auto psXml = stateXml->getChildByName("ParametersState");
    psXml->replaceChildElement(psXml->getChildByName("Preset"), presetXml.release());

    // also fix the <param name="Preset Name" entry
    auto npp = stateXml->getChildByName("NotPresetableParams")->getChildByName("Preset")->getChildByName("Parameters");
    npp->getChildByAttribute("name", "Preset Name")->setAttribute("value", presetName);

    stateXml->writeTo(assetsDir.getChildFile("test.xml"));
#endif
}

//static StringArray allTags;

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

    //allTags.sort(true);
    //for (auto tag : allTags) DBG(tag);

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
    else if (file.getFileExtension() == ".sh2glb")
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processPresetFile(file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
#if 1
        else
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);
#endif

        fileCount++;
    }
}

XmlElement* PatchConverter::processPresetFile(File inFile, String& newPatchNameOrErrorMessage)
{
    String presetName = inFile.getFileNameWithoutExtension();

    auto presetXml = parseXML(inFile);

#if 0
    StringArray pTags = StringArray::fromTokens(presetXml->getStringAttribute("tags"), "|", "");
    for (auto tag : pTags) allTags.addIfNotAlreadyThere(tag);
    return nullptr;
#endif

    String author = presetXml->getStringAttribute("author");
    String tags = presetXml->getStringAttribute("tags").replace("|", ", ");

    // load preset file as text and parse the metadata
    String prefix = "UNKNOWN";
    String category;
    if (presetName == "Organ Chords")
    {
        prefix = "ORG";
        category = "Organ, Chord";
    }
    else if (presetName == "- Init -")
    {
        prefix = "TEMPLATE";
        category = "Template";
    }
    else for (auto& tpc : tagPrefixCategory)
    {
        if (tags.containsIgnoreCase(tpc.tag))
        {
            prefix = tpc.prefix;
            category = tpc.category;

            if (!prefix.contains("SPLIT") && tags.containsIgnoreCase("Split"))
                prefix = prefix + " SPLIT";
            if (!prefix.startsWith("BPM") && tags.containsIgnoreCase("Arpeggio"))
                prefix = "BPM ARP";
            if (!prefix.startsWith("BPM") && tags.containsIgnoreCase("Sequence"))
                prefix = "BPM " + prefix;

            break;
        }
    }
    newPatchNameOrErrorMessage = prefix + " - " + presetName;

    // add name attribute to preset XML
    presetXml->setAttribute("name", presetName);

    // get a fresh copy of lush2stateXml and swap in the modified preset XML
    std::unique_ptr<XmlElement> stateXml(new XmlElement(*lush2stateXml));
    auto psXml = stateXml->getChildByName("ParametersState");
    psXml->replaceChildElement(psXml->getChildByName("Preset"), presetXml.release());

    // also fix the <param name="Preset Name" entry
    auto npp = stateXml->getChildByName("NotPresetableParams")->getChildByName("Preset")->getChildByName("Parameters");
    npp->getChildByAttribute("name", "Preset Name")->setAttribute("value", presetName);

    // convert state XML to binary and convert to base64
    MemoryBlock mb;
    AudioProcessor::copyXmlToBinary(*stateXml, mb);
    auto b64state = mb.toBase64Encoding();

    // assemble a VST3PluginState XML structure, with this base64 state-string
    std::unique_ptr<XmlElement> vst3stateXml(new XmlElement("VST3PluginState"));
    auto iComponentXml = new XmlElement("IComponent");
    vst3stateXml->addChildElement(iComponentXml);
    iComponentXml->addTextElement(b64state);

    // now base64 encode that entire XML string, as new binary state
    AudioProcessor::copyXmlToBinary(*vst3stateXml, mb);
    b64state = mb.toBase64Encoding();

    // assemble new Unify patch XML
    auto patchXml = new XmlElement(*unifyPatchXml); // deep copy
    auto layerXml = patchXml->getChildByName("Layer");
    layerXml->setAttribute("layerTitle", newPatchNameOrErrorMessage);
    auto instXml = layerXml->getChildByName("Instrument");
    instXml->setAttribute("stateInformation", b64state);
    auto pmXml = patchXml->getChildByName("PresetMetadata");
    pmXml->setAttribute("name", newPatchNameOrErrorMessage);
    pmXml->setAttribute("author", author.isEmpty() ? "D16 Group" : author);
    pmXml->setAttribute("category", category);
    pmXml->setAttribute("tags", tags);
    pmXml->setAttribute("comment", "Factory presets by D16 Group");
    if (libraryName.isNotEmpty()) pmXml->setAttribute("library", libraryName);

    return patchXml;
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
