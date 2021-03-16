#include "includes.h"

void loop() ///nfc LOOP
{           //Serial.print("nfcloop running on core ");
  // Serial.println(xPortGetCoreID());

  if (!digitalRead(0) && (apMode == 0) && (abs(millis() - apDelay) >= 500))
  {

    apDelayCount++;
    apDelay = millis();
    if (apDelayCount > 5)
    {
      while (!digitalRead(0))
      {
        /* code */
      }

      count = 0;
      if (apMode == 0)
      {
        apMode = 1;
      }

      apActivate = 0;

      Serial.print("ap forzado");
      EEPROM.write(2, 25); //(pos,data) saber si entra en AP forzado
      EEPROM.commit();
      ESP.restart();
    }
  }
  wifiLedBlink();
  if (guardarAp == 1)
  {
    save_config1_spiff();
    ESP.restart();
  }

  if ((abs(millis() - nfcDelay) >= 150) && (bussyMqtt == 0) && (apMode == 0) && (sendTag == 0))
  {
    claimSPI("NFC"); // Claim SPI bus
    tagId = nfc_Loop();
    releaseSPI(); // Release SPI bus
    if (tagIdPrev == tagId || (tagId == 0) || (tagId == 1))
    {
      tagIdPrev = tagId;
      tagId = 0;
    }
    else
    {

      tagIdPrev = tagId;
      sendTag = 1;
    }

    nfcDelay = millis();
    DEBUG_PRINT("begin:");
    DEBUG_PRINTLN(inicio);
  }
}
void WebComm(void *parameter) ///webloop
{

  for (;;)
  {
    if ((inicio == 0) && (apMode == 0))
    {
      //goAP = 0;
      claimSPI("WebComm"); // Claim SPI bus
      wifi_mqtt_setup();
      releaseSPI(); // Release SPI bus
      goAP = 1;
    }
    if ((inicio == 1) && (apMode != 1))
    { //DEBUG_PRINT("inicio1:");
      //DEBUG_PRINTLN(inicio);
      //goAP = 0;
      claimSPI("WebComm");                        // Claim SPI bus
      wifi_mqtt_reconnect(MQTTTopic, MQTTTopic2); //mqtt protocol
      releaseSPI();
      //goAP = 1; // Release SPI bus
    }
    //Serial.print("WebComm() running on core ");
    // Serial.println(xPortGetCoreID());
    //MQTT
    if ((inicio == 2) && (apMode != 1))
    {
      // DEBUG_PRINT("inicio2:");
      // DEBUG_PRINTLN(inicio);
      // DEBUG_PRINT("client state:");
      // DEBUG_PRINTLN(mqttclient.state());
      if (mqttclient.state() != 0 || subscribed == 0)
      {
        if (mqttclient.state() != -1) ///-1 disconnected
        {
          mqttclient.disconnect();
          subscribed = 0;
        }
        if (mqttclient.state() == -1)
        {
          //goAP = 0;
          DEBUG_PRINT("esta pasando esto");
          claimSPI("WebComm");                        // Claim SPI bus
          wifi_mqtt_reconnect(MQTTTopic, MQTTTopic2); //mqtt protocol
          releaseSPI();                               // Release SPI bus
          //goAP = 1;
        }
      }
      if (subscribed == 0 && (mqttclient.state() == 0))
      {
        //goAP = 0;
        DEBUG_PRINT("esta pasando esto?");
        String topic_s = "";
        topic_s = "SERVER/POLL";
        DEBUG_PRINTLN(topic_s);
        claimSPI("WebComm");
        wifi_mqtt_subscribe(topic_s.c_str());
        releaseSPI(); // Release SPI bus
        topic_s = "";
        topic_s = "SERVER/" + chipid;
        DEBUG_PRINTLN(topic_s);
        claimSPI("WebComm");
        wifi_mqtt_subscribe(topic_s.c_str());
        releaseSPI(); // Release SPI bus
        //goAP = 1;
      }
    }

    if (mqttclient.state() == 0 && (apMode != 1))
    {
      //goAP = 0;
      claimSPI("WebComm"); // Claim SPI bus
      wifi_mqtt_loop();
      releaseSPI(); // Release SPI bus
      //goAP = 1;
    }
  }
  vTaskDelay(10000);
}
void APmode(void *parameter) ///APmode
{

  for (;;)
  {

    if ((apMode) && (!apActivate) && goAP == 1)
    {
      Serial.println("Force ap Mode");
      apActivate = 1;
      setupAPSSID(0);
      web_setup();
    }
    if (cambioIp)
    {
      apActivate = 1;
      cambioIp = 0;
      setupAPSSID(1);
      web_setup();
      apMode = 2;
      goAP = 1;
    }

    if (apMode != 0 && apActivate == 1 && goAP == 1) /// apMode 1 forzado  2 ya conectado a una red wifi
    {
      // Serial.print("APmode() running on core ");
      // Serial.println(xPortGetCoreID());
      claimSPI("apmode"); // Claim SPI bus
      apweb_loop();
      releaseSPI(); // Release SPI bus
    }
  }
  vTaskDelay(3000);
}

//**************************************************************************************************
//                                      C L A I M S P I                                            *
//**************************************************************************************************
// Claim the SPI bus.  Uses FreeRTOS semaphores.                                                   *
// If the semaphore cannot be claimed within the time-out period, the function continues without   *
// claiming the semaphore.  This is incorrect but allows debugging.                                *
//**************************************************************************************************
void claimSPI(const char *p)
{
  const TickType_t ctry = 100;        // Time to wait for semaphore
  uint32_t count = 0;                 // Wait time in ticks
  static const char *old_id = "none"; // ID that holds the bus

  while (xSemaphoreTake(SPIsem, ctry) != pdTRUE) // Claim SPI bus
  {
    if (count++ > 250)
    {
    }
    if (count >= 1000)
    {
      DEBUG_PRINTLN("");
      DEBUG_PRINT("SPI semaphore not taken within");
      DEBUG_PRINT(count * ctry);
      DEBUG_PRINT(" ticks by CPU ");

      DEBUG_PRINT(xPortGetCoreID());
      DEBUG_PRINT(", Id ");
      DEBUG_PRINTLN(p);
      DEBUG_PRINT("Semaphore is claimed by");
      DEBUG_PRINTLN(old_id);
      return; // Continue without semaphore
    }
  }
  old_id = p; // Remember ID holding the semaphore
}

//**************************************************************************************************
//                                   R E L E A S E S P I                                           *
//**************************************************************************************************
// Free the the SPI bus.  Uses FreeRTOS semaphores.                                                *
//**************************************************************************************************
void releaseSPI()
{
  xSemaphoreGive(SPIsem); // Release SPI bus
}
