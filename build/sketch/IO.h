#line 1 "d:\\Repositorios\\Domo\\Esp8266\\Modulos Especificos\\IR\\IO.h"
/**
******************************************************
* @file IO.h
* @brief Documentacion de Modulo Outlet wifi
* @author Jose Martinez Molero
* @version 1.1
* @date Junio 2020
*
*
*******************************************************/
#ifndef IO_H
	#define IO_H



	#ifdef NODEMCU
		int OutputIR = 14;													//Pin Salida IR
		int InputIR = 5;													//Pin Entrada de IR
		int PinLed = 4;														//Pin Led azul
		int PinReset = 13;													//Pin Reset
	#endif	

	#ifdef ESP01
		int OutputIR = 3;													//Pin Salida IR
		int InputIR = 5;													//Pin Entrada de IR
		int PinLed = 0;														//Pin Led azul
		int PinReset = 13;													//Pin Reset
	#endif	

#endif