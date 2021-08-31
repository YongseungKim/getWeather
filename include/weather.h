#include <Arduino.h>

/* Definitions */
#define WIFI_SSID ""
#define WIFI_PWD ""
#define REGION "tenan"
#define COUNTRY "kr"
#define APPID ""

#define TIME_ZONE 9
#define DAYTIME_SAVING 0
#define REGION_NAME_LEN 30

/* Weather information structure */
typedef struct
{
	char name[REGION_NAME_LEN];
	double temp;
	int humidity;
	int temp_min;
	int temp_max;
	int speed;
	int direction;
	int conditionId;
} _weatherinfo;

