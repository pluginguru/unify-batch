#include "CategoryBasedConverter.h"

static struct
{
    String category;
    String prefix;
}
categoryPrefix[] =
{
    // Dune 3
    //{ "BPM Arp, Arp", "BPM ARP - " },
    //{ "BPM Sequence, BPM Synth", "BPM SEQ - " },
    //{ "BPM Synth, Gated", "BPM SYNTH - " },
    //{ "Bass", "BASS - " },
    //{ "Chords", "CHORD - " },
    //{ "Drum Groove, BPM Drum", "BPM DRUM - " },
    //{ "Keys", "KEY - " },
    //{ "Leads", "LEAD - " },
    //{ "Pads", "PAD - " },
    //{ "Plucked", "PLUCK - " },
    //{ "Sfx", "SFX - " },
    //{ "Textures", "TEXT - " }

    // Diva
    //{ "Dreamy, Synth", "PAD - " },
    //{ "Lead", "LEAD - " },
    //{ "Perc, Synth", "PERC - " },
    //{ "Poly, Synth", "SYNTH - " },
    //{ "Rhythmic, Synth", "BPM - " },
    //{ "Sfx, Synth", "SFX - " },
    //{ "Synth Bass", "BASS - " },

    // Bazille
    //{ "Synth Bass", "BASS - " },
    //{ "Synth Perc", "PERC - " },
    //{ "Synth Hits", "HIT - " },
    //{ "Synth Pads", "PAD - " },
    //{ "Synth Sfx", "SFX - " },
    //{ "Synth Lead", "LEAD - " },
    //{ "Synth Keys", "KEY - " },
    //{ "Synth Melody", "BPM SYNTH - " },
    //{ "Synth Wobble", "SYNTH - " },
    //{ "Synth Chords", "CHORD - " },

    // Hive 2
    { "Synth Loops", "BPM SYNTH - " },
    { "Synth Plucks", "PLUCK - " },
    { "Synth Drum", "DRUM - " },
    { "Synth Bass", "BASS - " },
    { "Synth Sfx", "SFX - " },
    { "Synth Pads", "PAD - " },
    { "Synth Keys", "KEY - " },
    { "Synth Mono Lead", "LEAD - " },
    { "Synth Poly Lead", "LEAD - " },
    { "Synth Chord", "CHORD - " },

    // Repro-1, Repro-5
    //{ "Synth Arp", "BPM ARP - " },
    //{ "Synth Bass Seq", "BPM BASS - " },
    //{ "Synth Bass", "BASS - " },
    //{ "Synth Brass", "BRASS - " },
    //{ "Synth Chord", "CHORD - " },
    //{ "Synth Historic", "SYNTH - " },
    //{ "Synth Key", "KEY - " },
    //{ "Synth Lead", "LEAD - " },
    //{ "Synth Loop", "BPM SYNTH - " },
    //{ "Synth Pad", "PAD - " },
    //{ "Synth Perc Seq", "BPM PERC - " },
    //{ "Synth Perc", "PERC - " },
    //{ "Synth Pluck", "PLUCK - " },
    //{ "Synth Seq", "BPM SEQ - " },
    //{ "Synth Sfx", "SFX - " },

    // Zebra2
    //{ "Synth Bass", "BASS - " },
    //{ "Bass, Analogue", "BASS - " },
    //{ "Bass, Lead, Analogue", "BASS - " },
    //{ "Lead", "LEAD - " },
    //{ "Bass", "BASS - " },
    //{ "Lead, Analogue", "LEAD - " },
    //{ "Lead, Plucks", "PLUCK - " },
    //{ "Bass, Sub", "BASS - " },
    //{ "Leads, Plucks", "PLUCK - " },
    //{ "Synth Keys", "SYNTH - " },
    //{ "Synth Organ", "ORGAN - " },
    //{ "SFX", "SFX - " },
    //{ "Synth Lupins", "BPM SYNTH - " },
    //{ "Synth Pads", "PAD - " },
    //{ "Perc", "PERC - " },
    //{ "Synth Lead", "LEAD - " },
    //{ "Leads, Strings, Synth", "STRING - " },
    //{ "Arp, Synth", "BPM ARP - " },
    //{ "Leads, Analogue, Synth", "LEAD - " },
    //{ "Bass, Plucks, Synth", "PLUCK - " },
    //{ "Pads, Strings, Synth", "STRING - " },
    //{ "Arp, Seq, Synth", "BPM SYNTH - " },
    //{ "Bells, Mallets, Synth", "BELL - " },
    //{ "Keys, Guitars, SFX, Metal, Synth", "KEY - " },

    // ZebraHZ
    //{ "Arp, Bass", "BPM BASS - " },
    //{ "Arp", "BPM SYNTH - " },
    //{ "Leads", "LEAD - " },
    //{ "Pads, Synth", "PAD - " },
    //{ "Drums", "BPM DRUM - " },
    //{ "SFX", "SFX - " },
    //{ "Synth Bass", "BASS - " },
    //{ "Organ, Synth", "ORGAN - " },
    //{ "Synth Bells", "BELL - " },
    //{ "Bass, Synth", "BASS - " },
    //{ "Keys, Synth", "KEY - " },
    //{ "Arp, Synth", "BPM ARP - " },
    //{ "Brass, Strings, Synth", "BRASS - " },
    //{ "Brass, Synth", "BRASS - " },
    //{ "Arp, Bass, Synth", "BPM BASS - " },
    //{ "Strings, Synth", "STRING - " },
    //{ "Lead, Synth", "LEAD - " },
    //{ "Lead, Brass, Synth", "LEAD - " },
    //{ "SFX, Synth", "SFX - " },
    //{ "Perc, Synth", "BPM PERC - " },
    //{ "Lead, Vocal, Synth", "VOX - " },
    //{ "Bells, Synth", "BELL - " },
};

void CategoryBasedConverter::processPatchXml(XmlElement* patchXml)
{
    XmlElement* pmXml = patchXml->getChildByName("PresetMetadata");
    String patchName = pmXml->getStringAttribute("name");
    String category = pmXml->getStringAttribute("category");
    bool match = false;
    for (auto& cpr : categoryPrefix)
    {
        if (category.toLowerCase().startsWith(cpr.category.toLowerCase()))
        {
            pmXml->setAttribute("name", cpr.prefix + patchName);
            match = true;
            break;
        }
    }
    if (!match)
    {
        // Print out the names of patches which have no match, to the Debug window
        DBG(patchName + "|" + category);
    }
}
