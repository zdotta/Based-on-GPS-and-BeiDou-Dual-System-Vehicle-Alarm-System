#ifndef _UART_H_
#define _UART_H_
#include "common.h"
#include "ds1302.h"

extern code char matchhead[6] = "$GNGGA";
extern xdata char buf[72];									//���ݽ�����
extern uchar hour_temp;										//Сʱ������ʱ������
extern struct_DS1302_RTC time;								//ʱ��
extern uchar lon_h,lon_m,lon_s,lat_h,lat_m,lat_s;			//���յ��ľ�γ��


#endif