//
// Created by mfran on 09/04/2020.
//

#if Threads_FOUND
#   include <thread>
#endif
#include "File.hpp"
#include "Command.hpp"
#include "CommandHelper.hpp"

// PRIVATE DECLARATIONS

/**
 * Given a command call, generates the string to pass to the command interpreter.
 * @param commandCall The command call settings
 * @return A string to use.
 */
static File::SFilename_t prepareCommandString(const CommandCall& commandCall);

// PRIVATE DEFINITIONS

File::SFilename_t prepareCommandString(const CommandCall& commandCall) {
    File::OSStream_t oss;

    oss << "\"" << commandCall.executable << "\" ";

    for (const auto& argument : commandCall.arguments) {
        oss << argument << " ";
    }

    // TODO handle in the good order input output error

    switch (commandCall.inputChoice) {
        case InputChoice::NONE:
            break;

        case InputChoice::STRING:
            oss.seekp(0, std::ios_base::beg);
            oss << "echo \"" << commandCall.inputString << "\" | ";
            oss.seekp(0, std::ios_base::end);
            break;
        case InputChoice::FROM_FILE: {
            decltype(oss)::pos_type currentPosition = oss.tellp();
            oss << " < " << commandCall.inputFile;
            oss.seekp(currentPosition);
            break;
        }
    }

    return oss.str();
}

// PUBLIC DEFINITIONS

void Command(const CommandCall& commandCall, CommandReturn& commandReturn) {
    CommandRunner runner;
    runner.executable = &commandCall.executable;
    runner.arguments = &commandCall.arguments;

    switch (commandCall.outputChoice) {
        case OutputChoice::KEEP:
            break;
        case OutputChoice::KILL:
            runner.setOutput(new ProcessOutputStream_Kill());
            break;
        case OutputChoice::EXPORT:
            runner.setOutput(new ProcessOutputStream_Export(false, commandCall.outputFile));
            break;
        case OutputChoice::EXPORT_APPEND:
            runner.setOutput(new ProcessOutputStream_Export(true, commandCall.outputFile));
            break;
        case OutputChoice::RETRIEVE:
            runner.setOutput(new ProcessOutputStream_Retrieve());
            break;
    }

    switch (commandCall.errorsChoice) {
        case ErrorChoice::KEEP:
            break;
        case ErrorChoice::KILL:
            runner.setError(new ProcessOutputStream_Kill());
            break;
        case ErrorChoice::EXPORT:
            runner.setError(new ProcessOutputStream_Export(false, commandCall.outputFile));
            break;
        case ErrorChoice::EXPORT_APPEND:
            runner.setError(new ProcessOutputStream_Export(true, commandCall.outputFile));
            break;
        case ErrorChoice::RETRIEVE:
            runner.setError(new ProcessOutputStream_Retrieve());
            break;
    }

    switch (commandCall.inputChoice) {
        case InputChoice::NONE:
            break;
        case InputChoice::STRING:
            runner.setInput(new ProcessInputStream_String(commandCall.inputString));
            break;
        case InputChoice::FROM_FILE:
            runner.setInput(new ProcessInputStream_FromFile(commandCall.inputFile));
            break;
    }

    runner.start();
    // TODO handle program stopping option
    runner.stop();

    if (commandCall.outputChoice == OutputChoice::RETRIEVE) {
        commandReturn.outputText = runner.getOutput();
    }
    if (commandCall.errorsChoice == ErrorChoice::RETRIEVE) {
        commandReturn.errorText = runner.getError();
    }
}
