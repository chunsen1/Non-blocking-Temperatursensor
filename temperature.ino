//device temperature sensor settings
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

//ground temperature sensor settings
#define ONE_WIRE_BUS 3 // Data wire is plugged into pin 2 on the Arduino


void getTemperature(struct data *temperature) {
  // Setup a oneWire instance to communicate with any OneWire devices
  OneWire oneWire(ONE_WIRE_BUS);
  // Pass our oneWire reference to Dallas Temperature.
  DallasTemperature sensors(&oneWire);

  sensors.requestTemperatures();
  temperature->tempGround = sensors.getTempCByIndex(0);

  // Initialize DHT sensor for normal 16mhz Arduino
  DHT dht(DHTPIN, DHTTYPE);
  temperature->tempDevice = dht.readTemperature();
}
