#define CHIP_SELECT 10 //pin for accessing the sd
#define MAX_LENGTH_FILENAME 8
#define CONFIG_FILE "conf.txt" //config file to store the current logfile name



///////////////////////////////////////////////////
//readConfig
//reads the logfilename from the config file
///////////////////////////////////////////////////
void readConfig(char* filename) {
  uint8_t pos = 0;
  char character;
  //char buffer[14] = "";
  //String settingValue;
  File configFile = SD.open(CONFIG_FILE);
  if (configFile) {
    while (configFile.available()) {
      character = configFile.read();
      //while( configFile.available() && (character != '[')){
      //  character = configFile.read();
      //}
      while (configFile.available() && (character != '=')) { //&& pos < 14) {
        character = configFile.read();
      }
      character = configFile.read(); // leave out the '='
      while (configFile.available() && (character != ']') && pos < 14) {
        filename[pos] = character;
        pos++;
        //settingValue = settingValue + character;
        character = configFile.read();
      }
      //Debugging Printing
      // write("Current config", buffer);
    }
    // close the file:
    configFile.close();
    uint8_t length;
    for (length = 14; length > 0; length--) {
      if (filename[length - 1] == ' ') filename[length - 1] = 0;
      else break;
    }
    return;
  }
  else {
    // if the file didn't open, print an error:
    write("Error read config");
    while (true);
    //Serial.println("Error opening configuration");
    return;
  }
}

///////////////////////////////////////////////////
//writeConfig
//writes the current logfilename to the config file
///////////////////////////////////////////////////
void writeConfig(char* filename, bool printExtension = true) {
  // Delete the old One
  SD.remove(CONFIG_FILE);
  // Create new one
  File logFile = SD.open(CONFIG_FILE, FILE_WRITE);
  logFile.print("[");
  logFile.print("filename=");
  logFile.print(filename);
  if (printExtension) logFile.println(".txt]");
  // close the file:
  logFile.close();
}

///////////////////////////////////////////////////
//storeMeasurement
//Messwert speichern in ausgewählte Logdatei
//speichern
///////////////////////////////////////////////////
void storeMeasurement(struct data *currentData) {
  //TODO
  char filename[14] = "";
  readConfig(filename);
  File datafile = SD.open(filename, FILE_WRITE);
  // if the file is available, write to it:
  if (datafile) {
    datafile.print(currentData->lat, 8);
    datafile.print(",");
    datafile.print(currentData->lng, 8);
    datafile.print(",");
    datafile.print(currentData->date);
    datafile.print(",");
    datafile.print(currentData->time);
    datafile.print(",");
    datafile.print(currentData->tempGround, 8);
    datafile.print(",");
    datafile.print(currentData->tempDevice, 8);
    datafile.println("");

    // dataFile.println(position);
    datafile.close();
    //Serial.println("Measurement done");
  }
  // if the file isn't open, pop up an error:
  else {
    write("Error writing", "measurement to file");
    //waitButton();
    //Serial.println("Error open file");
    //delay(3000);
  }
}

///////////////////////////////////////////////////
//initializeSDCard
//opens communication to sd card
///////////////////////////////////////////////////
void initializeSDCard() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  //Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH); // Add this line

  // see if the card is present and can be initialized:
  if (!SD.begin(CHIP_SELECT)) {
    //if (!SD.begin(10, 11, 12, 13)) {
    write("Card failed", "or not present");
    while (true);
    //Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  write("SD initialized");
  delay(1000);
  //Serial.println("Card initialized.");
  lcd.clear();
}
