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

int contarLineas(const char* nombreFichero) {
    FILE* f = fopen(nombreFichero, "r");
    if (!f) return -1;
    int contador = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), f)) contador++;
    fclose(f);
    return contador - 1; // restamos la cabecera
}

int contarVolumenes(const char* linea) {
    int contador = 0;
    const char* p = linea;
    while (*p) {
        if (*p == ',') contador++;
        p++;
    }
    return contador - 2; // quitamos cuenca, embalse, mes
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
    fgets(buffer, sizeof(buffer), f); // saltar cabecera
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



void calcularMediaMensualPorCuenca(Embalse* embalses, int nEmbalses) {
    int capacidad = 10;
    int totalCuencas = 0;
    char** cuencas = (char**)malloc(capacidad * sizeof(char*));

    // Construir lista de cuencas únicas
    for (int i = 0; i < nEmbalses; i++) {
        int existe = 0;
        for (int j = 0; j < totalCuencas; j++) {
            if (strcmp(cuencas[j], embalses[i].cuenca) == 0) {
                existe = 1;
                break;
            }
        }
        if (!existe) {
            if (totalCuencas == capacidad) {
                capacidad *= 2;
                cuencas = (char**)realloc(cuencas, capacidad * sizeof(char*));
            }
            cuencas[totalCuencas++] = strdup(embalses[i].cuenca);
        }
    }

    // Mostrar menú de cuencas
    printf("\n=== LISTA DE CUENCAS DISPONIBLES ===\n");
    for (int i = 0; i < totalCuencas; i++) {
        printf("%d. %s\n", i + 1, cuencas[i]);
    }

    int seleccionCuenca;
    printf("Selecciona el número de la cuenca: ");
    if (scanf("%d", &seleccionCuenca) != 1 || seleccionCuenca < 1 || seleccionCuenca > totalCuencas) {
        printf("Selección no válida.\n");
        while (getchar() != '\n');
        return;
    }
    getchar(); // limpiar buffer

    char* cuencaBuscada = cuencas[seleccionCuenca - 1];

    int mesBuscado;
    printf("Introduce el número del mes (1-12): ");
    if (scanf("%d", &mesBuscado) != 1 || mesBuscado < 1 || mesBuscado > 12) {
        printf("Mes no válido.\n");
        while (getchar() != '\n');
        return;
    }
    getchar(); // limpiar buffer

    int suma = 0, contador = 0;

    for (int i = 0; i < nEmbalses; i++) {
        if (strcmp(embalses[i].cuenca, cuencaBuscada) == 0) {
            int mesEmbalse;
            if (sscanf(embalses[i].mes, "%d", &mesEmbalse) == 1 && mesEmbalse == mesBuscado) {
                for (int j = 0; j < embalses[i].nVolumenes; j++) {
                    suma += embalses[i].volumen[j];
                    contador++;
                }
            }
        }
    }

    if (contador == 0) {
        printf("No se encontraron datos para esa cuenca y mes.\n");
    } else {
        float media = (float)suma / contador;
        printf("Media para la cuenca '%s' en el mes %d: %.2f\n", cuencaBuscada, mesBuscado, media);
    }

    for (int i = 0; i < totalCuencas; i++) {
        free(cuencas[i]);
    }
    free(cuencas);
}


void calcularMediaAnualPorCuenca(Embalse* embalses, int nEmbalses) {
    int capacidad = 10;
    int totalCuencas = 0;
    char** cuencas = (char**)malloc(capacidad * sizeof(char*));

    for (int i = 0; i < nEmbalses; i++) {
        int repetida = 0;
        for (int j = 0; j < totalCuencas; j++) {
            if (strcmp(cuencas[j], embalses[i].cuenca) == 0) {
                repetida = 1;
                break;
            }
        }
        if (!repetida) {
            if (totalCuencas == capacidad) {
                capacidad *= 2;
                cuencas = (char**)realloc(cuencas, capacidad * sizeof(char*));
            }
            cuencas[totalCuencas++] = strdup(embalses[i].cuenca);
        }
    }

    printf("\n=== CUENCAS DISPONIBLES ===\n");
    for (int i = 0; i < totalCuencas; i++) {
        printf("%d. %s\n", i + 1, cuencas[i]);
    }

    int seleccion;
    printf("Selecciona la cuenca: ");
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalCuencas) {
        printf("Selección no válida.\n");
        while (getchar() != '\n');
        return;
    }
    getchar();
    char* cuencaSeleccionada = cuencas[seleccion - 1];

    int anio;
    printf("Introduce el año (2012 a 2021): ");
    if (scanf("%d", &anio) != 1 || anio < 2012 || anio > 2021) {
        printf("Año fuera de rango.\n");
        while (getchar() != '\n');
        return;
    }
    getchar();

    int anioIndex = anio - 2012;
    int suma = 0, entradas = 0;

    for (int i = 0; i < nEmbalses; i++) {
        if (strcmp(embalses[i].cuenca, cuencaSeleccionada) == 0 && anioIndex < embalses[i].nVolumenes) {
            suma += embalses[i].volumen[anioIndex];
            entradas++;
        }
    }

    if (entradas == 0) {
        printf("No se encontraron datos para la cuenca '%s' en el año %d.\n", cuencaSeleccionada, anio);
    } else {
        float media = (float)suma / entradas;
        printf("Media anual de la cuenca '%s' en %d: %.2f hectómetros cúbicos\n", cuencaSeleccionada, anio, media);
    }

    for (int i = 0; i < totalCuencas; i++) {
        free(cuencas[i]);
    }
    free(cuencas);
}




