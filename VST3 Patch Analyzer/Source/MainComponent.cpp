#include "MainComponent.h"
#include "PatchAnalyzer.h"

MainComponent::MainComponent()
{
    setSize(600, 400);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setFont(16.0f);
    g.setColour(Colours::white);
    g.drawText("Drag one or more .unify files here", getLocalBounds(), Justification::centred, true);
}

void MainComponent::resized()
{
}

bool MainComponent::isInterestedInFileDrag(const StringArray& paths)
{
    for (auto& path : paths)
        if (!path.endsWith(".unify")) return false;
    return true;
}

void MainComponent::filesDropped(const StringArray& paths, int, int)
{
    for (auto& path : paths)
        analyzePatch(path);
}
