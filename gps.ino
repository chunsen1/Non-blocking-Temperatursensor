



///////////////////////////////////////////////////////////////
bool feedGPS() {
  while (softwareSerial.available() > 0) {
    if (gps.encode(softwareSerial.read()))
      return true;
  }
  return false;
}

void getGPSPosition(struct data *position) {

  if (gps.location.isValid())
  { position->lat = (float)gps.location.lat();
    position->lng = (float)gps.location.lng();
  }
  if (gps.date.isValid())
  {
    position->year = (gps.date.year());
    position->month = (gps.date.month());
    position->day = (gps.date.day());
    sprintf(position->date, "%02d.%02d.%4d",
            position->day, position->month, position->year);
  }
  if (gps.time.isValid()) {
    position->hoD = int(gps.time.hour());
    position->minD = int(gps.time.minute());
    position->secD = int(gps.time.second());
    sprintf(position->time, "%02d:%02d:%02d",
            position->hoD, position->minD, position->secD);
  }
  position->numberSatellites = int(gps.satellites.value());
  position->hdop = int(gps.hdop.value());

  //  sprintf(position->date, "%02d.%02d.%4d",
  //          31, 12, 2015);
  //  sprintf(position->time, "%02d:%02d:%02d",
  //          20, 15, 34);
  //  position->lat = 222.34;
  //  position->lng = 111.56;
  //  position->numberSatellites = 8;

}

void printGPSToSerial()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

