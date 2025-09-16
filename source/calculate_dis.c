#include "fun.h"
#include "data.h"
#include "cmath.h"

double radian(float angle){     //角度转弧度
    return angle*PI/180;    
}
int cal_dis(){          //计算距离，单位为10m
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