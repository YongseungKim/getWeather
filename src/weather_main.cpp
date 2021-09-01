// #include <Arduino.h>
// #include <ArduinoJson.h>
// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <HTTPClient.h> // HTTPClient 라이브러리
// #include <WiFiAP.h>

// #include "images.h"
// #include "weather.h"

// /* Definitions */
// #define WIFI_SSID "VCOMM_LAB"
// #define WIFI_PWD "vcomm13579"
// #define REGION "seoul"
// #define COUNTRY "kr"
// #define APPID "498fa8d4933dad1819c0ed24486f3afc"

// #define TIME_ZONE 9
// #define DAYTIME_SAVING 0
// #define REGION_NAME_LEN 30

// _weatherinfo weatherinfo;
// char strTime[6]; /* e.g. 12:12\0 */

// const char *ssid = WIFI_SSID;	 // 연결할 SSID
// const char *password = WIFI_PWD; // 연결할 SSID의 비밀번호
// bool tickerFlag = false;
// bool initWiFi();
// void initTime();
// void requestWeatherInfo();
// void parseWeatherJson(String buffer);
// const uint8_t *getWeatherIcon(int conditionId);

// /* Set flag for weather information request */
// void tickerCallback()
// {
// 	tickerFlag = true;
// }



// unsigned long t_loop;

// void setup()
// {
// 	// put your setup code here, to run once:
// 	Serial.begin(9600);
// 	initWiFi();
// 	initTime();
// 	/* Reuqest info */
// 	requestWeatherInfo();
// 	t_loop = millis();
// }


// void loop()
// {
// 	// put your main code here, to run repeatedly:
// 	if( millis() > (t_loop + 2000)){
// 		requestWeatherInfo();
// 		t_loop = millis();
// 	}
// }

// bool initWiFi()
// {
// 	Serial.println();

// 	if (!WiFi.begin(ssid, password))
// 	{
// 		Serial.println("ERROR: WiFi.begin");
// 		return false;
// 	}

// 	Serial.println("OK: WiFi.begin");

// 	while (WiFi.status() != WL_CONNECTED)
// 	{
// 		delay(100);
// 		Serial.print(".");
// 	}

// 	Serial.println();
// 	Serial.println("OK: WiFi connected");

// 	delay(1000);

// 	return true;
// }

// void initTime()
// {
// 	/* Congiure Time */
// 	Serial.println("Initialize Time...");

// 	configTime(TIME_ZONE * 3600, DAYTIME_SAVING, "pool.ntp.org", "time.nist.gov");

// 	while (!time(nullptr))
// 	{
// 		Serial.print(".");
// 		delay(1000);
// 	}

// 	time_t now = time(nullptr);
// 	tm *local = localtime(&now);
// 	sprintf(strTime, "%02d:%02d", local->tm_hour, local->tm_min);

// 	Serial.println("OK: initTime");
// }

// void requestWeatherInfo()
// {
// 	HTTPClient httpClient;
// 	httpClient.setTimeout(2000);

// 	/* Connect & Request */
// 	String url = String("/data/2.5/weather?q=") + String(REGION) + String(",") + String(COUNTRY) + String("&units=metric&appid=") + String(APPID);
// 	Serial.print("Weater URL : http://api.openweathermap.org");
// 	Serial.println(url);

// 	if (!httpClient.begin("api.openweathermap.org", 80, url.c_str()))
// 	{
// 		Serial.println("ERROR: HTTPClient.begin");
// 		return;
// 	}

// 	Serial.println("OK: HTTPClient.begin");

// 	int httpCode = httpClient.GET();

// 	/* Check response */
// 	if (httpCode > 0)
// 	{
// 		Serial.printf("[HTTP] request from the client was handled: %d\n", httpCode);
// 		String payload = httpClient.getString();
// 		parseWeatherJson(payload);
// 	}
// 	else
// 	{
// 		Serial.printf("[HTTP] connection failed: %s\n", httpClient.errorToString(httpCode).c_str());
// 	}

// 	httpClient.end();
// }

// void parseWeatherJson(String buffer)
// {
// 	int JsonStartIndex = buffer.indexOf('{');
// 	int JsonLastIndex = buffer.lastIndexOf('}');

