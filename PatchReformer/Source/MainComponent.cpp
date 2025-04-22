#include "MainComponent.h"
#include <stdlib.h>
#include <time.h>

MainComponent::MainComponent()
{
    Colour darkText = getLookAndFeel().findColour(ComboBox::textColourId).withMultipliedAlpha(0.5f);

    message = "Edit settings below, then drag .unify files or folders here";

	referencePatchPathLabel.setText("Reference Patch:", dontSendNotification);
	referencePatchPathLabel.setJustificationType(Justification::right);
	referencePatchPathLabel.attachToComponent(&referencePatchPath, true);
	referencePatchPath.setButtonText(">> click to select reference patch <<");
	addAndMakeVisible(referencePatchPath);
	referencePatchPath.onClick = [this]()
	{
		FileChooser chooser("Select reference patch", File(), "*.unify");
		if (chooser.browseForFileToOpen())
		{
			String path = chooser.getResult().getFullPathName();
			referencePatchPath.setButtonText(path);
			patchConverter.referencePatchPath = path;
		}
	};

	updateL1Label.setText("While processing:", dontSendNotification);
	updateL1Label.setJustificationType(Justification::right);
	updateL1Label.attachToComponent(&updateLayer1TitleToggle, true);
	updateLayer1TitleToggle.setButtonText("Update INST1 layer title to match patch name");
	updateLayer1TitleToggle.onStateChange = [this]()
	{
		patchConverter.updateLayer1Title = updateLayer1TitleToggle.getToggleState();
	};
	addAndMakeVisible(updateLayer1TitleToggle);

	preserveLabel.setText("Preserve:", dontSendNotification);
	preserveLabel.setJustificationType(Justification::right);
	preserveLabel.attachToComponent(&saveInstToggle, true);

	saveInstToggle.setButtonText("INST1 Instrument");
	saveInstToggle.setToggleState(patchConverter.saveInstPlugin, dontSendNotification);
	saveInstToggle.onStateChange = [this]()
		{
			patchConverter.saveInstPlugin = saveInstToggle.getToggleState();
		};
	addAndMakeVisible(saveInstToggle);

	saveMidiFxToggle.setButtonText("INST1 MIDI FX");
	saveMidiFxToggle.setToggleState(patchConverter.saveMidiFx, dontSendNotification);
	saveMidiFxToggle.onStateChange = [this]()
	{
		patchConverter.saveMidiFx = saveMidiFxToggle.getToggleState();
	};
	addAndMakeVisible(saveMidiFxToggle);

	saveAudioFxToggle.setButtonText("INST1 Audio FX");
	saveAudioFxToggle.setToggleState(patchConverter.saveAudioFx, dontSendNotification);
	saveAudioFxToggle.onStateChange = [this]()
	{
		patchConverter.saveAudioFx = saveAudioFxToggle.getToggleState();
	};
	addAndMakeVisible(saveAudioFxToggle);

	outputPathLabel.setJustificationType(Justification::topLeft);
	outputPathLabel.setText("Output path: if not set, files are processed in-place", dontSendNotification);
	addAndMakeVisible(outputPathLabel);

	outputFolderPathLabel.setText("Output Folder:", dontSendNotification);
	outputFolderPathLabel.setJustificationType(Justification::right);
	outputFolderPathLabel.attachToComponent(&outputFolderPath, true);
	outputFolderPath.setButtonText(">> click to select output folder (optional) <<");
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

    setSize(800, 340);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(40, 20);
	bounds.removeFromTop(80);		// space for message (and a gap below)

	bounds.removeFromLeft(120);		// space for labels
	bounds.removeFromRight(20);		// balance space on right

	referencePatchPath.setBounds(bounds.removeFromTop(24));
	bounds.removeFromTop(20);
	updateLayer1TitleToggle.setBounds(bounds.removeFromTop(24));
	bounds.removeFromTop(8);
	auto row = bounds.removeFromTop(24);
	saveInstToggle.setBounds(row.removeFromLeft(180));
	saveMidiFxToggle.setBounds(row.removeFromLeft(160));
	saveAudioFxToggle.setBounds(row.removeFromLeft(160));
	bounds.removeFromTop(30);
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

	if (patchConverter.referencePatchPath.isEmpty())
	{
		message = "You must specify a reference patch";
	}
	else
	{
		message = String(fileCount) + " files processed.";
	}
	repaint();
}
