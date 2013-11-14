#ifndef ADXL345_H
#define ADXL345_H

#include "../ObservableIO.h"
#include <uv.h>

class ADXL345 : public IOEventSource {
public:
    ADXL345(IOEventObserver* callback);
    ~ADXL345();

private:
    static void PollDataReady(uv_poll_t*, int, int);

    uv_poll_t m_pollReq;

    int m_i2cFile;
    int m_gpioFile;
};

void ioThreadMain(void *context);

#endif // ADXL345_H