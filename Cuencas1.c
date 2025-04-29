#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

typedef struct {
    char* cuenca;
    char* embalse;
    char* mes;
    int* volumen;
    int nVolumenes;
} Embalse;

typedef struct {
    char* cuenca;
    int sumaTotal;
    int entradas;
} CuencaMedia;

int obtenerIndiceAnio(int anio) {
    int base = 2012;
    if (anio >= base && anio <= 2021)
        return anio - base;
    return -1;
}

int contarLineas(const char* nombreFichero) {
    FILE* f = fopen(nombreFichero, "r");
    if (!f) return -1;
    int contador = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), f)) contador++;
    fclose(f);
    return contador - 1;
}

int contarVolumenes(const char* linea) {
    int contador = 0;
    const char* p = linea;
    while (*p) {
        if (*p == ',') contador++;
        p++;
    }
    return contador - 2;
}

Embalse* cargarDatos(const char* nombreFichero, int* nEmbalses, int* nVolumenes) {
    FILE* f = fopen(nombreFichero, "r");
    if (!f) return NULL;

    *nEmbalses = contarLineas(nombreFichero);
    if (*nEmbalses <= 0) {
        fclose(f);
        return NULL;
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), f); // cabecera
    *nVolumenes = contarVolumenes(buffer);

    Embalse* embalses = (Embalse*)malloc(*nEmbalses * sizeof(Embalse));
    if (!embalses) {
        fclose(f);
        return NULL;
    }

    int i = 0;
    while (fgets(buffer, sizeof(buffer), f) && i < *nEmbalses) {
        char* token = strtok(buffer, ",");
        embalses[i].cuenca = token ? strdup(token) : strdup("");

        token = strtok(NULL, ",");
        embalses[i].embalse = token ? strdup(token) : strdup("");

        token = strtok(NULL, ",");
        embalses[i].mes = token ? strdup(token) : strdup("");

        embalses[i].volumen = (int*)malloc(*nVolumenes * sizeof(int));
        embalses[i].nVolumenes = *nVolumenes;

        for (int j = 0; j < *nVolumenes; j++) {
            token = strtok(NULL, ",");
            embalses[i].volumen[j] = token ? atoi(token) : 0;
        }

        i++;
    }

    fclose(f);
    return embalses;
}

void mostrarDatos(Embalse* embalses, int nEmbalses) {
    printf("\n=== REGISTROS CARGADOS (primeros 5) ===\n\n");
    for (int i = 0; i < nEmbalses && i < 5; i++) {
        printf("%s - %s - %s: ", embalses[i].cuenca, embalses[i].embalse, embalses[i].mes);
        for (int j = 0; j < embalses[i].nVolumenes; j++) {
            printf("%d ", embalses[i].volumen[j]);
        }
        printf("\n");
    }
}

void calcularMediaMensualPorCuenca(Embalse* embalses, int nEmbalses, int nVolumenes) {
    char nombreMes[20];
    int anio;
    printf("\nIntroduce el nombre del mes (por ejemplo: Marzo): ");
    fgets(nombreMes, sizeof(nombreMes), stdin);
    nombreMes[strcspn(nombreMes, "\n")] = 0;

    printf("Introduce el anio (entre 2012 y 2021): ");
    scanf("%d", &anio);
    getchar();

    int anioIndex = obtenerIndiceAnio(anio);
    if (anioIndex == -1) {
        printf("Anio no valido.\n");
        return;
    }

    int capacidad = 10;
    int nCuencas = 0;
    CuencaMedia* medias = malloc(capacidad * sizeof(CuencaMedia));

    for (int i = 0; i < nEmbalses; i++) {
        if (strcasecmp(embalses[i].mes, nombreMes) != 0)
            continue;

        int j;
        for (j = 0; j < nCuencas; j++) {
            if (strcmp(medias[j].cuenca, embalses[i].cuenca) == 0) {
                medias[j].sumaTotal += embalses[i].volumen[anioIndex];
                medias[j].entradas++;
                break;
            }
        }

        if (j == nCuencas) {
            if (nCuencas == capacidad) {
                capacidad *= 2;
                medias = realloc(medias, capacidad * sizeof(CuencaMedia));
            }
            medias[nCuencas].cuenca = strdup(embalses[i].cuenca);
            medias[nCuencas].sumaTotal = embalses[i].volumen[anioIndex];
            medias[nCuencas].entradas = 1;
            nCuencas++;
        }
    }

    printf("\n=== MEDIA MENSUAL PARA %s DE %d ===\n", nombreMes, anio);
    for (int i = 0; i < nCuencas; i++) {
        float media = (float)medias[i].sumaTotal / medias[i].entradas;
        printf("Cuenca: %-15s | Media: %.2f hectómetros cúbicos\n", medias[i].cuenca, media);
        free(medias[i].cuenca);
    }

    free(medias);
}

