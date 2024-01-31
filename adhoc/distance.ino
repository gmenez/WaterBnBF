#include <math.h>
#include "distance.h"

#define PI 3.14159265358979323846

extern double longitude, latitude;

/**
 * Convert degrees to radians
 */
double toRadians(double degree) {
    return degree * (PI / 180);
}

/**
 * Get the distance between the ESP32 and the location in kilometers
 */
double getDistance(double lng, double lat) {
  double lon1 = toRadians(longitude);
  double lat1 = toRadians(latitude);
  double lon2 = toRadians(lng);
  double lat2 = toRadians(lat);

  double dlon = lon2 - lon1;
  double dlat = lat2 - lat1;
  double a = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
  double c = 2 * asin(sqrt(a));

  double r = 6371;

  return(c * r);
}
