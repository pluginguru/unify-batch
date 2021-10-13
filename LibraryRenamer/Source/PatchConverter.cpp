#include "PatchConverter.h"

PatchConverter::PatchConverter()
{
}

int PatchConverter::processFiles(const StringArray& filePaths)
{
    jassert(unifyRootFolderPath.isNotEmpty());
    unifyRootFolder = File(unifyRootFolderPath);

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
    if (file.isDirectory())
    {
        for (DirectoryEntry entry : RangedDirectoryIterator(file, false, "*", File::findFilesAndDirectories))
            processFile(entry.getFile(), fileCount);
    }
    else if (file.getFileExtension() == ".unify")
    {
        DBG(file.getFileName());

        MemoryBlock inBlock;
        file.loadFileAsData(inBlock);

        std::unique_ptr<XmlElement> xml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
        XmlElement* pmXml = xml->getChildByName("PresetMetadata");

        pmXml->setAttribute("library", libraryName);
        processPatchXml(xml.get(), false);

        MemoryBlock outBlock;
        AudioProcessor::copyXmlToBinary(*xml, outBlock);

        if (outputFolderPath.isNotEmpty())
        {
            // Save to new file
            File outFile = File(outputFolderPath).getChildFile(pmXml->getStringAttribute("name") + ".unify");
            outFile.create();
            outFile.replaceWithData(outBlock.getData(), outBlock.getSize());
        }
        else
        {
            // Overwrite original file
            file.replaceWithData(outBlock.getData(), outBlock.getSize());
        }

        fileCount++;
    }
}

void PatchConverter::processPatchXml(XmlElement* patchXml, bool embedded)
{
    forEachXmlChildElementWithTagName(*patchXml, layerXml, "Layer")
        processLayerXml(layerXml, embedded);
    forEachXmlChildElementWithTagName(*patchXml, layerXml, "AudioFxLane")
        processLayerXml(layerXml, embedded);
    forEachXmlChildElementWithTagName(*patchXml, layerXml, "MidiLane")
        processLayerXml(layerXml, embedded);
}

void PatchConverter::processLayerXml(XmlElement* layerXml, bool embedded)
{
    if (layerXml->getTagName() == "Layer")
    {
        // MIDI inserts on INST layer
        auto midiInsertsXml = layerXml->getChildByName("MidiInserts");
        forEachXmlChildElementWithTagName(*midiInsertsXml, insertXml, "InsertEffect")
        {
            auto pluginXml = insertXml->getChildByName("PLUGIN");
            String pluginName = pluginXml->getStringAttribute("name");
            String pluginFormat = pluginXml->getStringAttribute("format");

            if (pluginName == "MIDIBox" && pluginFormat == "Built-In")
            {
                String stateInfo = insertXml->getStringAttribute("stateInformation");
                stateInfo = convertMIDIBoxState(stateInfo);
                insertXml->setAttribute("stateInformation", stateInfo);
            }
            else if (pluginName == "ComboBox" && pluginFormat == "Built-In")
            {
                String stateInfo = insertXml->getStringAttribute("stateInformation");
                stateInfo = convertComboBoxState(stateInfo);
                insertXml->setAttribute("stateInformation", stateInfo);
            }
        }

#if 0   // Not needed yet, because no built-in audio effects require library paths
        // Audio inserts on INST layer
        auto audioInsertsXml = layerXml->getChildByName("AudioInserts");
        forEachXmlChildElementWithTagName(*audioInsertsXml, insertXml, "InsertEffect")
        {
            auto pluginXml = insertXml->getChildByName("PLUGIN");
            String pluginName = pluginXml->getStringAttribute("name");
            String pluginFormat = pluginXml->getStringAttribute("format");

            if (pluginName == "NoizBox" && pluginFormat == "Built-In")
            {
                String stateInfo = insertXml->getStringAttribute("stateInformation");
                stateInfo = convertNoizBoxState(stateInfo);
                insertXml->setAttribute("stateInformation", stateInfo);
            }
        }
#endif

        // Instrument plug-in
        auto instXml = layerXml->getChildByName("Instrument");
        auto pluginXml = instXml->getChildByName("PLUGIN");
        String pluginName = pluginXml->getStringAttribute("name");
        String pluginFormat = pluginXml->getStringAttribute("format");

        if (pluginName == "Unify" && pluginFormat == "Built-In")
        {
            String stateInfo = instXml->getStringAttribute("stateInformation");
            stateInfo = convertUnifyState(stateInfo);
            instXml->setAttribute("stateInformation", stateInfo);
        }

        if (pluginName == "ComboBox" && pluginFormat == "Built-In")
        {
            String stateInfo = instXml->getStringAttribute("stateInformation");
            stateInfo = convertComboBoxState(stateInfo);
            instXml->setAttribute("stateInformation", stateInfo);
        }

        if (pluginName == "Guru Sampler" && pluginFormat == "Built-In")
        {
            String stateInfo = instXml->getStringAttribute("stateInformation");
            stateInfo = convertGuruSamplerState(stateInfo);
            instXml->setAttribute("stateInformation", stateInfo);
        }
    }

    else if (layerXml->getTagName() == "MidiLane")
    {
        // MIDI inserts on MIDI layer
        auto midiInsertsXml = layerXml->getChildByName("MidiInserts");
        forEachXmlChildElementWithTagName(*midiInsertsXml, insertXml, "InsertEffect")
        {
            auto pluginXml = insertXml->getChildByName("PLUGIN");
            String pluginName = pluginXml->getStringAttribute("name");
            String pluginFormat = pluginXml->getStringAttribute("format");

            if (pluginName == "MIDIBox" && pluginFormat == "Built-In")
            {
                String stateInfo = insertXml->getStringAttribute("stateInformation");
                stateInfo = convertMIDIBoxState(stateInfo);
                insertXml->setAttribute("stateInformation", stateInfo);
            }
            else if (pluginName == "ComboBox" && pluginFormat == "Built-In")
            {
                String stateInfo = insertXml->getStringAttribute("stateInformation");
                stateInfo = convertComboBoxState(stateInfo);
                insertXml->setAttribute("stateInformation", stateInfo);
            }
        }
    }

#if 0   // Not needed yet, because no built-in audio effects require library paths
    else if (layerXml->getTagName() == "AudioFxLane")
    {
        auto audioInsertsXml = layerXml->getChildByName("AudioInserts");
        forEachXmlChildElementWithTagName(*audioInsertsXml, insertXml, "InsertEffect")
        {
            auto pluginXml = insertXml->getChildByName("PLUGIN");
            String pluginName = pluginXml->getStringAttribute("name");
            String pluginFormat = pluginXml->getStringAttribute("format");

            if (pluginName == "NoizBox" && pluginFormat == "Built-In")
            {
                String stateInfo = insertXml->getStringAttribute("stateInformation");
                stateInfo = convertNoizBoxState(stateInfo);
                insertXml->setAttribute("stateInformation", stateInfo);
            }
        }
    }
#endif
}

