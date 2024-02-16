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

    void paint (Graphics&) override;
    void resized() override;

    Label versionLabel, resultLabel;

    TextButton unifyToSyndicateButton, syndicateToUnifyButton;
    TextButton unifyToKshmrChainButton, kshmrChainToUnifyButton;

protected:
    void test();

    void exportUnifyToSyndicate();
    void exportSyndicateToUnify();

    void exportUnifyToKshmrChain();
    void exportKshmrChainToUnify();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
