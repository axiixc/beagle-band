
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "i2c-dev.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

#include "gpio-utils.h"
#include "i2cbusses.h"

#define READ_BIT 1
#define BUTTON_GPIO_PINS {60}
#define BUTTON_ACTIVE_EDGES {0} 

#define SAMPLING_RATE 0b1010
#define RANGE 0b00

#define QUEUE_LENGTH 30

int write_i2c (int i2c_address, int reg_address, int data);

int main(int argc, char **argv, char **envp) {
	
	int file;
	const char *filename = "/dev/i2c-1";
	if((file = open(filename, O_RDWR)) < 0) {
		perror("Failed to open the i2c bus.\n");
		exit(1);
	}
	
	int device_addr = 0x53;
	if(ioctl(file, I2C_SLAVE, device_addr) < 0) {
		printf("Failed to acquire bus and/or talk to slave.\n");
		exit(1);
	}
	
	write_i2c(file, 0x2c, SAMPLING_RATE); //BW_RATE -- Last 4 bits = sampling rate
	write_i2c(file, 0x31, 0x20 | RANGE); //DATA_FORMAT -- Last 2 bits = range
	write_i2c(file, 0x2e, 0x02); //INT_ENABLE
	write_i2c(file, 0x38, 0x41); //FIFO_CTL
	write_i2c(file, 0x2d, 0x08); //POWER_CTL
	
	int buttons[] = BUTTON_GPIO_PINS;
	int button_size = sizeof(buttons)/sizeof(buttons[0]);
	int button_active_edges[] = BUTTON_ACTIVE_EDGES;
	
	struct pollfd fdset[button_size];
	int nfds = button_size;	
	int gpio_fd[button_size];
	int button;
	for(button = 0; button < button_size; button++)
	{
		gpio_export(buttons[button]);
		gpio_set_dir(buttons[button], "in");
		gpio_set_edge(buttons[button], button_active_edges[button] ? "rising" : "falling");
		gpio_fd[button] = gpio_fd_open(buttons[button], O_RDONLY);
	}
	int num_samples = 0;
	long int z_nooffset = 0;
	long int z_accumulator = 0;
	long int previous_z_accumulator = 0;
	long int z_diff = 0;
	long int previous_z = 0;
	long int z_jerk_fifo[QUEUE_LENGTH] = {0};
	long int z_accel_fifo[QUEUE_LENGTH] = {0};
	while(1) {

		memset((void*)fdset, 0, sizeof(fdset));
		for(button = 0; button < button_size; button++) {		
			fdset[button].fd = gpio_fd[button];
			fdset[button].events = POLLPRI;
		}
		poll(fdset, nfds, 100);
		for(button = 0; button < button_size; button++) {
			if(fdset[button].revents & POLLPRI) {
				char buf[1];
				read(fdset[button].fd, buf, 1);
				
				int button_state;
				gpio_get_value(buttons[button], &button_state);
				while(button_state == button_active_edges[button]) {
					char write_buf[1];
					write_buf[0] = 0x32;
					if(write(file, write_buf,1) != 1) {
						printf("Failed to write to the i2c bus.\n");
					}

					int reg_num;
					signed char read_buf[6];
					if(read(file, read_buf, 6) != 6) {
						printf("Failed to read from the i2c bus.\n");
					}
					else {
						long int z_data = ((((long int)read_buf[5]) << 8) 
							| (0xff & read_buf[4]));
						num_samples++;
						
						if( num_samples > 1 ) {
							z_diff = (z_data - previous_z);
							z_nooffset += (z_diff - 
								z_jerk_fifo[QUEUE_LENGTH - 1]); 
							z_accumulator = (z_nooffset);
						}
							
						/*if( num_samples % 1 == 0 ) {
							printf("                          \r");
							printf("Current vel = %d\r", z_nooffset);
							fflush(stdout);
						}*/
						
						if( (z_accumulator * previous_z_accumulator) < 0
							&& abs(z_accumulator - 
							previous_z_accumulator) > 50 ) {
							
							// Put stuff to do on hits here!
							printf("HIT!\n");
							printf("Acceleration = %d.\n", z_nooffset);
							fflush(stdout);
						}
						
						int index;
						for( index = QUEUE_LENGTH - 1; 
							index >= 0; index-- ){
							z_jerk_fifo[index] = z_jerk_fifo[index - 1];
							z_accel_fifo[index] = z_accel_fifo[index - 1];
						}
						z_jerk_fifo[0] = z_diff;
						z_accel_fifo[0] = z_nooffset;
						previous_z = z_data;
						previous_z_accumulator = z_accumulator;
					}
					gpio_get_value(buttons[button], &button_state);
				}
			}
		}	
	}
}

int write_i2c (int i2c_address, int reg_address, int data) {
	char write_buf[2];
	write_buf[0] = (char) reg_address;
	write_buf[1] = (char) data;
	return write(i2c_address, write_buf, 2);
}