void calcularMediaPorEmbalse(Embalse* embalses, int nEmbalses) {
    int capacidad = 10;
    int totalNombres = 0;
    char** nombres = (char**)malloc(capacidad * sizeof(char*));

    for (int i = 0; i < nEmbalses; i++) {
        int existe = 0;
        for (int j = 0; j < totalNombres; j++) {
            if (strcmp(nombres[j], embalses[i].embalse) == 0) {
                existe = 1;
                break;
            }
        }
        if (!existe) {
            if (totalNombres == capacidad) {
                capacidad *= 2;
                nombres = (char**)realloc(nombres, capacidad * sizeof(char*));
            }
            nombres[totalNombres++] = strdup(embalses[i].embalse);
        }
    }

    printf("\n=== LISTA DE EMBALSES DISPONIBLES ===\n");
    for (int i = 0; i < totalNombres; i++) {
        printf("%d. %s\n", i + 1, nombres[i]);
    }

    int seleccion = 0;
    printf("Selecciona el número del embalse: ");
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalNombres) {
        printf("Selección no válida.\n");
        while (getchar() != '\n');
        return;
    }
    getchar();

    char* embalseElegido = nombres[seleccion - 1];

    int tipoMedia = 0;
    printf("¿Deseas calcular la media mensual (1) o media anual (2)? ");
    if (scanf("%d", &tipoMedia) != 1 || (tipoMedia != 1 && tipoMedia != 2)) {
        printf("Opción no válida.\n");
        while (getchar() != '\n');
        return;
    }
    getchar();

    int suma = 0, entradas = 0;

    for (int i = 0; i < nEmbalses; i++) {
        if (strcmp(embalses[i].embalse, embalseElegido) == 0) {
            for (int j = 0; j < embalses[i].nVolumenes; j++) {
                suma += embalses[i].volumen[j];
            }
            entradas += (tipoMedia == 1) ? embalses[i].nVolumenes : 1;
        }
    }

    if (entradas == 0) {
        printf("No se encontraron datos para ese embalse.\n");
    } else {
        float media = (float)suma / entradas;
        printf("Media %s del embalse '%s': %.2f\n",
               tipoMedia == 1 ? "mensual" : "anual", embalseElegido, media);
    }

    for (int i = 0; i < totalNombres; i++) free(nombres[i]);
    free(nombres);
}
void calcularEvolucionAguaEstancada(Embalse* embalses, int nEmbalses) 
{
    int capacidad = 10;
    int totalNombres = 0;
    char** nombres = (char**)malloc(capacidad * sizeof(char*));

    for (int i = 0; i < nEmbalses; i++) 
	{
        int existe = 0;
        for (int j = 0; j < totalNombres; j++) 
		{
            if (strcmp(nombres[j], embalses[i].embalse) == 0) 
			{
                existe = 1;
                break;
            }
        }
        if (!existe) 
		{
            if (totalNombres == capacidad) {
                capacidad *= 2;
                nombres = (char**)realloc(nombres, capacidad * sizeof(char*));
            }
            nombres[totalNombres++] = strdup(embalses[i].embalse);
        }
    }

    printf("\n=== LISTA DE EMBALSES DISPONIBLES ===\n");
    for (int i = 0; i < totalNombres; i++) {
        printf("%d. %s\n", i + 1, nombres[i]);
    }

    int seleccion = 0;
    printf("Selecciona el número del embalse: ");
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalNombres) 
	{
        printf("Selección no válida.\n");
        while (getchar() != '\n');
        return;
    }
    getchar();

    char* embalseElegido = nombres[seleccion - 1];

    printf("\n=== EVOLUCIÓN ANUAL DEL AGUA ESTANCADA PARA '%s' ===\n", embalseElegido);
    int anios[10] = {2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021};
    int volumenTotalPorAnio[10] = {0};

    for (int i = 0; i < nEmbalses; i++) {
        if (strcmp(embalses[i].embalse, embalseElegido) == 0) 
		{
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                volumenTotalPorAnio[j] += embalses[i].volumen[j];
            }
        }
    }

    for (int i = 0; i < 10; i++) 
	{
        printf("Año %d: %d hectómetros cúbicos\n", anios[i], volumenTotalPorAnio[i]);
    }

    for (int i = 0; i < totalNombres; i++) free(nombres[i]);
    free(nombres);
}

