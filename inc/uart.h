#ifndef _UART_H_
#define _UART_H_
#include "common.h"
#include "ds1302.h"

extern code char matchhead[6] = "$GNGGA";
extern xdata char buf[72];									//数据接收区
extern uchar hour_temp;										//小时，用于时区调整
extern struct_DS1302_RTC time;								//时间
extern uchar lon_h,lon_m,lon_s,lat_h,lat_m,lat_s;			//接收到的经纬度


#endif