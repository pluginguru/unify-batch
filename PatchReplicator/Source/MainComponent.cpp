#include "MainComponent.h"
#include <stdlib.h>
#include <time.h>

MainComponent::MainComponent()
{
    Colour darkText = getLookAndFeel().findColour(ComboBox::textColourId).withMultipliedAlpha(0.5f);

    message = "Edit settings below, then drag .sfz/.gsd files or folders here";

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
			referencePatch = chooser.getResult();
			String path = referencePatch.getFullPathName();
			referencePatchPath.setButtonText(path);
			patchConverter.referencePatchPath = path;

			outputPatchNamePattern.setText(referencePatch.getFileNameWithoutExtension() + " %sfz%");
		}
	};

	outputPatchNameInstruction.setText("Editable Patch-Name pattern (%sfz% is replaced by SFZ name):", dontSendNotification);
	addAndMakeVisible(outputPatchNameInstruction);
	outputPatchNameLabel.setText("Pattern:", dontSendNotification);
	outputPatchNameLabel.attachToComponent(&outputPatchNamePattern, true);
	addAndMakeVisible(outputPatchNamePattern);

	updateLayerLabels.setText("While processing:", dontSendNotification);
	updateLayerLabels.setJustificationType(Justification::right);
	updateLayerLabels.attachToComponent(&updateLayerTitleToggle, true);
	updateLayerTitleToggle.setButtonText("Update INST layer title(s) to match SFZ name");
	updateLayerTitleToggle.setToggleState(true, dontSendNotification);
	updateLayerTitleToggle.onStateChange = [this]()
	{
		patchConverter.updateLayerTitles = updateLayerTitleToggle.getToggleState();
	};
	addAndMakeVisible(updateLayerTitleToggle);

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
	updateLayerTitleToggle.setBounds(bounds.removeFromTop(24));
	bounds.removeFromTop(20);
	outputPatchNameInstruction.setBounds(bounds.removeFromTop(26));
	outputPatchNamePattern.setBounds(bounds.removeFromTop(24));
	bounds.removeFromTop(20);
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

void MainComponent::filesDropped(const StringArray& sfzPaths, int, int)
{
	if (patchConverter.referencePatchPath.isEmpty())
	{
		message = "You must specify a reference patch";
	}
	else
	{
		if (patchConverter.outputFolderPath.isEmpty())
			patchConverter.outputFolderPath = referencePatch.getParentDirectory().getFullPathName();
		int fileCount = patchConverter.processFiles(outputPatchNamePattern.getText(), sfzPaths);
		message = String(fileCount) + " files processed.";
	}
	repaint();
}
