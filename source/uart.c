#include "uart.h"
#include "func.h"

//������յ��ı�������

void uart_callback(){

	//UTCʱ��У׼
	hour_temp = (buf[7] - '0')*10+(buf[8] - '0')+8;
	if(hour_temp > 24)
		hour_temp -= 24;
	time.hour = (hour_temp/10)*16 + hour_temp%10;
	time.minute = (buf[9] - '0')*16 + (buf[10] - '0');
	time.second = (buf[11] - '0')*16 + (buf[12] - '0');
	RTC_Write(time);

	//��γ������
	lat_h = (buf[18] - '0')*10 + (buf[19] - '0');
	lat_m = (buf[20] - '0')*10 + (buf[21] - '0');
	lat_s = ((buf[23] - '0')*1000 + (buf[24] - '0')*100+(buf[25] - '0')*10 + (buf[26] - '0'))*6/1000;
	lon_h = (buf[31] - '0')*100 + (buf[32] - '0')*10 + (buf[33] - '0');
	lon_m = (buf[34] - '0')*10 + (buf[35] - '0');
	lon_s = ((buf[37] - '0')*1000 + (buf[38] - '0')*100+(buf[39] - '0')*10 + (buf[40] - '0'))*6/1000;

	//�������
	if(lock_flag == 1||status == 0x18)
		dis = cal_dis();
	else if(lock_flag == 1 &&dis>safety_dis)
		status = 0x44;

}
