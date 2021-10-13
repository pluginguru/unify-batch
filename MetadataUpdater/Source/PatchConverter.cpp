#pragma once
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

        std::unique_ptr<XmlElement> patchXml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
        XmlElement* pmXml = patchXml->getChildByName("PresetMetadata");

        if (patchName.isNotEmpty())
            pmXml->setAttribute("name", patchName.trim());

        if (patchAuthor.isNotEmpty())
        {
            if (replaceAuthor) pmXml->setAttribute("author", patchAuthor.trim());
            else
            {
                StringArray list = StringArray::fromTokens(pmXml->getStringAttribute("author"), ";,", {});
                for (auto& item : list) item = item.trim();
                StringArray newList = StringArray::fromTokens(patchAuthor, ";,", {});
                for (auto& item : newList) item = item.trim();
                list.addArray(newList);
                pmXml->setAttribute("author", list.joinIntoString(", "));
            }
        }

        if (categoryList.isNotEmpty())
        {
            if (replaceCategory) pmXml->setAttribute("category", categoryList.trim());
            else
            {
                StringArray list = StringArray::fromTokens(pmXml->getStringAttribute("category"), ";,", {});
                for (auto& item : list) item = item.trim();
                StringArray newList = StringArray::fromTokens(categoryList, ";,", {});
                for (auto& item : newList) item = item.trim();
                list.addArray(newList);
                pmXml->setAttribute("category", list.joinIntoString(", "));
            }
        }

        if (tagsList.isNotEmpty())
        {
            if (replaceTags) pmXml->setAttribute("tags", tagsList.trim());
            else
            {
                StringArray list = StringArray::fromTokens(pmXml->getStringAttribute("tags"), ";,", {});
                for (auto& item : list) item = item.trim();
                StringArray newList = StringArray::fromTokens(tagsList, ";,", {});
                for (auto& item : newList) item = item.trim();
                list.addArray(newList);
                pmXml->setAttribute("tags", list.joinIntoString(", "));
            }
        }
            
        if (patchComment.isNotEmpty())
            pmXml->setAttribute("comment", patchComment.trim());

        MemoryBlock outBlock;
        AudioProcessor::copyXmlToBinary(*patchXml, outBlock);

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
