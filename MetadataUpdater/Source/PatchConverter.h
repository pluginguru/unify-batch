#pragma once
#include <JuceHeader.h>

class PatchConverter
{
public:
    String unifyRootFolderPath;

    String patchName;
    String patchAuthor;
    String categoryList;
    String tagsList;
    String patchComment;

    bool replaceAuthor, replaceCategory, replaceTags;

    String outputFolderPath;

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:	
    void processFile(File, int& fileCount);

private:
    File unifyRootFolder, outputFolder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
