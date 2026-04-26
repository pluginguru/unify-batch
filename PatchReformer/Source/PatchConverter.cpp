#include "PatchConverter.h"

PatchConverter::PatchConverter()
    : metronomeOnly(false)
    , updateLayer1Title(false)
    , saveInstPlugin(true)
    , saveMidiFx(false)
    , saveAudioFx(false)
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
    refPatchXml = AudioProcessor::getXmlFromBinary(memoryBlock.getData(), int(memoryBlock.getSize()));

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
        std::unique_ptr<XmlElement> inputPatchXml = AudioProcessor::getXmlFromBinary(inBlock.getData(), int(inBlock.getSize()));
        String patchName = inputPatchXml->getChildByName("PresetMetadata")->getStringAttribute("name");
        std::unique_ptr<XmlElement> outputPatchXml;

        if (metronomeOnly)
        {
            // output will basically be the input patch
            outputPatchXml.reset(inputPatchXml.release());

            // if it already has a MetronomePatch, remove it
            XmlElement* inputMetronomePatchXml = outputPatchXml->getChildByName("MetronomePatch");
            if (inputMetronomePatchXml) outputPatchXml->removeChildElement(inputMetronomePatchXml, true);

            // isolate MetronomePatch from reference patch, swap in a deep copy of it
            XmlElement* refMetronomePatchXml = refPatchXml->getChildByName("MetronomePatch");
            if (refMetronomePatchXml) outputPatchXml->addChildElement(new XmlElement(*refMetronomePatchXml));
        }
        else
        {
            // Prepare outputPatchXml as a deep copy of refPatchXml
            outputPatchXml.reset(new XmlElement(*refPatchXml));

            // Swap (deep copy of) metadata from input patch into outputPatchXml
            XmlElement* pmXml = inputPatchXml->getChildByName("PresetMetadata");
            std::unique_ptr<XmlElement> refPmXml(new XmlElement(*pmXml));
            outputPatchXml->replaceChildElement(outputPatchXml->getChildByName("PresetMetadata"), refPmXml.release());

            // get state of INST1 instrument, replace into outputPatchXml
            auto inputInst1Xml = inputPatchXml->getChildByName("Layer")->getChildByName("Instrument");
            String inst1State = inputInst1Xml->getStringAttribute("stateInformation");
            int inst1Program = inputInst1Xml->getIntAttribute("currentProgram");
            auto outInst1Xml = outputPatchXml->getChildByName("Layer")->getChildByName("Instrument");
            outInst1Xml->setAttribute("stateInformation", inst1State);
            outInst1Xml->setAttribute("currentProgram", inst1Program);
            if (saveInstPlugin)
            {
                // overwrite reference patch's INST1 plugin (this is the default)
                auto inputPluginXml = inputInst1Xml->getChildByName("PLUGIN");
                std::unique_ptr<XmlElement> pluginXml(new XmlElement(*inputPluginXml));
                auto refPluginXml = outInst1Xml->getChildByName("PLUGIN");
                outInst1Xml->replaceChildElement(refPluginXml, pluginXml.release());
            }

            if (saveMidiFx)
            {
                // overwrite reference patch's INST1 MIDI insert effects (usually NOT wanted)
                XmlElement* inputMidiInsertsXml = inputPatchXml->getChildByName("Layer")->getChildByName("MidiInserts");
                std::unique_ptr<XmlElement> savedMidiInsertsXml(new XmlElement(*inputMidiInsertsXml));  // deep copy
                XmlElement* outputMidiInsertsXml = outputPatchXml->getChildByName("Layer")->getChildByName("MidiInserts");
                outputPatchXml->getChildByName("Layer")->replaceChildElement(outputMidiInsertsXml, savedMidiInsertsXml.release());
            }

            if (saveAudioFx)
            {
                // overwrite reference patch's INST1 audio insert effects (usually NOT wanted)
                XmlElement* inputAudioInsertsXml = inputPatchXml->getChildByName("Layer")->getChildByName("AudioInserts");
                std::unique_ptr<XmlElement> savedAudioInsertsXml(new XmlElement(*inputAudioInsertsXml)); // deep copy
                XmlElement* outputAudioInsertsXml = outputPatchXml->getChildByName("Layer")->getChildByName("AudioInserts");
                outputPatchXml->getChildByName("Layer")->replaceChildElement(outputAudioInsertsXml, savedAudioInsertsXml.release());
            }
        }

        if (updateLayer1Title)
        {
            String patchName = outputPatchXml->getChildByName("PresetMetadata")->getStringAttribute("name");
            auto layer1Xml = outputPatchXml->getChildByName("Layer");
            layer1Xml->setAttribute("layerTitle", patchName);
        }

        MemoryBlock outBlock;
        AudioProcessor::copyXmlToBinary(*outputPatchXml, outBlock);

        if (outputFolderPath.isNotEmpty())
        {
            // Save to new file
            File outFile = File(outputFolderPath).getChildFile(patchName + ".unify");
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
