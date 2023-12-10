#include <Arduino.h>
#include <elapsedMillis.h>

#include "constants.h"
#include "EstadoCadena.h"
#include "EstadoMotor.h"

bool error = false;
bool buttonPressed = false; // Añade esta línea
bool accionRealizada = false;

// tipo Motor
bool isTrifasico = true;

// cadena
bool cadenaBajada = false;

// error
bool ledErrorEncendido = false;

elapsedMillis mandoPulsado;         // Tiempo transcurrido desde la última vez que el pinMando fue pulsado
elapsedMillis tiempoMotorEncendido; // Tiempo transcurrido desde que se arrancó el motor
elapsedMillis tiempoCadenaBajada;

EstadoMotor estadoMotor;

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
  digitalWrite(pinMotorTrabajo, LOW);
  estadoMotor = APAGADO;
}

elapsedMillis tiempoArranqueMotor;
void arrancarMotor(bool haciaArriba)
{
  cadenaBajada = false;
  if (haciaArriba)
  {
    Serial.println("Arrancando motor hacia arriba");
    digitalWrite(pinArranqueArriba, HIGH);
    digitalWrite(pinArranqueAbajo, LOW);
    if (!isTrifasico)
    {
      digitalWrite(pinMotorTrabajo, HIGH);
    }

    estadoMotor = SUBIENDO;
  }
  else
  {
    Serial.println("Arrancando motor hacia abajo");
    digitalWrite(pinArranqueArriba, LOW);
    digitalWrite(pinArranqueAbajo, HIGH);
    if (!isTrifasico)
    {
      digitalWrite(pinMotorTrabajo, HIGH);
    }
    estadoMotor = BAJANDO;
  }
  tiempoMotorEncendido = 0;
  delay(400); // tiempo para ignorar ruido de reles.
}

// Controla la funcionalidad de mantener el mando pulsado
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
  if (estadoCadena == ARRIBA || estadoCadena == ABAJO && digitalRead(pinCorte) == HIGH)
  {
    error = false;
  }
  if (estadoCadena == ARRIBA && estadoMotor == SUBIENDO)
  {
    Serial.println("Fin de carrera arriba");
    apagarMotor();
  }
  if (estadoCadena == ABAJO && estadoMotor == BAJANDO)
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
  pinMode(pinTipoMotor, INPUT_PULLDOWN);

  pinMode(pinMotorTrabajo, OUTPUT);
  pinMode(pinArranqueArriba, OUTPUT);
  pinMode(pinArranqueAbajo, OUTPUT);
  pinMode(pinLedError, OUTPUT);

  isTrifasico = digitalRead(pinTipoMotor) == HIGH ? false : true;
  Serial.println("Finalizado Setup");

  if (obtenerEstadoCadena() == NINGUNO)
  {
    arrancarMotor(false);
  }
}

elapsedMillis tiempoArranqueManual;
void arrancarMotorError(bool haciaArriba)
{
  if (haciaArriba)
  {
    Serial.println("Arrancando motor hacia arriba");
    digitalWrite(pinArranqueArriba, HIGH);
    digitalWrite(pinArranqueAbajo, LOW);
    tiempoArranqueManual = 0;
    if (!isTrifasico)
    {
      digitalWrite(pinMotorTrabajo, HIGH);
    }
  }
  else
  {
    Serial.println("Arrancando motor hacia abajo");
    digitalWrite(pinArranqueArriba, LOW);
    digitalWrite(pinArranqueAbajo, HIGH);
    tiempoArranqueManual = 0;
    if (!isTrifasico)
    {
      digitalWrite(pinMotorTrabajo, HIGH);
    }
  }

  if (!isTrifasico)
  {
    while (
        (digitalRead(pinBotonSubirManual) == HIGH || digitalRead(pinBotonBajarManual) == HIGH) &&
        tiempoArranqueManual < tiempoTrabajoArranque)
    {
    }
    digitalWrite(pinArranqueArriba, LOW);
    digitalWrite(pinArranqueAbajo, LOW);
  }
}

void gestionarArranqueManual()
{
  if (digitalRead(pinBotonSubirManual) == HIGH)
  {
    arrancarMotorError(true);
    while (digitalRead(pinBotonSubirManual) == HIGH)
    {
    }

    apagarMotor();
  }
  else if (digitalRead(pinBotonBajarManual) == HIGH)
  {
    arrancarMotorError(false);
    while (digitalRead(pinBotonBajarManual) == HIGH)
    {
    }

    apagarMotor();
  }
}

void loop()
{
  if (digitalRead(pinCorte) == LOW)
  {
    apagarMotor();
    error = true;
    if (digitalRead(pinLedError) == LOW)
    {
      digitalWrite(pinLedError, HIGH);
    }
  }

  if (error)
  {
    if (!ledErrorEncendido)
    {
      digitalWrite(pinLedError, HIGH);
      ledErrorEncendido = true;
    }

    gestionarArranqueManual();
  }
  else
  {
    // si el led error esta encendido, lo apagamos
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

    if ((estadoMotor == SUBIENDO || estadoMotor == BAJANDO) && !isTrifasico && tiempoMotorEncendido > tiempoTrabajoArranque)
    {
      digitalWrite(pinArranqueAbajo, LOW);
      digitalWrite(pinArranqueArriba, LOW);
    }

    if (estadoCadena == NINGUNO && estadoMotor == APAGADO)
    {
      error = true;
    }

    if (estadoCadena == ABAJO && estadoMotor == APAGADO && tiempoCadenaBajada > tiempoMaximoCadenaBajada && digitalRead(pinSistemaAutomaticoAnulado) == LOW)
    {
      Serial.println("Tiempo cadena bajada excedido, subiendo automaticamente");
      arrancarMotor(true);
    }

    if (tiempoMotorEncendido > (tiempoMaximoTrabajo) && !error && (estadoMotor == SUBIENDO || estadoMotor == BAJANDO))
    {
      if (estadoMotor == SUBIENDO)
      {
        Serial.println("Tiempo máximo de trabajo alcanzado, y ademas subiendo, bajando automaticamente");
        apagarMotor();
        delay(500);
        arrancarMotor(false);
      }
      else
      {
        Serial.println("Tiempo máximo de trabajo alcanzado");
        apagarMotor();
      }
    }
  }

  leerFinesCarrera();
}
