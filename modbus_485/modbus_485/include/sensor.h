/*
 * @Author: zhou yang 
 * @Date: 2023-08-04 00:29:58 
 * @Last Modified by: zhou yang
 * @Last Modified time: 2023-08-04 01:04:46
 */
#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "modbus/modbus.h"

#define JXBS_3001_TR
#define VMS_3000_TR_3W3S
#define WX_WQX7

/*************************JXBS_3001_TR_Sensor*************************************/

#ifdef JXBS_3001_TR
typedef struct
{
    uint16_t pH;
    uint16_t moisture;
    uint16_t temperature;
    uint16_t electric_conductivity;
    uint16_t nitrogen;
    uint16_t phosphorus;
    uint16_t potassium;
    uint16_t address;
    uint16_t baud_rate;
} JXBS_3001_TR_Sensor;

#define JXBS_COUNT 9
const uint16_t JXBS_addr[JXBS_COUNT] = {0x0006, 0x0012, 0x0013, 0x0015,
                                        0x001E, 0x1F, 0x0020, 0x0100, 0x0101};
const int JXBS_DEVICE_ADDR = 1;
const int JXBS_BAUD_RATE = 9600;

bool JXBS_get(modbus_t *ctx_, JXBS_3001_TR_Sensor *s)
{
    int rc;
    uint16_t tab_reg[512];
    memset(tab_reg, 0, 1024);
    uint16_t temp[JXBS_COUNT];

    modbus_set_slave(ctx_, JXBS_DEVICE_ADDR);
    for (int i = 0; i < JXBS_COUNT; i++)
    {
        rc = modbus_read_registers(ctx_, JXBS_addr[i], 1, tab_reg);
        if (rc == -1)
        {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return FALSE;
        }
        temp[i] = tab_reg[0];
        usleep(20000);
    }
    memcpy(s, temp, sizeof(JXBS_3001_TR_Sensor));
    return TRUE;
}

void JXBS_print(const JXBS_3001_TR_Sensor *s)
{
    printf("====================JXBS_3001_TR_Sensor====================\n");
    printf("%20s: %d\n", "pH (0.01pH)", s->pH);
    printf("%20s: %d\n", "moisture (0.1%RH)", s->moisture);
    printf("%20s: %d\n", "temperature (0.1C)", s->temperature);
    printf("%20s: %d\n", "electric_cond (us/cm)", s->electric_conductivity);
    printf("%20s: %d\n", "nitrogen (mg/kg)", s->nitrogen);
    printf("%20s: %d\n", "phosphorus (mg/kg)", s->phosphorus);
    printf("%20s: %d\n", "potassium (mg/kg)", s->potassium);
    printf("%20s: %d\n", "device address", s->address);
    printf("%20s: %d\n", "baud_rate", s->baud_rate);
    printf("\n");
}
#endif

/*************************VMS-3000-TR-3W3S*************************************/

#ifdef VMS_3000_TR_3W3S
typedef struct
{
    uint16_t moisture_1;
    uint16_t temperature_1;
    uint16_t moisture_2;
    uint16_t temperature_2;
    uint16_t moisture_3;
    uint16_t temperature_3;
    uint16_t address;
    uint16_t baud_rate;
} VMS_3000_TR_3W3S_Sensor;

#define VMS_COUNT 8
const uint16_t VMS_addr[VMS_COUNT] = {0x0000, 0x0001, 0x0002, 0x0003,
                                      0x0004, 0x0005, 0x07D0, 0x07D1};
const int VMS_DEVICE_ADDR = 2;
const int VMS_BAUD_RATE = 9600;

bool VMS_get(modbus_t *ctx_, VMS_3000_TR_3W3S_Sensor *s)
{
    int rc;
    uint16_t tab_reg[512];
    memset(tab_reg, 0, 1024);
    uint16_t temp[VMS_COUNT];

    modbus_set_slave(ctx_, VMS_DEVICE_ADDR);
    for (int i = 0; i < VMS_COUNT; i++)
    {
        rc = modbus_read_registers(ctx_, VMS_addr[i], 1, tab_reg);
        if (rc == -1)
        {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return FALSE;
        }
        temp[i] = tab_reg[0];
        usleep(20000);
    }
    memcpy(s, temp, sizeof(VMS_3000_TR_3W3S_Sensor));
    return TRUE;
}

