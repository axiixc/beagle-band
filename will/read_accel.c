
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define READ_BIT 1
#define BUTTON_GPIO_PINS {60}
#define BUTTON_ACTIVE_EDGES {0} 

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
	
	write_i2c(file, 0x31, 0x03); //0x03 into DATA_FORMAT
	write_i2c(file, 0x2d, 0x08); //0x08 into POWER_CTL
	write_i2c(file, 0x2e, 0x02); //0x02 into INT_ENABLE
	write_i2c(file, 0x38, 0x50); //0x50 into FIFO_CTL
	
	//setup interrupts
	int buttons[] = BUTTON_GPIO_PINS;
	int button_size = sizeof(buttons)/sizeof(buttons[0]);
	int button_active_edges[] = BUTTON_ACTIVE_EDGES;
	int sensor_addresses[] = SENSOR_ADDRESSES;
	
	struct pollfd fdset[button_size];
	int nfds = button_size;	
	int gpio_fd[button_size];
	for(i = 0; i < button_size; i++)
	{
		gpio_export(buttons[i]);
		gpio_set_dir(buttons[i], "in");
		gpio_set_edge(buttons[i], button_active_edges[i] ? "rising" : "falling");
		gpio_fd[i] = gpio_fd_open(buttons[i], O_RDONLY);
	}
	while(keepgoing) {

		memset((void*)fdset, 0, sizeof(fdset));
		for(i = 0; i < button_size; i++) {		
			fdset[i].fd = gpio_fd[i];
			fdset[i].events = POLLPRI;
		}
		poll(fdset, nfds, 100);
		for(i = 0; i < button_size; i++) {
			if(fdset[i].revents & POLLPRI) {
				char buf[1];
				read(fdset[i].fd, buf, 1);
				
				int button_state;
				gpio_get_value(buttons[i], &button_state);
				while(button_state == button_active_edges[i]) {
					char write_buf[1];
					write_buf[0] = 0x32;
					if(write(file, write_buf,1) != 1) {
						printf("Failed to write to the i2c bus.\n");
					}

					int reg_num;
					char read_buf[6];
					if(read(file, read_buf, 6) != 1) {
						printf("Failed to read from the i2c bus.\n");
					}
					else {
						for(reg_num = 0; reg_num < 6; reg_num++) {
							printf("Register %d contains: %x\n", 
							reg_num, read_buf[reg_num]);
						}
					}
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

