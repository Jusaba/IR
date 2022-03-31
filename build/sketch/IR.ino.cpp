#include <Arduino.h>
#line 1 "d:\\Repositorios\\Domo\\Esp8266\\Modulos Especificos\\IR\\IR.ino"


/**
******************************************************
* @file Sensor.ino
* @brief Sensor para temperatura, viento, ....
* @author Julian Salas Bartolome
* @version 1.0
* @date Julio 2020
*
*
*******************************************************/
//#include <GDBStub.h>

#include "ServerPic.h"
#include "IO.h"

#line 18 "d:\\Repositorios\\Domo\\Esp8266\\Modulos Especificos\\IR\\IR.ino"
void setup();
#line 90 "d:\\Repositorios\\Domo\\Esp8266\\Modulos Especificos\\IR\\IR.ino"
void loop();
#line 18 "d:\\Repositorios\\Domo\\Esp8266\\Modulos Especificos\\IR\\IR.ino"
void setup() {

  	#ifdef Debug														//Usamos el puereto serie solo para debugar	
		Serial.begin(9600);												//Si no debugamos quedan libres los pines Tx, Rx para set urilizados
		Serial.println("Iniciando........");
	#endif
	EEPROM.begin(128);													//Reservamos zona de EEPROM
	//BorraDatosEprom ( 0, 128 );										//Borramos 128 bytes empezando en la posicion 0		

  	pinMode(PinReset, INPUT);   	        								//Configuramos el pin de reset como entrada
	pinMode(PinLed, OUTPUT);	              							//configuramos el pin del led verde como salida

	EnciendeLed();


	if ( LeeByteEprom ( FlagConfiguracion ) == 0 )						//Comprobamos si el Flag de configuracion esta a 0
	{																	// y si esta
		ModoAP();														//Lo ponemos en modo AP

	}else{																//Si no esta
		if ( ClienteSTA() )												//Lo poenmos en modo STA y nos conectamos a la SSID
		{																//Si jha conseguido conectarse a ls SSID en modo STA
	        if ( ClienteServerPic () )									//Intentamos conectar a ServerPic
    		{
				CheckFirmware();    									//Comprobamos si el firmware esta actualizado a la ultima version
		    	#ifdef Debug
        			Serial.println(" ");
        			Serial.println("Conectado al servidor");
      			#endif 
      			cSalida = " " ;

				DataConfig aCfg = EpromToConfiguracion ();     							 //Leemos la configuracin de la EEprom
				char USUARIO[1+aCfg.Usuario.length()]; 
				(aCfg.Usuario).toCharArray(USUARIO, 1+1+aCfg.Usuario.length());          //Almacenamos en el array USUARIO el nombre de usuario 
				cDispositivo = USUARIO;
				char PUSH[1+aCfg.Push.length()]; 
				(aCfg.Push).toCharArray(PUSH, 1+1+aCfg.Push.length());                   //Almacenamos en el array USUARIO el nombre de usuario 
				cClientePush = PUSH;
				lHomeKit = aCfg.lHomeKit;
				lWebSocket = aCfg.lWebSocket;
				lEstadisticas = aCfg.lEstadisticas;
				lPush = aCfg.lPush;

				if ( lEstadisticas )									//Si están habilitadas las estadisticas, actualizamos el numero de inicios
				{
					GrabaVariable ("inicios", 1 + LeeVariable("inicios") );
				}

      			ApagaLed();   

    			cSalida = LeeValor();													//Arrancamos con el ultimo valor
      			if ( cSalida == "ERROR")												//Si no habia ultimo valor, arrancamos con On
      			{
      				DispositivoOn();
      			}else{																	//Si existia ultimo valor, arrancamos con el valor registrado
      				if (cSalida == "On")
      				{
      					DispositivoOn();
      				}
      				if (cSalida == "Off")
      				{
      					DispositivoOff();
      				}	

      			}	

    		}
    	}	
	}
}


