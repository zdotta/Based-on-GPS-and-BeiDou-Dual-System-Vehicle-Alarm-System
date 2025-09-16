
#include "data.h"
#include "func.h"
#include "ds1302.h"
#include "displayer.h"
#include "beep.h"

#ifdef _displayer_H_                          //g~Ng 
code char decode_table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01,0x41,0x48,0x76,0x38,0x40,0x00,	
	              /* :   0   	1    2	   3    4	    5    6	  7   8	   	9	 		10	11	 12   13   14   15    16   17   18   19	 */
                /* :   0   	1    2     3    4     5    6    7   8    9  ()   -  -  -  -  -  H    L    - 	*/  
	                       0x3f|0x80,0x06|0x80,0x5b|0x80,0x4f|0x80,0x66|0x80,0x6d|0x80,0x7d|0x80,0x07|0x80,0x7f|0x80,0x6f|0x80 };  
             /* §³     20         21         22         23      24        25        26        27        28        29        */
#endif

void display(){
	
	//§µ
	if((status&0x0f)==0x01){
		time = RTC_Read();
		Seg7Print(time.hour/16,time.hour%16,12,time.minute/16,time.minute%16,12,time.second/16,time.second%16);
	}

	//¦Ã
	else if((status&0x0f)==0x02||status==0x08){
		if(lon_lat==0){
			Seg7Print(10,lon_h/100,(lon_h/10)%10,lon_h%10+20,lon_m/10,lon_m%10+20,(int)lon_s/10,(int)lon_s%10);
		}
		else{
			Seg7Print(10,10,lat_h/10,lat_h%10+20,lat_m/10,lat_m%10+20,(int)lat_s/10,(int)lat_s%10);
		}
	}

	//g'
	else if((status&0x0f)==0x04){
		if(status==0x04){							//key2key3
			if(lock_flag==0)
				Seg7Print(10,10,10,10,10,0,30,30);	//OFF
			else
				Seg7Print(10,10,10,10,10,10,0,31);	//ON
			
		}
		else if(status==0x14){							//g'
			Seg7Print(10,10,10,10,(dis_temp/1000)+((weixuan&0x08)>>3)*20,(dis_temp/100)%10+((weixuan&0x04)>>2)*20,(dis_temp/10)%10+((weixuan&0x02)>>1)*20,dis_temp%10+(weixuan&0x01)*20);
		}
		else if(status==0x24){
			Seg7Print(10,10,10,10,(code_temp/1000)+((weixuan&0x08)>>3)*20,(code_temp/100)%10+((weixuan&0x04)>>2)*20,(code_temp/10)%10+((weixuan&0x02)>>1)*20,code_temp%10+(weixuan&0x01)*20);
		}
		else if(status==0x44){
			Seg7Print(10,10,10,10,(trycode/1000)+((weixuan&0x08)>>3)*20,(trycode/100)%10+((weixuan&0x04)>>2)*20,(trycode/10)%10+((weixuan&0x02)>>1)*20,trycode%10+(weixuan&0x01)*20);
		}

	//
		else if(status==0x18){
			Seg7Print((dis/1000000)%10,(dis/1000000)%10,(dis/100000)%10,(dis/10000)%10,(dis/1000)%10,(dis/100)%10,(dis/10)%10,dis%10);
		}

	//
		if(status==0x44){
			SetBeep(1000,20);
		LedPrint(status);
		}
		}
}
