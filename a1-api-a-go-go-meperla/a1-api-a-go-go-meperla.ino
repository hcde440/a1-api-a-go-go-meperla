#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> //provides the ability to parse and construct JSON objects

//const char* ssid = "University of Washington";
//const char* pass = "";
const char* ssid = "Half-G Guest";
const char* pass = "BeOurGuest";
//const char* key = "b13f6e301336635b9dd81ad6db67f2d4"; //Fixer API Key
//const char* username = "meperla"; //GeoNames API Key

typedef struct { //here we create a new data type definition, a box to hold other data types
  String na;    //
  String pp;    //for each name:value pair coming in from the service, we will create a slot
  String ln;
  String an;    //in our structure to hold our data
  String la;
} GeoNames;     //then we give our new data structure a name so we can use it in our code

typedef struct { //we created a new data type definition to hold the new data
  String bs;     //each name value pair is coming from the weather service, these create slots to hold the incoming data
  String da;
  String au;
  String ca;
  String ch;
  String cn;
  String gb;
  String jp;
  String us;
} Fixer;


GeoNames places; //we have created a GeoNames type, but not an instance of that type,
                  //so we create the variable 'location' of type GeoData
Fixer rates;//created a MetData and variable conditions"

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));
  
  Serial.print("Connecting to "); Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(); Serial.println("WiFi connected"); Serial.println();
  Serial.print("Your ESP has been assigned the internal IP address ");
  Serial.println(WiFi.localIP());
  Serial.println();

  getGeoNames();
  Serial.println();
  Serial.println(places.na + ", " + places.an + " longitude and latitude is: ");
  Serial.println(places.ln + ", " + places.la);
  Serial.println("and has a population of: " + places.pp);
  Serial.println();

  getFixer();
  Serial.println();
  Serial.println("The exchange rates for seven different countries is listed below all relative to the currency " + rates.bs + ".");
  Serial.println("AUD: " + rates.au + ", ");
  Serial.println("CAD: " + rates.ca + ", ");
  Serial.println("CHF: " + rates.ch + ", ");
  Serial.println("CNY: " + rates.cn + ", ");
  Serial.println("GBP: " + rates.gb + ", ");
  Serial.println("JPY: " + rates.jp + ", ");
  Serial.println("USD: " + rates.au );
}

void loop() {
  //if we put getIP() here, it would ping the endpoint over and over and we dont want that
}

String getIP() {
  HTTPClient theClient;
  String ipAddress;

  theClient.begin("http://api.ipify.org/?format=json");
  int httpCode = theClient.GET();

  if (httpCode > 0) {
    if (httpCode == 200) {

      DynamicJsonBuffer jsonBuffer;

      String payload = theClient.getString();
      JsonObject& root = jsonBuffer.parse(payload);
      ipAddress = root["ip"].as<String>();

    } else {
      Serial.println("Something went wrong with connecting to the endpoint.");
      return "error";
    }
  }
  return ipAddress;
}

void getGeoNames() {
  HTTPClient theClient;
  Serial.println("Making HTTP request");
  theClient.begin("http://api.geonames.org/searchJSON?q=seattle&maxRows=1&username=meperla"); //return IP as .json object
  int httpCode = theClient.GET();

  if (httpCode > 0) {
    if (httpCode == 200) {
      Serial.println("Received HTTP payload.");
      DynamicJsonBuffer jsonBuffer;
      String payload = theClient.getString();
      Serial.println("Parsing...");
      JsonObject& root = jsonBuffer.parse(payload);

      // Test if parsing succeeds.
      if (!root.success()) {
        Serial.println("parseObject() failed");
        Serial.println(payload);
        return;
      }

      //Some debugging lines below:
      //      Serial.println(payload);
      //      root.printTo(Serial);

      //Using .dot syntax, we refer to the variable "location" which is of
      //type DarkSky, and place our data into the data structure.

      places.na = root["geonames"][0]["name"].as<String>();            //we cast the values as Strings b/c the 'slots' in GeoNames are Strings
      places.pp = root["geonames"][0]["population"].as<String>();
      places.ln = root["geonames"][0]["lng"].as<String>();
      places.an = root["geonames"][1]["adminName1"].as<String>(); 
      places.la = root["geonames"][1]["lat"].as<String>(); 

    } else {
      Serial.println("Something went wrong with connecting to the endpoint in getGeoNames.");
    }
  }
}
void getFixer() {
  HTTPClient theClient;
  Serial.println("Making HTTP request");
  theClient.begin("http://data.fixer.io/api/latest?access_key=b13f6e301336635b9dd81ad6db67f2d4"); //returns IP as .json object
  int httpCode = theClient.GET();
  
  if (httpCode > 0) {
    if (httpCode == 200) {
      Serial.println("Received HTTP payload.");
      DynamicJsonBuffer jsonBuffer;
      String payload = theClient.getString();
      Serial.println("Parsing...");
      JsonObject& root = jsonBuffer.parse(payload);

      // Test if parsing succeeds.
      if (!root.success()) {
        Serial.println("parseObject() failed");
        Serial.println(payload);
        return;
      }

      rates.bs = root["base"].as<String>();    //casting these values as Strings because the metData "slots" are Strings
      rates.da = root["date"].as<String>();
      rates.au = root["rates"]["AUD"].as<String>();
      rates.ca = root["rates"]["CAD"].as<String>();
      rates.ch = root["rates"]["CHF"].as<String>();
      rates.cn = root["rates"]["CNY"].as<String>();
      rates.gb = root["rates"]["GBP"].as<String>();
      rates.jp = root["rates"]["JPY"].as<String>();
      rates.us = root["rates"]["USD"].as<String>();
    }
  }
  else {
    Serial.printf("Something went wrong with connecting to the endpoint in getFixer().");
  }
}  

