# MySensors-Arduino-Sketches
My combinations of various sketches for Arduino / Openhab2 / MySenors


#1. Multisensor1---this sketch combines the standard RTC Display Sensor which requests the time from the controller and saves it to an RTC module and displays it on the LCD. Also added suppport for DHT11/22 sensor( also displayed on the LCD), and support for LDR(shown in server), and multi-relay-with-button-acuator which allows you to control multiple relays from the server and from physical buttons aswell. You can define the number of buttons and their corrosponding pins aswell as the number of relays and their pins(see code)

#2. MQTT_DHT22_OLED-An ESP8266-12e with DHT22 sensor and 128x32 OLED i2c display. The ESP connects to my Openhab2 server running on my Pi3, which also hosts my mosquitto server and client. This in turn connects to my Openhab server. it sendsor data to the server every minute and also listens for callbacks from the server. One pin is connected to a relay which is controlled from openhab via mqtt.
