#pragma once

#include <fileapi.h>
#include <string>
#include <vector>

class COM {

private:

    static const int ERROR_NO_FILE_HANDLE = 1;
    static const int ERROR_OPENING_FILE = 2;
    static const int ERROR_CONFIGURING_TIMEOUTS = 3;
    static const int ERROR_CONFIGURING_DEVICE = 4;
    static const int ERROR_READING = 5;
    static const int ERROR_WRITING = 6;

    static const unsigned int TIMEOUT = 1000;

    HANDLE handle;

    void open_port(const std::string &port);

    void cfg_buffers();

    void cfg_timeouts();

    void cfg_params(int baudrate);

public:

    COM();

    virtual ~COM();

    void connect(const std::string &port, int baudrate);

    void disconnect();

    bool isActive();

    virtual void read(uint8_t* buf, int buf_size);

    virtual void write(const uint8_t *data, int data_size);

    class Exception : public std::exception {
    private:
        int error_code;
    public:
        explicit Exception(int error_code);
        virtual const char *what() const noexcept override;
    };
};
