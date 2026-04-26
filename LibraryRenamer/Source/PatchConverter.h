#pragma once
#include <JuceHeader.h>

class PatchConverter
{
public:
    String unifyRootFolderPath;
    String libraryName;
    String outputFolderPath;

    bool updateGuruSamplerLibraryName = true;
    bool updateMIDIBoxLibraryName = true;

    bool updateGuruSamplerFolderName = false;
    String guruSamplerFolderName;

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:	
    void processFile(File, int& fileCount);
    void processPatchXml(XmlElement*, bool embedded);
    void processLayerXml(XmlElement*, bool embedded);
    String convertUnifyState(String stateInfo);
    String convertComboBoxState(String stateInfo);
    String convertGuruSamplerState(String stateInfo);
    String convertMIDIBoxState(String stateInfo);
    String convertKlangFalterState(String stateInfo);

private:
    File unifyRootFolder, outputFolder;
    String oldLibraryName;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
