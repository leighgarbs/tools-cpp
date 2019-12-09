#include <string>

#include "PositionalArgument.hpp"

//==============================================================================
PositionalArgument::PositionalArgument(const std::string& name,
                                       const std::string& description) :
    Argument(name, description)
{
}

//==============================================================================
PositionalArgument::PositionalArgument(
    const PositionalArgument& positional_argument)
{
    *this = positional_argument;
}

//==============================================================================
PositionalArgument::~PositionalArgument()
{
}

//==============================================================================
void PositionalArgument::process(const std::string& argument)
{
    setValue(argument);
}

//==============================================================================
PositionalArgument&
PositionalArgument::operator=(const PositionalArgument& positional_argument)
{
    // Don't do anything if we're assigning to ourselves
    if (this != &positional_argument)
    {
        std::string value;
        positional_argument.getValue(value);

        setValue(value);
    }

    return *this;
}