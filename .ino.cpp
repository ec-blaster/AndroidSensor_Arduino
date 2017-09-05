#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2017-09-05 09:12:39

#include "Arduino.h"
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <TSL2561.h>
#include "protocolo.h"
void setup() ;
void loop() ;
void solicitarInicializacion() ;
void inicializacion(String params) ;
void addSensor(String gpio, String tipo) ;
void realizarLectura(String params) ;
void enviarValorSensor(int sensor, String valor) ;
void enviarValorSensor(int sensor, float valor) ;
void procesarComando(String cadena) ;
void enviarComando(String comando, String params) ;
void enviarComando(String comando) ;
void enviarError(String mensaje) ;
void led(bool encender) ;


#include "AndroidSensor_Arduino.ino"

#endif
