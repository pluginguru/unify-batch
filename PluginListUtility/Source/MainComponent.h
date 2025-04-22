#pragma once
#include <JuceHeader.h>

class MainComponent : public Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(Graphics&) override;
    void resized() override;

protected:
    void makeFullBucketList();

protected:
    TextButton makeFullBucketBtn;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
