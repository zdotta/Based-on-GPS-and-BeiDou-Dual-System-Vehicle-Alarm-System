#include "data.h"
#include "func.h"
#include "M24C02.h"
#include "sys.h"
#include "key.h"

void key_callback(){

	//����key1����ģʽ�л�����
	if(GetKeyAct(enumKey1) == enumKeyPress){
	  if(((status&0x0f)==0)||((status&0x0f)==0x08)){
	  	status = 0x01;
		}
	  else{
	  	status = (status&0x0f)*2;
		}
	}

	//��λģʽ�°���key2��¼��ǰ���� 
	if((status&0x0f) == 0x02){
		if(GetKeyAct(enumKey2) == enumKeyPress){
		
		M24C02_Write(point_addr,lat_h);
		Delay10ms();
  		
		M24C02_Write(point_addr+0x01,lat_m);
		Delay10ms();

		M24C02_Write(point_addr+0x02,lat_s);
		Delay10ms();

		M24C02_Write(point_addr+0x03,lon_h);
		Delay10ms();

		M24C02_Write(point_addr+0x04,lon_m);
		Delay10ms();

		M24C02_Write(point_addr+0x05,lon_s);
		Delay10ms();

		status = status + 0x10;		//��¼����ĵ�λ�ƶ�1λ
		point_addr = ((status>>4)&0x0f)*6;
		}
	}

	//��������ʧģʽ�°���key2������������ý���
	else if((status&0x0f)==0x04){
		if(GetKeyAct(enumKey2)==enumKeyPress)
		status = 0x14;
	}

	//�������ģʽ�°���key2����¼��ǰ���겢��ʼ����
	else if((status&0x0f)==0x08){
		if(GetKeyAct(enumKey2)==enumKeyPress){
			if(status==0x18)
				status = 0x08;
			else{
				status = 0x18;
																				    
				//���µ�ǰ����Ϊ��¼���� 
				xlon_h=lon_h,
				xlon_m=lon_m,
				xlon_s=lon_s,
				xlat_h=lat_h,
				xlat_m=lat_m,
				xlat_s=lat_s;
			}
		}
   }
}


