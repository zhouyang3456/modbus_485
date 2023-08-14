/*
 * @Author: zhou yang 
 * @Date: 2023-08-04 00:30:11 
 * @Last Modified by: zhou yang
 * @Last Modified time: 2023-08-04 01:04:51
 */
#ifndef __SWITCH485__
#define __SWITCH485__

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "modbus/modbus.h"

#define SERVER_GPIO_INDEX   "475"

static int _server_ioctl_init(void)
{
   int fd;
   // index config
   fd = open("/sys/class/gpio/export", O_WRONLY);
   if(fd < 0)
         return 1;

   write(fd, SERVER_GPIO_INDEX, strlen(SERVER_GPIO_INDEX));
   close(fd);

   // direction config
   fd = open("/sys/class/gpio/gpio" SERVER_GPIO_INDEX "/direction", O_WRONLY);
   if(fd < 0)
         return 2;

   write(fd, "out", strlen("out"));
   close(fd);

   return 0;
}

static int _server_ioctl_on(void)
{
   int fd;

   fd = open("/sys/class/gpio/gpio" SERVER_GPIO_INDEX "/value", O_WRONLY);
   if(fd < 0)
         return 1;

   write(fd, "0", 1);
   close(fd);
   return 0;
}

static int _server_ioctl_off(void)
{
   int fd;

   fd = open("/sys/class/gpio/gpio" SERVER_GPIO_INDEX "/value", O_WRONLY);
   if(fd < 0)
         return 1;

   write(fd, "1", 1);
   close(fd);
   return 0;
}

static void _modbus_rtu_server_ioctl(modbus_t *ctx, int on)
{
   if (on) {
         _server_ioctl_on();
   } else {
         _server_ioctl_off();
   }
}

#endif