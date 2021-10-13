#include <JuceHeader.h>
#include "Sampler_Unifier.h"
#include "BassLine_Unifier.h"
#include "J8_Unifier.h"
#include "UnoLX_Unifier.h"

int main (int /*argc*/, char** /*argv*/)
{
    // Uncomment exactly one of the following
    //Sampler_Unifier unifier;
    //BassLine_Unifier unifier;
    J8_Unifier unifier;
    //UnoLX_Unifier unifier;

    unifier.makeUnifiedLibrary();

    return 0;
}
