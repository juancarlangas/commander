# Propiedad de la interfaz ncurses

`main()` crea una única instancia de `Screen` antes de utilizar la interfaz.
`Screen` inicia ncurses, configura los colores y crea las ventanas principales.
También contiene la instancia de `Orchestra`, que antes era global.

El orden de cierre es:

1. `Screen` libera las ventanas y los paneles de la pantalla principal.
2. Se destruye `Orchestra` y sus componentes liberan sus propios recursos.
3. Se destruye `NcursesSession` y se restaura el terminal mediante `endwin()`.

El orden de los miembros de `Screen` es intencional: `session_` se declara antes
que `orchestra_` para que se destruya después. No se debe llamar a `endwin()`
antes de que se destruyan los objetos de la interfaz durante el cierre normal.
Las llamadas existentes a `exit()` no ejecutan destructores de objetos locales;
convertir esas salidas de error queda pendiente.

`Window`, `Popup`, `BoxedWindow`, `BoxedPopup`, `Field` y `Form` liberan los
recursos que poseen. Los paneles se eliminan antes que sus ventanas. No se
pueden copiar los objetos que poseen recursos. `Window` tiene un destructor
virtual para que destruir una instancia derivada a través de la clase base
libere también sus paneles y bordes.

Los métodos `init()` y `Field::create()` permiten reconstruir los componentes
sin acumular las ventanas anteriores. Las coordenadas, los colores y los
controles existentes se mantienen.

## Migración pendiente

Los punteros `extern WINDOW*` y `extern PANEL*` de `ncurses.hpp` se conservan
como acceso para el código de impresión y los diálogos antiguos. `Screen`
controla su creación y liberación; esos consumidores no deben liberarlos ni
conservar copias entre redimensionamientos. Esta etapa no permite varias
pantallas simultáneas.

El siguiente paso puede ser migrar un componente pequeño (por ejemplo, el
indicador MIDI) a un miembro privado de `Screen` y retirar su puntero global.
No es necesario reescribir todos los componentes en una sola entrega.

## Validación

Con las dependencias de desarrollo disponibles:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

`ui_lifecycle` utiliza ncurses real con salida a archivos temporales, sin
necesitar un terminal interactivo. Comprueba reinicialización repetida,
destrucción por la clase base, formularios y liberación de paneles antes que
ventanas, detectando recursos que queden sin liberar. Usa las opciones
`--wrap` del enlazador GNU para observar las operaciones de ncurses.

Para comprobar la integración, arrancar Commander con datos válidos, cambiar
el tamaño del terminal, abrir y cerrar la orquestación y el formulario de
etiquetas, buscar en el catálogo y salir con Ctrl-Q.