void detectarAnomalias(Embalse* embalses, int nEmbalses, int nVolumenes) {
    int capacidad = 10;
    int totalEmbalses = 0;
    char** nombresEmbalses = (char**)malloc(capacidad * sizeof(char*));

    for (int i = 0; i < nEmbalses; i++) {
        int existe = 0;
        for (int j = 0; j < totalEmbalses; j++) {
            if (strcmp(nombresEmbalses[j], embalses[i].embalse) == 0) {
                existe = 1;
                break;
            }
        }
        if (!existe) {
            if (totalEmbalses == capacidad) {
                capacidad *= 2;
                nombresEmbalses = (char**)realloc(nombresEmbalses, capacidad * sizeof(char*));
            }
            nombresEmbalses[totalEmbalses++] = strdup(embalses[i].embalse);
        }
    }

    printf("\n=== LISTA DE EMBALSES DISPONIBLES ===\n");
    for (int i = 0; i < totalEmbalses; i++) {
        printf("%d. %s\n", i + 1, cuenca[i]);
    }

    int seleccionCuenca;
    printf("Selecciona el número del embalse: ");
    if (scanf("%d", &seleccionEmbalse) != 1 || seleccionEmbalse < 1 || seleccionEmbalse > totalEmbalses) {
        printf("Selección no válida.\n");
        while (getchar() != '\n');
        for (int i = 0; i < totalEmbalses; i++) free(nombresEmbalses[i]);
        free(nombresEmbalses);
        return;
    }
    getchar();
    char* embalseSeleccionado = nombresEmbalses[seleccion - 1];

    int ANIO_INICIO = 2012;
    const float UMBRAL = 0.30f;
    printf("\n=== CAMBIOS NOTABLES DETECTADOS (>%.0f%%) PARA '%s' ===\n", UMBRAL * 100, embalseSeleccionado);

    int encontrado = 0;
    for (int i = 0; i < nEmbalses; i++) {
        if (strcmp(embalses[i].embalse, embalseSeleccionado) == 0) {
            for (int j = 1; j < nVolumenes; j++) {
                int ant = embalses[i].volumen[j - 1];
                int act = embalses[i].volumen[j];
                if (ant > 0) {
                    float diff = (act - ant) / (float)ant;
                    if (labs(diff) > UMBRAL) {
                        printf("⚠ %s - %s (%s): %d → %d entre %d y %d (%.1f%%)\n",
                            embalses[i].cuenca,
                            embalses[i].embalse,
                            embalses[i].mes,
                            ant, act,
                            ANIO_INICIO + j - 1,
                            ANIO_INICIO + j,
                            diff * 100);
                        encontrado = 1;
                    }
                }
            }
        }
    }

    if (!encontrado) {
        printf("No se detectaron anomalías para el embalse '%s'.\n", embalseSeleccionado);
    }

    for (int i = 0; i < totalEmbalses; i++) free(nombresEmbalses[i]);
    free(nombresEmbalses);
}


