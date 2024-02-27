#pragma once
#include "JuceHeader.h"
#include "PatchConverter.h"

class MainComponent : public Component
                    , public FileDragAndDropTarget
{
public:
    MainComponent();
    virtual ~MainComponent() = default;

    // Component
    void resized() override;
    void paint(Graphics&) override;

    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const StringArray&) override;
    void filesDropped(const StringArray&, int, int) override;

private:
    std::unique_ptr<PropertiesFile> settings;
    PatchConverter patchConverter;

    Label outputPathLabel;
    TextButton outputFolderPath; Label outputFolderPathLabel;

    String message;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
