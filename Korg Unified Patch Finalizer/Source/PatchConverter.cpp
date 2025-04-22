#pragma once
#include "PatchConverter.h"

PatchConverter::PatchConverter()
{
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

        std::unique_ptr<XmlElement> patchXml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
        XmlElement* pmXml = patchXml->getChildByName("PresetMetadata");

        processPatchXml(patchXml.get());

#if 1
        MemoryBlock outBlock;
        AudioProcessor::copyXmlToBinary(*patchXml, outBlock);

        if (outputFolderPath.isNotEmpty())
        {
            // Save to new file
            String fileName = File::createLegalFileName(pmXml->getStringAttribute("name") + ".unify");
            File outFile = File(outputFolderPath).getChildFile(fileName);
            outFile.create();
            outFile.replaceWithData(outBlock.getData(), outBlock.getSize());
        }
        else
        {
            // Overwrite original file
            file.replaceWithData(outBlock.getData(), outBlock.getSize());
        }
#endif

        fileCount++;
    }
}
