#ifndef ADXL345_H
#define ADXL345_H

#include "../ObservableIO.h"
#include <uv.h>

#define QUEUE_LENGTH 30
#define BUTTON_NO 60

class ADXL345 : public IOEventSource {
public:
    ADXL345(IOEventObserver* callback);
    ~ADXL345();

private:
    static void PollDataReady(uv_poll_t*, int, int);

    uv_poll_t m_pollReq;

    int m_i2cFile;
    int m_gpioFile;
    
    int m_numsamples;
    
	long int m_filtered;
	long int m_accumulator;
	long int m_prev_accumulator;
	long int m_previous_data;
	long int m_accel_fifo[QUEUE_LENGTH] ;
	long int m_vel_fifo[QUEUE_LENGTH];
};

void ioThreadMain(void *context);

#endif // ADXL345_H
