
#include "data.h"
#include "func.h"
#include "ds1302.h"
#include "M24C02.h"
#include "adc.h"
#include "sys.h"
#include "key.h"
#include "uart1.h"

void nav_callback(){

    
    if((status&0x0f)==0x02){    //若处于定位模式，使用导航键时
        if(GetAdcNavAct(enumAdcNavKeyRight)==enumKeyPress||GetAdcNavAct(enumAdcNavKeyLeft)==enumKeyPress)
            lon_lat = !lon_lat;
        else if(GetAdcNavAct(enumAdcNavKeyUp)==enumKeyPress){
            status += 0x10;                    //存储单元+1
            point_addr = ((status>>4)&0x0f)*6;  //一个坐标占6个字节
        }
        else if(GetAdcNavAct(enumAdcNavKey3)==enumKeyPress){    //发送坐标数据
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

    
    else if((status&0x0f)==0x04){   //若处于防盗防走失模式，使用导航键
        //开关显示
        if(status == 0x04){         //处于防盗防走失初始界面
            if(GetAdcNavAct(enumAdcNavKeyCenter) == enumKeyPress){
                if(lock_flag == 0x00){         //若未上锁，则打开界面
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
                    status = 0x44;             //关闭该界面需要密码，进入密码输入状态
            }

            if(GetAdcNavAct(enumAdcNavKey3) == enumKeyPress)
                status = 0x24;                 //按下key3，进入密码设置状态

            //如果处于距离设置模式
            if(status == 0x14){     //设置距离界面
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
                }
            }

            //如果处于密码设置模式
            else if(status == 0x24){    //设置密码界面
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
                    M24C02_Write(0x85,safety_dis);
                }
            }

            //输入密码模式
            else if(status == 0x44){    //输入密码界面
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
                        lock_flag = 0;      //解锁，警报功能关闭
                        M24C02_Write(0x80,lock_flag);
                        status = 0x04;      //返回到防盗防走失模式的初始界面
                        trycode = 0;
                    }
                }
            } 
        } 
    }
    
    //若处于测距模式，使用导航键
    else if(status == 0x08){    //测距功能界面
        if(GetAdcNavAct(enumAdcNavKeyRight)==enumKeyPress||GetAdcNavAct(enumAdcNavKeyLeft)==enumKeyPress)
                lon_lat = !lon_lat;
    }

    //经过上述操作后更新led和数码管的显示
    display();    
}
