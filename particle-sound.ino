// This #include statement was automatically added by the Particle IDE.
#include <Ubidots.h>

//All external libraries are added here
#include "Particle.h"
#include "Arduino.h"

#define SoundSensorPin A1  //this pin read the analog voltage from the sound level meter
#define VREF  5.0  //voltage on AREF pin, default:operating voltage

//Global variable for the sound sensor data
float dbValue;

float startTime;
bool requestSent = false;
void hookResponseHandler(const char *event, const char *data);


void setup() {
    
	Serial1.begin(9600); //default buad rate for NEO-6M gps is 9600
	//receive response after publishing data to particle cloud
	Particle.subscribe("hook-response/Ubidots", hookResponseHandler, MY_DEVICES);
	//Particle.subscribe("hook-response/GoogleCloud", hookResponseHandler, MY_DEVICES);
	startTime = millis();  //time count start
}

void loop() {
    if (millis() > startTime + 15000) {  //publish data every 15 second
       startTime = millis();
	   //call function to read sound level and to publish the data 
       read_sound_level();
       publish_data();
       delay(1000);
    }
}

void hookResponseHandler(const char *event, const char *data) {
	float elevation = atof(data);
}

// This function read the data from sound sensor and convert the value into decibel. 
void read_sound_level() {
    float voltageValue;
    voltageValue = analogRead(SoundSensorPin)/1024.0 * VREF;
    dbValue = voltageValue * 5.0;  //convert voltage to decibel value

}

void publish_data() {
    char data[256];
    snprintf(data, sizeof(data), "{\"noise\":%.2f}", dbValue);
    
    Particle.publish("Ubidots", data, PRIVATE);  //trigger webhooks for Ubidots for data visualisation
    Particle.publish("GoogleCloud", data, PRIVATE); //trigger webhook to send data to google cloud platform and then to a database
    Particle.publish("Google_Sheet", data, PRIVATE); //trigger webhook to Zapier for uploading to Google Sheet
	Particle.publish("Firebase", data, PRIVATE);  //Store data to firebase
	Particle.publish("Firebase_put", data, PRIVATE); //Update data to firebase.
	//The main difference here is that the requestType is PUT, not POST. The PUT method doesn't create a table of values; 
	//it just creates a single element to hold the data, overwriting any previous data at that location.
}