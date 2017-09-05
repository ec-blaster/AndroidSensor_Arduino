/*
 * protocolo.h
 *
 * Definición de los comandos del protocolo
 *
 *  Created on: 31 ago. 2017
 *      Author: isidoro
 */

#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#define CMD_ERROR   "AS_ERROR"    // Error general
#define CMD_RESET   "AS_RESET"    // Orden de reinicio para el Arduino
#define CMD_INITREQ "AS_INITREQ"  // Comando para petición de inicialización por parte del Arduino
#define CMD_INIT    "AS_INITDATA" // Comando para envío de datos de inicialización de Android al Arduino
#define CMD_INITOK  "AS_INITOK"   // Comando para confirmar que la inicialización ha ido bien
#define CMD_INITERR "AS_INITERR"  // Comando para indicar un error de inicialización por parte del Arduino
#define CMD_READ    "AS_READ"     // Comando de petición de lectura de un sensor
#define CMD_READOK  "AS_READOK"   // Comando de envío de datos de un sensor a Android

#endif /* PROTOCOLO_H_ */
