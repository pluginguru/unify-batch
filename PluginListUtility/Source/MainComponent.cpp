#include "MainComponent.h"

MainComponent::MainComponent()
{
    makeFullBucketBtn.setButtonText("Make FBM list");
    makeFullBucketBtn.onClick = [this] { makeFullBucketList(); };
    addAndMakeVisible(makeFullBucketBtn);

    setSize(300, 200);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(20);
    makeFullBucketBtn.setBounds(area.removeFromTop(24));
}

void MainComponent::makeFullBucketList()
{
    PropertiesFile::Options options;
    options.applicationName = "PluginList";
    options.filenameSuffix = ".xml";
    options.folderName = "PlugInGuru";
    options.osxLibrarySubFolder = "Application Support";
    PropertiesFile propsFile(options);
    auto fullListXml = propsFile.getXmlValue("pluginList");

    File fbmListXmlFile = File::getSpecialLocation(File::userDesktopDirectory).getChildFile("plugins-fbm.xml");
    std::unique_ptr<XmlElement> fbmListXml(new XmlElement("KNOWNPLUGINS"));

    Array<XmlElement*> toRemove;
    for (auto pluginXml : fullListXml->getChildIterator())
    {
        if (pluginXml->getStringAttribute("manufacturer").equalsIgnoreCase("Full Bucket Music"))
        {
            auto pluginXmlCopy(new XmlElement(*pluginXml));
            toRemove.add(pluginXml);

            String path = pluginXmlCopy->getStringAttribute("file").replace("\\", "/");
            String fileName = path.fromLastOccurrenceOf("/", false, false);
            pluginXmlCopy->setAttribute("file", "\\FBM\\" + fileName);
            fbmListXml->addChildElement(pluginXmlCopy);
        }
    }
    for (auto pluginXml : toRemove)
        fullListXml->removeChildElement(pluginXml, true);
    propsFile.setValue("pluginList", fullListXml.get());

    fbmListXml->writeTo(fbmListXmlFile);
    propsFile.saveIfNeeded();
}