void loop() {

	    long Inicio, Fin;

 		/*----------------
 		Comprobacion Reset
 		------------------*/

		//TestBtnReset (PinReset);

 		/*----------------
 		Comprobacion Conexion
 		------------------*/
		if ( TiempoTest > 0 )
		{
			if ( millis() > ( nMiliSegundosTest + TiempoTest ) )			//Comprobamos si existe conexion  
			{
		        EnciendeLed();        									//Encendemos el led para indicar que se comprueba la conexion
				nMiliSegundosTest = millis();
				if ( !TestConexion(lEstadisticas) )							//Si se ha perdido la conexion
				{
					lConexionPerdida = 1;									//Ponemos el flag de perdida conexion a 1
					if ( GetDispositivo() )									//Si el dispositivo estaba a On
					{
						lEstado = 1;										//Guardamos ese estado para tenerlo en cuenta en la reconexion
						DispositivoOff();									//Ponemos a Off el dispositivo	
					}	
				}else{														//Si existe conexion
			        ApagaLed();											//Apagamos el led para indicar que se ha finalizado el test                                                                                      
					if ( lConexionPerdida )									//Comprobamos si es una reconexion ( por perdida anterior )
					{														//Si lo es
						lConexionPerdida = 0;								//Reseteamos flag de reconexion
						if ( lEstado )
						{
							DispositivoOn();
						}else{
							DispositivoOff();	
						}
						lEstado = 0;										//Reseteamos el Flag lEstado
					}	
				}	
 	   		}	
    	}
 		/*----------------
 		Comprobacion de que el PIR haya detectado movimiento
 		------------------*/
   		if (lOnOff)														//Si el PIR está habilitado
   		{

		}	
 		/*----------------
 		Analisis comandos
 		------------------*/
		oMensaje = Mensaje ();								 			//Iteractuamos con ServerPic, comprobamos si sigue conectado al servidor y si se ha recibido algun mensaje

		if ( oMensaje.lRxMensaje)										//Si se ha recibido ( oMensaje.lRsMensaje = 1)
		{
	    	#ifdef Debug				
				Serial.println(oMensaje.Remitente);						//Ejecutamos acciones
				Serial.println(oMensaje.Mensaje);
			#endif	
			FlashLed();													//Flash de led que indica que se ha recibido mensaje			
			//En este punto empieza el bloque de programa particular del dispositivo segun la utilizacion					

			if (oMensaje.Mensaje == "On")								//Si se recibe "On", se pone PinSalida a '1'
			{	
				DispositivoOn();	
				cSalida = "On";
			}
			if (oMensaje.Mensaje == "Off")								//Si se recibe "Off", se pone PinSalida a '0'
			{	
				DispositivoOff();	
				cSalida = "Off";
			}
			if (oMensaje.Mensaje == "Change")							//Si se recibe 'Change', cambia el estado de PinSalida 
			{	
				if ( GetDispositivo() )
				{
					DispositivoOff();
					cSalida = "Off";
				}else{
					DispositivoOn();
					cSalida = "On";
				}
			}
			if (oMensaje.Mensaje == "ChangeGet")						//Si se recibe 'ChangeGet', cambia el estado de PinSalida y devuelve el nuevo estado al remitente 
			{	
				if ( GetDispositivo() )
				{
					DispositivoOff();
					cSalida = "Off";
				}else{
					DispositivoOn();
					cSalida = "On";					
				}
				oMensaje.Mensaje = cSalida;								//Confeccionamos el mensaje a enviar hacia el servidor	
				oMensaje.Destinatario = oMensaje.Remitente;
				EnviaMensaje(oMensaje);									//Y lo enviamos
			}			
			if (oMensaje.Mensaje == "Get")								//Si se recibe 'Get', se devuelve el estado de PinSalida al remitente
			{	
				if ( GetDispositivo() )
				{
					cSalida = "On";
				}else{
					cSalida = "Off";
				}
				oMensaje.Mensaje = cSalida;
				oMensaje.Destinatario = oMensaje.Remitente;
				EnviaMensaje(oMensaje);	
				cSalida = String(' ');									//No ha habido cambio de estado, Vaciamos cSalida para que no se envie a WebSocket y a HomeKit 
			}	

			if ((oMensaje.Mensaje).indexOf("AAOn-:-")==0)								//Si se recibe 'Get', se devuelve el estado de PinSalida al remitente
			{
				String cValor = String(oMensaje.Mensaje).substring(  3 + String(oMensaje.Mensaje).indexOf("-:-"),  String(oMensaje.Mensaje).length() ); 
				cModelo = String(cValor).substring(0, String(cValor).indexOf("-:-"));
				cValor = String(cValor).substring(  3 + String(cValor).indexOf("-:-"),  String(cValor).length() );
				String cTemperatura = String(cValor).substring(0, String(cValor).indexOf("-:-"));
				cValor = String(cValor).substring(  3 + String(cValor).indexOf("-:-"),  String(cValor).length() );
				cModo = String(cValor).substring(0, String(cValor).indexOf("-:-"));
				String cVentilador =  String(cValor).substring(3 + String(cValor).indexOf("-:-"),  String(cValor).length());
        		nTemperatura = (cTemperatura+'\0').toInt();
				nVentilador = (cVentilador+'\0').toInt();
        		nRepeticiones = 2;
				cSalida = String(' ');		
				AAOn ( cModelo, nTemperatura, cModo, nVentilador, nRepeticiones );
				
			}	

			if (oMensaje.Mensaje == "AAOff")								//Si se recibe 'Get', se devuelve el estado de PinSalida al remitente
			{
Serial.print('--------');
Serial.println(cModelo);				
        		AAOff (cModelo, nTemperatura, cModo, nVentilador, nRepeticiones );
			}	
	 		/*----------------
 			Actualizacion ultimo valor
 			------------------*/
			if ( cSalida != String(' ') )				//Si hay cambio de estado
			{	
				EnviaValor (cSalida);					//Actualizamos ultimo valor
			}

	 		/*----------------
 			Actualizacion WebSocket
 			------------------*/
			#ifdef WebSocket
				if ( cSalida != String(' ') && lWebSocket )				//Si está habililtado WebSocket y hay cambio de estado en el dispositivo, se notifica a WebSocket
				{	
					EnviaMensajeWebSocket(cSalida);						//Actualizamos las páginas html conectadas
				}
	 		#endif

	 		/*----------------
 			Notificacion HomeKit
 			------------------*/
			#ifdef HomeKit
				if ( cSalida != String(' ') && oMensaje.Remitente != ( cDispositivo + String("_") ) && lHomeKit ) //Si está habililtadoHomeKit y hay cambio de estado en el dispositivo, se notifica a HomeKit
				{
					oMensaje.Destinatario = cDispositivo + String("_");
					oMensaje.Mensaje = cSalida;
					EnviaMensaje(oMensaje);
				}
			#endif		
			
			cSalida = String(' ');										//Limpiamos cSalida para iniciar un nuevo bucle

			if ( lEstadisticas )									 	//Si están habilitadas las estadisticas, actualizamos el numero de comandos recibidos
			{
				GrabaVariable ("comandos", 1 + LeeVariable("comandos") );
			}	

			nMiliSegundosTest = millis();		
	
		}

	    wdt_reset(); 													//Refrescamos WDT

}


