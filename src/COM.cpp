#include "COM.h"

#include <fileapi.h>
#include <commapi.h>

using namespace std;

COM::COM() {
    this->handle = nullptr;
}

COM::~COM() {
    disconnect();
}

void COM::connect(const string &port, int baudrate) {
    open_port(port);
    cfg_buffers();
    cfg_timeouts();
    cfg_params(baudrate);
}

void COM::disconnect() {
    if (this->handle == nullptr) {
        CloseHandle(this->handle);
        this->handle = nullptr;
    }
}

bool COM::isActive() {
    return this->handle != nullptr;
}

void COM::open_port(const std::string &port) {
    this->handle = CreateFile(
            port.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

    if (this->handle == (HANDLE) -1) {
        this->handle = nullptr;
        throw Exception(ERROR_OPENING_FILE);
    }
}

void COM::cfg_buffers() {
    SetupComm(this->handle, 1500, 1500);
}

void COM::cfg_timeouts() {
    COMMTIMEOUTS commtimeouts;
    commtimeouts.ReadIntervalTimeout = 0xFFFFFFFF;
    commtimeouts.ReadTotalTimeoutMultiplier = 0;
    commtimeouts.ReadTotalTimeoutConstant = TIMEOUT;
    commtimeouts.WriteTotalTimeoutMultiplier = 0;
    commtimeouts.WriteTotalTimeoutConstant = TIMEOUT;

    if (!SetCommTimeouts(this->handle, &commtimeouts)) {
        CloseHandle(this->handle);
        this->handle = nullptr;
        throw Exception(ERROR_CONFIGURING_TIMEOUTS);
    }
}

void COM::cfg_params(int baudrate) {
    DCB comDCM;
    memset(&comDCM, 0, sizeof(comDCM));
    comDCM.DCBlength = sizeof(DCB);
    GetCommState(this->handle, &comDCM);
    comDCM.BaudRate = DWORD(baudrate);
    comDCM.ByteSize = 8;
    comDCM.Parity = NOPARITY;
    comDCM.StopBits = ONESTOPBIT;
    comDCM.fAbortOnError = TRUE;
    comDCM.fDtrControl = DTR_CONTROL_DISABLE;
    comDCM.fRtsControl = RTS_CONTROL_TOGGLE;
    comDCM.fBinary = TRUE;
    comDCM.fParity = FALSE;
    comDCM.fInX = comDCM.fOutX = FALSE;
    comDCM.XonChar = 0;
    comDCM.XoffChar = uint8_t(0xff);
    comDCM.fErrorChar = FALSE;
    comDCM.fNull = FALSE;
    comDCM.fOutxCtsFlow = FALSE;
    comDCM.fOutxDsrFlow = FALSE;
    comDCM.XonLim = 128;
    comDCM.XoffLim = 128;

    if (!SetCommState(this->handle, &comDCM)) {
        CloseHandle(this->handle);
        this->handle = nullptr;
        throw Exception(ERROR_CONFIGURING_DEVICE);
    }
}

void COM::read(uint8_t *buf, int buf_size) {
    if (this->handle == nullptr) {
        throw Exception(ERROR_NO_FILE_HANDLE);
    }

    DWORD feedback = 0;
    int attempts = 3;
    while (buf_size && attempts--) {
        if (!ReadFile(this->handle, buf, buf_size, &feedback, nullptr)) {
            CloseHandle(this->handle);
            this->handle = nullptr;
            throw Exception(ERROR_READING);
        }
        buf_size -= feedback;
        buf += feedback;
    }

    if (buf_size) {
        CloseHandle(this->handle);
        this->handle = nullptr;
        throw Exception(ERROR_READING);
    }
}

void COM::write(const uint8_t *data, int data_size) {
    if (this->handle == nullptr) {
        throw Exception(ERROR_NO_FILE_HANDLE);
    }

    DWORD feedback;
    if (!WriteFile(this->handle, data, data_size, &feedback, nullptr) || feedback != data_size) {
        CloseHandle(this->handle);
        this->handle = nullptr;
        throw Exception(ERROR_WRITING);
    }
}

COM::Exception::Exception(int error_code) {
    this->error_code = error_code;
}

const char *COM::Exception::what() const noexcept {
    switch (this->error_code) {
        case ERROR_NO_FILE_HANDLE:
            return "No file handle";
        case ERROR_CONFIGURING_TIMEOUTS:
            return "Error configuring timeouts";
        case ERROR_CONFIGURING_DEVICE:
            return "Error configuring device";
        case ERROR_OPENING_FILE:
            return "Error opening file";
        case ERROR_READING:
            return "Read error";
        case ERROR_WRITING:
            return "Write error";
    }
}
