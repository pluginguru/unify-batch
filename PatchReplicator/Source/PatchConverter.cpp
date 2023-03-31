#include "PatchConverter.h"

PatchConverter::PatchConverter()
    : updateLayerTitles(false)
{
}

int PatchConverter::processFiles(String patchNamePattern, const StringArray& sfzPaths)
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
    for (auto sfzPath : sfzPaths)
    {
        File file(sfzPath);
        processFile(file, patchNamePattern, fileCount);
    }
    return fileCount;
}

void PatchConverter::processFile(File file, String patchNamePattern, int& fileCount)
{
    if (file.isDirectory())
    {
        for (DirectoryEntry entry : RangedDirectoryIterator(file, false, "*", File::findFilesAndDirectories))
            processFile(entry.getFile(), patchNamePattern, fileCount);
    }
    else if (file.getFileExtension() == ".sfz" || file.getFileExtension() == ".gsd")
    {
        String sfzName = file.getFileNameWithoutExtension();
        DBG(sfzName);

        // Prepare outputPatchXml as a deep copy of refPatchXml
        std::unique_ptr<XmlElement> outputPatchXml(new XmlElement(*refPatchXml));

        // Update patch name to match SFZ name
        auto pmXml = outputPatchXml->getChildByName("PresetMetadata");
        String patchName = patchNamePattern.replace("%sfz%", sfzName);
        pmXml->setAttribute("name", patchName);

        for (auto* layerXml : outputPatchXml->getChildWithTagNameIterator("Layer"))
        {
            auto instXml = layerXml->getChildByName("Instrument");
            auto pluginXml = instXml->getChildByName("PLUGIN");
            if ((pluginXml->getStringAttribute("name") == "Guru Sampler") &&
                (pluginXml->getStringAttribute("format") == "Built-In"))
            {
                if (updateLayerTitles)
                    layerXml->setAttribute("layerTitle", sfzName);

                String samplerState = instXml->getStringAttribute("stateInformation");
                String newState = convertGuruSamplerState(samplerState, sfzName);
                instXml->setAttribute("stateInformation", newState);
            }
        }

        MemoryBlock outBlock;
        AudioProcessor::copyXmlToBinary(*outputPatchXml, outBlock);

        // Save to new file
        String patchFileName = File::createLegalFileName(patchName + ".unify");
        File outFile = File(outputFolderPath).getChildFile(patchFileName);
        outFile.create();
        outFile.replaceWithData(outBlock.getData(), outBlock.getSize());

        fileCount++;
    }
}

String PatchConverter::convertGuruSamplerState(String stateInfo, String newSfzName)
{
    MemoryBlock inBlock;
    inBlock.fromBase64Encoding(stateInfo);

    std::unique_ptr<XmlElement> xml = AudioProcessor::getXmlFromBinary(inBlock.getData(), int(inBlock.getSize()));
    jassert(xml);

    xml->setAttribute("osc1SampleSet", newSfzName);

    MemoryBlock outBlock;
    AudioProcessor::copyXmlToBinary(*xml, outBlock);

    return outBlock.toBase64Encoding();
}
