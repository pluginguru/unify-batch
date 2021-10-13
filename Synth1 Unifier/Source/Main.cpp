#include <JuceHeader.h>
#include "Synth1_Unifier.h"

int main (int argc, char** argv)
{
    ScopedJuceInitialiser_GUI juceInit;

    Synth1_Unifier unifier;

    unifier.makeUnifiedLibrary();

    return 0;
}
