#include "MainComponent.h"
#include <stdlib.h>
#include <time.h>

MainComponent::MainComponent()
{
    message = "Edit settings below, then drag .unify files or folders here";

	// Paths
	outputPathLabel.setJustificationType(Justification::topLeft);
	outputPathLabel.setText("Output path: if not set, files are processed in-place", dontSendNotification);
	addAndMakeVisible(outputPathLabel);

	outputFolderPathLabel.setText("Output Folder:", dontSendNotification);
	outputFolderPathLabel.setJustificationType(Justification::right);
	outputFolderPathLabel.attachToComponent(&outputFolderPath, true);
	addAndMakeVisible(outputFolderPath);
	outputFolderPath.onClick = [this]()
	{
		FileChooser chooser("Select output folder");
		if (chooser.browseForDirectory())
		{
			String path = chooser.getResult().getFullPathName();
			outputFolderPath.setButtonText(path);
			patchConverter.outputFolderPath = path;
		}
	};

    setSize(800, 220);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(40, 20);
	bounds.removeFromTop(80);		// space for message (and a gap below)

	bounds.removeFromLeft(120);		// space for labels
	bounds.removeFromRight(20);		// balance space on right

	// Paths
	outputPathLabel.setBounds(bounds.removeFromTop(26));
	outputFolderPath.setBounds(bounds.removeFromTop(24));
}

void MainComponent::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	auto messageBox = getLocalBounds().reduced(40, 20).removeFromTop(60);

	g.setFont(Font(16.0f));
	g.setColour(Colours::white);
	g.drawText(message, messageBox, Justification::centred);
}

bool MainComponent::isInterestedInFileDrag(const StringArray&)
{
    return true;
}

void MainComponent::filesDropped(const StringArray& filePaths, int, int)
{
	int fileCount = patchConverter.processFiles(filePaths);
	message = String(fileCount) + " files processed.";

	repaint();
}
