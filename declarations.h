/***DEBUG_PORT***/
#define DEBUG
#ifdef DEBUG
#define DEBUG_PORT Serial 
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_WRITE(x) Serial.write(x)
#define DEBUG_PRINTx(x, y) Serial.print(x, y)
#define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTLNx(x, y) Serial.println(x, y)
#else
#define DEBUG_PORT
#define DEBUG_WRITE(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTx(x, y)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTLNx(x, y)
#endif
////////////////
#include "EEPROM.h" 
#include <RTClib.h>
RTC_Millis rtcSoft;
#include <SPI.h>
#include "FS.h"
#include "SPIFFS.h"
// #define ssl
// #ifdef ssl
// #include <WiFiClientSecure.h>
//#include <credentials.h>
//WiFiClientSecure client1;
//WiFiClientSecure client_1;
//#else
#include <WiFi.h>
WiFiClient client1;
//WiFiClient client_1;
//#endif
#include <ESP32WebServer.h>
#define AP
ESP32WebServer server(80); //web server
/**SPIFF**/
#define fun_spiff
File logFile; 
/**** REGISTERS ****/
#define devBoard //comment if is final board
#ifdef  devBoard
#define wifiled 5                    //32 final board
#define NFCPOWER 27 // 5 final board
#else
#define wifiled 32                    //32 final board
#define NFCPOWER 5 // 5 final board
#endif
#define RetardoAntirebotePulsador 25 
#define salidabajoactiva
#ifdef salidabajoactiva 
#define OFF_bit 1
#define ON_bit 0
#define OFF HIGH
#define ON LOW
#endif
#ifdef salidaaltoactiva 
#define OFF_bit 0
#define ON_bit 1
#define OFF LOW
#define ON HIGH
#endif
/****VARIABLES****/
/****fun_web Variables****/
bool noAP = 1;
int tnoAP = 0;
String webpage = "";     
bool SD_present = false; 
/*************/
unsigned long actualizar_hora = 0;
unsigned long t_ultima_accion = 0;
unsigned long cuentaSegundos = 0;
uint8_t count = 0;
uint8_t countRtc = 0;
bool connected = 0;
//----------------------------------------------//
//           TIME Variables                    //
//----------------------------------------------//
char Dia_semana[][10] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"}; /*Su=1 Mo=2 Tu=3 We=4 Th=5 Fr=6 Sa=7 */
int dia_semana_int = 0;
uint8_t segundo; /* seconds */
uint8_t minuto;  /* minutes */
uint8_t hora;    /* hours */
uint8_t dia;     /* day of the week */
uint8_t mes;     /* month */
int16_t ano;     /* year */
uint8_t prevmes; /* month */
int16_t prevano; /* year */
/*Auxiliares*/
uint8_t segundo1; /* seconds */
uint8_t minuto1;  /* minutes */
uint8_t hora1;    /* hours */
uint8_t dia1;     /* day of the week */
uint8_t mes1;     /* month */
int16_t ano1;     /* year */
DateTime now;
//----------------------------------------------//
//             General purpose variables         //
//----------------------------------------------//
int prevmin = 0, prevday = 0;
int rtcrestart = 0; //
unsigned long timeout1 = 0, timeout2 = 0;
bool webwork = 0; 
bool envia = 0;
int change = 0;  //if changes happens sends data
int modo_automatico = 0; 
int numero_horarios = 0;
bool sal = 0;
int ciclo_actual = 0;
int Sistema_apagado = 0; //
String ch, ch1, ch2, ch3;
int chlength = 0;
String formdata, vacio = "";
///-------------Set wifi network parameters---------------------
/****WIFI****/
char ssid[60] = "orbittas";     //WIFI SSID
char password[60] = "20075194"; //WIFI PASSWORD
char ssid2[60] = "REGISTER";       //AP SSID
char password2[60] = "12345678"; //AP PASSWORD
String WRSSI;
/****SPIFF****/
long int lastposition2 = 0; 
long int lastposition = 0;  
long int lastposition3 = 0; 
long int lastposition4 = 0; 
//////MQTT
#define USEWIFIMQTT        
#include "PubSubClient.h" 
WiFiClient espClient;     
PubSubClient mqttclient(espClient); 
/****Variables WIFI MQTT****/
char host[120] = "broker.mqttdashboard.com";
char datarecvd[512]; 
int reconnect = 0;   
/****parametros mqtt ****/
char MQTTHost[120] = "broker.mqttdashboard.com"; 
char MQTTPort[6] = "1883";                       
char MQTTClientID[60] = "";                      
char MQTTTopic[60] = "";                         
char MQTTTopic2[60] = "";                        
char MQTTUsername[60] = "*";                     
char MQTTPassword[120] = "*";                    
uint32_t ChipId32, wait = 0, wait2 = 0, wait3 = 0;
uint16_t ChipId16;
uint64_t ChipId;
String chipid = "";
String clientId = "REGISTER/";
///changes
int modo_automatico_aux;
int modo_nowc = 1;
int modo_nowc_aux = 0;
///changes mqtt
int modo_automatico_aux1;
int modo_nowc1 = 1;
int modo_nowc_aux1 = 0;
char ssid_aux[60] = "";      //WIFI SSID
char password_aux[60] = "";  //WIFI PASSWORD
char ssid2_aux[60] = "";     //AP SSID
char password2_aux[60] = ""; //AP PASSWORD
char host_aux[120] = "";
char MQTTUsername_aux[60] = "";
char MQTTPassword_aux[120] = "";
char MQTTHost_aux[120] = ""; 
char MQTTPort_aux[6] = "";
int changev = 0;  
int changev2 = 0; 
int inicio = 0;
int proximo_ciclo_aux = 0;
int proximo_ciclo_aux1 = 0;
long int mqttdelay = 0, blikDelay = 0;
int env_prox = 0; 
bool solicitud_web = 0;
bool envia_horarios = 0;
bool guardarHorarios = 0;
int subscribed = 0; 
bool rtcFalla = 0;
bool wifiLedState = false;
bool apMode = 0;
bool apActivate=0;
bool guardarAp = 0;
String ipRed = "0.0.0.0";
bool cambioIp = 0;
long int minutosEnApMode = 0;
int minutoAuxAp = 0;
bool cambioFechaHora = false;
///////////pn532 declarations
// If using the breakout with SPI, define the pins for SPI communication.
#include <Adafruit_PN532.h>
#define PN532_SCK (18)
#define PN532_MISO (19)
#define PN532_MOSI (23)
#define PN532_SS (15)
#define PN532DEBUG
#define bootX 2
/////NFC variables
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
long int nfcDelay = 0;
uint8_t tag;
uint32_t tagId = 0;
uint32_t tagIdPrev = 1;
uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
bool bussyMqtt = 0;
/////
String Shora = "", Sfecha = "", Sday = "", Smonth = "", Syear = "", Shr = "", Smin = "";
/////server protocol
bool serverPoll = 0; /// 
char devName[11] = "room-x";