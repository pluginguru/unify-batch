#pragma once
#include <JuceHeader.h>

/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (Graphics&) override;
    void resized() override;

    Label versionLabel, resultLabel;
    TextButton exportUnifyToSyndicateButton;
    TextButton exportSyndicateToUnifyButton;

protected:
    void exportUnifyToSyndicate();
    void exportSyndicateToUnify();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
