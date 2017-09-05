#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <TSL2561.h>

#include "protocolo.h"

#define PIN_LED			13

struct Sensor {
	/**
	 * Tipo de sensor
	 */
	String tipo;
	/**
	 * Puerto al que está conectado el sensor (si es necesario)
	 */
	int gpio;
	/**
	 * Objeto propio para controlar este sensor. Dependerá del tipo.
	 */
	void *objSensor;
};

bool inicializado = false;
int numSensores = 0;
String cadena;

// Podremos definir un máximo de 20 sensores
Sensor sensores[20];

void setup() {
	// Configuramos el LED
	pinMode(PIN_LED, OUTPUT);

	// Inicializamos el puerto serie y esperamos hasta que esté disponible
	Serial.begin(9600);
	while (!Serial) {
		;
	}

	// Solicitamos la inicialización a Android
	solicitarInicializacion();
}

void loop() {
	// Si hay algo en la entrada, lo leemos
	if (Serial.available() > 0) {
		cadena = Serial.readStringUntil(10);
		procesarComando(cadena);
	}
}

/**
 Solicitamos a Android (insistentemente) que nos mande la inicialización de los sensores a leer
 */
void solicitarInicializacion() {
	// Mandamos el comando cada segundo hasta que se reciba algo
	while (Serial.available() <= 0) {
		led(true);
		enviarComando(CMD_INITREQ);
		led(false);
		delay(1000);
	}
}

/**
 * Llevamos a cabo la inicialización de sensores que nos llega desde Android
 */
void inicializacion(String params) {
	led(true);
	Serial.println("Inicialización");

	unsigned int pos = 0;
	String gpio = "", tipo = "";
	char caracter;
	bool esGpio = true;
	while (pos < params.length()) {
		caracter = params[pos];
		if (caracter == '=')
			esGpio = false;
		else if (caracter == ',') {
			esGpio = true;
			addSensor(gpio, tipo);
			gpio = "";
			tipo = "";
		} else if (esGpio)
			gpio += caracter;
		else
			tipo += caracter;
		pos++;
	}
	if (gpio.length() > 0)
		addSensor(gpio, tipo);

	Serial.println("Inicializados " + String(numSensores) + " sensores");
	inicializado = true;
	enviarComando(CMD_INITOK);
	led(false);
}

void addSensor(String gpio, String tipo) {
	sensores[numSensores].gpio = gpio.toInt();
	sensores[numSensores].tipo = tipo;

	if (tipo.equals("dht11_temp") || tipo.equals("dht11_hum")) {
		sensores[numSensores].objSensor = new DHT(gpio.toInt(), DHT11, 1);
		((DHT*) sensores[numSensores].objSensor)->begin();
	} else if (tipo.equals("dht22_temp") || tipo.equals("dht22_hum")) {
		sensores[numSensores].objSensor = new DHT(gpio.toInt(), DHT22, 1);
		((DHT*) sensores[numSensores].objSensor)->begin();
	} else if (tipo.equals("lux")) {
		sensores[numSensores].objSensor = new TSL2561(TSL2561_ADDR_FLOAT);
		((TSL2561*) sensores[numSensores].objSensor)->begin();
		((TSL2561*) sensores[numSensores].objSensor)->enable();
	}

	numSensores++;
	Serial.println("Sensor " + tipo + " conectado a GPIO" + gpio);

}

void realizarLectura(String params) {
	int ns = params.toInt();

	if (ns < 0 || ns > numSensores) {
		enviarError("El sensor " + params + " no está definido");
	} else {
		led(true);
		Sensor sensor = sensores[ns];

		if (sensor.tipo.equals("dht11_temp")
				|| sensor.tipo.equals("dht22_temp")) {
			DHT *oSensor = (DHT*) sensor.objSensor;
			float temp = oSensor->readTemperature();
			enviarValorSensor(ns, temp);
		} else if (sensor.tipo.equals("dht11_hum")
				|| sensor.tipo.equals("dht22_hum")) {
			DHT *oSensor = (DHT*) sensor.objSensor;
			float hum = oSensor->readHumidity();
			enviarValorSensor(ns, hum);
		} else if (sensor.tipo.equals("lux")) {
			TSL2561 *oSensor = (TSL2561*) sensor.objSensor;
			uint16_t lum = oSensor->getLuminosity(TSL2561_VISIBLE);
			enviarValorSensor(ns, lum);
		}
		led(false);
	}
}

void enviarValorSensor(int sensor, String valor) {
	enviarComando(CMD_READOK, String(sensor) + " " + valor);
}

void enviarValorSensor(int sensor, float valor) {
	enviarValorSensor(sensor, String(valor));
}

void procesarComando(String cadena) {
	String comando = "";
	String params = "";
	Serial.println("RECIBIDO: " + cadena);

	int sep = cadena.indexOf(' ');
	if (sep != -1) {
		comando = cadena.substring(0, sep);
		params = cadena.substring(1 + sep);
	} else
		comando = cadena;

// Comprueba si lo que nos viene es un comando
	if (comando.startsWith("AS_")) {
		Serial.println("COMANDO: " + comando);
		Serial.println("PARAMETROS: " + params);

		// Interpretamos lo que hace cada uno
		if (comando.equals(CMD_INIT)) {
			// Android nos manda la inicialización que le habíamos pedido
			inicializacion(params);
		} else if (comando.equals(CMD_RESET)) {
			// Android nos solicita que reinicialicemos
			Serial.println("Reiniciando...");
			inicializado = false;
			numSensores = 0;
			Serial.flush();
			solicitarInicializacion();
		} else if (comando.equals(CMD_READ)) {
			// Android nos pide que leamos un sensor
			realizarLectura(params);
		}
	} else
		enviarError("Comando no reconocido");
}

void enviarComando(String comando, String params) {
	Serial.println(comando + " " + params);
}

void enviarComando(String comando) {
	Serial.println(comando);
}

void enviarError(String mensaje) {
	enviarComando(CMD_ERROR, mensaje);
}

/**
 Encendemos o apagamos el led
 */
void led(bool encender) {
	digitalWrite(PIN_LED, encender ? HIGH : LOW);
}
