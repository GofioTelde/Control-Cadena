#include <Arduino.h>       // Librería principal de Arduino para manejar pines, comunicación serial, etc.
#include <elapsedMillis.h> // Librería que permite rastrear el tiempo transcurrido fácilmente

#include "constants.h"    // Incluye constantes definidas externamente
#include "EstadoCadena.h" // Incluye definiciones y estados de la cadena
#include "EstadoMotor.h"  // Incluye definiciones y estados del motor

// Variables globales de estado y configuración
bool error = false;           // Variable para indicar si hay un error
bool buttonPressed = false;   // Indica si el botón ha sido presionado
bool accionRealizada = false; // Indica si se ha realizado una acción basada en la pulsación del botón

// Configuración del tipo de motor
bool isTrifasico = true; // true = trifásico, false = monofásico

// Estado de la cadena
bool cadenaBajada = false; // Indica si la cadena está en posición de bajada

// Estado del LED de error
bool ledErrorEncendido = false; // Variable para controlar el estado del LED de error

// Objetos de elapsedMillis para rastrear tiempos específicos
elapsedMillis mandoPulsado;         // Tiempo desde la última vez que se pulsó el mando
elapsedMillis tiempoMotorEncendido; // Tiempo desde que el motor comenzó a funcionar
elapsedMillis tiempoCadenaBajada;   // Tiempo desde que la cadena está en bajada

// Variable para almacenar el estado actual del motor
EstadoMotor estadoMotor;

// Función para obtener el estado de la cadena en función de los sensores de fin de carrera
EstadoCadena obtenerEstadoCadena()
{
  bool finCarreraArriba = digitalRead(pinFinCarreraArriba) == HIGH; // Verifica si el sensor de fin de carrera arriba está activado
  bool finCarreraAbajo = digitalRead(pinFinCarreraAbajo) == HIGH;   // Verifica si el sensor de fin de carrera abajo está activado

  // Retorna el estado correspondiente según los sensores de fin de carrera
  if (finCarreraArriba && finCarreraAbajo)
  {
    return AMBOS; // Ambos sensores están activados
  }
  else if (finCarreraArriba)
  {
    return ARRIBA; // Solo el sensor de arriba está activado
  }
  else if (finCarreraAbajo)
  {
    return ABAJO; // Solo el sensor de abajo está activado
  }
  else
  {
    return NINGUNO; // Ningún sensor está activado
  }
}

// Función para apagar el motor y establecer su estado en APAGADO
void apagarMotor()
{
  digitalWrite(pinArranqueArriba, LOW); // Apaga el pin de arranque hacia arriba
  digitalWrite(pinArranqueAbajo, LOW);  // Apaga el pin de arranque hacia abajo
  digitalWrite(pinMotorTrabajo, LOW);   // Apaga el pin de trabajo del motor
  estadoMotor = APAGADO;                // Cambia el estado del motor a APAGADO
}

// Tiempo de arranque del motor
elapsedMillis tiempoArranqueMotor;

// Función para arrancar el motor en una dirección específica
void arrancarMotor(bool haciaArriba)
{
  cadenaBajada = false; // Marca que la cadena ya no está en bajada
  if (haciaArriba)
  {
    Serial.println("Arrancando motor hacia arriba"); // Mensaje indicando arranque hacia arriba
    digitalWrite(pinArranqueArriba, HIGH);           // Activa el pin de arranque en dirección arriba
    digitalWrite(pinArranqueAbajo, LOW);             // Asegura que el pin de arranque en dirección abajo esté desactivado
    if (!isTrifasico)
    {
      digitalWrite(pinMotorTrabajo, HIGH); // Activa el pin de trabajo si el motor es monofásico
    }
    estadoMotor = SUBIENDO; // Actualiza el estado del motor a SUBIENDO
  }
  else
  {
    Serial.println("Arrancando motor hacia abajo"); // Mensaje indicando arranque hacia abajo
    digitalWrite(pinArranqueArriba, LOW);           // Asegura que el pin de arranque en dirección arriba esté desactivado
    digitalWrite(pinArranqueAbajo, HIGH);           // Activa el pin de arranque en dirección abajo
    if (!isTrifasico)
    {
      digitalWrite(pinMotorTrabajo, HIGH); // Activa el pin de trabajo si el motor es monofásico
    }
    estadoMotor = BAJANDO; // Actualiza el estado del motor a BAJANDO
  }
  tiempoMotorEncendido = 0; // Resetea el temporizador del motor encendido
  delay(400);               // Pausa breve para evitar ruidos de los relés
}

