#include "PatchConverter.h"

PatchConverter::PatchConverter()
    : updateLayer1Title(false)
    , saveMidiFx(false), saveAudioFx(false)
{
}

int PatchConverter::processFiles(const StringArray& filePaths)
{
    if (outputFolderPath.isNotEmpty())
    {
        outputFolder = File(outputFolderPath);
        outputFolder.createDirectory();
    }

    File refPatchFile(referencePatchPath);
    MemoryBlock memoryBlock;
    refPatchFile.loadFileAsData(memoryBlock);
    refPatchXml = AudioProcessor::getXmlFromBinary(memoryBlock.getData(), memoryBlock.getSize());

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
        std::unique_ptr<XmlElement> inputPatchXml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
        XmlElement* pmXml = inputPatchXml->getChildByName("PresetMetadata");

        // Prepare outputPatchXml as a deep copy of refPatchXml
        std::unique_ptr<XmlElement> outputPatchXml(new XmlElement(*refPatchXml));

        // Swap (deep copy of) metadata from input patch into outputPatchXml
        std::unique_ptr<XmlElement> refPmXml(new XmlElement(*pmXml));
        outputPatchXml->replaceChildElement(outputPatchXml->getChildByName("PresetMetadata"), refPmXml.release());

        if (updateLayer1Title)
        {
            String patchName = outputPatchXml->getChildByName("PresetMetadata")->getStringAttribute("name");
            auto layer1Xml = outputPatchXml->getChildByName("Layer");
            layer1Xml->setAttribute("layerTitle", patchName);
        }

        // get state of INST1 instrument, replace into outputPatchXml
        auto inputInst1Xml = inputPatchXml->getChildByName("Layer")->getChildByName("Instrument");
        String inst1State = inputInst1Xml->getStringAttribute("stateInformation");
        int inst1Program = inputInst1Xml->getIntAttribute("currentProgram");
        auto outInst1Xml = outputPatchXml->getChildByName("Layer")->getChildByName("Instrument");
        outInst1Xml->setAttribute("stateInformation", inst1State);
        outInst1Xml->setAttribute("currentProgram", inst1Program);

        if (saveMidiFx)
        {
            XmlElement* inputMidiInsertsXml = inputPatchXml->getChildByName("Layer")->getChildByName("MidiInserts");
            std::unique_ptr<XmlElement> savedMidiInsertsXml(new XmlElement(*inputMidiInsertsXml));  // deep copy
            XmlElement* outputMidiInsertsXml = outputPatchXml->getChildByName("Layer")->getChildByName("MidiInserts");
            outputPatchXml->getChildByName("Layer")->replaceChildElement(outputMidiInsertsXml, savedMidiInsertsXml.release());
        }

        if (saveAudioFx)
        {
            XmlElement* inputAudioInsertsXml = inputPatchXml->getChildByName("Layer")->getChildByName("AudioInserts");
            std::unique_ptr<XmlElement> savedAudioInsertsXml(new XmlElement(*inputAudioInsertsXml)); // deep copy
            XmlElement* outputAudioInsertsXml = outputPatchXml->getChildByName("Layer")->getChildByName("AudioInserts");
            outputPatchXml->getChildByName("Layer")->replaceChildElement(outputAudioInsertsXml, savedAudioInsertsXml.release());
        }

        MemoryBlock outBlock;
        AudioProcessor::copyXmlToBinary(*outputPatchXml, outBlock);

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
