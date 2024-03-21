#pragma once
#include <JuceHeader.h>

class PatchConverter
{
public:
    String outputFolderPath;

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:	
    void processFileOrFolder(File sfzFile, int& fileCount);
    void processLayerXml(XmlElement*, FileOutputStream&, int);
    void processPatchXml(XmlElement*, FileOutputStream&, int);
    void processComboXml(XmlElement*, FileOutputStream&, int);
    void processPluginXml(String, XmlElement*, FileOutputStream&, int);

private:
    File outputFolder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
