#include "MainComponent.h"
#include <stdlib.h>
#include <time.h>

MainComponent::MainComponent()
{
    PropertiesFile::Options options;
    options.applicationName = "Unify";
    options.filenameSuffix = ".settings";
    options.folderName = "PlugInGuru";
    options.osxLibrarySubFolder = "Application Support";
    settings.reset(new PropertiesFile(options));
    patchConverter.unifyRootFolderPath = settings->getValue("dataDirectoryPath");

    srand(unsigned(time(0)));
    Colour darkText = getLookAndFeel().findColour(ComboBox::textColourId).withMultipliedAlpha(0.5f);

    message = "Edit settings below, then drag .unify files or folders here";

	// Metadata
	metadataLabel.setJustificationType(Justification::topLeft);
	metadataLabel.setText("Metadata: fill in only what should change, check replace vs. append", dontSendNotification);
	addAndMakeVisible(metadataLabel);

	patchNameLabel.setText("Patch Name:", dontSendNotification);
	patchNameLabel.setJustificationType(Justification::right);
	patchNameLabel.attachToComponent(&patchName, true);
	patchName.setTextToShowWhenEmpty("Enter new patch name here", darkText);
	addAndMakeVisible(patchName);

	authorLabel.setText("Author:", dontSendNotification);
	authorLabel.setJustificationType(Justification::right);
	authorLabel.attachToComponent(&author, true);
	author.setTextToShowWhenEmpty("Enter new author name here", darkText);
	addAndMakeVisible(author);

	authorToggle.setButtonText("Replace");
	addAndMakeVisible(authorToggle);

	categoryLabel.setText("Category:", dontSendNotification);
	categoryLabel.setJustificationType(Justification::right);
	categoryLabel.attachToComponent(&category, true);
	category.setTextToShowWhenEmpty("Enter new category here", darkText);
	addAndMakeVisible(category);

	categoryToggle.setButtonText("Replace");
	addAndMakeVisible(categoryToggle);

	tagsLabel.setText("Tags:", dontSendNotification);
	tagsLabel.setJustificationType(Justification::right);
	tagsLabel.attachToComponent(&tags, true);
	tags.setTextToShowWhenEmpty("Enter new tags here", darkText);
	addAndMakeVisible(tags);

	tagsToggle.setButtonText("Replace");
	addAndMakeVisible(tagsToggle);

	commentLabel.setText("Comments:", dontSendNotification);
	commentLabel.setJustificationType(Justification::right);
	commentLabel.attachToComponent(&comment, true);
	comment.setMultiLine(true);
	comment.setTextToShowWhenEmpty("Enter new comment here", darkText);
	addAndMakeVisible(comment);

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

    setSize(800, 440);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(40, 20);
	bounds.removeFromTop(80);		// space for message (and a gap below)

	bounds.removeFromLeft(120);		// space for labels
	bounds.removeFromRight(20);		// balance space on right

	// Metadata
	metadataLabel.setBounds(bounds.removeFromTop(26));
	patchName.setBounds(bounds.removeFromTop(24));
	bounds.removeFromTop(2);
	auto row = bounds.removeFromTop(24);
	authorToggle.setBounds(row.removeFromRight(70));
	row.removeFromRight(10);
	author.setBounds(row);
	bounds.removeFromTop(2);
	row = bounds.removeFromTop(24);
	categoryToggle.setBounds(row.removeFromRight(70));
	row.removeFromRight(10);
	category.setBounds(row);
	bounds.removeFromTop(2);
	row = bounds.removeFromTop(24);
	tagsToggle.setBounds(row.removeFromRight(70));
	row.removeFromRight(10);
	tags.setBounds(row);
	bounds.removeFromTop(2);
	comment.setBounds(bounds.removeFromTop(4 * 24));
	bounds.removeFromTop(20);

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
	patchConverter.patchName	= patchName.getText();
	patchConverter.patchAuthor	= author.getText();
	patchConverter.categoryList	= category.getText();
	patchConverter.tagsList		= tags.getText();
	patchConverter.patchComment	= comment.getText();

	patchConverter.replaceAuthor = authorToggle.getToggleState();
	patchConverter.replaceCategory = categoryToggle.getToggleState();
	patchConverter.replaceTags	 = tagsToggle.getToggleState();

	if (patchConverter.patchName.isNotEmpty() && outputFolderPath.getButtonText().isEmpty())
	{
		message = "You must set an output folder path when changing patch names";
	}
	else
	{
		int fileCount = patchConverter.processFiles(filePaths);
		message = String(fileCount) + " files processed.";
	}

	repaint();
}