// 	/* Substring JSON string */
// 	String JsonStr = buffer.substring(JsonStartIndex, JsonLastIndex + 1);
// 	Serial.println("PARSE JSON WEATHER INFORMATION: " + JsonStr);

// 	/* Parse JSON string */
// 	// DynamicJsonBuffer jsonBuffer;
// 	// JsonObject &root = jsonBuffer.parseObject(JsonStr);
// 	DynamicJsonDocument jsonDoc(4096);
// 	JsonObject root = jsonDoc.to<JsonObject>();
// 	// prepare response
// 	root.clear();
// 	root["result"] = "success";
// 	serializeJson(root, JsonStr);

// //http://api.openweathermap.org//data/2.5/weather?q=seoul,kr&units=metric&appid=498fa8d4933dad1819c0ed24486f3afc
// //    {"coord":{"lon":126.9778,"lat":37.5683},
// // 	"weather":[
// // 		{"id":501,"main":"Rain","description":"moderate rain","icon":"10d"},
// // 		{"id":701,"main":"Mist","description":"mist","icon":"50d"}
// // 	],
// // 	"base":"stations",
// // 	"main":{
// // 		"temp":18.39,
// // 		"feels_like":18.74,
// // 		"temp_min":17.78,
// // 		"temp_max":19.73,
// // 		"pressure":1015,
// // 		"humidity":94
// // 		},
// // 	"visibility":4000,
// // 	"wind":{
// // 		"speed":6.17,
// // 		"deg":100
// // 		},
// // 	"rain":{
// // 		"1h":3.16
// // 		},
// // 	"clouds":{
// // 		"all":90
// // 	},
// // 	"dt":1630399770,
// // 	"sys":{
// // 		"type":1,
// // 		"id":8105,
// // 		"country":"KR",
// // 		"sunrise":1630357285,
// // 		"sunset":1630404241
// // 		},
// // 	"timezone":32400,
// // 	"id":1835848,
// // 	"name":"Seoul",
// // 	"cod":200
// // 	}
// 	// if (root["result"] != "success")
// 	if(1)
// 	{
// 		/* Get information */
// 		weatherinfo.temp = root["main"]["temp"];
		
// 		weatherinfo.humidity = root["main"]["humidity"];
// 		weatherinfo.temp_min = root["main"]["temp_min"];
// 		weatherinfo.temp_max = root["main"]["temp_max"];
// 		weatherinfo.speed = root["wind"]["speed"];
// 		weatherinfo.direction = root["wind"]["direction"];
// 		weatherinfo.conditionId = root["weather"][0]["id"];
// 		const char *name = root["name"];
// 		int namelen = strlen(name);
// 		// strncpy(weatherinfo.name, root["name"], namelen > REGION_NAME_LEN ? REGION_NAME_LEN : namelen);

// 		/* Serial Output */
// 		Serial.printf("Name: %s\r\n", weatherinfo.name);
// 		Serial.printf("Temp: %3.1f\r\n", weatherinfo.temp);
// 		Serial.printf("Humidity: %d\r\n", weatherinfo.humidity);
// 		Serial.printf("Min. Temp: %d\r\n", weatherinfo.temp_min);
// 		Serial.printf("Max. Temp: %d\r\n", weatherinfo.temp_max);
// 		Serial.printf("Wind Speed: %d\r\n", weatherinfo.speed);
// 		Serial.printf("Wind Direction: %d\r\n", weatherinfo.direction);
// 		Serial.printf("ConditionId: %d\r\n", weatherinfo.conditionId);
// 	}
// 	else
// 	{
// 		Serial.println("jsonBuffer.parseObject failed");
// 	}
// }

// const uint8_t *getWeatherIcon(int conditionId)
// {
// 	/* Return string for conditionId */
// 	if (conditionId >= 200 && conditionId < 300)
// 		return STORM;
// 	else if (conditionId >= 300 && conditionId < 400)
// 		return RAIN;
// 	else if (conditionId >= 500 && conditionId < 600)
// 		return RAIN;
// 	else if (conditionId >= 600 && conditionId < 700)
// 		return SNOW;
// 	else if (conditionId >= 700 && conditionId < 800)
// 		return FOG;
// 	else if (conditionId == 800)
// 		return SUNNY;
// 	else if (conditionId > 800 && conditionId < 900)
// 		return CLOUD;
// 	else
// 		return SUNNY;
// }