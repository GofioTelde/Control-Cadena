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
