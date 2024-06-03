#pragma once
#include "PatchConverter.h"

PatchConverter::PatchConverter()
{
    unifyPatchXml = parseXML(BinaryData::Unify_patch_xml);

    test();
}

void PatchConverter::test()
{
    File assetsDir("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\Toybox Audio XML Unifiers\\Assets");

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
    auto presetXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    presetXml->writeTo(assetsDir.getChildFile("Plugin State.xml"));
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
    else if (file.getFileExtension() == ".preset")
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

static struct { String folderName, category, prefix; } categoryTable[] =
{
    { "Bass", "Bass", "BASS" },
    { "Bell And Pluck", "Pluck", "PLUCK" },
    { "Brass", "Brass", "BRASS" },
    { "Drums", "Drum", "DRUM" },
    { "FX", "SFX", "SFX" },
    { "High Low", "Hi-Lo", "HILO" },
    { "Hits", "Hit", "HIT" },
    { "Keys", "Keyboard", "KEY" },
    { "Lead", "Lead", "LEAD" },
    { "Loops", "Loop", "LOOP" },
    { "Pads", "Pad", "PAD" },
};

static void getCategoryAndPrefix(const String folderName, String& category, String& prefix)
{
    for (auto& cte : categoryTable)
    {
        if (cte.folderName == folderName)
        {
            category = cte.category;
            prefix = cte.prefix;
            return;
        }
    }

    DBG(folderName);
    category = "";
    prefix = "UNKNOWN";
}

XmlElement* PatchConverter::processPresetFile(File inFile, String& newPatchNameOrErrorMessage)
{
    String category, prefix;
    getCategoryAndPrefix(inFile.getParentDirectory().getFileName(), category, prefix);

    auto presetXml = parseXML(inFile);
    newPatchNameOrErrorMessage = prefix + " - " + inFile.getFileNameWithoutExtension();

#if 1
    // remove redundant WAV data which some presets contain
    for (auto chXml : presetXml->getChildIterator())
    {
        if (chXml->getTagName().startsWith("manager") && chXml->hasAttribute("base64Data"))
        {
            String path = chXml->getStringAttribute("path").replace("\\", "/");
            chXml->setAttribute("path",
                path.replace("C:/Users/rough/AppData/Local", "/Users/davidalexander/Library/Application Support"));
            chXml->setAttribute("base64Data", "");
        }
    }
#endif

    // at this point, presetXml is the state XML
    // convert state XML to binary and convert to base64
    MemoryBlock mb;
    AudioProcessor::copyXmlToBinary(*presetXml, mb);
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
    //pmXml->setAttribute("author", author.isNotEmpty() ? author : "GForce Software");
    pmXml->setAttribute("category", category);
    //pmXml->setAttribute("tags", tags);
    //pmXml->setAttribute("comment", notes.isNotEmpty() ? notes : "Factory presets by GForce Software");
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
