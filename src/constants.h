#pragma once

// Definir los pines GPIO y otras constantes
const int pinMotorTrabajo = 19;
const int pinArranqueArriba = 21;
const int pinArranqueAbajo = 23;
const int pinFinCarreraArriba = 17;
const int pinFinCarreraAbajo = 13;
const int pinMando = 16;
const int pinBotonSubirManual = 32;
const int pinBotonBajarManual = 33;
const int pinCorte = 4;
const int pinSistemaAutomaticoAnulado = 22;
const int pinLedError = 18;
const int pinTipoMotor = 14;

// Variable que controla cuanto tiempo tarda en bajar si la cadena esta subiendo y no llega arriba
const unsigned long tiempoEntreSubirYBajar = 500; // 0.5 segundos

const unsigned long tiempoMaximoTrabajo = 6000;       // 6 segundos
const unsigned long tiempoTrabajoArranque = 500;      // 0.5 segundos
const unsigned long tiempoMaximoCadenaBajada = 120000; // 2 minutos

const unsigned long tiempoMandoPresionado = 600;    // 0.6 segundos
const unsigned long tiempoIgnorarEnArranque = 1500; //  1.5 segundos
