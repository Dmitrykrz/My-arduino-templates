

#include <math.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Korzhenevskiy_tick_tack_2.h"


const char* mqtt_server = "...";
const char* mqtt_user = "...";
const char* mqtt_password = "...";
const int mqttport = 1883;

WiFiClient espClient;
byte mac[6];
IPAddress ip;
PubSubClient client(espClient);


const char* MQTTclientname = "My_name_in_MQTT";
const char* MQTTChannelToPublish = "channel";
const char* MQTTChannelToSubsribe = "channel";
const char* willMessage = "esp is dead";

# define Sline Serial.println("-------------------------------------------------------------------")
#define serialdebug


#ifdef serialdebug
#define Snl Serial.println
#define S Serial.print
#endif

#ifndef serialdebug
#define Snl
#define S
#endif



//-------------------Wi-Fi-------------------//

struct wificredentials {
	const char* ssid;
	const char* pass;
	int power = -500;
};
wificredentials networks[3];

//-------------------Wi-Fi------END--------//




















//____________________________________________________________________
/////////////////////////// SETUP ////////////////////////////////////
void setup(void)
{


	networks[0].ssid = "ssid1";
	networks[0].pass = NULL;
	networks[1].ssid = "ssid2";
	networks[1].pass = NULL;
	networks[2].ssid = "ssid3";
	networks[2].pass = "pass";


#ifdef serialdebug
	Serial.begin(115200);
	delay(100);
	Snl("START. Serial debug ON");
#endif

	client.setServer(mqtt_server, mqttport);
	client.setCallback(callback);



	Snl("Setup end"); Sline; Snl();
}
/////////////////////////// SETUP END /////////////////////////////////
//____________________________________________________________________







//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOOP START                LOOP START                LOOP START                LOOP START                LOOP START                LOOP START                LOOP START
// LOOP START                LOOP START                LOOP START                LOOP START                LOOP START                LOOP START                LOOP START
// LOOP START                LOOP START                LOOP START                LOOP START                LOOP START                LOOP START                LOOP START
// LOOP START                LOOP START                LOOP START                LOOP START                LOOP START                LOOP START                LOOP START
// LOOP START                LOOP START                LOOP START                LOOP START                LOOP START                LOOP START                LOOP START
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
	if (WiFi.status() != WL_CONNECTED) connecttobestwifi();
	else  if (!client.connected()) connecttoMQTT();
	client.loop();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END
// LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END
// LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END
// LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END                  LOOP END
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void connecttobestwifi()
{
	Snl("connect to best wifi");
	Snl("");
	Snl("List of networks:");
	bool founnetworkfromlist = 0;

	int numSsid = WiFi.scanNetworks();

	for (int thisNet = 0; thisNet < numSsid; thisNet++)
	{
		S(WiFi.SSID(thisNet));
		for (int i3 = 0; i3 < 3; i3++)
		{


			if (WiFi.SSID(thisNet) == networks[i3].ssid)
			{
				networks[i3].power = WiFi.RSSI(thisNet);
				founnetworkfromlist = 1;
				S("     I have this network on my list! ");
				//S(networks[i3].ssid);
				S(" power= ");
				S(networks[i3].power);
				S(" dbm");

			}
		}
		Snl();

	}


	if (founnetworkfromlist == 1)
	{
		int bestpower = _max(_max(networks[0].power, networks[1].power), networks[2].power);
		int bestwifinumber;
		if (networks[0].power == bestpower) bestwifinumber = 0;
		if (networks[1].power == bestpower) bestwifinumber = 1;
		if (networks[2].power == bestpower) bestwifinumber = 2;
		Snl();
		S("The best wifi is "); Snl(networks[bestwifinumber].ssid);
		WiFi.persistent(false);
		WiFi.mode(WIFI_OFF);
		WiFi.mode(WIFI_STA);
		S("Connecting");
		WiFi.begin(networks[bestwifinumber].ssid, networks[bestwifinumber].pass);

		int waittoconnect = 0;
		while (WiFi.status() != WL_CONNECTED) {
			delay(250);
			S(". ");
			waittoconnect++;
			if (waittoconnect == 30) break;
		}
		Snl();

		if (WiFi.status() == WL_CONNECTED)
		{
			Snl("Connected to wifi     :)");
			WiFi.macAddress(mac);
			Snl();
			S("MAC: ");
			S(mac[5], HEX);
			S(":");
			S(mac[4], HEX);
			S(":");
			S(mac[3], HEX);
			S(":");
			S(mac[2], HEX);
			S(":");
			S(mac[1], HEX);
			S(":");
			Snl(mac[0], HEX);
			S("IP:  ");
			ip = WiFi.localIP();
			Snl(ip);
			Snl();
		}
		else Snl("connect to wifi FAIL");


	}
	else {
		Snl();
		Snl("No listed networks found       :(");
		delay(2000);
	}
	Sline;
}









void connecttoMQTT()
{
	if (!client.connected())
	{ // CHANDED FOM WHILE
		Snl("Attempting MQTT connection...");

		//boolean connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage)
		if (client.connect(MQTTclientname, mqtt_user, mqtt_password, MQTTChannelToPublish, 1, false, willMessage))
		{
			Snl("iot cat Connected to MQTT");
			client.subscribe(MQTTChannelToSubsribe);
			client.publish(MQTTChannelToPublish, "iot cat Connected to MQTT");


		}
		else
		{
			Snl("MQTT connect fail. ");
			delay(1000);
		}
	}
	Sline;
}





















void callback(char* topic, byte* payload, unsigned int length) {

	String message;

	for (int i = 0; i < length; i++) {
		message = message + (char)payload[i];
	}
	S("New message from MQTT: "); Snl(message);
	const size_t bufferSize = JSON_OBJECT_SIZE(1) + 25;
	DynamicJsonBuffer jsonBuffer(bufferSize);

	JsonObject& root = jsonBuffer.parseObject(message);

	if (!root.success()) {
		Snl("JSON parse failed");
	}
	else
	{
		Snl("JSON parse success");
		if (root.containsKey("goto")) {
			int gotolevel = root["goto"];

			if (gotolevel == 1)
			{

			}

		}

		if (root.containsKey("command")) {
			String command = root["command"];
			if (command == "reset") ESP.restart();
		}






		//make_megasend();
	}

	Sline;

}









