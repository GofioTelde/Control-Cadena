# Documentación del código de main.cpp

El código en `main.cpp` es un programa de Arduino que controla un motor y una cadena.

## Variables globales

- `bool error`: Indica si ha ocurrido un error.
- `bool buttonPressed`: Indica si se ha presionado un botón.
- `bool accionRealizada`: Indica si se ha realizado una acción.
- `bool isTrifasico`: Indica si el motor es trifásico.
- `bool cadenaSubiendo`: Indica si la cadena está subiendo.
- `bool cadenaBajando`: Indica si la cadena está bajando.
- `bool cadenaBajada`: Indica si la cadena ha bajado.
- `bool ledErrorEncendido`: Indica si el LED de error está encendido.

## Funciones

### `bool cadenaEnMovimiento()`

Esta función devuelve `true` si la cadena está subiendo o bajando.

### `EstadoCadena obtenerEstadoCadena()`

Esta función lee los pines de los fines de carrera y devuelve el estado de la cadena.

### `void apagarMotor()`

Esta función apaga el motor y establece `cadenaSubiendo` y `cadenaBajando` en `false`.

### `void arrancarMotor(bool haciaArriba)`

Esta función arranca el motor en la dirección especificada por `haciaArriba`.

### `void gestionMando()`

Esta función gestiona la funcionalidad de mantener el mando pulsado.

### `void leerFinesCarrera()`

Esta función lee los fines de carrera y realiza acciones en función de su estado.

### `void setup()`

Esta función se ejecuta una vez al inicio del programa. Inicializa los pines y arranca el motor si es necesario.

### `void arrancarMotorError(bool haciaArriba)`

Esta función arranca el motor en la dirección especificada por `haciaArriba` en caso de error.

### `void gestionarArranqueManual()`

Esta función gestiona el arranque manual del motor en caso de error.

### `void loop()`

Esta es la función principal del programa. Se ejecuta continuamente y gestiona el estado del motor y de la cadena.


# Documentación del código de EstadoCadena.h

El archivo `EstadoCadena.h` define un `enum` llamado `EstadoCadena`. Este `enum` se utiliza para representar el estado de una cadena en el programa.

## Enumeración EstadoCadena

La enumeración `EstadoCadena` tiene cuatro posibles valores:

- `ARRIBA`: Este valor indica que la cadena está en la posición superior.
- `ABAJO`: Este valor indica que la cadena está en la posición inferior.
- `AMBOS`: Este valor indica que ambos fines de carrera están activados. Esto podría indicar un error o una situación anómala.
- `NINGUNO`: Este valor indica que ninguno de los fines de carrera está activado. Esto podría indicar que la cadena está en movimiento.

El `enum` se utiliza en el programa para controlar el movimiento de la cadena y para tomar decisiones basadas en el estado actual de la cadena.

# Documentación del código de constants.h

El archivo `constants.h` define varias constantes que se utilizan en todo el programa.

## Constantes de pines GPIO

Las siguientes constantes definen los números de los pines GPIO que se utilizan para controlar el motor, los botones, los fines de carrera y otros componentes:

- `pinMotorTrabajo`: El pin que controla el motor de tipo Monofasico.
- `pinArranqueArriba`: El pin que se utiliza para arrancar el motor hacia arriba en Monofasico, y en Trifasico, no necesita de un pin de trabajo por lo que tambien funciona como Trabajo.
- `pinArranqueAbajo`: El pin que se utiliza para arrancar el motor hacia abajo en Monofasico, y en Trifasico, no necesita de un pin de trabajo por lo que tambien funciona como Trabajo.
- `pinFinCarreraArriba`: El pin que se utiliza para detectar el fin de carrera superior.
- `pinFinCarreraAbajo`: El pin que se utiliza para detectar el fin de carrera inferior.
- `pinMando`: El pin que se utiliza para el mando.
- `pinBotonSubirManual`: El pin que se utiliza para el botón de subida manual.
- `pinBotonBajarManual`: El pin que se utiliza para el botón de bajada manual.
- `pinCorte`: El pin que se utiliza para el corte.
- `pinSistemaAutomaticoAnulado`: El pin que se utiliza para anular el sistema automático de subida de cadena en caso de llevar X tiempo en el suelo.
- `pinLedError`: El pin que se utiliza para el LED de error.
- `pinTipoMotor`: El pin que se utiliza para determinar el tipo de motor.

## Constantes de tiempo

Las siguientes constantes definen varios tiempos que se utilizan en el programa:

- `tiempoEntreSubirYBajar`: El tiempo que se espera entre subir y bajar si la cadena está subiendo y no llega arriba.
- `tiempoMaximoTrabajo`: El tiempo máximo que el motor puede trabajar de forma continua.
- `tiempoTrabajoArranque`: El tiempo que el motor trabaja al arrancar.
- `tiempoMaximoCadenaBajada`: El tiempo máximo que se espera para que la cadena baje.
- `tiempoMandoPresionado`: El tiempo que se espera para considerar que el mando está presionado.
- `tiempoIgnorarEnArranque`: El tiempo que se ignora después de arrancar el motor.