void liberarDatos(Embalse* embalses, int nEmbalses) 
{
    for (int i = 0; i < nEmbalses; i++) 
	{
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
    
    //Bienvenida al codigo
    printf("¡ BIENVENIDO AL PROGRAMA DE ANÁLISIS DE DATOS DE MITECO ! ----> CuencaStast \n");
    printf("\n Hemos registrado los datos relativos a los volúmenes de agua almacenada, en los embalses españoles, agrupados por cuencas hidrográficas.\n");
    printf("\n Y a partir de este programa, creado por :\n ");
    printf("\n 1-Marcos Tejero Monedero \n");
    printf("\n 2-Miguel Martínez-Lacaci \n");
    printf("\n 3-Juan Moreno Aymerich \n");
    printf("\n 4-Manuel López Romera \n");
    printf("\n Podrás elegir diferentes opciones, para estudiar este conjunto de datos, de manera exacta y efectiva. \n");
    printf(" \n ¿Estás listo? \n");
    printf("\n \n Para ello, deberás de introducir el nombre del fichero CSV que le hemos proporcinado (dataset.csv): ");
    fgets(nombreFichero, sizeof(nombreFichero), stdin);
    nombreFichero[strcspn(nombreFichero, "\n")] = 0;

    embalses = cargarDatos(nombreFichero, &nEmbalses, &nVolumenes);
    if (!embalses) {
        printf("No se pudo abrir el archivo o está vacío.\n");
        return 1;
    }

    printf("Archivo cargado correctamente (%d registros).\n", nEmbalses);
    printf("\n -------¡BIENVENIDO!------ \n");
   

    char continuar = 's';
    while (continuar == 's' || continuar == 'S') 
	{
        int opcion;
        printf("\n=== MENÚ DE OPCIONES ===\n");
        printf("1. Calcular media mensual por cuenca\n");
        printf("2. Calcular media anual por cuenca\n");
        printf("3. Calcular media (Anual o Mensual) por embalse\n");
        printf("4. Calcular evolucion del agua estancada a lo largo del tiempo\n");
	printf("5. Detectar periodos anómalos");
        printf("6. Salir\n");
        printf("Selecciona una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) 
		{
            case 1:
                calcularMediaMensualPorCuenca(embalses, nEmbalses);
                break;
            case 2:
                calcularMediaAnualPorCuenca(embalses, nEmbalses);
                break;
            case 3:
                calcularMediaPorEmbalse(embalses, nEmbalses);
                break;
            case 4:
                calcularEvolucionAguaEstancada(embalses, nEmbalses);
                break;
             case 5:
            	detectarAnomalias(embalses, nEmbalses, nVolumenes);
            	break;
            case 6:
                liberarDatos(embalses, nEmbalses);
                printf("Programa finalizado.\n");
                system("pause");
                return 0;
            default:
                printf("Opción no válida.\n");
        }

        printf("\n¿Deseas ejecutar otra funcion?: ");
        scanf(" %c", &continuar);
        getchar();
    }

    liberarDatos(embalses, nEmbalses);
    printf("Programa finalizado.\n");
    return 0;
}
