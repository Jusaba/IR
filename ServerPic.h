/**
******************************************************
* @file ServerPic.h
* @brief Documentacion de Modulo Outlet wifi
* @author FabLab Lleida
* @author Jose Martinez Molero
* @author Julian Salas Bartolome
* @version 1.0
* @date Febrero 2018
*
*
* Compilado con Stino ESP 8266 2.5.2
* Opciones de compilacion
* 	Placa: 					Generic ESP8266 Module  ( para ESP01 y ESP11 )
* 	CPU Frecuency:			80 MHZ
* 	CRYSTAL Frecuency:		26 MHZ
*	Debul Level:			none
* 	Debug Pot:				Disabled
*	Esase Flash:			Only sketch
*	Expressid FW:			nonos-sdk 2.2.1 (legacy)
*   Exceptiosn:				Disabled	
*	Flash Frecuency:		40 MHZ
*	Flash Mode:				DOUT ( compatible )
*	Flash Size:				1M(64k SPIFFS)
*	IwIP Variant:			v2 Lower Memory
*	Reset Method:           ck
*	SSL SUport:             ALL SSL chiphers (most compatible)
*	Upload Speed:			115200
*	VTables:				Flash
*	
*	version: 1.0
*   ------------
*   Properties: xtal=80,CrystalFreq=26,lvl=None____,dbg=Disabled,wipe=none,sdk=nonosdk221,exception=disabled,FlashFreq=40,FlashMode=dout,eesz=1M,ip=lm2f,ResetMethod=ck,ssl=all,baud=115200,vt=flash 
*	
*	version: 1.1
*   ------------
*   Añadidos flags lHomekit, lWebsocket y lEstadisticas
*   Añadidas estadisticas de inicios, comando y desconexion serverpic
*******************************************************/

#ifndef SERVERPIC_H
	#define SERVERPIC_H

	//----------------------------------------------
	//Includes
	//----------------------------------------------
	#include <ESP8266WiFi.h>
	#include <WiFiClient.h> 
	#include <ESP8266WebServer.h>
	#include <WiFiUdp.h>
	#include <ArduinoOTA.h>

	#include "Global.h"
	#include "Configuracion.h"
	#include "Eprom.h"
	#include "ModoAP.h"
	#include "ModoSTA.h"
	#include "Servidor.h"

	#include "IRremoteESP8266.h"
	#include "IRsend.h"
	#include "ir_Mitsubishi.h"
	#include "ir_Panasonic.h"
	#include "ir_Gree.h"

	#define ESP01
	#define Mitsubishi

	#ifdef ESP01
		#define Placa "Proto"
		#define Modelo "ESP01"
		#define Ino "IR"					
		//-----------------
		//TiEmpo de rebotes
		//-----------------
 		#define TempoRebotes 150
	#endif

	#ifdef NODEMCU
		#define Placa "Nodemcu"
		#define Modelo "ESP12"
		#define Ino "IR"					
		//-----------------
		//TiEmpo de rebotes
		//-----------------
 		#define TempoRebotes 150
	#endif


	//----------------------------
	//CARACTERISTICAS DISPOSITIVO
	//----------------------------
	#define VIno "1.1"						//Version del programa principal
	#define VBuild  "3"
	//---------------------------------
	//CARACTERISTICAS DE LA COMPILACION
	//---------------------------------
	#define Compiler "Stino";				//Compilador
	#define VCore "2.5.2";					//Versión del Core Arduino


	#include "IO.h";



	//----------------------------------------------
	//DEBUG
	//----------------------------------------------
	//#define Debug
	//----------------------------------------------
	//LED 
	//----------------------------------------------	
	//#define Led
	//----------------------------------------------
	//HOME KIT
	//----------------------------------------------
	//#define HomeKit
	//----------------------------------------------
	//Web Socket
	//----------------------------------------------
	#define WebSocket		

	//----------------------------------------------
	//Teimpo de Test de conexion
	//----------------------------------------------
	#define TiempoTest	14000												//Tiempo en milisegundos para Test de conexion a servidor


	//----------------------------------------------
	//TiEmpo de rebotes
	//----------------------------------------------
 	#define TempoRebotes 150




	boolean GetDispositivo (void);
	void DispositivoOn (void);
	void DispositivoOff (void);	
	void GrabaTiempoActuacion ( int nSegundos );
	int LeeTiempoActuacion ( void );
	void PirOn (void);
	void PirOff (void);	
	void EnciendeLed (void);
	void ApagaLed (void);
	void FlashLed (void);	

