#include "ADXL345.h"
#include "i2c/gpio-utils.h"
#include "i2c/i2c-dev.h"

#include <functional>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <sys/ioctl.h>
#include <unistd.h>

#ifdef linux
#include <error.h>
#endif

static char const* DeviceI2CFilename = "/dev/i2c-1";
static int const DeviceAddress = 0x53;
static int const DataReadyGPIO = 60;

#define BW_RATE 0x2c
#define DATA_FORMAT 0x31
#define INT_ENABLED 0x2e
#define FIFO_CTL 0x38
#define POWER_CTL 0x2d

#define SAMPLING_RATE 0b1010
#define RANGE 0b00

int write_i2c(int i2c_address, char reg_address, char data)
{
    char write_buf[] = { reg_address, data };
    return write(i2c_address, write_buf, 2);
}

ADXL345::ADXL345(IOEventObserver* observer)
: IOEventSource(observer)
, m_numsamples(0)
, m_filtered(0)
, m_accumulator(0)
{
    memset(&m_accel_fifo, 0, QUEUE_LENGTH * sizeof(long int));
    memset(&m_vel_fifo, 0, QUEUE_LENGTH * sizeof(long int));
    if ((m_i2cFile = open(DeviceI2CFilename, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus.\n");
        abort();
    }

    if (ioctl(m_i2cFile, I2C_SLAVE, DeviceAddress) < 0) {
        printf("Failed to aquire bus and/or talk to slave.\n");
        abort();
    }

    write_i2c(m_i2cFile, BW_RATE, SAMPLING_RATE); // Last 4 bits = sampling rate
    write_i2c(m_i2cFile, DATA_FORMAT, 0x20 | RANGE); // Last 2 bits = range
    write_i2c(m_i2cFile, INT_ENABLED, 0x02);
    write_i2c(m_i2cFile, FIFO_CTL, 0x41);
    write_i2c(m_i2cFile, POWER_CTL, 0x08);

    gpio_export(DataReadyGPIO);
    gpio_set_dir(DataReadyGPIO, "in");
    gpio_set_edge(DataReadyGPIO, "falling");
    m_gpioFile = gpio_fd_open(DataReadyGPIO, O_RDONLY);

    m_pollReq.data = (void *) this;

    uv_poll_init(uv_default_loop(), &m_pollReq, m_gpioFile);
    uv_poll_start(&m_pollReq, UV_READABLE, &ADXL345::PollDataReady);
}

ADXL345::~ADXL345()
{
}

void ADXL345::PollDataReady(uv_poll_t *handle, int status, int events)
{
    ADXL345 *eventSource = static_cast<ADXL345*>(handle->data);

    unsigned int button_state;
    gpio_get_value(BUTTON_NO, &button_state);
    while(!button_state) {
        long int diff = 0;
        char write_buf[1];
        write_buf[0] = 0x32;
        if(write(eventSource->m_i2cFile, write_buf,1) != 1) {
            printf("Failed to write to the i2c bus.\n");
        }

        signed char read_buf[6];
        if(read(eventSource->m_i2cFile, read_buf, 6) != 6) {
            printf("Failed to read from the i2c bus.\n");
        }
        else {
            long int data = ((((long int)read_buf[5]) << 8)
                | (0xff & read_buf[4]));

            eventSource->m_numsamples++;

            if( eventSource->m_numsamples > 1 ) {
                diff = data - eventSource->m_previous_data;
                eventSource->m_filtered += (diff - eventSource->m_accel_fifo[QUEUE_LENGTH - 1]);
                eventSource->m_accumulator = eventSource->m_filtered;
            }

            /*if( (eventSource->m_numsamples) % 100 == 0 ) {
                //printf("                          \r");
                printf("Current vel = %ld\n", eventSource->m_accel_fifo[QUEUE_LENGTH - 1]);
                fflush(stdout);
            }*/

            if( (eventSource->m_accumulator * eventSource->m_prev_accumulator) < 0
                && abs(eventSource->m_accumulator -  eventSource->m_prev_accumulator) > 50 ) {

                float intensity = (float) abs(eventSource->m_accumulator) / 256.0;
                intensity = (intensity > 1.) ? 1. : intensity;

                eventSource->PostCallback(({
                    std::stringstream temp;
                    temp << intensity;
                    temp.str();
                }));
            }

            int index;
            for( index = QUEUE_LENGTH - 1; index >= 0; index-- ){
                eventSource->m_accel_fifo[index] = eventSource->m_accel_fifo[index - 1];
                eventSource->m_vel_fifo[index] = eventSource->m_vel_fifo[index - 1];
            }
            eventSource->m_accel_fifo[0] = (eventSource->m_numsamples) ? 0 : diff;
            eventSource->m_vel_fifo[0] = (eventSource->m_numsamples) ? 0 : eventSource->m_filtered;
            eventSource->m_previous_data = data;
            eventSource->m_prev_accumulator = eventSource->m_accumulator;
        }
        gpio_get_value(BUTTON_NO, &button_state);

    }
}
