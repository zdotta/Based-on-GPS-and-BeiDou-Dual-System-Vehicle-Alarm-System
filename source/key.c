#include "data.h"
#include "func.h"
#include "M24C02.h"
#include "sys.h"
#include "key.h"

void key_callback(){

	//按下key1进入模式切换界面
	if(GetKeyAct(enumKey1) == enumKeyPress){
	  if(((status&0x0f)==0)||((status&0x0f)==0x08)){
	  	status = 0x01;
		}
	  else{
	  	status = (status&0x0f)*2;
		}
	}

	//定位模式下按下key2记录当前坐标 
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

		status = status + 0x10;		//记录坐标的单位移动1位
		point_addr = ((status>>4)&0x0f)*6;
		}
	}

	//防盗防走失模式下按下key2，进入距离设置界面
	else if((status&0x0f)==0x04){
		if(GetKeyAct(enumKey2)==enumKeyPress)
		status = 0x14;
	}

	//距离测量模式下按下key2，记录当前坐标并开始测量
	else if((status&0x0f)==0x08){
		if(GetKeyAct(enumKey2)==enumKeyPress){
			if(status==0x18)
				status = 0x08;
			else{
				status = 0x18;
																				    
				//更新当前坐标为记录坐标 
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


