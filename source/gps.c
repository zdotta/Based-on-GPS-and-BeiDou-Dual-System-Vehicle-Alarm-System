#include "STC15F2K60S2.h"        //���롣
#include "sys.h"                 //���롣
#include "displayer.h"
#include "key.h"
#include "uart1.h"
#include "DS1302.h"
#include "adc.h"
#include "M24C02.h"
#include "beep.h"
#include <math.h>
#include "music.h"
#include "uart2.h"
#include "vib.h"
#warning "Created by zdot"
code unsigned long SysClock=11059200;         //���롣����ϵͳ����ʱ��Ƶ��(Hz)���û������޸ĳ���ʵ�ʹ���Ƶ�ʣ�����ʱѡ��ģ�һ��
#ifdef _displayer_H_                          //��ʾģ��ѡ��ʱ���롣���������ʾ������Ñ����޸ġ����ӵȣ� 
code char decode_table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01,0x41,0x48,0x76,0x38,0x40,0x00,	
	              /* ���:   0   	1    2	   3    4	    5    6	  7   8	   	9	 		10	11	 12   13   14   15    16   17   18   19	 */
                /* ��ʾ:   0   	1    2     3    4     5    6    7   8    9  (��)   ��-  ��-  ��-  ����-  ����-  H    L    - 	���ޣ�*/  
	                       0x3f|0x80,0x06|0x80,0x5b|0x80,0x4f|0x80,0x66|0x80,0x6d|0x80,0x7d|0x80,0x07|0x80,0x7f|0x80,0x6f|0x80,   
             /* ��С����     20         21         22         23      24        25        26        27        28        29        */
							0x5c,0x77,0x71,0x37,0x31,0x39,0x3e,0x38};
						// 30 o	 31A  32F  33N  34T	 35C  36U	37L
#endif

#ifndef uchar 
#define uchar unsigned char
#endif

#ifndef uint  
#define uint  unsigned int
#endif


code uchar alarm[]=
{	0x36,0x18,
	0x33,0x18,
	0x36,0x18,
	0x33,0x18,
	0x21,0x18,
	0x21,0x18,
	0x21,0x18,
};
					
#define PI 3.1415926535
#define EARTH_RADIUS 637100 //��λΪ10m
  char zdot;
  code char matchhead[6] = "$GNGGA";
  int automode =0;											//�����ж��Ƿ����Զ���λ
  xdata char buf[72],uart2_buf[72];									//���ݽ�����
  uchar hour_temp;										//Сʱ������ʱ������
  struct_DS1302_RTC time;								//ʱ��
  uchar lon_h,lon_m,lon_s,lat_h,lat_m,lat_s;			//���յ��ľ�γ��
  bit lon_lat = 0;										//��ʾ���Ȼ�γ��
  uchar status = 0;									//ģʽѡ��
  uchar i,weixuan = 1;									//��������ѡ��
  uchar lock_flag = 0;									//����/����ʧ�Ƿ������洢��0x80���ϵ�ʱ��ȡ
  uchar xlon_h,xlon_m,xlon_s,xlat_h,xlat_m,xlat_s;		//����/��๦�ܼ�¼������㾭γ��
  uchar point_addr = 0;								//����洢�ڷ���ʧ�Դ洢��λ��
  xdata uint safety_dis,dis_temp,dis=0;				//��ȫ���룬�洢��0x81���ϵ�ʱ��ȡ���趨���룻ʵ�ʾ���
  xdata uint lockcode,code_temp = 0,trycode = 0;		//���룬�洢��0x85���ϵ�ʱ��ȡ���趨ʱ��������룻���Խ������������
  xdata uchar send_buf[19];								//���ݷ�����



void uart1_callback();		//����1���ջص�
void uart2_callback();		//����2���ջص�
void key_callback();		//�����ص�			  	
void nav_callback();		//���������ص�
void display();				//����ܼ�led��ʾ��1s������ʾ���°�ť�������ʾ
void Delay10ms();			//��ʱ����������д�����ʧ�Դ洢��
int cal_dis();				//�������
double radian(float);		//�Ƕ�ת����
void M24C02_Init();			//ϴ�ڴ�
void vib_callback();		//�쳣�𶯼��


void Delay10ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}


void M24C02_Init(){
	int a = 0;
	for(;a<256;a++){
	M24C02_Write(a,0);
	Delay10ms();
	}
}