// Función para gestionar la pulsación del mando y ejecutar acciones en consecuencia
void gestionMando()
{
  if (digitalRead(pinMando) == HIGH) // Si el pin de mando está en alto, indica pulsación
  {
    if (!buttonPressed) // Si el botón no estaba registrado como pulsado previamente
    {
      buttonPressed = true;    // Marca el botón como pulsado
      mandoPulsado = 0;        // Resetea el tiempo transcurrido desde la última pulsación
      accionRealizada = false; // Resetea la acción realizada para permitir una nueva acción
    }
  }
  else
  {
    buttonPressed = false; // Marca el botón como no pulsado si el pinMando está en LOW
  }

  // Ejecuta acción si el mando ha estado presionado el tiempo necesario
  if (mandoPulsado > tiempoMandoPresionado && buttonPressed && !accionRealizada)
  {
    EstadoCadena estadoCadena = obtenerEstadoCadena(); // Obtiene el estado actual de la cadena

    Serial.println("Botón pulsado durante más de 600ms"); // Indica que el botón fue pulsado suficientemente

    if (estadoCadena == ARRIBA) // Si la cadena está en la posición ARRIBA
    {
      Serial.println("Cadena arriba");
      arrancarMotor(false); // Arranca el motor hacia abajo
    }
    else if (estadoCadena == ABAJO) // Si la cadena está en la posición ABAJO
    {
      Serial.println("Cadena abajo");
      arrancarMotor(true); // Arranca el motor hacia arriba
    }
    accionRealizada = true; // Marca que la acción ha sido realizada
  }
}

// Función para leer y procesar los sensores de fin de carrera
void leerFinesCarrera()
{
  EstadoCadena estadoCadena = obtenerEstadoCadena(); // Obtiene el estado actual de la cadena

  // Nueva condición de error: ambos fines de carrera activados
  if (estadoCadena == AMBOS)
  {
    Serial.println("Error: Ambos fines de carrera activados"); // Mensaje de error
    error = true;                                              // Activa el estado de error
    digitalWrite(pinLedError, HIGH);                           // Enciende el LED de error
    ledErrorEncendido = true;                                  // Marca el LED de error como encendido
    apagarMotor();                                             // Apaga el motor
  }

  // Condición de fin de carrera ARRIBA activado y motor subiendo
  if (estadoCadena == ARRIBA && estadoMotor == SUBIENDO)
  {
    Serial.println("Fin de carrera arriba");
    apagarMotor(); // Apaga el motor si la cadena está arriba y el motor subiendo
  }

  // Condición de fin de carrera ABAJO activado y motor bajando
  if (estadoCadena == ABAJO && estadoMotor == BAJANDO)
  {
    Serial.println("Fin de carrera abajo");
    cadenaBajada = true;    // Marca que la cadena está en posición de bajada
    tiempoCadenaBajada = 0; // Resetea el temporizador de cadena bajada
    apagarMotor();          // Apaga el motor si la cadena está abajo y el motor bajando
  }
}

// Función para verificar si el error se ha subsanado
void verificarError()
{
  // Verifica si el error se puede subsanar:
  // El pinCorte debe estar en HIGH, y solo uno o ninguno de los sensores de fin de carrera debe estar activo
  if (digitalRead(pinCorte) == HIGH && obtenerEstadoCadena() != AMBOS)
  {
    error = false;                  // Desactiva el estado de error
    digitalWrite(pinLedError, LOW); // Apaga el LED de error
    ledErrorEncendido = false;      // Marca el LED como apagado
  }
}

