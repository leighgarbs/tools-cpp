#include <csignal>
#include <iostream>

#include "ProgramUT.hpp"

//==============================================================================
ProgramUT::ProgramUT(int argc, char** argv) :
    Program(argc, argv),
    signal_handled(false)
{
}

//==============================================================================
ProgramUT::~ProgramUT()
{
}

//==============================================================================
int ProgramUT::run()
{
    processDeliveredSignals();

    return !signal_handled;
}

//==============================================================================
void ProgramUT::processDeliveredSignals()
{
    if (isSignalDelivered(SIGINT))
    {
        signal_handled = true;
    }

    unsignalAll();
}
