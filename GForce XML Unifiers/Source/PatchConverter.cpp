#pragma once
#include "PatchConverter.h"

PatchConverter::PatchConverter()
{
    unifyPatchXml = parseXML(BinaryData::Unify_patch_xml);

    //test();
}

void PatchConverter::test()
{
    File assetsDir("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\GForce XML Unifiers\\Assets");

#if 1
    // tear the sample patch apart to create the plugin-state XML file
    auto layerXml = unifyPatchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");

    MemoryBlock mb;
    bool b64ok = mb.fromBase64Encoding(stateInfoString);

    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    vst3stateXml->writeTo(assetsDir.getChildFile("vst3state.xml"));

    auto icDataString = vst3stateXml->getChildByName("IComponent")->getAllSubText();
    b64ok = mb.fromBase64Encoding(icDataString);
    assetsDir.getChildFile("ic.txt").replaceWithData(mb.getData(), mb.getSize());

    // the state XML file is only needed to observe how it differs from preset XML files
    //auto presetXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    //presetXml->writeTo(assetsDir.getChildFile("Plugin State.xml"));
#endif
}

int PatchConverter::processFiles(const StringArray& filePaths)
{
#ifdef CHECK_CATEGORIES
    categories.clear();
#endif

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

#ifdef CHECK_CATEGORIES
    categories.sortNatural();
    for (auto c : categories) DBG(c);
#endif

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
    else if (file.getFileExtension() == ".xml")
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processPresetFile(file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
#ifndef CHECK_CATEGORIES
        else
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);
#endif

        fileCount++;
    }
}

XmlElement* PatchConverter::processPresetFile(File inFile, String& newPatchNameOrErrorMessage)
{
    auto presetXml = parseXML(inFile);
    presetXml->setTagName("STATE");

    auto metadataXml = presetXml->getChildByName("metadata");
#if defined(PLUGIN_IN_IMPOSCAR3)
    metadataXml->setAttribute("path", inFile.getFullPathName());
#endif
    String presetName = metadataXml->getStringAttribute("name");
    String author = metadataXml->getStringAttribute("author");
    String collection = metadataXml->getStringAttribute("collection");
    String notes = metadataXml->getStringAttribute("notes") + "\nCollection: " + collection;

#ifdef CHECK_CATEGORIES
    categories.addIfNotAlreadyThere(metadataXml->getStringAttribute("category"), true);
    return nullptr;
#endif

    String category, prefix;
    getCategoryAndPrefix(metadataXml->getStringAttribute("category"), category, prefix);
    newPatchNameOrErrorMessage = prefix + " - " + presetName;

    String types = metadataXml->getStringAttribute("types");
    String timbres = metadataXml->getStringAttribute("timbres");
    String tags = (types + "," + timbres)
        .trimCharactersAtEnd(",").trimCharactersAtStart(",")
        .replace(",,", ",");

#if defined(PLUGIN_IN_IMPOSCAR3)
    // at this point, presetXml is the state XML
    // convert state XML to binary and convert to base64
    MemoryBlock mb;
    AudioProcessor::copyXmlToBinary(*presetXml, mb);
    auto b64state = mb.toBase64Encoding();
#elif defined(PLUGIN_IS_AXXESS)
    auto pdXml = new XmlElement("processor_data");
    pdXml->setAttribute("GlobalTempo", "120.0");
    pdXml->setAttribute("isExtended", "1");
    pdXml->setAttribute("isBrowserVisible", "0");
    pdXml->setAttribute("ReverbLock", "0");
    pdXml->setAttribute("DelayLock", "0");
    pdXml->setAttribute("DistortionLock", "0");
    pdXml->setAttribute("bendRangeLock", "0");
    pdXml->setAttribute("ReverbPreset", "0");
    pdXml->setAttribute("SequencerSliderView", "0");
    pdXml->setAttribute("SequencerRecord", "0");
    pdXml->setAttribute("SequencerPlayMode", "0");
    pdXml->setAttribute("KeyboardHold", "0");
    presetXml->addChildElement(pdXml);

    MemoryBlock encodedXml;
    AudioProcessor::copyXmlToBinary(*presetXml, encodedXml);
    
    MemoryBlock mb(BinaryData::head_bin, BinaryData::head_binSize);
    int byteCount = encodedXml.getSize() + BinaryData::tail_binSize;
    uint8* bp = ((uint8*)mb.getData()) + 174;
    *bp++ = (byteCount >> 8) & 0xFF;
    *bp++ = byteCount & 0xFF;
    byteCount += 152;
    bp = ((uint8*)mb.getData()) + 22;
    *bp++ = (byteCount >> 8) & 0xFF;
    *bp++ = byteCount & 0xFF;

    mb.append(encodedXml.getData(), encodedXml.getSize());
    mb.append(BinaryData::tail_bin, BinaryData::tail_binSize);
    auto b64state = mb.toBase64Encoding();
#endif

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
    pmXml->setAttribute("author", author.isNotEmpty() ? author : "GForce Software");
    pmXml->setAttribute("category", category);
    pmXml->setAttribute("tags", tags);
    pmXml->setAttribute("comment", notes.isNotEmpty() ? notes : "Factory presets by GForce Software");
    pmXml->setAttribute("library", libraryName);

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

static struct { String catFromPreset, category, prefix; } categoryTable[] =
{
    { "Arpegs", "Arpeggio", "BPM ARP" },
    { "Sequences", "Sequence", "BPM SEQ" },
    { "Basses", "Bass", "BASS" },
    { "Bells", "Bell", "BELL" },
    { "Chords", "Chord", "CHORD" },
    { "Effects", "SFX", "SFX" },
    { "Keys", "Keyboard", "KEY" },
    { "Leads", "Lead", "LEAD" },
    { "Organ", "Organ", "ORGAN" },
    { "Pads", "Pad", "PAD" },
    { "Hits", "Drum", "DRUM" },
};

void PatchConverter::getCategoryAndPrefix(String categoryFromPreset, String& category, String& prefix)
{
    for (auto& cte : categoryTable)
    {
        if (cte.catFromPreset == categoryFromPreset)
        {
            category = cte.category;
            prefix = cte.prefix;
            return;
        }
    }

    category = "";
    prefix = "UNKNOWN";
}
