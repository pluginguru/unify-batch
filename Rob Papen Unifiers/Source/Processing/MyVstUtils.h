#pragma once
#include <JuceHeader.h>

// This code is adapted from parts of the VST wrapper code in JUCE

struct MyVstConv
{
    static const int fxbVersionNum = 1;

    struct fxProgram
    {
        int32 chunkMagic;    // 'CcnK'
        int32 byteSize;      // of this chunk, excl. magic + byteSize
        int32 fxMagic;       // 'FxCk'
        int32 version;
        int32 fxID;          // fx unique id
        int32 fxVersion;
        int32 numParams;
        char prgName[28];
        float params[1];        // variable no. of parameters
    };

    struct fxSet
    {
        int32 chunkMagic;    // 'CcnK'
        int32 byteSize;      // of this chunk, excl. magic + byteSize
        int32 fxMagic;       // 'FxBk'
        int32 version;
        int32 fxID;          // fx unique id
        int32 fxVersion;
        int32 numPrograms;
        char future[128];
        fxProgram programs[1];  // variable no. of programs
    };

    struct fxChunkSet
    {
        int32 chunkMagic;    // 'CcnK'
        int32 byteSize;      // of this chunk, excl. magic + byteSize
        int32 fxMagic;       // 'FxCh', 'FPCh', or 'FBCh'
        int32 version;
        int32 fxID;          // fx unique id
        int32 fxVersion;
        int32 numPrograms;
        char future[128];
        int32 chunkSize;
        char chunk[8];          // variable
    };

    struct fxProgramSet
    {
        int32 chunkMagic;    // 'CcnK'
        int32 byteSize;      // of this chunk, excl. magic + byteSize
        int32 fxMagic;       // 'FxCh', 'FPCh', or 'FBCh'
        int32 version;
        int32 fxID;          // fx unique id
        int32 fxVersion;
        int32 numPrograms;
        char name[28];
        int32 chunkSize;
        char chunk[8];          // variable
    };

    MemoryBlock memoryBlock;
    String programName;
    Array<float> paramValues;

    bool restoreProgramSettings(const fxProgram* const prog);
    void createTempParameterStore(MemoryBlock& dest);
    void restoreFromTempParameterStore(const MemoryBlock& m);
    void setParamsInProgramBlock(fxProgram* prog);


    int getNumPrograms() { return 1; }
    int getCurrentProgram() { return 0; }
    void setCurrentProgram(int) {}
    String getCurrentProgramName() { return "progName"; }
    void changeProgramName(int index, const String& newName);

    int getNumParams() { return 1; }
    void setParamValue(int index, const float value);
    float getParamValue(int /*index*/) { return 0.0f; }

    bool usesChunks() { return true; }
    void setChunkData(const void* data, const int size, bool isPreset);
    void getChunkData(MemoryBlock& mb, bool isPreset, int maxSizeMB);

    int getUID() { return 1; }
    int getVersionNumber() { return 1; }

    bool loadFromFXBFile (const void* const data, const size_t dataSize);
    bool saveToFXBFile (MemoryBlock& dest, bool isFXB, int maxSizeMB = 128);
};

XmlElement* getVstPluginState(AudioPluginInstance* instance);
void setVstPluginState(AudioPluginInstance* instance, XmlElement* xml, String& errorMessage);

XmlElement* decodeVstPluginState(AudioPluginInstance*, MemoryBlock&);
void encodeVstPluginState(XmlElement*, AudioPluginInstance*, MemoryBlock&);
void encodeVstPluginState(MemoryBlock&, AudioPluginInstance*, MemoryBlock&);
