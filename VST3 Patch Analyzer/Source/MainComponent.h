#pragma once
#include <JuceHeader.h>

class MainComponent : public Component
                    , public FileDragAndDropTarget
{
public:
    MainComponent();
    ~MainComponent() override;

    // Component
    void paint(Graphics&) override;
    void resized() override;

    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const StringArray&) override;
    void filesDropped(const StringArray&, int, int) override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