// Configuración inicial de pines y estado en el setup
void setup()
{
  Serial.begin(115200); // Inicializa la comunicación serial a 115200 baudios

  // Configura los pines de entrada con resistencia de pull-down
  pinMode(pinMando, INPUT_PULLDOWN);
  pinMode(pinFinCarreraArriba, INPUT_PULLDOWN);
  pinMode(pinFinCarreraAbajo, INPUT_PULLDOWN);
  pinMode(pinSistemaAutomaticoAnulado, INPUT_PULLDOWN);
  pinMode(pinCorte, INPUT_PULLDOWN);
  pinMode(pinBotonSubirManual, INPUT_PULLDOWN);
  pinMode(pinBotonBajarManual, INPUT_PULLDOWN);
  pinMode(pinTipoMotor, INPUT_PULLDOWN);

  // Configura los pines de salida
  pinMode(pinMotorTrabajo, OUTPUT);
  pinMode(pinArranqueArriba, OUTPUT);
  pinMode(pinArranqueAbajo, OUTPUT);
  pinMode(pinLedError, OUTPUT);

  // Lee el tipo de motor al inicio
  isTrifasico = digitalRead(pinTipoMotor) == HIGH ? false : true;
  Serial.println("Finalizado Setup");

  // Si no se detecta ningún fin de carrera, el motor baja
  if (obtenerEstadoCadena() == NINGUNO)
  {
    arrancarMotor(false);
  }
}

// Manejo de arranque manual en caso de error
elapsedMillis tiempoArranqueManual;

void arrancarMotorError(bool haciaArriba)
{
  if (haciaArriba)
  {
    Serial.println("Arrancando motor hacia arriba"); // Mensaje de arranque hacia arriba
    digitalWrite(pinArranqueArriba, HIGH);           // Activa arranque en dirección arriba
    digitalWrite(pinArranqueAbajo, LOW);             // Asegura que el pin de arranque hacia abajo esté desactivado
    tiempoArranqueManual = 0;                        // Resetea el temporizador de arranque manual
    if (!isTrifasico)
    {
      digitalWrite(pinMotorTrabajo, HIGH); // Activa el pin de trabajo si el motor es monofásico
    }
  }
  else
  {
    Serial.println("Arrancando motor hacia abajo"); // Mensaje de arranque hacia abajo
    digitalWrite(pinArranqueArriba, LOW);           // Asegura que el pin de arranque hacia arriba esté desactivado
    digitalWrite(pinArranqueAbajo, HIGH);           // Activa arranque en dirección abajo
    tiempoArranqueManual = 0;
    if (!isTrifasico)
    {
      digitalWrite(pinMotorTrabajo, HIGH);
    }
  }

  if (!isTrifasico)
  {
    // Mientras esté presionado el botón y el tiempo de trabajo no se exceda
    while ((digitalRead(pinBotonSubirManual) == HIGH || digitalRead(pinBotonBajarManual) == HIGH) && tiempoArranqueManual < tiempoTrabajoArranque)
    {
      // Espera hasta que termine el tiempo de trabajo
    }
    digitalWrite(pinArranqueArriba, LOW); // Apaga el arranque arriba
    digitalWrite(pinArranqueAbajo, LOW);  // Apaga el arranque abajo
  }
}

// Función que gestiona el arranque manual utilizando botones de subida y bajada
void gestionarArranqueManual()
{
  if (digitalRead(pinBotonSubirManual) == HIGH)
  {
    arrancarMotorError(true); // Arranca el motor hacia arriba en caso de error
    while (digitalRead(pinBotonSubirManual) == HIGH)
    {
    }              // Espera a que se suelte el botón
    apagarMotor(); // Apaga el motor una vez soltado el botón
  }
  else if (digitalRead(pinBotonBajarManual) == HIGH)
  {
    arrancarMotorError(false); // Arranca el motor hacia abajo en caso de error
    while (digitalRead(pinBotonBajarManual) == HIGH)
    {
    }              // Espera a que se suelte el botón
    apagarMotor(); // Apaga el motor una vez soltado el botón
  }
}

