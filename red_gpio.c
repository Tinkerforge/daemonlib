/*
 * daemonlib
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * red_gpio.h: GPIO functions for RED Brick
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// perror
#include <stdio.h>
#include <errno.h>

// sysconf
#include <unistd.h>

// mmap
#include <sys/mman.h>

#include "red_gpio.h"

#define GPIO_BASE         0x01c20800

static volatile GPIOPort *gpio_port;

int gpio_init() {
	int fd;
	uint32_t address_start, address_offset;
	uint32_t page_size, page_mask;
	void *mapped_base;

	fd = open("/dev/mem", O_RDWR);
	if (fd < 0) {
		goto error;
	}

	page_size = sysconf(_SC_PAGESIZE);
	page_mask = ~(page_size - 1);

	address_start  = GPIO_BASE &  page_mask;
	address_offset = GPIO_BASE & ~page_mask;
      
	mapped_base = (void *)mmap(0, 
	                           page_size*2, 
							   PROT_READ|PROT_WRITE, 
							   MAP_SHARED, 
							   fd, 
							   address_start);

	if (mapped_base == MAP_FAILED) {
		goto error;
	}

	gpio_port = mapped_base + address_offset;
   
	close(fd);
	return 0;

error:
	perror("Unable to mmap /dev/mem");
	return -1;
}

void gpio_mux_configure(const GPIOPin pin, const GPIOMux mux_config) {
	uint32_t config_index = (pin.pin_index >> 3);
	uint32_t offset       = (pin.pin_index & 0x7) << 2;
 
	uint32_t config       = gpio_port[pin.port_index].config[config_index];
	config &= ~(0xF << offset);
	config |= mux_config << offset;

	gpio_port[pin.port_index].config[config_index] = config;
}

void gpio_output_set(const GPIOPin pin) {
	gpio_port[pin.port_index].value |= (1 << pin.pin_index);
}

void gpio_output_clear(const GPIOPin pin) {
	gpio_port[pin.port_index].value &= ~(1 << pin.pin_index);
}

uint32_t gpio_input(const GPIOPin pin) {
	return gpio_port[pin.port_index].value & (1 << pin.pin_index);
}