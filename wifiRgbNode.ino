#include <Arduino.h>
#include <WiFiManager.h>
#include <Ticker.h>

extern HardwareSerial Serial;

Ticker ticker;
WiFiEventHandler disconnectedEventHandler, gotIpEventHandler;

void tick()
{
    //toggle state
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());
    //entered config mode, make led toggle faster
    ticker.attach(0.2, tick);
}

void cbWifiDisconnected(const WiFiEventStationModeDisconnected &event)
{
    Serial.println("Lost Wi-Fi connection");
    ticker.attach(0.6, tick);
}

void cbGotIp(const WiFiEventStationModeGotIP &event)
{
    Serial.println("Got IP");
    ticker.detach();
    digitalWrite(LED_BUILTIN, HIGH);
}

void setup()
{
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // put your setup code here, to run once:
    Serial.begin(115200);

    //set led pin as output
    pinMode(LED_BUILTIN, OUTPUT);
    // start ticker with 0.5 because we start in AP mode and try to connect
    ticker.attach(0.6, tick);

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;
    wm.setClass("invert");
    //reset settings - for testing
    // wm.resetSettings();

    //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wm.setAPCallback(configModeCallback);

    //fetches ssid and pass and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    if (!wm.autoConnect())
    {
        Serial.println("failed to connect and hit timeout");
        //reset and try again, or maybe put it to deep sleep
        ESP.restart();
        delay(1000);
    }

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    ticker.detach();
    //keep LED on
    digitalWrite(LED_BUILTIN, HIGH);
    disconnectedEventHandler = WiFi.onStationModeDisconnected(cbWifiDisconnected);
    gotIpEventHandler = WiFi.onStationModeGotIP(cbGotIp);
}

void loop()
{
}
