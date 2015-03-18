//
//  LOCATION_TRANS.h
//  Medicine
//
//  Created by stonefeng on 15/1/29.
//  Copyright (c) 2015年 tencent. All rights reserved.
//

#ifndef Medicine_LOCATION_TRANS_h
#define Medicine_LOCATION_TRANS_h
#import <CoreLocation/CoreLocation.h>

const double pi = M_PI;
const double a = 6378245.0;
const double ee = 0.00669342162296594323;
const double x_pi = M_PI * 3000.0 / 180.0;

bool outOfChina(double lat, double lon)
{
    if (lon < 72.004 || lon > 137.8347)
        return true;
    if (lat < 0.8293 || lat > 55.8271)
        return true;
    return false;
}

double transformLat(double x, double y)
{
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(abs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;
    return ret;
}

double transformLon(double x, double y)
{
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(abs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;
    ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;
    return ret;
}

/**
 * 地球坐标转换为火星坐标
 * World Geodetic System ==> Mars Geodetic System
 *
 * @param wgLat  地球坐标
 * @param wgLon
 *
 * mglat,mglon 火星坐标
 */
static inline CLLocationCoordinate2D transform2Mars(CLLocationCoordinate2D coordinate)
{
    double wgLat = coordinate.latitude;
    double wgLon = coordinate.longitude;
    CLLocationCoordinate2D newCoordinate;
    if (outOfChina(wgLat, wgLon))
    {
        newCoordinate.latitude = wgLat;
        newCoordinate.longitude = wgLon;
        return newCoordinate;
    }
    
    double dLat = transformLat(wgLon - 105.0, wgLat - 35.0);
    double dLon = transformLon(wgLon - 105.0, wgLat - 35.0);
    double radLat = wgLat / 180.0 * pi;
    double magic = sin(radLat);
    magic = 1 - ee * magic * magic;
    double sqrtMagic = sqrt(magic);
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
    newCoordinate.latitude = wgLat + dLat;
    newCoordinate.longitude = wgLon + dLon;
    return newCoordinate;
}

/**
 * 火星坐标转换为百度坐标
 * @param gg_lat
 * @param gg_lon
 */
static inline CLLocationCoordinate2D bd_encrypt(CLLocationCoordinate2D coordinate)
{
    CLLocationCoordinate2D newCoordinate;
    double x = coordinate.longitude, y = coordinate.latitude;
    double z = sqrt(x * x + y * y) + 0.00002 * sin(y * x_pi);
    double theta = atan2(y, x) + 0.000003 * cos(x * x_pi);
    newCoordinate.longitude = z * cos(theta) + 0.0065;
    newCoordinate.latitude = z * sin(theta) + 0.006;
    return newCoordinate;
}

/**
 * 百度转火星
 * @param bd_lat
 * @param bd_lon
 */
static inline CLLocationCoordinate2D bd_decrypt(CLLocationCoordinate2D coordinate)
{
    CLLocationCoordinate2D newCoordinate;
    double bd_lon = coordinate.longitude;
    double bd_lat = coordinate.latitude;
    double x = bd_lon - 0.0065, y = bd_lat - 0.006;
    double z = sqrt(x * x + y * y) - 0.00002 * sin(y * x_pi);
    double theta = atan2(y, x) - 0.000003 * cos(x * x_pi);
    newCoordinate.longitude = z * cos(theta);
    newCoordinate.latitude = z * sin(theta);
    return newCoordinate;
}

#endif