String PatchConverter::convertUnifyState(String stateInfo)
{
    MemoryBlock inBlock;
    inBlock.fromBase64Encoding(stateInfo);

    std::unique_ptr<XmlElement> xml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
    jassert(xml);

    processPatchXml(xml.get(), true);

    MemoryBlock outBlock;
    AudioProcessor::copyXmlToBinary(*xml, outBlock);

    return outBlock.toBase64Encoding();
}

String PatchConverter::convertComboBoxState(String stateInfo)
{
    MemoryBlock inBlock;
    inBlock.fromBase64Encoding(stateInfo);

    std::unique_ptr<XmlElement> xml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
    jassert(xml);

    forEachXmlChildElementWithTagName(*xml, filterXml, "FILTER")
    {
        auto pluginXml = filterXml->getChildByName("PLUGIN");
        if (pluginXml)
        {
            String pluginName = pluginXml->getStringAttribute("name");
            String pluginFormat = pluginXml->getStringAttribute("format");
            if (pluginFormat != "Built-In") continue;

            if (pluginName == "Unify")
            {
                auto stateXml = filterXml->getChildByName("STATE");
                String pluginState = convertUnifyState(stateXml->getAllSubText());
                stateXml->getFirstChildElement()->setText(pluginState);
            }
            if (pluginName == "ComboBox")
            {
                auto stateXml = filterXml->getChildByName("STATE");
                String pluginState = convertComboBoxState(stateXml->getAllSubText());
                stateXml->getFirstChildElement()->setText(pluginState);
            }
            if (pluginName == "Guru Sampler")
            {
                auto stateXml = filterXml->getChildByName("STATE");
                String pluginState = convertGuruSamplerState(stateXml->getAllSubText());
                stateXml->getFirstChildElement()->setText(pluginState);
            }
            else if (pluginName == "MIDIBox")
            {
                auto stateXml = filterXml->getChildByName("STATE");
                String pluginState = convertMIDIBoxState(stateXml->getAllSubText());
                stateXml->getFirstChildElement()->setText(pluginState);
            }
        }
    }

    MemoryBlock outBlock;
    AudioProcessor::copyXmlToBinary(*xml, outBlock);

    return outBlock.toBase64Encoding();
}

String PatchConverter::convertGuruSamplerState(String stateInfo)
{
    MemoryBlock inBlock;
    inBlock.fromBase64Encoding(stateInfo);

    std::unique_ptr<XmlElement> xml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
    jassert(xml);

    String libName = xml->getStringAttribute("osc1LibraryName");
    jassert(libName.isNotEmpty());

    if (xml->getStringAttribute("osc1LibraryName") != "Unify Standard Library")
        xml->setAttribute("osc1LibraryName", libraryName);

    MemoryBlock outBlock;
    AudioProcessor::copyXmlToBinary(*xml, outBlock);

    return outBlock.toBase64Encoding();
}

String PatchConverter::convertMIDIBoxState(String stateInfo)
{
    MemoryBlock inBlock;
    inBlock.fromBase64Encoding(stateInfo);

    std::unique_ptr<XmlElement> xml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
    jassert(xml);

    String libName = xml->getStringAttribute("libraryName");
    String midiFilePath = xml->getStringAttribute("midiFilePath").replace("\\", "/");

    if (xml->getStringAttribute("libraryName") != "Unify Standard Library")
    {
        xml->setAttribute("libraryName", libraryName);

        File midiRoot = unifyRootFolder.getChildFile("Libraries").getChildFile(libraryName).getChildFile("MIDI Files");
        String midiFileName;
        if (midiFilePath.startsWithChar('/'))
            midiFileName = midiFilePath.fromLastOccurrenceOf("/", false, false);
        else
        {
            File oldMidiFile = midiRoot.getChildFile(midiFilePath);
            midiFileName = oldMidiFile.getFileName();
        }

        for (DirectoryEntry entry : RangedDirectoryIterator(midiRoot, true, "*.mid", File::findFiles))
        {
            const File& file = entry.getFile();
            if (file.getFileName() == midiFileName)
            {
                xml->setAttribute("midiFilePath", file.getRelativePathFrom(midiRoot));
                break;
            }
        }
    }

    MemoryBlock outBlock;
    AudioProcessor::copyXmlToBinary(*xml, outBlock);

    return outBlock.toBase64Encoding();
}
