//
// Created by mfran on 27/04/2020.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP

#include "File.hpp"
#if defined(_WIN32)
#   include <Windows.h>
#else
#endif

// Declarations
class CommandComponent;
class CommandRunner;
class ProcessInputStream;
class ProcessInputStream_None;
class ProcessInputStream_String;
class ProcessInputStream_File;
class ProcessOutputStream;
class ProcessOutputStream_Keep;
class ProcessOutputStream_Kill;
class ProcessOutputStream_Export;
class ProcessOutputStream_Retrieve;
class ProcessErrorStream_Keep;

// ///////////////////////////////////////////////////////////////
// ///////////// COMMAND COMPONENT interface /////////////////////
// ///////////////////////////////////////////////////////////////

class CommandComponent {
public:
    /// This will be called right before the command is started.
    virtual void beforeStart();
    /// This will be called right after the command is started.
    virtual void afterStart();
    /// This will be called right before we initiate the process stopping procedure.
    virtual void beforeStop();
    /// This will be called right after the process is stopped.
    virtual void afterStop();
    virtual ~CommandComponent() = default;
};

// ///////////////////////////////////////////////////////////////
// /////////////////////// INPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

class ProcessInputStream : public CommandComponent {
public:
#if defined(_WIN32)
    virtual HANDLE getHandle() const = 0;
#endif
};

class ProcessInputStream_None : public ProcessInputStream {
public:
#if defined(_WIN32)
    HANDLE getHandle() const override;
#endif
};

class ProcessInputStream_String : public ProcessInputStream {
public:
    explicit ProcessInputStream_String(const File::SFilename_t& string);
    void beforeStart() override;
    void afterStart() override;
    void afterStop() override;
#if defined(_WIN32)
    HANDLE getHandle() const override;
protected:
    HANDLE readPipeHandle = nullptr;
    HANDLE writeToPipeHandle = nullptr;
    const File::SFilename_t& inputString;
#endif
};

class ProcessInputStream_FromFile : public ProcessInputStream {
public:
    explicit ProcessInputStream_FromFile(const File::SFilename_t& filename);
    void beforeStart() override;
    void afterStop() override;
#if defined(_WIN32)
    HANDLE getHandle() const override;
protected:
    HANDLE fileHandle = nullptr;
    const File::SFilename_t& filename;
#endif
};

// ///////////////////////////////////////////////////////////////
// ////////////////////// OUTPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

class ProcessOutputStream : public CommandComponent {
public:
    virtual std::string retrieveOutput();
#if defined(_WIN32)
    virtual HANDLE getHandle() const = 0;
#endif
};

class ProcessOutputStream_Keep : public ProcessOutputStream {
public:
#if defined(_WIN32)
    HANDLE getHandle() const override;
#endif
};

class ProcessErrorStream_Keep : public ProcessOutputStream {
public:
#if defined(_WIN32)
    HANDLE getHandle() const override;
#endif
};

class ProcessOutputStream_Kill : public ProcessOutputStream {
public:
    void beforeStart() override;
    void afterStop() override;
#if defined(_WIN32)
    HANDLE getHandle() const override;
protected:
    HANDLE nulHandle = nullptr;
#endif
};

class ProcessOutputStream_Export : public ProcessOutputStream {
public:
    const bool APPEND;
    explicit ProcessOutputStream_Export(bool append, const File::SFilename_t& filename);
    void beforeStart() override;
    void afterStop() override;
#if defined(_WIN32)
    HANDLE getHandle() const override;
protected:
    HANDLE fileHandle = nullptr;
#endif
    const File::SFilename_t& filename;
};

class ProcessOutputStream_Retrieve : public ProcessOutputStream {
public:
    void beforeStart() override;
    void beforeStop() override;
    void afterStop() override;
    std::string retrieveOutput() override;
#if defined(_WIN32)
    HANDLE getHandle() const override;
protected:
    HANDLE readPipeHandle = nullptr;
    HANDLE writeToPipeHandle = nullptr;
    std::ostringstream oss;
#endif
};

// ///////////////////////////////////////////////////////////////
// ////////////////////// COMMAND RUNNER /////////////////////////
// ///////////////////////////////////////////////////////////////

class CommandRunner {
public:
    void setInput(ProcessInputStream* stream);
    void setOutput(ProcessOutputStream* stream);
    void setError(ProcessOutputStream* stream);
    void start();
    void stop();
    std::string getOutput();
    std::string getError();
    int getStatusCode();
    ~CommandRunner();

    const File::SFilename_t* executable = nullptr;
    const std::vector<File::SFilename_t>* arguments = nullptr;

protected:
    void internalStart();
    void internalStop();
    int internalGetStatusCode();

    ProcessOutputStream* processOutputStream = new ProcessOutputStream_Keep;
    ProcessOutputStream* processErrorStream = new ProcessErrorStream_Keep;
    ProcessInputStream* processInputStream = new ProcessInputStream_None;

private:
#if defined(_WIN32)
    HANDLE processHandle = INVALID_HANDLE_VALUE;
#else
#endif
};

#endif //MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP
