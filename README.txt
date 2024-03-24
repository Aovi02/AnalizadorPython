Para compilar este analizador léxico, simplemente usa "make". Lo he probado en Ubuntu 20.04 LTS el 23/03/2023 y ha funcionado sin warnings y sin otros problemas.
Para ejecutarlo, usa "./main".
También he probado el programa usando valgrind con el comando "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main" y todo se libera correctamente.

En este documento voy a explicar algunas de las decisiones que he tomado a la hora de hacer el analizador, que pueden liar a primera vista.

El funcionamiento básico es que el sintáctico pide al léxico un componente léxico, lo imprime, y así hasta el final del archivo.
El sistema de entrada va a leer caracteres de los dos buffers que hemos configurado, y creo que mirando el código y leyendo los comentarios debería de ser suficiente.
Lo que sí debo explicar es el uso de posDelantero y posInicio. Están codificados de tal manera que indiquen la posición del puntero.
Así, si el puntero delantero está al principio de bloqueA, posDelantero será 0, y si está al principio del bloqueB, será TAM_BLOQUE (sea cual sea, pero es 10 en mi caso)
Los necesito porque no tengo forma de saber dónde está cada puntero de manera precisa. Aunque sepa su dirección de memoria, estos ints me permiten saber siempre donde están.
Estas posiciones me sirven para los casos en los que devolver un char implica volver al bloque anterior, puedo hacerlo más fácilmente.
posFinal tiene un nombre extraño (legacy), pero lo que indica es el final del fread que ha hecho. Un fread siempre leerá TAM_BLOQUE chars SALVO si estamos al final del archivo.
En ese caso, leerá menos, y se usa ese hecho para meter un EOF antes de donde se correspondería en el buffer para evitar leer basura.
Este EOF es el único que se llegará a procesar por el léxico porque en el código de siguienteCaracter, cada vez que delantero encuentra un EOF salta al siguiente char, salvo en esa instancia.
La variable esBloqueA es más bien legacy. Podría comprobar que posDelantero fuese 0 para saber si delantero está en bloqueA, pero en su momento lo hice con este booleano y por necesidades de tiempo no lo pude cambiar.

Lo otro que tengo que explicar es la variable lexemaSigue, que es la que puede confundir también.
Es un booleano que indica si el lexema sigue. El analizador léxico es quien le dice al sistema de entrada si este lexema sigue, porque el SE no tiene ni idea de cuándo acabar.
Si el lexema sigue, me sirve para contar caracteres leidos y evitar desbordamiento del lexema. Además me permite mover el puntero de inicio junto al delantero sin esperar a que se devuelva caracter.
Por ejemplo, en los comentarios, el lexema manipula este valor para decirle al SE que no hay lexema, y entonces el SE le va a dar caracteres que no contará, de esta forma ignorándolos.
Pero para un número o cadena alfanumérica, está a 1 porque el SE necesita contar esos caracteres.

La variable del SE de noQuitar sirve para indicar que no se quiten ninguno de los bloques actuales.
Si por ejemplo tengo la cadena "import " y el espacio está en la primera posición de bloqueB y el resto estában en bloqueA, al volver atrás no puedo borrar el bloqueB.
Uso este flag justo para indicar que no debo borrar ese bloque, porque coloco el delantero sobre el EOF del bloque anterior, y entonces en la siguiente lectura intentaría borrarlo.

El árbol que constituye la tabla de símbolos es binario y sus funciones son muy básicas. Creo que se entienden bien.

Hay una cierta decisión de diseño que en su momento tomé y entiendo que pueda estar mal, y es la del ComponenteLexico. Se puede ver que tiene los punteros a izq y der porque es un nodo de un árbol.
Obivamente el sintáctico no necesita esos punteros, pero podría acceder a ellos.
Además, la decisión de devolver puramente ComponenteLexico hace que el código sea legible, pero también implica que tengo que hacer un malloc y un free por cada componente lexico que aparece que no esté en la TS.
Por ejemplo un "." es un ComponenteLexico pero no está en la TS, por lo que el sintáctico se tiene que encargar de limpiar esa variable y también tiene que verificar que no sea una palabra reservada o ID para hacerlo.
Si bien intenté mantener la transparencia entre las partes, hay detalles que se tuvieron que exponer para que funcione y lo tengo en mente, pero no puedo cambiarlo por falta de tiempo.
Lo bueno es que no creo copias de los componentes, todos son únicos y no se van a repetir.

El lexico.c lee varios caracteres antes de meterse en un autómata para comprobar qué tipos de números hay. Es la parte más complicada del autómata porque hay que considerar hexadecimales y exponenciales.
La idea es leer caracteres de forma preventiva para leer las "e", o "x" y decidir qué tipo de número es. Por ese motivo, al iniciar el autómata de enteros o de floats tengo que indicarle dónde empezar, porque ya habré
guardado unos cuantos caracteres de antemano, y volver atrás múltiples caracteres me parecía demasiado (es el argumento llamado start).

Para el resto de casos, como el SE devuelve NULL si el lexema es demasiado grande, todos los autómatas ignorarán caracteres válidos hasta devolver.
Como es de entender, el argumento de lexemaSigue está a 0 cuando se hace eso, por eso digo que es como indicador para ignorar caracteres.
Por ejemplo, si tengo la palabra "import" y TAM_BLOQUE es 3, se lee hasta "imp" y el resto se ignoran aunque sean válidos con lexemaSigue = 0. Luego se imprime "imp" y ya está.
Por lo tanto, ante un lexema demasiado grande, imprimo siempre el principio.

El resto de la funcionalidad considero que es bastante intuitiva de entender.