// Bucle principal de ejecución del código
void loop()
{
  if (digitalRead(pinCorte) == LOW) // Si el pinCorte está bajo, indica un estado de error
  {
    apagarMotor();          // Apaga el motor en caso de error
    error = true;           // Activa el estado de error
    if (!ledErrorEncendido) // Si el LED de error no está encendido
    {
      digitalWrite(pinLedError, HIGH); // Enciende el LED de error
      ledErrorEncendido = true;        // Marca el LED de error como encendido
    }
  }

  if (error) // Si el estado de error está activo
  {
    gestionarArranqueManual(); // Permite el arranque manual en estado de error
    verificarError();          // Verifica si el error se ha resuelto
  }
  else // Si no hay error activo
  {
    if (ledErrorEncendido) // Si el LED de error está encendido
    {
      digitalWrite(pinLedError, LOW);     // Apaga el LED de error
      ledErrorEncendido = false;          // Marca el LED de error como apagado
      if (obtenerEstadoCadena() == ABAJO) // Si la cadena está abajo
      {
        tiempoCadenaBajada = 0; // Resetea el temporizador de cadena bajada
        cadenaBajada = true;    // Marca que la cadena está en posición de bajada
      }
    }

    gestionMando(); // Gestiona el mando para posibles acciones de usuario

    EstadoCadena estadoCadena = obtenerEstadoCadena(); // Obtiene el estado actual de la cadena

    // Si el motor está subiendo o bajando, el motor no es trifásico y se excede el tiempo de arranque
    if ((estadoMotor == SUBIENDO || estadoMotor == BAJANDO) && !isTrifasico && tiempoMotorEncendido > tiempoTrabajoArranque)
    {
      digitalWrite(pinArranqueAbajo, LOW);  // Desactiva el arranque hacia abajo
      digitalWrite(pinArranqueArriba, LOW); // Desactiva el arranque hacia arriba
    }

    // Si la cadena está en estado NINGUNO y el motor está apagado, entra en error
    if (estadoCadena == NINGUNO && estadoMotor == APAGADO)
    {
      error = true;                    // Activa el estado de error
      digitalWrite(pinLedError, HIGH); // Enciende el LED de error
      ledErrorEncendido = true;
    }

    // Verifica si la cadena ha estado en posición de bajada demasiado tiempo sin activación manual
    if (estadoCadena == ABAJO && estadoMotor == APAGADO && tiempoCadenaBajada > tiempoMaximoCadenaBajada && digitalRead(pinSistemaAutomaticoAnulado) == LOW)
    {
      Serial.println("Tiempo cadena bajada excedido, subiendo automaticamente");
      arrancarMotor(true); // Arranca el motor hacia arriba automáticamente
    }

    // Verifica si se excede el tiempo máximo de trabajo del motor
    if (tiempoMotorEncendido > tiempoMaximoTrabajo && !error && (estadoMotor == SUBIENDO || estadoMotor == BAJANDO))
    {
      if (estadoMotor == SUBIENDO)
      {
        Serial.println("Tiempo máximo de trabajo alcanzado, bajando automaticamente");
        apagarMotor();                 // Apaga el motor
        delay(tiempoEntreSubirYBajar); // Pausa breve
        arrancarMotor(false);          // Inicia bajada automáticamente
      }
      else
      {
        Serial.println("Tiempo máximo de trabajo alcanzado");
        error = true;                    // Activa el estado de error
        digitalWrite(pinLedError, HIGH); // Enciende el LED de error
        ledErrorEncendido = true;
        apagarMotor(); // Apaga el motor si estaba bajando
      }
    }
  }

  leerFinesCarrera(); // Verifica los sensores de fin de carrera
}
