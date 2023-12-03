#include <Arduino.h>
#include <elapsedMillis.h>

#include "constants.h"
#include "EstadoCadena.h"

bool error = false;
bool buttonPressed = false; // Añade esta línea
bool accionRealizada = false;

// cadena
bool cadenaSubiendo = false;
bool cadenaBajando = false;
bool cadenaBajada = false;

// error
bool ledErrorEncendido = false;

elapsedMillis mandoPulsado;  // Tiempo transcurrido desde la última vez que el pinMando fue pulsado
elapsedMillis tiempoTrabajo; // Tiempo transcurrido desde que se arrancó el motor
elapsedMillis tiempoCadenaBajada;

bool cadenaEnMovimiento()
{
  return cadenaSubiendo || cadenaBajando;
}

EstadoCadena obtenerEstadoCadena()
{
  bool finCarreraArriba = digitalRead(pinFinCarreraArriba) == HIGH;
  bool finCarreraAbajo = digitalRead(pinFinCarreraAbajo) == HIGH;

  if (finCarreraArriba && finCarreraAbajo)
  {
    return AMBOS;
  }
  else if (finCarreraArriba)
  {
    return ARRIBA;
  }
  else if (finCarreraAbajo)
  {
    return ABAJO;
  }
  else
  {
    return NINGUNO;
  }
}

void apagarMotor()
{
  Serial.println("Apagando motor");
  digitalWrite(pinArranqueArriba, LOW);
  digitalWrite(pinArranqueAbajo, LOW);
  cadenaSubiendo = false;
  cadenaBajando = false;
}

void arrancarMotor(bool haciaArriba)
{
  cadenaBajada = false;
  if (haciaArriba)
  {
    Serial.println("Arrancando motor hacia arriba");
    digitalWrite(pinArranqueArriba, HIGH);
    digitalWrite(pinArranqueAbajo, LOW);
    cadenaBajando = false;
    cadenaSubiendo = true;
  }
  else
  {
    Serial.println("Arrancando motor hacia abajo");
    digitalWrite(pinArranqueArriba, LOW);
    digitalWrite(pinArranqueAbajo, HIGH);
    cadenaBajando = true;
    cadenaSubiendo = false;
  }
  tiempoTrabajo = 0;
}

void comprobarPosicionCadena()
{
}

/**
 *  este código realiza una acción una vez si el botón ha sido presionado
 *  durante más de tiempoMandoPresionado milisegundos
 */
void gestionMando()
{

  if (digitalRead(pinMando) == HIGH) // Si el pinMando está en HIGH
  {
    if (!buttonPressed) // Si el botón no estaba pulsado
    {
      buttonPressed = true; // Marca el botón como pulsado
      mandoPulsado = 0;     // Resetea el tiempo transcurrido
      accionRealizada = false;
    }
  }
  else
  {
    buttonPressed = false; // Marca el botón como no pulsado
  }

  if (mandoPulsado > tiempoMandoPresionado && buttonPressed && !accionRealizada) // Si han pasado más de 1000ms desde que el botón fue pulsado
  {
    EstadoCadena estadoCadena = obtenerEstadoCadena();

    Serial.println("Botón pulsado durante más de 600ms");

    if (estadoCadena == ARRIBA)
    {
      Serial.println("Cadena arriba");
      arrancarMotor(false);
    }
    else if (estadoCadena == ABAJO)
    {
      Serial.println("Cadena abajo");
      arrancarMotor(true);
    }
    accionRealizada = true;
  }
}

void leerFinesCarrera()
{
  EstadoCadena estadoCadena = obtenerEstadoCadena();
  if (estadoCadena == ARRIBA || estadoCadena == ABAJO)
  {
    error = false;
  }
  if (estadoCadena == ARRIBA && cadenaSubiendo)
  {
    Serial.println("Fin de carrera arriba");
    apagarMotor();
  }
  if (estadoCadena == ABAJO && cadenaBajando)
  {
    Serial.println("Fin de carrera abajo");
    cadenaBajada = true;
    tiempoCadenaBajada = 0;
    apagarMotor();
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(pinMando, INPUT_PULLDOWN);
  pinMode(pinFinCarreraArriba, INPUT_PULLDOWN);
  pinMode(pinFinCarreraAbajo, INPUT_PULLDOWN);
  pinMode(pinSistemaAutomaticoAnulado, INPUT_PULLDOWN);
  pinMode(pinCorte, INPUT_PULLDOWN);
  pinMode(pinBotonSubirManual, INPUT_PULLDOWN);
  pinMode(pinBotonBajarManual, INPUT_PULLDOWN);

  pinMode(pinArranqueArriba, OUTPUT);
  pinMode(pinArranqueAbajo, OUTPUT);
  pinMode(pinLedError, OUTPUT);
  Serial.println("Finalizado Setup");

  if (obtenerEstadoCadena() != ABAJO)
  {
    arrancarMotor(false);
  }
}

void loop()
{
  if (error)
  {
    if (!ledErrorEncendido)
    {
      digitalWrite(pinLedError, HIGH);
      ledErrorEncendido = true;
    }

    if (digitalRead(pinBotonSubirManual) == HIGH)
    {
      arrancarMotor(true);
      while (digitalRead(pinBotonSubirManual) == HIGH)
      {
      }

      apagarMotor();
    }
    else if (digitalRead(pinBotonBajarManual) == HIGH)
    {
      arrancarMotor(false);
      while (digitalRead(pinBotonBajarManual) == HIGH)
      {
      }

      apagarMotor();
    }
  }
  else
  {
    if (ledErrorEncendido)
    {
      digitalWrite(pinLedError, LOW);
      ledErrorEncendido = false;
      if (obtenerEstadoCadena() == ABAJO)
      {
        tiempoCadenaBajada = 0;
        cadenaBajada = true;
      }
    }
    gestionMando();

    EstadoCadena estadoCadena = obtenerEstadoCadena();

    if (estadoCadena == NINGUNO && !cadenaEnMovimiento())
    {
      error = true;
    }

    if (cadenaBajada && !cadenaEnMovimiento() && tiempoCadenaBajada > tiempoMaximoCadenaBajada && digitalRead(pinSistemaAutomaticoAnulado) == LOW)
    {
      Serial.println("Tiempo cadena bajada excedido, subiendo automaticamente");
      arrancarMotor(true);
    }
  }

  leerFinesCarrera();

  if (tiempoTrabajo > tiempoMaximoTrabajo && !error && cadenaEnMovimiento())
  {
    if (cadenaSubiendo)
    {
      Serial.println("Tiempo máximo de trabajo alcanzado, y ademas subiendo, bajando automaticamente");
      arrancarMotor(false);
    }
    else
    {
      Serial.println("Tiempo máximo de trabajo alcanzado");
      apagarMotor();
    }
  }

  if (digitalRead(pinCorte) == HIGH && !error)
  {
    error = true;
  }
}
