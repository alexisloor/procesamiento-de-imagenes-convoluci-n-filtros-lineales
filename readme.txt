Procesamiento Paralelo de Imágenes PGM
Este programa en C permite aplicar un filtro de desenfoque a imágenes en formato PGM utilizando un enfoque de procesamiento paralelo con hilos.

Instrucciones de Uso

1. Compilación:

Abre una terminal en el directorio del programa.
Compila el programa utilizando el siguiente comando:

gcc -o <nombre ejecutable> imgProccess.c

por ejemplo:

gcc -o blur imgProccess.c

2. Ejecución

Ejecuta el programa recién compilado:

./blur

3. Interacción:

Ingresa el nombre de uno de los archivos de imagen PGM.
El programa aplicará un filtro de desenfoque y generará un archivo de imagen llamado output_blurred.pgm.


4. Dependencias principales
librería pthread: para gestionar los hilos.
Librería pgm.h: para lectura y escritura de imágenes en formato PGM.

