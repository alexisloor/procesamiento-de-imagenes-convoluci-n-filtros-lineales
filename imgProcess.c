#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pgm.h"


// Estructura para pasar datos a cada hilo
typedef struct {
    byte* inputImage;
    byte* outputImage;
    int width;
    int height;
    int startY;
    int endY;
} ThreadData;

// Función para aplicar el filtro de desenfoque a una sub-imagen
void* applyBlurFilterThread(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    // Filtro de desenfoque simple (media de 3x3)
    float blurFilter[3][3] = {
        {1.0 / 9, 1.0 / 9, 1.0 / 9},
        {1.0 / 9, 1.0 / 9, 1.0 / 9},
        {1.0 / 9, 1.0 / 9, 1.0 / 9}
    };

    int x, y, i, j;
    float sum;

    // Aplicar el filtro a cada píxel de la sub-imagen
    for (y = data->startY; y < data->endY; ++y) {
        for (x = 1; x < data->width - 1; ++x) {
            sum = 0.0;

            // Aplicar la convolución del filtro
            for (j = -1; j <= 1; ++j) {
                for (i = -1; i <= 1; ++i) {
                    sum += data->inputImage[(y + j) * data->width + (x + i)] * blurFilter[j + 1][i + 1];
                }
            }

            // Asegurar que el valor está en el rango válido (0 - 255)
            sum = sum < 0 ? 0 : sum > 255 ? 255 : sum;

            // Asignar el valor resultante al píxel correspondiente en la sub-imagen de salida
            data->outputImage[y * data->width + x] = (byte)sum;
        }
    }

    pthread_exit(NULL);
}

int main() {
    
    char inputFileName[100]; // Nombre del archivo de entrada
    char* outputFileName = "output_blurred.pgm"; // Nombre del archivo de salida

    printf("Ingrese el nombre del archivo de la imagen: ");
    scanf("%s", inputFileName);
    
    byte* inputImage;
    int width, height;

    // Leer la imagen de entrada
    int readResult = ReadPGM(inputFileName, &inputImage, &width, &height);

    if (readResult == 0) {
        // Crear una imagen de salida del mismo tamaño que la imagen de entrada
        byte* outputImage = malloc(width * height);
        
        // Obtener el número de núcleos disponibles en la CPU
        int numCores = sysconf(_SC_NPROCESSORS_ONLN);
        
        // Ajustar dinámicamente el número de hilos a utilizar
        int numThreads = (numCores > 1) ? numCores - 1 : 1;
        

        // Arreglo de identificadores de hilos
        pthread_t threads[numThreads];
        
        // Arreglo para almacenar los datos de cada hilo
        ThreadData threadData[numThreads];

        // Calcula el número de filas por hilo
        int rowsPerThread = height / numThreads;

        // Inicializar y ejecutar los hilos
        for (int i = 0; i < numThreads; ++i) {
            // Calcula los límites de la sub-imagen para cada hilo
            threadData[i].inputImage = inputImage;
            threadData[i].outputImage = outputImage;
            threadData[i].width = width;
            threadData[i].height = height;
            threadData[i].startY = i * rowsPerThread;
            threadData[i].endY = (i == numThreads - 1) ? height : (i + 1) * rowsPerThread;

            pthread_create(&threads[i], NULL, applyBlurFilterThread, (void*)&threadData[i]);
        }

        // Esperar a que todos los hilos hayan terminado
        for (int i = 0; i < numThreads; ++i) {
            pthread_join(threads[i], NULL);
        }

        // Escribir la imagen procesada en un nuevo archivo PGM
        WritePGM(outputFileName, outputImage, width, height);

        // Liberar la memoria utilizada
        free(inputImage);
        free(outputImage);

        printf("Procesamiento completado. Imagen de salida guardada como %s\n", outputFileName);
    } else {
        printf("Error al leer el archivo de entrada %s\n", inputFileName);
    }

    return 0;
}
