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

const unsigned long tiempoMaximoTrabajo = 6500;       // 6.5 segundos
const unsigned long tiempoTrabajoArranque = 1000;     // 1 segundo
const unsigned long tiempoMaximoCadenaBajada = 10000; // 10 segundos

const unsigned long tiempoMandoPresionado = 600; // 0.6 segundos