void calcularMediaAnualPorCuenca(Embalse* embalses, int nEmbalses, int nVolumenes) {
    int anio;
    printf("\nIntroduce el anio (entre 2012 y 2021): ");
    scanf("%d", &anio);
    getchar();

    int anioIndex = obtenerIndiceAnio(anio);
    if (anioIndex == -1) {
        printf("Anio no valido.\n");
        return;
    }

    int capacidad = 10;
    int nCuencas = 0;
    CuencaMedia* medias = malloc(capacidad * sizeof(CuencaMedia));

    for (int i = 0; i < nEmbalses; i++) {
        int j;
        for (j = 0; j < nCuencas; j++) {
            if (strcmp(medias[j].cuenca, embalses[i].cuenca) == 0) {
                medias[j].sumaTotal += embalses[i].volumen[anioIndex];
                medias[j].entradas++;
                break;
            }
        }

        if (j == nCuencas) {
            if (nCuencas == capacidad) {
                capacidad *= 2;
                medias = realloc(medias, capacidad * sizeof(CuencaMedia));
            }
            medias[nCuencas].cuenca = strdup(embalses[i].cuenca);
            medias[nCuencas].sumaTotal = embalses[i].volumen[anioIndex];
            medias[nCuencas].entradas = 1;
            nCuencas++;
        }
    }

    printf("\n=== MEDIA ANUAL PARA %d ===\n", anio);
    for (int i = 0; i < nCuencas; i++) {
        float media = (float)medias[i].sumaTotal / medias[i].entradas;
        printf("Cuenca: %-15s | Media: %.2f hectómetros cúbicos\n", medias[i].cuenca, media);
        free(medias[i].cuenca);
    }

    free(medias);
}

void liberarDatos(Embalse* embalses, int nEmbalses) {
    for (int i = 0; i < nEmbalses; i++) {
        free(embalses[i].cuenca);
        free(embalses[i].embalse);
        free(embalses[i].mes);
        free(embalses[i].volumen);
    }
    free(embalses);
}

int main() {
    setlocale(LC_ALL, "Spanish");

    Embalse* embalses = NULL;
    int nEmbalses = 0;
    int nVolumenes = 0;
    char nombreFichero[256];

    printf("Introduce el nombre del fichero CSV (por ejemplo: datos.csv): ");
    fgets(nombreFichero, sizeof(nombreFichero), stdin);
    nombreFichero[strcspn(nombreFichero, "\n")] = 0;

    embalses = cargarDatos(nombreFichero, &nEmbalses, &nVolumenes);
    if (!embalses) {
        printf("No se pudo abrir el archivo o esta vacio.\n");
        return 1;
    }

    printf("Archivo cargado correctamente (%d registros).\n", nEmbalses);
    mostrarDatos(embalses, nEmbalses);

    char continuar = 's';
    while (continuar == 's' || continuar == 'S') {
        int opcion;
        printf("\n=== MENU DE OPCIONES ===\n");
        printf("1. Calcular media mensual por cuenca (mes y anio)\n");
        printf("2. Calcular media anual por cuenca (solo anio)\n");
        printf("3. Salir\n");
        printf("Selecciona una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1:
                calcularMediaMensualPorCuenca(embalses, nEmbalses, nVolumenes);
                break;
            case 2:
                calcularMediaAnualPorCuenca(embalses, nEmbalses, nVolumenes);
                break;
            case 3:
                liberarDatos(embalses, nEmbalses);
                printf("Programa finalizado.\n");
                return 0;
            default:
                printf("Opcion no valida.\n");
        }

        printf("\nDeseas ejecutar otra funcion? (s/n): ");
        scanf(" %c", &continuar);
        getchar();
    }

    liberarDatos(embalses, nEmbalses);
    printf("Programa finalizado.\n");
    return 0;
}