void vib_callback(){
	if(GetVibAct()==enumVibQuake){
		status=0x44;}
	else{
		status=0x04;}
}
//������յ��ı�������
void uart1_callback()
{		
	if(automode==0){					   //�����ֶ���λģʽʱ�����ô���1������
	//UTCʱ��У׼
	DS1302Init(time);				
	hour_temp = (buf[7] - '0')*10+(buf[8] - '0')+8;
	if(hour_temp > 24)
		hour_temp -= 24;
	time.hour = (hour_temp/10)*16 + hour_temp%10;
	time.minute = (buf[9] - '0')*16 + (buf[10] - '0');
	time.second = (buf[11] - '0')*16 + (buf[12] - '0');
	Seg7Print(time.hour/16,time.hour%16,12,time.minute/16,time.minute%16,12,time.second/16,time.second%16);	
	RTC_Write(time);
	display();
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
	if(lock_flag == 1 && dis>safety_dis)
		status = 0x44;	
	 }	
}
void uart2_callback(){
	if(automode==1){					   //�����Զ���λģʽʱ�����ô���2������
	//UTCʱ��У׼				
	hour_temp = (uart2_buf[7] - '0')*10+(uart2_buf[8] - '0')+8;
	if(hour_temp > 24)
		hour_temp -= 24;
	time.hour = (hour_temp/10)*16 + hour_temp%10;
	time.minute = (uart2_buf[9] - '0')*16 + (uart2_buf[10] - '0');
	time.second = (uart2_buf[11] - '0')*16 + (uart2_buf[12] - '0');
	Seg7Print(time.hour/16,time.hour%16,12,time.minute/16,time.minute%16,12,time.second/16,time.second%16);
	RTC_Write(time);
	

	//��γ������
	lat_h = (uart2_buf[18] - '0')*10 + (uart2_buf[19] - '0');
	lat_m = (uart2_buf[20] - '0')*10 + (uart2_buf[21] - '0');
	lat_s = ((uart2_buf[23] - '0')*1000 + (uart2_buf[24] - '0')*100+(uart2_buf[25] - '0')*10 + (uart2_buf[26] - '0'))*6/1000;
	lon_h = (uart2_buf[31] - '0')*100 + (uart2_buf[32] - '0')*10 + (uart2_buf[33] - '0');
	lon_m = (uart2_buf[34] - '0')*10 + (uart2_buf[35] - '0');
	lon_s = ((uart2_buf[37] - '0')*1000 + (uart2_buf[38] - '0')*100+(uart2_buf[39] - '0')*10 + (uart2_buf[40] - '0'))*6/1000;

	//�������
	if(lock_flag == 1||status == 0x18)
		dis = cal_dis();

	if(lock_flag == 1 && dis>safety_dis)
		status = 0x44;
		display();
	}
}
void key_callback(){

	//����key1����ģʽ�л�����
	if(GetKeyAct(enumKey1) == enumKeyPress){
	  if((((status&0x0f)==0)||((status&0x0f)==0x08))&&(status&0x0f)!=0x03){	//�����ڳ�ʼ״̬�������һ��ģʽʱ�����ص���һ��ģʽ
	  	status = 0x01;
		}
	  else{
	  	status = (status&0x0f)*2;  		//������һģʽ
		}
	}

	//������ʱ��У׼ģʽʱ��key2�����Զ���λ��key3�ر��Զ���λ
	if((status&0x0f) == 0x01){
		if(GetKeyAct(enumKey2) == enumKeyPress){
			status=0x03;
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
			if(status==0x18)		//���Ѿ�������ĵڶ����꣬�򷵻ز����� 
				status = 0x08;
			else{
				status = 0x18;		//���ڲ�๦��ʱ������ʱ��������ĵ�һ�����꣬��status���£�׼��������һ����																				    
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
   //display();
}
void nav_callback(){
	 
	 if((status&0x0f)==0x01){		//������ʱ��ģʽ������key3����ϴ�����ڴ�
	 	if(GetAdcNavAct(enumAdcNavKey3)==enumKeyPress){
 			automode=0;					//ϴ�����ڴ��ͬʱ��Ҳȡ���Զ���λ״̬
			M24C02_Init();
		  }
		}
	 if((status&0x0f)==0x03){
		if(GetAdcNavAct(enumAdcNavKeyCenter)==enumKeyPress){
			status=0x01;
		
		}
		if(GetAdcNavAct(enumAdcNavKeyUp)==enumKeyPress){
			automode=1;
		}
		if(GetAdcNavAct(enumAdcNavKeyDown)==enumKeyPress){
			automode=0;
		}
	}

    if((status&0x0f)==0x02){    //�����ڶ�λģʽ��ʹ�õ�����ʱ
        if(GetAdcNavAct(enumAdcNavKeyRight)==enumKeyPress||GetAdcNavAct(enumAdcNavKeyLeft)==enumKeyPress)
            lon_lat = !lon_lat;
        else if(GetAdcNavAct(enumAdcNavKeyUp)==enumKeyPress){
            status += 0x10;                    //�洢��Ԫ+1
            point_addr = ((status>>4)&0x0f)*6;  //һ������ռ6���ֽ�
        }
		else if(GetAdcNavAct(enumAdcNavKeyDown)==enumKeyPress){
            status = ((status&0xf0)-0x10)|(status&0x0f);
			point_addr = ((status>>4)&0x0f)*6;  //һ������ռ6���ֽ�
        }
        else if(GetAdcNavAct(enumAdcNavKey3)==enumKeyPress){    //������������
            i = M24C02_Read(point_addr);
            send_buf[0] = i/10+'0',
            send_buf[1] = i%10+'0',
            send_buf[2] = '.';

            i = M24C02_Read(point_addr+1);
            send_buf[3] = i/10+'0',
            send_buf[4] = i%10+'0',
            send_buf[5] = '.';

            i = M24C02_Read(point_addr+2);
            send_buf[6] = i/10+'0',
            send_buf[7] = i%10+'0',
            send_buf[8] = '.';

            i = M24C02_Read(point_addr+3);
            send_buf[9] = i/100+'0',
            send_buf[10] = (i/10)%10+'0',
            send_buf[11] = i%10+'0',
            send_buf[12] = '.';

            i = M24C02_Read(point_addr+4);
            send_buf[13] = i/10+'0',
            send_buf[14] = i%10+'0',
            send_buf[15] = '.';

            i = M24C02_Read(point_addr+5);
            send_buf[16] = i/10+'0',
            send_buf[17] = i%10+'0',
            send_buf[18] = '.';

            Uart1Print(send_buf,19);
        }
    }

    
    else if((status&0x0f)==0x04){   //�����ڷ�������ʧģʽ��ʹ�õ�����
        //������ʾ
        if(status == 0x04){         //���ڷ�������ʧ��ʼ����
            if(GetAdcNavAct(enumAdcNavKeyCenter) == enumKeyPress){
                if(lock_flag == 0x00){         //��δ��������򿪽���
                    lock_flag = 0x01;
                    M24C02_Write(0x80,lock_flag);
                    xlon_h=lon_h,
                    xlon_m=lon_m,
                    xlon_s=lon_s,
                    xlat_h=lat_h,
                    xlat_m=lat_m,
                    xlat_s=lat_s;
                    trycode = 0;
                }
                else
                    status = 0x44;             //�رոý�����Ҫ���룬������������״̬
            }
		  }
            if(GetAdcNavAct(enumAdcNavKey3) == enumKeyPress)
                status = 0x24;                 //����key3��������������״̬

            //������ڰ�ȫ��������ģʽ
            if(status == 0x14){     //���ð�ȫ�������
                if(GetAdcNavAct(enumAdcNavKeyLeft)==enumKeyPress){
                    weixuan = (weixuan<<1)&0x0f;
                    if(weixuan == 0)
                        weixuan =1;
                }
                else if(GetAdcNavAct(enumAdcNavKeyRight)==enumKeyPress){
                    weixuan = (weixuan>>1);
                    if(weixuan == 0)
                        weixuan = 1;

                }
                else if(GetAdcNavAct(enumAdcNavKeyUp)==enumKeyPress){
                    dis_temp += (weixuan&0x08)*125+(weixuan&0x04)*25+(weixuan&0x02)*5+(weixuan&0x01);
                    if(dis_temp>9999)
                        dis_temp=0;

                }
                else if(GetAdcNavAct(enumAdcNavKeyDown)==enumKeyPress){
                    dis_temp -= (weixuan&0x08)*125+(weixuan&0x04)*25+(weixuan&0x02)*5+(weixuan&0x01);
                    if(dis_temp<0)
                        dis_temp=0;
                }
                else if(GetAdcNavAct(enumAdcNavKeyCenter)==enumKeyPress){
                    safety_dis = dis_temp;
                    M24C02_Write(0x81,safety_dis);
					status = 0x04;
                }
            }

            //���������������ģʽ
            else if(status == 0x24){    //�����������
                if(GetAdcNavAct(enumAdcNavKeyLeft)==enumKeyPress){
                    weixuan = (weixuan<<1)&0x0f;
                    if(weixuan == 0)
                        weixuan = 1;
                }
                else if(GetAdcNavAct(enumAdcNavKeyRight)==enumKeyPress){
                    weixuan = (weixuan>>1);
                    if(weixuan == 0)
                        weixuan = 1;

                }
                else if(GetAdcNavAct(enumAdcNavKeyUp)==enumKeyPress){
                    code_temp += (weixuan&0x08)*125+(weixuan&0x04)*25+(weixuan&0x02)*5+(weixuan&0x01);
                    if(code_temp>9999)
                        code_temp=0;

                }
                else if(GetAdcNavAct(enumAdcNavKeyDown)==enumKeyPress){
                    code_temp -= (weixuan&0x08)*125+(weixuan&0x04)*25+(weixuan&0x02)*5+(weixuan&0x01);
                    if(code_temp<0)
                        code_temp=0;
                }
                else if(GetAdcNavAct(enumAdcNavKeyCenter)==enumKeyPress){
                    lockcode = code_temp;
                    M24C02_Write(0x85,lockcode);
					status = 0x04;
                }

//				   if(GetAdcNavAct(enumAdcNavKeyLeft)==enumKeyPress){			 //����ǵ�������
//                    num++;
//                    Seg7Print(10,10,10,10,10,10,10,num);
//					LedPrint(status);
//					print[0]=num;
//					Uart1Print(print,1);
//                }
            }

            //��������ģʽ
            else if(status == 0x44){    //�����������
                if(GetAdcNavAct(enumAdcNavKeyLeft)==enumKeyPress){
                    weixuan = (weixuan<<1)&0x0f;
                    if(weixuan == 0)
                        weixuan = 1;
                }
                else if(GetAdcNavAct(enumAdcNavKeyRight)==enumKeyPress){
                    weixuan = (weixuan>>1);
                    if(weixuan == 0)
                        weixuan = 1;

                }
                else if(GetAdcNavAct(enumAdcNavKeyUp)==enumKeyPress){
                    trycode += (weixuan&0x08)*125+(weixuan&0x04)*25+(weixuan&0x02)*5+(weixuan&0x01);
                    if(trycode>9999)
                        trycode=0;

                }
                else if(GetAdcNavAct(enumAdcNavKeyDown)==enumKeyPress){
                    trycode -= (weixuan&0x08)*125+(weixuan&0x04)*25+(weixuan&0x02)*5+(weixuan&0x01);
                    if(trycode<0)
                        trycode=0;
                }
                else if(GetAdcNavAct(enumAdcNavKeyCenter)==enumKeyPress){
                    if(trycode == lockcode){
                        lock_flag = 0;      	//�������������ܹر�
						SetPlayerMode(enumModeStop); 	//ֹͣ����
                        M24C02_Write(0x80,lock_flag);
                        status = 0x04;      	//���ص���������ʧģʽ�ĳ�ʼ����
                        trycode = 0;
                    }
                }
            }  
    }
    
    //�����ڲ��ģʽ��ʹ�õ�����
    else if(status == 0x08){    //��๦�ܽ���
        if(GetAdcNavAct(enumAdcNavKeyRight)==enumKeyPress||GetAdcNavAct(enumAdcNavKeyLeft)==enumKeyPress)
                lon_lat = !lon_lat;
    }

    //�����������������led������ܵ���ʾ
    display();    
}
void display(){

	if(lock_flag==1){
		SetEventCallBack(enumEventVib,vib_callback);
	} 
	//У׼ʱ��ģʽ
	if((status&0x0f)==0x01){
		time = RTC_Read();
		Seg7Print(time.hour/16,time.hour%16,12,time.minute/16,time.minute%16,12,time.second/16,time.second%16);
	}
	
	if((status&0x0f)==0x03){
		Seg7Print(31,36,34,0,10,10,10,automode);
	}

	//��λģʽ
	else if((status&0x0f)==0x02||status==0x08){
		if(lon_lat==0){
			Seg7Print(30,lon_h/100,(lon_h/10)%10,lon_h%10+20,lon_m/10,lon_m%10+20,(int)lon_s/10,(int)lon_s%10); 	//��ʾo������longitude γ��
		}
		else{
			Seg7Print(31,10,lat_h/10,lat_h%10+20,lat_m/10,lat_m%10+20,(int)lat_s/10,(int)lat_s%10);				//��ʾA������latitude  ����	
		}
	}

	//����ģʽ
	else if((status&0x0f)==0x04){
		if(status==0x04){							//key2key3
			if(lock_flag==0)
				Seg7Print(36,37,35,10,10,0,32,32);	//ULC	OFF
			else
				Seg7Print(37,35,10,10,10,10,0,33);	//LC	ON
			
		}
		else if(status==0x14){							//���ð�ȫ����ģʽ
			Seg7Print(0,5,34,10,(dis_temp/1000)+((weixuan&0x08)>>3)*20,(dis_temp/100)%10+((weixuan&0x04)>>2)*20,(dis_temp/10)%10+((weixuan&0x02)>>1)*20,dis_temp%10+(weixuan&0x01)*20);
		}
		else if(status==0x24){							//��������ģʽ
			Seg7Print(5,35,34,10,(code_temp/1000)+((weixuan&0x08)>>3)*20,(code_temp/100)%10+((weixuan&0x04)>>2)*20,(code_temp/10)%10+((weixuan&0x02)>>1)*20,code_temp%10+(weixuan&0x01)*20);
			//Seg7Print(5,35,34,10,10,10,10,num);	   //���ԭ�ǵ�������

		}
		else if(status==0x44){
			Seg7Print(10,10,10,10,(trycode/1000)+((weixuan&0x08)>>3)*20,(trycode/100)%10+((weixuan&0x04)>>2)*20,(trycode/10)%10+((weixuan&0x02)>>1)*20,trycode%10+(weixuan&0x01)*20);
		}	
		}
		
	//���ģʽ
	else if(status==0x18){
		Seg7Print((dis/1000000)%10,(dis/1000000)%10,(dis/100000)%10,(dis/10000)%10,(dis/1000)%10,(dis/100)%10,(dis/10)%10,dis%10);
	}
	
	
	//����ģʽ				   	
	if(status==0x44){								 //��������
		
		if(lock_flag==1){
			
			SetMusic(120,0xFC,alarm,sizeof(alarm),enumMscDrvLed);		  //���𾯱�
			SetPlayerMode(enumModePlay);
			//SetEventCallBack(enumEventSys10mS,alarm_callback);
			}
		else{
			//SetPlayerMode(enumModePause);
			status=0x04;
			}
	}
		
	LedPrint(status);
}
double radian(float angle){     //�Ƕ�ת������ 
    return angle*PI/180;    
}
int cal_dis(){          //��λ10m 
    xdata double X1,Y1,X2,Y2,res;
    xdata double a;
    xdata double b;
    X1 = radian(xlat_h + ((float)xlat_m)/100.0 + ((float)xlat_s)/10000.0);
    X2 = radian(lat_h + ((float)lat_m)/100.0 + ((float)lat_s)/10000.0);
    Y1 = radian(xlon_h + ((float)xlon_m)/100.0 + ((float)xlon_s)/10000.0);
    Y2 = radian(lon_h + ((float)lon_m)/100.0 + ((float)lon_s)/10000.0);
    a = X1-X2;
    b = Y1-Y2;
    res = 2*EARTH_RADIUS*asin((sqrt(pow(sin(a/2),2)+cos(X1)*cos(X2)*pow(sin(b/2),2))));
    return (int)(res/10);
}


void main(){
	automode=0;								//ÿ��reset��Ҫ��ʼ��autoΪ�㣬ȷ����ǰΪ���ֶ���λ״̬
	KeyInit();
	DisplayerInit();
	DS1302Init(time);
	AdcInit(ADCexpEXT);					//����Ҫ����EXIT�ϵĵ�ƽ��Ϣ����Ϊ������gpsģ����д���2��ͨ��
	BeepInit();
	MusicPlayerInit();
	VibInit();

	SetDisplayerArea(0,7);
	LedPrint(status);
	Uart1Init(9600);
	Uart2Init(9600,Uart2UsedforEXT);  		//������Ϊ9600��������Դ��EXIT��

	SetUart1Rxd(buf, sizeof(buf), matchhead, 6);
	SetEventCallBack(enumEventUart1Rxd,uart1_callback);
	
	SetEventCallBack(enumEventUart2Rxd,uart2_callback);
	SetUart2Rxd(uart2_buf, sizeof(uart2_buf), matchhead, 6);
	//SetEventCallBack(enumEventUart2Rxd,uart_callback);


	SetEventCallBack(enumEventKey,key_callback);
	SetEventCallBack(enumEventNav,nav_callback);
	SetEventCallBack(enumEventSys100mS,display);
	MySTC_Init();	    
	while(1)             	
	{
		MySTC_OS();    
	}
}
