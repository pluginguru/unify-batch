#include "MainComponent.h"
#include <stdlib.h>
#include <time.h>

MainComponent::MainComponent()
{
    message = "Edit settings below, then drag .unify files or folders here";

	// Exact match
	matchSoundNote1.setText("Check to leave part level/pan/ranges in Omnisphere (perfect match),", dontSendNotification);
	addAndMakeVisible(matchSoundNote1);
	matchSoundNote2.setText("Un-check to rebuild in Unify (manual editing required)", dontSendNotification);
	addAndMakeVisible(matchSoundNote2);
	matchSoundToggleLabel.setText("Exact Sound", dontSendNotification);
	matchSoundToggleLabel.setJustificationType(Justification::right);
	matchSoundToggleLabel.attachToComponent(&matchSoundToggle, true);
	matchSoundToggle.setToggleState(true, dontSendNotification);
	addAndMakeVisible(matchSoundToggle);

	// Patch name suffix
	patchNameSuffixNote.setText("Suffix to add to output patch/file names. Leave blank to OVERWRITE source patches", dontSendNotification);
	addAndMakeVisible(patchNameSuffixNote);
	patchNameSuffixLabel.setText("Patch-name suffix", dontSendNotification);
	patchNameSuffixLabel.setJustificationType(Justification::right);
	patchNameSuffixLabel.attachToComponent(&patchNameSuffixEditor, true);
	addAndMakeVisible(patchNameSuffixEditor);
	
	// Output path
	outputPathLabel.setJustificationType(Justification::topLeft);
	outputPathLabel.setText("Click to set. If not set, files are processed in-place", dontSendNotification);
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

	clearOutputPathButton.setButtonText("Clear");
	clearOutputPathButton.onClick = [this]()
	{
		outputFolderPath.setButtonText("");
		patchConverter.outputFolderPath = "";
	};
	addAndMakeVisible(clearOutputPathButton);

    setSize(800, 340);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(40, 20);
	bounds.removeFromTop(80);		// space for message (and a gap below)

	bounds.removeFromLeft(120);		// space for labels
	bounds.removeFromRight(20);		// balance space on right

	matchSoundNote1.setBounds(bounds.removeFromTop(20));
	matchSoundNote2.setBounds(bounds.removeFromTop(24));
	matchSoundToggle.setBounds(bounds.removeFromTop(24).removeFromLeft(100));

	bounds.removeFromTop(20);
	patchNameSuffixNote.setBounds(bounds.removeFromTop(24));
	patchNameSuffixEditor.setBounds(bounds.removeFromTop(24));

	bounds.removeFromTop(30);
	outputPathLabel.setBounds(bounds.removeFromTop(26));
	auto row = bounds.removeFromTop(24);
	clearOutputPathButton.setBounds(row.removeFromRight(80));
	row.removeFromRight(10);
	outputFolderPath.setBounds(row);
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
	patchConverter.patchNameSuffix = patchNameSuffixEditor.getText().trim();

	bool exactSound = matchSoundToggle.getToggleState();
	patchConverter.setLayerLevelAndPan = !exactSound;
	patchConverter.setLayerSplitDetails = !exactSound;
	patchConverter.retainPartLevelAndPan = exactSound;
	patchConverter.retainPartSplitDetails = exactSound;

	int fileCount = patchConverter.processFiles(filePaths);
	message = String(fileCount) + " files processed.";

	repaint();
}
