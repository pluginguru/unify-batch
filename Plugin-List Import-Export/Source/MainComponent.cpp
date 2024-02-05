#include "MainComponent.h"
#include "Exporters.h"

#define xstr(a) str(a)
#define str(a) #a

MainComponent::MainComponent()
{
    versionLabel.setText("Plimpex v" xstr(JUCE_APP_VERSION), dontSendNotification);
    versionLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(versionLabel);

    resultLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(resultLabel);

    exportUnifyToSyndicateButton.setButtonText("Export Unify's plug-in list to Syndicate by White Elephant");
    exportUnifyToSyndicateButton.onClick = [this]() { exportUnifyToSyndicate(); };
    addAndMakeVisible(exportUnifyToSyndicateButton);

    exportSyndicateToUnifyButton.setButtonText("Export Syndicate's plug-in list to Unify");
    exportSyndicateToUnifyButton.onClick = [this]() { exportSyndicateToUnify(); };
    addAndMakeVisible(exportSyndicateToUnifyButton);

    setSize (600, 400);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(20, 10);
    versionLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(20);
    resultLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(20);

    exportUnifyToSyndicateButton.setBounds(bounds.removeFromTop(24));
    bounds.removeFromTop(20);
    exportSyndicateToUnifyButton.setBounds(bounds.removeFromTop(24));
}

void MainComponent::exportUnifyToSyndicate()
{
    UnifyKnownPluginsList unifyKPIL;
    SyndicateKnownPluginsList syndicateKPIL;

    if (syndicateKPIL.replaceWith(unifyKPIL.getXml()))
        resultLabel.setText("Success!", dontSendNotification);
    else
        resultLabel.setText("Unable to complete operation", dontSendNotification);
}

void MainComponent::exportSyndicateToUnify()
{
    UnifyKnownPluginsList unifyKPIL;
    SyndicateKnownPluginsList syndicateKPIL;

    if (unifyKPIL.replaceWith(syndicateKPIL.getXml()))
        resultLabel.setText("Success!", dontSendNotification);
    else
        resultLabel.setText("Unable to complete operation", dontSendNotification);
}
