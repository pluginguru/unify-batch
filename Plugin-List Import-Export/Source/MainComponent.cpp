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

    unifyToSyndicateButton.setButtonText("Export Unify's plug-in list to Syndicate by White Elephant");
    unifyToSyndicateButton.onClick = [this]() { exportUnifyToSyndicate(); };
    addAndMakeVisible(unifyToSyndicateButton);

    syndicateToUnifyButton.setButtonText("Export Syndicate's plug-in list to Unify");
    syndicateToUnifyButton.onClick = [this]() { exportSyndicateToUnify(); };
    addAndMakeVisible(syndicateToUnifyButton);

    unifyToKshmrChainButton.setButtonText("Export Unify's plug-in list to KSHMR Chain by Excite Audio");
    unifyToKshmrChainButton.onClick = [this]() { exportUnifyToKshmrChain(); };
    addAndMakeVisible(unifyToKshmrChainButton);

    kshmrChainToUnifyButton.setButtonText("Export KSHMR Chain's plug-in list to Unify");
    kshmrChainToUnifyButton.onClick = [this]() { exportKshmrChainToUnify(); };
    addAndMakeVisible(kshmrChainToUnifyButton);

    setSize (600, 400);

    test();
}

void MainComponent::test()
{
    auto folder = File::getSpecialLocation(File::commonApplicationDataDirectory);
    DBG(folder.getFullPathName());

    KshmrChainKnownPluginsList chainKPIL;
    for (auto pluginXml : chainKPIL.getXml()->getChildWithTagNameIterator("PLUGIN"))
    {
        DBG(pluginXml->getStringAttribute("name"));
    }
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
    unifyToSyndicateButton.setBounds(bounds.removeFromTop(24));
    bounds.removeFromTop(20);
    syndicateToUnifyButton.setBounds(bounds.removeFromTop(24));

    bounds.removeFromTop(20);
    unifyToKshmrChainButton.setBounds(bounds.removeFromTop(24));
    bounds.removeFromTop(20);
    kshmrChainToUnifyButton.setBounds(bounds.removeFromTop(24));
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

void MainComponent::exportUnifyToKshmrChain()
{
    UnifyKnownPluginsList unifyKPIL;
    KshmrChainKnownPluginsList kshmrChainKPIL;

    DBG("Unify list has " + String(unifyKPIL.getXml()->getNumChildElements()) + " plugins");

    std::unique_ptr<XmlElement> effectsOnlyList(new XmlElement("cachedPlugins"));
    for (auto pluginXml : unifyKPIL.getXml()->getChildWithTagNameIterator("PLUGIN"))
    {
        if (pluginXml->getBoolAttribute("isInstrument")) continue;
        effectsOnlyList->addChildElement(new XmlElement(*pluginXml));   // copy plugin XML
    }

    DBG("Modified list has " + String(effectsOnlyList->getNumChildElements()) + " plugins");

    if (kshmrChainKPIL.replaceWith(effectsOnlyList.get()))
        resultLabel.setText("Success!", dontSendNotification);
    else
        resultLabel.setText("Unable to complete operation", dontSendNotification);
}

void MainComponent::exportKshmrChainToUnify()
{
    UnifyKnownPluginsList unifyKPIL;
    KshmrChainKnownPluginsList kshmrChainKPIL;

    std::unique_ptr<XmlElement> kpilCopy(new XmlElement(*kshmrChainKPIL.getXml()));
    kpilCopy->setTagName("KNOWNPLUGINS");

    if (unifyKPIL.replaceWith(kpilCopy.get()))
        resultLabel.setText("Success!", dontSendNotification);
    else
        resultLabel.setText("Unable to complete operation", dontSendNotification);
}