void VMS_print(const VMS_3000_TR_3W3S_Sensor *s)
{
    printf("====================VMS_3000_TR_3W3S_Sensor====================\n");
    printf("%20s: %5d\n", "moisture_1 (0.1%RH)", s->moisture_1);
    printf("%20s: %5d\n", "temperature_1 (0.1C)", s->temperature_1);
    printf("%20s: %5d\n", "moisture_2 (0.1%RH)", s->moisture_2);
    printf("%20s: %5d\n", "temperature_2 (0.1C)", s->temperature_2);
    printf("%20s: %5d\n", "moisture_3 (0.1%RH)", s->moisture_3);
    printf("%20s: %5d\n", "temperature_3 (0.1C)", s->temperature_3);
    printf("%20s: %5d\n", "device address", s->address);
    printf("%20s: %5d\n", "baud_rate", s->baud_rate);
    printf("\n");
}
#endif

/*************************WX_WQX7*************************************/
#ifdef WX_WQX7
typedef struct
{
    uint16_t wind_speed;     // 0x0000
    uint16_t wind_direction; // 0x0001
    uint16_t temperature;
    uint16_t humidity;
    uint16_t pressure;
    uint16_t rainfall_high;
    uint16_t rainfall_low;
    uint16_t total_radiation_high;
    uint16_t total_radiation_low;
} WX_WQX7_Sensor;

const int WIND_SENSOR = 0xC8;
const int AIR_SENSOR = 0x66;
const int RAIN_SENSOR = 0xCA;
const int SUN_SENSOR = 0xCC;

#define WX_COUNT 9
const uint16_t WX_addr[WX_COUNT] = {0x0000, 0x0001,
                                    0x0000, 0x0001, 0x0002,
                                    0x0000, 0x0001,
                                    0x0003, 0x0004};
const int WX_BAUD_RATE = 9600;

bool WX_get(modbus_t *ctx_, WX_WQX7_Sensor *s)
{
    int rc;
    uint16_t tab_reg[512];
    memset(tab_reg, 0, 1024);
    uint16_t temp[WX_COUNT];

    modbus_set_slave(ctx_, WIND_SENSOR);
    for (int i = 0; i < WX_COUNT; i++)
    {
        if (i == 2)
        {
            modbus_set_slave(ctx_, AIR_SENSOR);
        }
        else if (i == 5)
        {
            modbus_set_slave(ctx_, RAIN_SENSOR);
        }
        else if (i == 7)
        {
            modbus_set_slave(ctx_, SUN_SENSOR);
        }

        rc = modbus_read_registers(ctx_, WX_addr[i], 1, tab_reg);
        if (rc == -1)
        {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return FALSE;
        }
        temp[i] = tab_reg[0];
        usleep(20000);
    }
    memcpy(s, temp, sizeof(WX_WQX7_Sensor));
    return TRUE;
}

void WX_print(const WX_WQX7_Sensor *s)
{
    printf("====================WX_WQX7_Sensor====================\n");
    printf("%20s: %5d\n", "wind_speed (0.01m/s)", s->wind_speed);
    printf("%20s: %5d\n", "wind_direction", s->wind_direction);
    printf("%20s: %5d\n", "temperature (0.01C)", s->temperature);
    printf("%20s: %5d\n", "humidity (0.01%RH)", s->humidity);
    printf("%20s: %5d\n", "pressure (0.01Kpa)", s->pressure);
    printf("%20s: %5d\n", "rainfall_high", s->rainfall_high);
    printf("%20s: %5d\n", "rainfall_low", s->rainfall_low);
    printf("%20s: %5d\n", "total_radiation_high", s->total_radiation_high);
    printf("%20s: %5d\n", "total_radiation_low", s->total_radiation_low);
    printf("\n");
}
#endif

#endif // __SENSOR_H__