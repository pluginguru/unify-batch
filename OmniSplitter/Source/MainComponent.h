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

    Label matchSoundNote1, matchSoundNote2;
    ToggleButton matchSoundToggle; Label matchSoundToggleLabel;

    Label patchNameSuffixNote;
    TextEditor patchNameSuffixEditor; Label patchNameSuffixLabel;

    Label outputPathLabel;
    TextButton outputFolderPath; Label outputFolderPathLabel;
    TextButton clearOutputPathButton;

    String message;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
