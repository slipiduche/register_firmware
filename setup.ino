void setup()
{
  if (!EEPROM.begin(512))
  {
    Serial.println("failed to initialise EEPROM please reset");
    return;
  }

  int boottime = EEPROM.read(1);

  Serial.begin(115200); // initialize serial for debugging

  pinMode(wifiled, OUTPUT);
  pinMode(NFCPOWER, OUTPUT);
  wifiLedBlink();
  NFCPOWEROn();
  DEBUG_PRINTLN("beginning2");

  chipid = get_chipidstr();
  clientId += String(chipid);

  fun_spiff_setup(); //alocated in fun_spiff
  if (boottime == bootX)
  {
    read_spiffconfig1(); //alocated in fun_spiff
  }
  else
  {
    setupAPSSID(0);
    save_config1_spiff();
    EEPROM.write(1, bootX); //(pos,data)
    EEPROM.commit();
  }

  //web_setup(); //apmode
  nfcSetup(); //nfc setup
  maintask = xTaskGetCurrentTaskHandle(); // My taskhandle
  SPIsem = xSemaphoreCreateMutex();
  setup_dualcore();
  DEBUG_PRINT("begin0:");
  DEBUG_PRINTLN(inicio);
  solicitud_web = 1;
  
}

TaskHandle_t Task2, Task3;

void setup_dualcore()
{

  xTaskCreatePinnedToCore(
      WebComm,
      "Task_2",
      20000,
      NULL,
      1,
      &Task2,
      0);

  xTaskCreatePinnedToCore(
      APmode,
      "Task_3",
      20000,
      NULL,
      1,
      &Task3,
      0);
}

String get_chipidstr()
{ ////////extracts the chipid from the esp32 in a character string
  String chip;

  Serial.print("chipId: ");
  ChipId = ESP.getEfuseMac();
  ChipId16 = ((uint16_t)(ChipId >> 32));
  ChipId32 = ((uint32_t)(ChipId));

  chip = String(ChipId16, HEX) + String(ChipId32, HEX);
  while (chip.length() < 12)
  {
    chip = "0" + chip;
  }

  Serial.println(chip);
  Serial.println(chip.length());
  return chip;
}

void setupAPSSID(int state)
{
  String SSID2 = "&" + String(state) + "G" + String(chipid) + String(devName);
  String set1 = "set1," + String(ssid) + "," + String(password) + "," + SSID2 + "," + String(password2) + "," + String(MQTTHost) + "," + String(MQTTPort) + "," + String(MQTTUsername) + "," + String(MQTTPassword) + ",1,";
  loadsdconfig(set1);
}
