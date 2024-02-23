#pragma once
#include "PatchConverter.h"
#include "MyVstUtils.h"

static struct
{
    String inPrefix;
    String outPrefix;
}
prefixPrefix[] =
{
    { "An Overview", "DEMO" },
    { "Bars", "BAR" },
    { "Bass", "BASS" },
    { "Gadi Sassoon", "SASS" },
    { "Hacked Acoustics", "HACK" },
    { "Krazy Keys", "KRAZY" },
    { "Leads", "LEAD" },
    { "Low Register", "LREG" },
    { "Misc", "MISC" },
    { "Oddities", "ODD" },
    { "Pads", "PAD" },
    { "Percussive", "PERC" },
    { "Resonant Chords", "CHORD" },
    { "Soundscapes", "ATMO" },
    { "Strikes", "HIT" },
    { "Strings", "STRING" },
    { "Twisted Leads", "LEAD" }
};

PatchConverter::PatchConverter()
{
    //unifyPatchXml = parseXML(BinaryData::One_Derailer_Layer_xml);
    unifyPatchXml = parseXML(BinaryData::One_Modus_Layer_xml);
    //unifyPatchXml = parseXML(BinaryData::One_Preparation_Layer_xml);

    test();
}

void PatchConverter::test()
{
#if 0
    auto layerXml = unifyPatchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");

    MemoryBlock mb;
    bool b64ok = mb.fromBase64Encoding(stateInfoString);

    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());

    auto icXml = vst3stateXml->getChildByName("IComponent");
    auto icDataString = icXml->getAllSubText();
    mb.setSize(0);
    b64ok = mb.fromBase64Encoding(icDataString);

    auto presetXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    DBG(presetXml->toString());
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
    else if (file.getFileExtension() == ".papreset")
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processPresetFile(file, newPatchNameOrErrorMessage, 1 + fileCount));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
        else
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);

        fileCount++;
    }
}

XmlElement* PatchConverter::processPresetFile(File inFile, String& newPatchNameOrErrorMessage, int fileNumber)
{
#if 0
    return nullptr;
#else
    auto fileName = inFile.getFileNameWithoutExtension();
    auto presetPrefix = fileName.upToFirstOccurrenceOf(".", false, false);
    auto presetName = fileName.fromFirstOccurrenceOf(".", false, false);

    String patchPrefix = "UNKNOWN";
    for (auto& pp : prefixPrefix)
    {
        if (pp.inPrefix == presetPrefix)
        {
            patchPrefix = pp.outPrefix;
            break;
        }
    }

    auto patchName = patchPrefix + " - " + presetName;
    newPatchNameOrErrorMessage = patchName;

    // load preset file as XML
    auto presetXml = parseXML(inFile);

    // "currentpreset" int-attribute sets which preset is selected in Derailer GUI, but it's wrong in most presets
    presetXml->setAttribute("currentpreset", fileNumber);

    // convert to memory block, then to base64
    MemoryBlock mb;
    AudioProcessor::copyXmlToBinary(*presetXml, mb);
    auto b64state = mb.toBase64Encoding();

    // assemble a VST3PluginState XML structure, with this base64 state-string
    std::unique_ptr<XmlElement> vst3stateXml(new XmlElement("VST3PluginState"));
    auto iComponentXml = new XmlElement("IComponent");
    vst3stateXml->addChildElement(iComponentXml);
    iComponentXml->addTextElement(b64state);

    // now base64 encode that entire XML string, as new Theia binary state
    AudioProcessor::copyXmlToBinary(*vst3stateXml, mb);
    b64state = mb.toBase64Encoding();

    // assemble new Unify patch XML
    auto patchXml = new XmlElement(*unifyPatchXml); // deep copy
    auto layerXml = patchXml->getChildByName("Layer");
    layerXml->setAttribute("layerTitle", presetPrefix + " | " + presetName);
    auto instXml = layerXml->getChildByName("Instrument");
    instXml->setAttribute("stateInformation", b64state);
    auto pmXml = patchXml->getChildByName("PresetMetadata");
    pmXml->setAttribute("name", patchName);
    pmXml->setAttribute("author", "Physical Audio");
    pmXml->setAttribute("category", presetPrefix);
    //pmXml->setAttribute("tags", tags);
    if (patchPrefix == "SASS")
        pmXml->setAttribute("comment", "Factory presets by Gadi Sassoon");
    else
        pmXml->setAttribute("comment", "Factory presets by Physical Audio");
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
