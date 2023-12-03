# Sistema de Control de Motor

Este proyecto es un sistema de control de motor diseñado para mover una cadena hacia arriba y hacia abajo. Incluye botones de control manual, un control remoto y características de seguridad, como detener el motor si ha estado funcionando durante demasiado tiempo.

## Características

- Botones de control manual
- Control remoto
- Funciones de seguridad
- Manejo de errores

## Bibliotecas Utilizadas

- [EasyButton](https://github.com/evert-arias/EasyButton)
- [elapsedMillis](https://playground.arduino.cc/Code/ElapsedMillis/)

## Cómo Utilizar

1. Conecta el motor y los botones a los pines correspondientes en tu placa Arduino. Las varibales indican que pines se usan estan en constans.h, cambialos segun tus necesidades.
2. Sube el código a tu placa Arduino.
3. Usa los botones o el control remoto para controlar el movimiento de la cadena.