void AAOn ( String cModelo, int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA );
void AAOn ( String cModelo, int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA );
void AAPanasonicOn(int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA);
void AAPanasonicOff(int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA);
void AAMitsubishiOn ( int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA );
void AAMitsubishiOff ( int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA );
void AAKosnerOn ( int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA );
void AAKosnerOff ( int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA );
void TaurusOn(int nTemperatura, int nResistencias);
void TaurusOff(void);


	//------------------------------------
	//Declaracion de variables UNIVERSALES
	//-----------------------------------
	ESP8266WebServer server(80);											//Definimos el objeto Servidor para utilizarlo en Modo AP
	WiFiClient Cliente;														//Definimos el objeto Cliente para utilizarlo en Servidor
	Telegrama oMensaje;									 					//Estructura mensaje donde se almacenaran los mensajes recibidos del servidor

	String cDispositivo = String(' ');										//Variable donde se deja el nombre del dsipositivo. Se utiliza para comunicar con HomeKit
	String cClientePush = String(' ');										//Variable donde se deja el nombre del Cliente Push. 

	String cSalida;															//Variable donde se deja el estado ( String ) para mandar en mensaje a ServerPic
	boolean lEstado = 0;													//Variable donde se deja el estado del dispositivo para reponer el estado en caso de reconexion
	boolean lConexionPerdida = 0;											//Flag de conexion perdida, se pone a 1 cuando se pierde la conexion. Se utiliza para saber si se ha perdido la conexion para restablecer estado anterior de la perdida

	boolean lHomeKit;													    //Flag para habililtar HomeKit ( 1 Habilitado, 0 deshabilitado )
	boolean lWebSocket ;  													//Flag para habililtar WebSocket ( 1 Habilitado, 0 deshabilitado )
	boolean lEstadisticas;	
	boolean lPush ;														    //Flag donde para que indica si se usa Push o no ( 1/0 )

	unsigned long nMiliSegundosTest = 0;									//Variable utilizada para temporizar el Test de conexion del modulo a ServerPic
	unsigned long nMilisegundosRebotes = 0;                					//Variable utilizada para temporizar el tiempo de absorcion de rebotes

	//------------------------------------
	//Declaracion de variables PARTICULARES
	//-----------------------------------
	boolean lOnOff = 1;														//Flag que indica si el PIR está habilitado o no												
	boolean lDispositivo = 0;												//Flag que indica que se ha dado la orden al un dispositivo remoto para que actue

	int nTemperatura;
	int nVentilador;
	String cModo;
	String cModelo;
	int nRepeticiones;

	//Variables donde se almacenan los datos definidos anteriormente para pasarlos a Serverpic.h
	//para mandar la información del Hardware y Software utilizados
	//En la libreria ServerPic.h estan definidos como datos externos y se utilizan en la funcion
	//Mensaje () para responder al comando generico #Info.
	//Con ese comando, el dispositivo mandara toda esta información al cliente que se lo pida
	// ESTOS DATOS NO SON PARA MODIFICAR POR USUARIO
	//----------------------------
	//Datos del programa principal
	//----------------------------
	String VMain = VIno;
	String Main = Ino; 
	String VModulo = VBuild;
	//----------------------------
	//Datos de compilación
	//----------------------------	
	String Compilador = Compiler;
	String VersionCore = VCore;

	//----------------------------
	//Datos de Hardware
	//----------------------------	
	String ModeloESP = Modelo;
	String Board = Placa;








	/**
	******************************************************
	* @brief Enciende el led
	*
	*/
	void EnciendeLed (void)
	{
  		digitalWrite(PinLed, HIGH);	
	}
	/**
	******************************************************
	* @brief Apaga el led
	*
	*/
	void ApagaLed (void)
	{
  		digitalWrite(PinLed, LOW);	
	}
	/**
	******************************************************
	* @brief Hace un flash en el led
	*
	*/
	void FlashLed (void)
	{
  		EnciendeLed();
		delay(50);
  		ApagaLed();
  		delay(50);
		EnciendeLed();
  		delay(50);
  		ApagaLed();
		delay(50);
  		EnciendeLed();
  		delay(50);
		ApagaLed();
	}
	
	/**
	******************************************************
	* @brief Devielve el estado del dispositivo
	*
	* @return devuelve <b>1</b> si el dispositivo esta conectado o <b>0<\b> en caso de que no este conectado
	*/
	boolean GetDispositivo (void)
	{
		return ( lOnOff );

	}
	/**
	******************************************************
	* @brief Pone el dispositivo en On
	*
	*/
	void DispositivoOn (void)
	{
		lOnOff=1;
	}
	/**
	******************************************************
	* @brief Pone el dispositivo en OPff
	*
	*/
	void DispositivoOff (void)
	{
 		lOnOff=0;
	}

	void AAOn ( String cModelo, int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA )
	{
		if ( cModelo ==  "Mitsubishi" )
		{
			AAMitsubishiOn ( nTemperaturaAA, cModoAA, nVentiladorAA, nRepeticionesAA );		
		}
		if ( cModelo == "Kosner" )
		{
			AAKosnerOn ( nTemperaturaAA, cModoAA, nVentiladorAA, nRepeticionesAA );		
		}
		if ( cModelo == "Panasonic" )
		{
			AAPanasonicOn ( nTemperaturaAA, cModoAA, nVentiladorAA, nRepeticionesAA );		
		}
	}

	void AAOff ( String cModelo, int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA )
	{
		if ( cModelo ==  "Mitsubishi" )
		{
			AAMitsubishiOff ( nTemperaturaAA, cModoAA, nVentiladorAA, nRepeticionesAA );				
		}
		if ( cModelo == "Kosner" )
		{
			AAKosnerOff ( nTemperaturaAA, cModoAA, nVentiladorAA, nRepeticionesAA );					
		}
		if ( cModelo == "Panasonic" )
		{
			AAPanasonicOff ( nTemperaturaAA, cModoAA, nVentiladorAA, nRepeticionesAA );		
		}

	}	
	void AAMitsubishiOn ( int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA )
	{
		int nRepeticiones = nRepeticionesAA;
		IRMitsubishi112 ac(OutputIR);  
		ac.begin();
		delay(50);
		ac.on();
		if ( cModoAA == "Calor")
		{
			ac.setMode(kMitsubishiAcHeat);
		}						
		if ( cModoAA == "Frio")
		{
			ac.setMode(kMitsubishiAcCool);
		}
  		ac.setFan(nVentiladorAA);					
  		ac.setTemp(nTemperaturaAA);	
		while ( nRepeticiones > 0 )
		{	
			ac.send();
			delay(1000);
			nRepeticiones --;
		}	
	}
	void AAMitsubishiOff ( int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA )
	{
		int nRepeticiones = nRepeticionesAA;
		IRMitsubishi112 ac(OutputIR);  
		ac.begin();
		delay(50);
		ac.off();
		if ( cModoAA == "Calor")
		{
			ac.setMode(kMitsubishiAcHeat);
		}						
		if ( cModoAA == "Frio")
		{
			ac.setMode(kMitsubishiAcCool);
		}
  		ac.setFan(nVentiladorAA);					
  		ac.setTemp(nTemperaturaAA);	
		while ( nRepeticiones > 0 )
		{	
			ac.send();
			delay(1000);
			nRepeticiones --;
		}	
	}	
	void AAPanasonicOn(int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA)
	{
		int nRepeticiones = 1;
		IRPanasonicAc32 ac(OutputIR);
		ac.begin();
		delay(50);
		ac.setPowerToggle(1);
		if (cModoAA == "Calor")
		{
			ac.setMode(kPanasonicAc32Heat);
		}
		if (cModoAA == "Frio")
		{
			ac.setMode(kPanasonicAc32Cool);
		}
		ac.setFan(nVentiladorAA);
		ac.setTemp(nTemperaturaAA);
		while (nRepeticiones > 0)
		{
			ac.send();
			delay(1000);
			nRepeticiones--;
		}
	}
	void AAPanasonicOff(int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA)
	{
		int nRepeticiones = 1;
		IRPanasonicAc32 ac(OutputIR);
		ac.begin();
		delay(50);
		ac.setPowerToggle(1);
		if (cModoAA == "Calor")
		{
			ac.setMode(kPanasonicAc32Heat);
		}
		if (cModoAA == "Frio")
		{
			ac.setMode(kPanasonicAc32Cool);
		}
		ac.setFan(nVentiladorAA);
		ac.setTemp(nTemperaturaAA);
		while (nRepeticiones > 0)
		{
			ac.send();
			delay(1000);
			nRepeticiones--;
		}
	}	
	void AAKosnerOn ( int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA )
	{
		int nRepeticiones = nRepeticionesAA;		
  		IRGreeAC ac(OutputIR);  
		ac.begin();
  		ac.on();
  		ac.setFan(nVentiladorAA);
		if ( cModoAA == "Calor")
		{
  			ac.setMode(kGreeHeat);
		}						
		if ( cModoAA == "Frio")
		{
			ac.setMode(kGreeCool);
		}  				
  		ac.setTemp(nTemperaturaAA);  
  		ac.setSwingVertical(true, kGreeSwingAuto);
  		ac.setXFan(false);
  		ac.setLight(true);
  		ac.setSleep(false);
  		ac.setTurbo(false);
		while ( nRepeticiones > 0 )
		{	
			ac.send();
			delay(1000);
			nRepeticiones --;
		}	
	}
	void AAKosnerOff ( int nTemperaturaAA, String cModoAA, int nVentiladorAA, int nRepeticionesAA )
	{
		int nRepeticiones = nRepeticionesAA;		
  		IRGreeAC ac(OutputIR);  
		ac.begin();
  		ac.off();
  		ac.setFan(nVentiladorAA);
		if ( cModoAA == "Calor")
		{
  			ac.setMode(kGreeHeat);
		}						
		if ( cModoAA == "Frio")
		{
			ac.setMode(kGreeCool);
		}  				
  		ac.setTemp(nTemperaturaAA);  
  		ac.setSwingVertical(true, kGreeSwingAuto);
  		ac.setXFan(false);
  		ac.setLight(true);
  		ac.setSleep(false);
  		ac.setTurbo(false);
		while ( nRepeticiones > 0 )
		{	
			ac.send();
			delay(1000);
			nRepeticiones --;
		}			
	}	

void TaurusOn(int nTemperatura, int nResistencias)
{
	int nTemperaturaI = 30;

	//On
	irsend.sendSymphony(0xDA0, 12, 3);
	delay(1000);
	//Por defecto se arranca con resistencia pequeña ( nResistencias = 1 )
	//Si se desea poner la resistencia grande ( nResistencia = 2)
	if (nResistencias == 2)
	{
		irsend.sendSymphony(0xD82, 12, 3);
		delay(1500);
	}
	//Si se desea poner las dos resistencias ( nResistencia = 3)
	if (nResistencias == 3)
	{
		irsend.sendSymphony(0xD82, 12, 3);
		delay(1500);
		irsend.sendSymphony(0xD82, 12, 3);
		delay(1000);
	}
	//La estufa arranca con 30 grados, para llegar a la temperatura objetivo debemos decrementar la temperatura
	irsend.sendSymphony(0xD81, 12, 3); //Menu
	delay(300);
	while (nTemperaturaI > nTemperatura)
	{
		irsend.sendSymphony(0xD88, 12, 3);
		delay(1000);
		nTemperaturaI--;
	}
}
void TaurusOff(void)
{
	irsend.sendSymphony(0xDA0, 12, 3);
	delay(1000);
}	
#endif