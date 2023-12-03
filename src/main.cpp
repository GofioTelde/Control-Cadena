#include <Arduino.h>
#include <EasyButton.h>
#include <elapsedMillis.h>

#include "constants.h"

// Variables globales
bool error = false;
bool cadenaBajada = false;
bool esMotorMonofasico = false; // Indica si el motor es trifasico o monofasico
bool cadenaSubiendo = false;    // Indica si el motor esta encendido o apagado
bool cadenaBajando = false;     // Indica si el motor esta encendido o apagado
bool ledErrorEncendido = false;
int tipoError = 0; // 0: no hay error, 1: error de motor, 2: error de fines de carrera

// Crear los botones
EasyButton mando(pinMando, 35, false, false);
EasyButton botonSubirManual(pinBotonSubirManual, 35, false, false);
EasyButton botonBajarManual(pinBotonBajarManual, 35, false, false);
EasyButton finCarreraArriba(pinFinCarreraArriba, 35, false, false);
EasyButton finCarreraAbajo(pinFinCarreraAbajo, 35, false, false);
EasyButton sistemaAutomaticoAnuladoEB(pinSistemaAutomaticoAnulado, 35, false, false);
EasyButton corte(pinCorte, 35, false, true);

elapsedMillis tiempoTranscurridoTrabajo; // Reemplaza 'unsigned long tiempoInicio'
elapsedMillis tiempoCadenaBajada;        // Para el tiempo que la cadena ha estado bajada

// Duration.
int duration = 1000;

/**
 * Comprueba la posicion de la cadena.
 *
 * @return true si la cadena no esta ni arriba ni abajo.
 */
bool cadenaEnLimbo()
{
  return finCarreraAbajo.isReleased() && finCarreraArriba.isReleased();
}

/**
 * Comprueba si la cadena esta en movimiento.
 *
 * @return true si la cadena esta en movimiento.
 */
bool cadenaEnMovimiento()
{
  return cadenaSubiendo || cadenaBajando;
}

void apagarMotor()
{
  digitalWrite(pinArranqueArriba, LOW);
  digitalWrite(pinArranqueAbajo, LOW);
  cadenaBajando = false;
  cadenaSubiendo = false;
}

void arrancarMotor(bool haciaArriba, String mensaje)
{
  Serial.println(mensaje);
  cadenaBajada = false;
  if (haciaArriba)
  {
    digitalWrite(pinArranqueArriba, HIGH);
    digitalWrite(pinArranqueAbajo, LOW);
    cadenaBajando = false;
    cadenaSubiendo = true;
  }
  else
  {
    digitalWrite(pinArranqueArriba, LOW);
    digitalWrite(pinArranqueAbajo, HIGH);
    cadenaBajando = true;
    cadenaSubiendo = false;
  }
  tiempoTranscurridoTrabajo = 0;
}

/**
 * Función encargada de gestionar los errores del sistema.
 * Comprueba diferentes condiciones y toma acciones correspondientes en caso de error.
 */
void gestionErrores()
{
  // la cadena no esta en movimiento y esta en limbo
  if (cadenaEnLimbo() && !cadenaEnMovimiento() && !error)
  {
    Serial.println("La cadena esta en limbo");
    apagarMotor();
    error = true;
    tipoError = 2;
  }

  // el corte esta activado
  if (corte.isPressed() && !error)
  {
    Serial.println("Corte de emergencia");
    apagarMotor();
    error = true;
    tipoError = 2;
  }

  // el led de error se enciende si hay un error
  if (error && !ledErrorEncendido)
  {
    digitalWrite(pinLedError, HIGH);
    ledErrorEncendido = true;
  }
  else if (!error && ledErrorEncendido)
  {
    digitalWrite(pinLedError, LOW);
    ledErrorEncendido = false;
  }

  // quitamos el error si el fin de carrera esta pulsado y el tipo de error es de fin de carrera
  if ((error && tipoError == 2) && !cadenaEnLimbo())
  {
    apagarMotor();
    error = false;
    tipoError = 0;
    if (finCarreraAbajo.isPressed())
    {
      cadenaBajada = true;
      tiempoCadenaBajada = 0;
    }
  }
}

// Callback.
void onMandoPressedForDuration()
{
  Serial.println("Mando pulsado por 1000");
  if (error)
  {
    Serial.println("Hay un error");
    return;
  }
  else
  {
    if (finCarreraAbajo.isPressed())
    {
      Serial.println("Subiendo la cadena");
      arrancarMotor(true, "Subiendo la cadena, desde onMandoPressedForDuration");
    }
    else if (finCarreraArriba.isPressed())
    {
      Serial.println("Bajando la cadena");
      arrancarMotor(false, "Bajando la cadena, desde onMandoPressedForDuration");
    }
  }
}

void sistemaAtutomatico()
{
  if (tiempoCadenaBajada > tiempoMaximoCadenaBajada && cadenaBajada && sistemaAutomaticoAnuladoEB.isReleased() && !error)
  {
    arrancarMotor(true, "Sistema automatico");
    cadenaBajada = false;
  }
}

void setup()
{
  Serial.begin(115200);

  // motor
  pinMode(pinArranqueArriba, OUTPUT);
  pinMode(pinArranqueAbajo, OUTPUT);
  pinMode(pinLedError, OUTPUT);

  // Inicializar los botones
  mando.begin();
  finCarreraAbajo.begin();
  finCarreraArriba.begin();
  botonBajarManual.begin();
  botonSubirManual.begin();
  sistemaAutomaticoAnuladoEB.begin();

  mando.onPressedFor(duration, onMandoPressedForDuration);
  Serial.println("Setup finalizado");

  if (cadenaEnLimbo())
  {
    arrancarMotor(false, "Desde SetUp");
  }
  if (finCarreraAbajo.isPressed() && finCarreraArriba.isReleased())
  {
    cadenaBajada = true;
    tiempoCadenaBajada = 0;
  }
}

void loop()
{
  mando.read();
  finCarreraAbajo.read();
  finCarreraArriba.read();
  sistemaAutomaticoAnuladoEB.read();
  botonBajarManual.read();
  botonSubirManual.read();
  corte.read();

  sistemaAtutomatico();

  gestionErrores();

  // tiempo de trabajo excedido
  if (tiempoTranscurridoTrabajo > tiempoMaximoTrabajo && cadenaEnMovimiento())
  {
    if (cadenaSubiendo)
    {
      arrancarMotor(false, "Tiempo de trabajo excedido");
    }
    else if (cadenaBajando)
    {
      Serial.println("El motor se ha parado");
      apagarMotor();
    }
  }

  if (finCarreraAbajo.isPressed() && cadenaBajando)
  {
    Serial.println("La cadena llegó abajo");
    cadenaBajada = true;
    tiempoCadenaBajada = 0;
    apagarMotor();
  }
  else if (finCarreraArriba.isPressed() && cadenaSubiendo)
  {
    Serial.println("La cadena llegó arriba");
    apagarMotor();
  }

  if (botonBajarManual.isPressed() && error)
  {
    Serial.println("Bajando la cadena manualmente");
    arrancarMotor(false, "Bajando la cadena manualmente");

    while (botonBajarManual.isPressed())
    {
      botonBajarManual.read();
    }
    apagarMotor();
  }
  if (botonSubirManual.isPressed() && error)
  {
    Serial.println("Subiendo la cadena manualmente");
    arrancarMotor(true, "Subiendo la cadena manualmente");
    while (botonSubirManual.isPressed())
    {
      botonSubirManual.read();
    }
    apagarMotor();
  }
}