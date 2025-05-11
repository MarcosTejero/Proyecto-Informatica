#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>

typedef struct 
{
    char* cuenca;     
    char* embalse;    
    char* mes;        
    int* volumen;     
    int nVolumenes;   
} Embalse;


typedef struct 
{
    char* cuenca;     
    int sumaTotal;    
    int entradas;     
} CuencaMedia;

// Función para contar las líneas de un archivo CSV
int contarLineas(const char* nombreFichero) 
{
    FILE* f = fopen(nombreFichero, "r");
    if (!f) return -1;  
    int contador = 0;
    char buffer[1024];
    
    // Leer línea por línea
    while (fgets(buffer, sizeof(buffer), f)) contador++;
    fclose(f);
 
    return contador - 1; 
}

// Función para contar el número de volúmenes de datos en una línea
int contarVolumenes(const char* linea) 
{
    int contador = 0;
    const char* p = linea;
    
    // Contar las comas en la línea, cada coma indica un volumen
    while (*p) 
	{
        if (*p == ',') contador++;
        p++;
    }

    return contador - 2; 
}

// Función para cargar los datos desde un archivo CSV a una lista de estructuras Embalse
Embalse* cargarDatos(const char* nombreFichero, int* nEmbalses, int* nVolumenes) 
{
    FILE* f = fopen(nombreFichero, "r");
    if (!f) return NULL;

    *nEmbalses = contarLineas(nombreFichero);  
    if (*nEmbalses <= 0) 
	{
        fclose(f);
        return NULL;
    }

    char buffer[1024];
    
    // Leer y descartar la cabecera
    fgets(buffer, sizeof(buffer), f); 
    *nVolumenes = contarVolumenes(buffer);
    
    // Reservar memoria para almacenar los datos de los embalses
    Embalse* embalses = (Embalse*)malloc(*nEmbalses * sizeof(Embalse));
    if (!embalses) 
	{
        fclose(f);
        return NULL;
    }

    int i = 0;
    // Leer los datos de cada línea del archivo
    while (fgets(buffer, sizeof(buffer), f) && i < *nEmbalses) 
	{
        char* token = strtok(buffer, ",");
        embalses[i].cuenca = token ? strdup(token) : strdup("");

        token = strtok(NULL, ",");
        embalses[i].embalse = token ? strdup(token) : strdup("");

        token = strtok(NULL, ",");
        embalses[i].mes = token ? strdup(token) : strdup("");

        embalses[i].volumen = (int*)malloc(*nVolumenes * sizeof(int)); 
        embalses[i].nVolumenes = *nVolumenes;

        // Leer los volúmenes de agua
        for (int j = 0; j < *nVolumenes; j++) 
		{
            token = strtok(NULL, ",");
            embalses[i].volumen[j] = token ? atoi(token) : 0;
        }

        i++;
    }

    fclose(f);
    return embalses;
}

// Función para calcular y mostrar la media mensual del volumen de agua para una cuenca seleccionada
void calcularMediaMensualPorCuenca(Embalse* embalses, int nEmbalses) 
{
    int capacidad = 10;
    int totalCuencas = 0;
    char** cuencas = (char**)malloc(capacidad * sizeof(char*));  

    // Construir lista de cuencas únicas
    for (int i = 0; i < nEmbalses; i++) 
	{
        int existe = 0;
        for (int j = 0; j < totalCuencas; j++) 
		{
			 // Verificar si la cuenca ya está registrada
            if (strcmp(cuencas[j], embalses[i].cuenca) == 0)  
			{
                existe = 1;
                break;
            }
        }
        
        // Si no existe la cuenca, agregarla a la lista
        if (!existe) 
		{
            if (totalCuencas == capacidad) 
			{
                capacidad *= 2;
                 // Aumentar la capacidad si es necesario
                cuencas = (char**)realloc(cuencas, capacidad * sizeof(char*)); 
            }
            cuencas[totalCuencas++] = strdup(embalses[i].cuenca);
        }
    }

    printf("\n=== LISTA DE CUENCAS DISPONIBLES ===\n");
    for (int i = 0; i < totalCuencas; i++) 
	{
        printf("%d. %s\n", i + 1, cuencas[i]);
    }

    int seleccionCuenca;
    printf("Selecciona el numero de la cuenca: ");
    
    // Verificar que la entrada del usuario sea válida
    if (scanf("%d", &seleccionCuenca) != 1 || seleccionCuenca < 1 || seleccionCuenca > totalCuencas) 
	{
        printf("Seleccion no valida.\n");
        while (getchar() != '\n');
        return;
    }
    // Limpiar el buffer de entrada
    getchar(); 
    // Obtener la cuenca seleccionada
    char* cuencaBuscada = cuencas[seleccionCuenca - 1]; 

    int mesBuscado;
    printf("Introduce el numero del mes (1-12): ");
    
    // Verificar que el mes sea válido
    if (scanf("%d", &mesBuscado) != 1 || mesBuscado < 1 || mesBuscado > 12) 
	{
        printf("Mes no valido.\n");
        while (getchar() != '\n');
        return;
    }
    // Limpiar el buffer
    getchar(); 

    int suma = 0, contador = 0;

    // Buscar y sumar los volúmenes correspondientes al mes y cuenca seleccionados
    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].cuenca, cuencaBuscada) == 0) 
		{
            int mesEmbalse;
            if (sscanf(embalses[i].mes, "%d", &mesEmbalse) == 1 && mesEmbalse == mesBuscado) 
			{
                for (int j = 0; j < embalses[i].nVolumenes; j++) 
				{
					// Sumar los volúmenes
                    suma += embalses[i].volumen[j];  
                    // Contar las entradas
                    contador++;  
                }
            }
        }
    }

    if (contador == 0) 
	{
        printf("No se encontraron datos para esa cuenca y mes.\n");
    } else 
	{
        float media = (float)suma / contador; 
        printf("Media para la cuenca '%s' en el mes %d: %.2f hectometros cubicos", cuencaBuscada, mesBuscado, media);
    }
    
    // Liberar la memoria asignada para las cuencas
    for (int i = 0; i < totalCuencas; i++) 
	{
        free(cuencas[i]);
    }
    free(cuencas);
}



// Función para calcular la media anual del agua estancada en una cuenca específica
void calcularMediaAnualPorCuenca(Embalse* embalses, int nEmbalses) 
{
    // Inicialización de variables: capacidad para las cuencas, número total de cuencas y arreglo dinámico de cuencas
    int capacidad = 10;
    int totalCuencas = 0;
    char** cuencas = (char**)malloc(capacidad * sizeof(char*));

    // Recorrer todos los embalses y agregar las cuencas únicas al arreglo de cuencas
    for (int i = 0; i < nEmbalses; i++) 
	{
        int repetida = 0;
        
        // Verificar si la cuenca ya existe en el arreglo
        for (int j = 0; j < totalCuencas; j++) 
		{
            if (strcmp(cuencas[j], embalses[i].cuenca) == 0) 
			{
                repetida = 1;  
                break;
            }
        }

        // Si no está repetida, agregar la cuenca al arreglo
        if (!repetida) 
		{
            if (totalCuencas == capacidad) 
			{
                // Si el arreglo está lleno, duplicar su capacidad
                capacidad *= 2;
                cuencas = (char**)realloc(cuencas, capacidad * sizeof(char*));
            }
            cuencas[totalCuencas++] = strdup(embalses[i].cuenca);  
        }
    }

    printf("\n=== CUENCAS DISPONIBLES ===\n");
    for (int i = 0; i < totalCuencas; i++) 
	{
        printf("%d. %s\n", i + 1, cuencas[i]);
    }

    int seleccion;
    printf("Selecciona la cuenca: ");

    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalCuencas) 
	{
        printf("Selección no valida.\n");
        while (getchar() != '\n');  
        return;
    }
    getchar(); 

    char* cuencaSeleccionada = cuencas[seleccion - 1];  

    int anio;
    printf("Introduce el anio (2012 a 2021): ");
    if (scanf("%d", &anio) != 1 || anio < 2012 || anio > 2021) 
	{
        printf("Anio fuera de rango.\n");
        while (getchar() != '\n');  
        return;
    }
    getchar(); 

    // Calcular el índice del año (0 basado)
    int anioIndex = anio - 2012;
    int suma = 0, entradas = 0;

    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].cuenca, cuencaSeleccionada) == 0 && anioIndex < embalses[i].nVolumenes) 
		{
            suma += embalses[i].volumen[anioIndex];  
            entradas++; 
        }
    }

    if (entradas == 0) 
	{
        printf("No se encontraron datos para la cuenca '%s' en el anio %d.\n", cuencaSeleccionada, anio);
    } else 
	{
        float media = (float)suma / entradas;
        printf("Media anual de la cuenca '%s' en %d: %.2f hectometros cubicos\n", cuencaSeleccionada, anio, media);
    }

    for (int i = 0; i < totalCuencas; i++) 
	{
        free(cuencas[i]);
    }
    free(cuencas);  
}


// Función para calcular la media anual o mensual de los volúmenes de agua de un embalse específico
void calcularMediaPorEmbalse(Embalse* embalses, int nEmbalses) 
{
    // Inicialización de variables: capacidad para los embalses, número total de embalses y arreglo dinámico de nombres de embalses
    int capacidad = 10;
    int totalNombres = 0;
    char** nombres = (char**)malloc(capacidad * sizeof(char*));

    // Recorrer todos los embalses y agregar los nombres únicos de embalses al arreglo
    for (int i = 0; i < nEmbalses; i++) 
	{
        int existe = 0;
        
        // Verificar si el nombre del embalse ya está en el arreglo
        for (int j = 0; j < totalNombres; j++) 
		{
            if (strcmp(nombres[j], embalses[i].embalse) == 0) 
			{
                existe = 1;  
                break;
            }
        }

        // Si no está repetido, agregar el nombre al arreglo
        if (!existe) 
		{
            if (totalNombres == capacidad) 
			{
                // Si el arreglo está lleno, duplicar su capacidad
                capacidad *= 2;
                nombres = (char**)realloc(nombres, capacidad * sizeof(char*));
            }
            nombres[totalNombres++] = strdup(embalses[i].embalse);  
        }
    }

    // Mostrar la lista de embalses disponibles para que el usuario seleccione
    printf("\n=== LISTA DE EMBALSES DISPONIBLES ===\n");
    for (int i = 0; i < totalNombres; i++) 
	{
        printf("%d. %s\n", i + 1, nombres[i]);
    }

    // Solicitar al usuario seleccionar un embalse
    int seleccion = 0;
    printf("Selecciona el numero del embalse: ");
    
    // Verificar que la selección sea válida
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalNombres) 
	{
        printf("Seleccion no valida.\n");
        while (getchar() != '\n');  
        return;
    }
    getchar(); 

    char* embalseElegido = nombres[seleccion - 1];  

    int tipoMedia = 0;
    printf("¿Deseas calcular la media mensual (1) o media anual (2)? ");
    if (scanf("%d", &tipoMedia) != 1 || (tipoMedia != 1 && tipoMedia != 2)) 
	{
        printf("Opción no valida.\n");
        while (getchar() != '\n');  
        return;
    }
    getchar(); 

    int suma = 0, entradas = 0;

    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].embalse, embalseElegido) == 0) 
		{
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                suma += embalses[i].volumen[j];  
            }
            entradas += (tipoMedia == 1) ? embalses[i].nVolumenes : 1;  
        }
    }

    // Si no se encontraron datos para el embalse, mostrar mensaje de error
    if (entradas == 0) 
	{
        printf("No se encontraron datos para ese embalse.\n");
    } else 
	{
        float media = (float)suma / entradas;
        printf("Media %s del embalse '%s': %.2f\n hectometros cubicos",
               tipoMedia == 1 ? "mensual" : "anual", embalseElegido, media);
    }

    // Liberar la memoria reservada para los nombres de embalses
    for (int i = 0; i < totalNombres; i++) 
	{
        free(nombres[i]);
    }
    free(nombres);  
}


void Evolucion_de_agua_estancada(Embalse* embalses, int nEmbalses) 
{
    // Se establece una capacidad inicial para el arreglo de nombres de embalses
    int capacidad = 10;
    int totalNombres = 0;
    
    // Se aloca memoria para el arreglo de nombres de embalses
    char** nombres = (char**)malloc(capacidad * sizeof(char*));

    // Iteramos sobre todos los embalses
    for (int i = 0; i < nEmbalses; i++) 
	{
        int existe = 0;
        
        // Comprobamos si el nombre del embalse ya está registrado en el arreglo de nombres
        for (int j = 0; j < totalNombres; j++) 
		{
            if (strcmp(nombres[j], embalses[i].embalse) == 0) 
			{
				// Si ya existe, no lo agregamos de nuevo
                existe = 1;  
                break;
            }
        }
        
        // Si el embalse no ha sido agregado previamente
        if (!existe) 
		{
            // Si el arreglo de nombres está lleno, se duplica su capacidad
            if (totalNombres == capacidad) 
			{
                capacidad *= 2;
                nombres = (char**)realloc(nombres, capacidad * sizeof(char*));
            }
            
            // Se agrega el nombre del embalse al arreglo
            nombres[totalNombres++] = strdup(embalses[i].embalse);
        }
    }

    printf("\n=== LISTA DE EMBALSES DISPONIBLES ===\n");
    for (int i = 0; i < totalNombres; i++) 
	{
        printf("%d. %s\n", i + 1, nombres[i]);
    }

    // Solicitar al usuario que seleccione un embalse de la lista
    int seleccion = 0;
    printf("Selecciona el numero del embalse: ");
    
    // Verificar que la selección del usuario sea válida
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalNombres) 
	{
        printf("Selección no valida.\n");
        while (getchar() != '\n');  
        return;
    }
    getchar(); 

    char* embalseElegido = nombres[seleccion - 1];
    printf("\n=== EVOLUCION ANUAL DEL AGUA ESTANCADA PARA '%s' ===\n", embalseElegido);
   
    int anios[10] = {2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021};
    
    // Inicializar el arreglo que almacenará el volumen total de agua por año
    int volumenTotalPorAnio[10] = {0};

    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].embalse, embalseElegido) == 0) 
		{
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                volumenTotalPorAnio[j] += embalses[i].volumen[j]; 
            }
        }
    }

    // Mostrar la evolución anual de los volúmenes de agua estancada
    for (int i = 0; i < 10; i++) 
	{
        printf("Anio %d: %d hectometros cubicos\n", anios[i], volumenTotalPorAnio[i]);
    }

    // Liberar la memoria asignada para los nombres de los embalses
    for (int i = 0; i < totalNombres; i++) free(nombres[i]);
    free(nombres);  
}


// Función que detecta incrementos o decrementos bruscos de volumen en un embalse
// Se considera un cambio notable si el volumen cambia más de un 30% de un año a otro.
void detectarAbundancia(Embalse* embalses, int nEmbalses, int nVolumenes, char* embalseSeleccionado) 
{
    int ANIO_INICIO = 2012;             
    // Umbral mínimo de cambio relativo para considerarlo "notable" (30%)
    const float UMBRAL = 0.30f;        

    // Encabezado informativo
    printf("\n=== CAMBIOS NOTABLES DETECTADOS (>%.0f%%) PARA '%s' ===\n", UMBRAL * 100, embalseSeleccionado);

    int encontrado = 0; 

    // Recorrer todos los registros del array de embalses
    for (int i = 0; i < nEmbalses; i++) 
	{
        // Filtrar solo el embalse que nos interesa
        if (strcmp(embalses[i].embalse, embalseSeleccionado) == 0) 
		{
            // Analizar los cambios de volumen año a año
            for (int j = 1; j < nVolumenes; j++) 
			{
				// Volumen del año anterior
                int ant = embalses[i].volumen[j - 1];  
                // Volumen del año actual
                int act = embalses[i].volumen[j];     

                // Solo se analiza si el volumen anterior es mayor que cero (evita divisiones inválidas)
                if (ant > 0) 
				{
                    // Calcular el cambio relativo (positivo o negativo)
                    float diff = (act - ant) / (float)ant;

                    // Verificar si supera el umbral definido (positivo o negativo)
                    if (fabs(diff) > UMBRAL) 
					{
                        // Mostrar información del cambio anómalo
                        printf(" %s - %s (%s): del mes %d a %d entre %d y %d (%.1f%%)\n",
                            embalses[i].cuenca,             
                            embalses[i].embalse,             
                            embalses[i].mes,                 
                            ant,                            
                            act,                             
                            ANIO_INICIO + j - 1,             
                            ANIO_INICIO + j,                 
                            diff * 100);                     
                        encontrado = 1;
                    }
                }
            }
        }
    }

    // Si no se encontró ningún cambio notable, se informa al usuario
    if (!encontrado) 
	{
        printf("No se detectaron anomalias para el embalse '%s'.\n", embalseSeleccionado);
    }
}


// Función que detecta periodos de sequía en un embalse concreto.
// Se define una sequía como al menos tres años consecutivos de descenso de volumen.
void detectarSequias(Embalse* embalses, int nEmbalses, int nVolumenes, char* embalseSeleccionado) 
{
    const int ANIO_INICIO = 2012;  
    // Bandera para saber si se encontró alguna sequía
    int encontrado = 0;           

    printf("\n=== PERIODOS DE SEQUIA DETECTADOS PARA '%s' ===\n", embalseSeleccionado);

    // Recorremos todos los embalses buscando coincidencias con el nombre seleccionado
    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].embalse, embalseSeleccionado) == 0) 
		{
            int inicio = -1;   
            int conteo = 0;   

            
            for (int j = 1; j < nVolumenes; j++) 
			{
                // Si el volumen ha disminuido respecto al año anterior
                if (embalses[i].volumen[j] < embalses[i].volumen[j - 1]) 
				{
					// Registrar el inicio si es el primer descenso
                    if (inicio == -1) inicio = j - 1;  
                    conteo++;
                } 
				else 
				{
                    // Si la racha de descensos fue de al menos 3 años, lo mostramos
                    if (conteo >= 2) 
					{
                        printf(" %s - %s (%s): sequia de %d anios desde %d a %d\n",
                            // Nombre de la cuenca
                            embalses[i].cuenca,       
                            // Nombre del embalse
                            embalses[i].embalse,      
                            // Mes (informativo)
                            embalses[i].mes,          
                            // Total de años en sequía
                            conteo + 1,               
                            // Año de inicio
                            ANIO_INICIO + inicio,     
                            // Año de fin
                            ANIO_INICIO + j - 1);     
                        encontrado = 1;
                    }
                    // Reiniciamos el conteo si no hubo descenso
                    conteo = 0;
                    inicio = -1;
                }
            }

            // Caso especial: si la racha descendente llega hasta el último año
            if (conteo >= 2) 
			{
                printf(" %s - %s (%s): sequia de %d anios desde %d a %d\n",
                    embalses[i].cuenca,
                    embalses[i].embalse,
                    embalses[i].mes,
                    conteo + 1,
                    ANIO_INICIO + inicio,
                    ANIO_INICIO + nVolumenes - 1);
                encontrado = 1;
            }
        }
    }

    // Si no se encontró ninguna racha descendente significativa
    if (!encontrado) 
	{
        printf("No se detectaron sequias para el embalse '%s'.\n", embalseSeleccionado);
    }
}


// Muestra un menú para seleccionar un embalse y detectar anomalías (abundancia o sequías).
void detectarAnomalias(Embalse* embalses, int nEmbalses, int nVolumenes) 
{
	// Capacidad inicial del array dinámico
    int capacidad = 10;  
    int totalEmbalses = 0;
    // Array para guardar nombres únicos de embalses
    char** nombresEmbalses = (char**)malloc(capacidad * sizeof(char*));  

    // Recoger todos los nombres únicos de embalses
    for (int i = 0; i < nEmbalses; i++) 
	{
        int existe = 0;
        for (int j = 0; j < totalEmbalses; j++) 
		{
            if (strcmp(nombresEmbalses[j], embalses[i].embalse) == 0) 
			{
                existe = 1;
                break;
            }
        }
        if (!existe) 
		{
			// Redimensionar si se alcanza la capacidad
            if (totalEmbalses == capacidad) 
			{
                capacidad *= 2;
                nombresEmbalses = (char**)realloc(nombresEmbalses, capacidad * sizeof(char*));
            }
            nombresEmbalses[totalEmbalses++] = strdup(embalses[i].embalse);
        }
    }

    int opcion;
    printf("\n=== OPCIONES DISPONIBLES ===\n");
    printf("1. Detectar abundancia hidrica\n");
    printf("2. Detectar periodos de sequia\n");
    printf("Seleccione una opcion: ");
    if (scanf("%d", &opcion) != 1 || (opcion != 1 && opcion != 2)) 
	{
        printf("Opcion no valida.\n");
        while (getchar() != '\n');  
        for (int i = 0; i < totalEmbalses; i++) free(nombresEmbalses[i]);
        free(nombresEmbalses);
        return;
    }

    printf("\n=== LISTA DE EMBALSES DISPONIBLES ===\n");
    for (int i = 0; i < totalEmbalses; i++) 
	{
        printf("%d. %s\n", i + 1, nombresEmbalses[i]);
    }

    int seleccion;
    printf("Selecciona el numero del embalse: ");
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalEmbalses) 
	{
        printf("Seleccion no valida.\n");
        while (getchar() != '\n');
        for (int i = 0; i < totalEmbalses; i++) free(nombresEmbalses[i]);
        free(nombresEmbalses);
        return;
    }
    getchar(); 

    char* embalseSeleccionado = nombresEmbalses[seleccion - 1];

    // Ejecutar la opción elegida
    if (opcion == 1)
        detectarAbundancia(embalses, nEmbalses, nVolumenes, embalseSeleccionado);
    else
        detectarSequias(embalses, nEmbalses, nVolumenes, embalseSeleccionado);

    for (int i = 0; i < totalEmbalses; i++) free(nombresEmbalses[i]);
    free(nombresEmbalses);
}

// Calcula la moda (valor más frecuente) de los volúmenes de un embalse seleccionado
void calcularModa(Embalse* embalses, int nEmbalses) 
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
            if (totalNombres == capacidad) 
			{
                capacidad *= 2;
                nombres = (char**)realloc(nombres, capacidad * sizeof(char*));
            }
            nombres[totalNombres++] = strdup(embalses[i].embalse);
        }
    }

    printf("\n=== LISTA DE EMBALSES DISPONIBLES ===\n");
    for (int i = 0; i < totalNombres; i++) 
	{
        printf("%d. %s\n", i + 1, nombres[i]);
    }

    int seleccion = 0;
    printf("Selecciona el numero del embalse: ");
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalNombres) 
	{
        printf("Seleccion no valida.\n");
        while (getchar() != '\n');
        for (int i = 0; i < totalNombres; i++) free(nombres[i]);
        free(nombres);
        return;
    }
    getchar();

    char* embalseElegido = nombres[seleccion - 1];

    // Arrays dinámicos para almacenar frecuencias y valores únicos
    int* frecuencias = NULL;
    int* valores = NULL;
    int nValores = 0;
    int capacidadValores = 100;

    frecuencias = (int*)malloc(capacidadValores * sizeof(int));
    valores = (int*)malloc(capacidadValores * sizeof(int));

    // Contar frecuencia de cada volumen
    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].embalse, embalseElegido) == 0) 
		{
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                int encontrado = 0;
                for (int k = 0; k < nValores; k++) 
				{
                    if (valores[k] == embalses[i].volumen[j]) 
					{
                        frecuencias[k]++;
                        encontrado = 1;
                        break;
                    }
                }
                if (!encontrado) 
				{
                    if (nValores == capacidadValores) 
					{
                        capacidadValores *= 2;
                        valores = (int*)realloc(valores, capacidadValores * sizeof(int));
                        frecuencias = (int*)realloc(frecuencias, capacidadValores * sizeof(int));
                    }
                    valores[nValores] = embalses[i].volumen[j];
                    frecuencias[nValores] = 1;
                    nValores++;
                }
            }
        }
    }

    if (nValores == 0) 
	{
        printf("No se encontraron datos para ese embalse.\n");
    } 
	else 
	{
        int maxFrecuencia = 0;
        for (int i = 0; i < nValores; i++) 
		{
            if (frecuencias[i] > maxFrecuencia) 
			{
                maxFrecuencia = frecuencias[i];
            }
        }

        printf("Moda del embalse '%s':\n ", embalseElegido);
        for (int i = 0; i < nValores; i++) 
		{
            if (frecuencias[i] == maxFrecuencia) 
			{
                printf("- %d hectometros cubicos (repetido %d veces)\n", valores[i], frecuencias[i]);
            }
        }
    }

    for (int i = 0; i < totalNombres; i++) free(nombres[i]);
    free(nombres);
    free(frecuencias);
    free(valores);
}


// Función que calcula la desviación típica del volumen de agua almacenado para un embalse seleccionado
void Desviaciontipica(Embalse* embalses, int nEmbalses) 
{
	// Capacidad inicial del array dinámico de nombres
    int capacidad = 10; 
    // Número de embalses únicos encontrados
    int totalNombres = 0; 
    // Array dinámico para almacenar nombres únicos de embalses
    char** nombres = (char**)malloc(capacidad * sizeof(char*)); 

    // Recorrer todos los embalses para construir una lista de nombres únicos
    for (int i = 0; i < nEmbalses; i++) 
	{
        int existe = 0;
        for (int j = 0; j < totalNombres; j++) 
		{   
		    // Compara si ya está registrado
            if (strcmp(nombres[j], embalses[i].embalse) == 0) 
			{
                existe = 1;
                break;
            }
        }
        // Si no está, lo añadimos a la lista de nombres
        if (!existe) 
		{
            if (totalNombres == capacidad) 
			{
                capacidad *= 2;
                nombres = (char**)realloc(nombres, capacidad * sizeof(char*));
            }
            nombres[totalNombres++] = strdup(embalses[i].embalse); 
        }
    }

    // Mostrar todos los embalses únicos encontrados
    printf("\n=== LISTA DE EMBALSES DISPONIBLES ===\n");
    for (int i = 0; i < totalNombres; i++)
	{
        printf("%d. %s\n", i + 1, nombres[i]);
    }

    // Solicitar al usuario el embalse del que desea calcular la desviación típica
    int seleccion = 0;
    printf("Selecciona el numero del embalse para calcular la desviacion tipica: ");
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalNombres) 
	{
        printf("Seleccion no valida.\n");
        // Limpiar el buffer de entrada
        while (getchar() != '\n'); 
        return;
    }
    // Consumir el salto de línea restante
    getchar(); 

    // Guardar el nombre del embalse seleccionado
    char* embalseElegido = nombres[seleccion - 1];

    // Calcular la suma total y el número de entradas para el embalse
    int suma = 0, entradas = 0;
    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].embalse, embalseElegido) == 0) 
		{
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                suma += embalses[i].volumen[j];
                entradas++;
            }
        }
    }

    // Verificar si hay datos
    if (entradas == 0) 
	{
        printf("No se encontraron datos para ese embalse.\n");
        for (int i = 0; i < totalNombres; i++) free(nombres[i]);
        free(nombres);
        return;
    }

    // Calcular la media de los volúmenes
    float media = (float)suma / entradas;

    // Calcular la suma de los cuadrados de las diferencias respecto a la media
    float sumaCuadrados = 0.0;
    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].embalse, embalseElegido) == 0) 
		{
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                float diff = embalses[i].volumen[j] - media;
                sumaCuadrados += diff * diff;
            }
        }
    }

    // Calcular la desviación típica como raíz cuadrada de la varianza
    float desviacionTipica = sqrt(sumaCuadrados / entradas);

    printf("Desviacion tipica del embalse '%s': %.2f hectometros cubicos\n",
           embalseElegido, desviacionTipica);

    // Liberar memoria dinámica
    for (int i = 0; i < totalNombres; i++) free(nombres[i]);
    free(nombres);
}


// Esta función calcula la desviación típica de los volúmenes de agua para una cuenca 
void Desviaciontipicaporcuenca(Embalse* embalses, int nEmbalses) 
{
    // Inicializar una lista dinámica de nombres de cuencas
    int capacidad = 10;
    int totalCuencas = 0;
    char** cuencas = (char**)malloc(capacidad * sizeof(char*));

    // Recorrer todos los embalses para obtener una lista única de cuencas
    for (int i = 0; i < nEmbalses; i++) 
	{
        int existe = 0;
        for (int j = 0; j < totalCuencas; j++) 
		{
            if (strcmp(cuencas[j], embalses[i].cuenca) == 0) 
			{
                existe = 1;
                break;
            }
        }
        // Si la cuenca no está aún en la lista, se añade
        if (!existe) 
		{
            if (totalCuencas == capacidad) 
			{
                capacidad *= 2;
                cuencas = (char**)realloc(cuencas, capacidad * sizeof(char*));
            }
            cuencas[totalCuencas++] = strdup(embalses[i].cuenca);
        }
    }

    printf("\n=== LISTA DE CUENCAS DISPONIBLES ===\n");
    for (int i = 0; i < totalCuencas; i++) 
	{
        printf("%d. %s\n", i + 1, cuencas[i]);
    }

    int seleccion = 0;
    printf("Selecciona el numero de la cuenca para calcular la desviacion tipica: ");
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalCuencas) 
	{
        printf("Seleccion no valida.\n");
        while (getchar() != '\n'); 
        return;
    }
    getchar(); 

    char* cuencaElegida = cuencas[seleccion - 1];

    // Calcular la suma total de volúmenes y el número total de entradas
    int suma = 0, entradas = 0;
    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].cuenca, cuencaElegida) == 0) 
		{
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                suma += embalses[i].volumen[j];
                entradas++;
            }
        }
    }

    // Si no hay datos, finalizar la función con aviso
    if (entradas == 0) 
	{
        printf("No se encontraron datos para esa cuenca.\n");
        for (int i = 0; i < totalCuencas; i++) free(cuencas[i]);
        free(cuencas);
        return;
    }
    
    float media = (float)suma / entradas;

    float sumaCuadrados = 0.0;
    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].cuenca, cuencaElegida) == 0) 
		{
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                float diff = embalses[i].volumen[j] - media;
                sumaCuadrados += diff * diff;
            }
        }
    }

    // Calcular desviación típica como raíz cuadrada de la varianza
    float desviacionTipica = sqrt(sumaCuadrados / entradas);

    printf("Desviacion tipica de la cuenca '%s': %.2f hectometros cubicos\n",
           cuencaElegida, desviacionTipica);

    // Liberar memoria usada por la lista de cuencas
    for (int i = 0; i < totalCuencas; i++) free(cuencas[i]);
    free(cuencas);
}

// Esta función calcula el volumen total acumulado de un embalse seleccionado por el usuario
void sumarVolumenTotalPorEmbalse(Embalse* embalses, int nEmbalses) 
{
    // Crear una lista dinámica para almacenar nombres únicos de embalses
    int capacidad = 10;
    int totalEmbalses = 0;
    char** nombresEmbalses = (char**)malloc(capacidad * sizeof(char*));

    // Recorremos los datos para construir la lista única de embalses
    for (int i = 0; i < nEmbalses; i++) 
	{
        int existe = 0;
        for (int j = 0; j < totalEmbalses; j++) 
		{
            if (strcmp(nombresEmbalses[j], embalses[i].embalse) == 0) 
			{
                existe = 1;
                break;
            }
        }
        // Si el embalse no está aún en la lista, lo añadimos
        if (!existe) 
		{
            if (totalEmbalses == capacidad) 
			{
                capacidad *= 2;
                nombresEmbalses = (char**)realloc(nombresEmbalses, capacidad * sizeof(char*));
            }
            nombresEmbalses[totalEmbalses++] = strdup(embalses[i].embalse);
        }
    }

    // Mostrar menú con todos los embalses disponibles
    printf("\n=== LISTA DE EMBALSES DISPONIBLES ===\n");
    for (int i = 0; i < totalEmbalses; i++) 
	{
        printf("%d. %s\n", i + 1, nombresEmbalses[i]);
    }

    // Solicitar al usuario la selección del embalse
    int seleccion;
    printf("Selecciona el embalse: ");
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalEmbalses) 
	{
        printf("Seleccion no valida.\n");
        while (getchar() != '\n');
        return;
    }
    getchar(); 
    char* embalseSeleccionado = nombresEmbalses[seleccion - 1];

    // Calcular el volumen total del embalse seleccionado
    long sumaTotal = 0;
    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].embalse, embalseSeleccionado) == 0) 
		{
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                sumaTotal += embalses[i].volumen[j];
            }
        }
    }

    // Mostrar resultado al usuario
    printf("El volumen acumulado total del embalse '%s' es de: %ld hectometros cubicos\n", embalseSeleccionado, sumaTotal);

    // Liberar memoria dinámica de la lista de nombres
    for (int i = 0; i < totalEmbalses; i++) 
	{
        free(nombresEmbalses[i]);
    }
    free(nombresEmbalses);
}

// Imprime en consola un gráfico de barras que representa la evolución anual del volumen de agua de un embalse.
void graficarEvolucionAnualPorEmbalse(Embalse* embalses, int nEmbalses) 
{
    int capacidad = 10;
    int totalEmbalses = 0;
    char** nombres = (char**)malloc(capacidad * sizeof(char*));

    for (int i = 0; i < nEmbalses; i++) 
	{
        int existe = 0;
        for (int j = 0; j < totalEmbalses; j++) 
		{
            if (strcmp(nombres[j], embalses[i].embalse) == 0) 
			{
                existe = 1;
                break;
            }
        }
        if (!existe) 
		{
            if (totalEmbalses == capacidad)
			{
                capacidad *= 2;
                nombres = (char**)realloc(nombres, capacidad * sizeof(char*));
            }
            nombres[totalEmbalses++] = strdup(embalses[i].embalse);
        }
    }

    printf("\n=== LISTA DE EMBALSES DISPONIBLES ===\n");
    for (int i = 0; i < totalEmbalses; i++) 
	{
        printf("%d. %s\n", i + 1, nombres[i]);
    }

    int seleccion;
    printf("Selecciona el numero del embalse para graficar su evolucion anual: ");
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalEmbalses) 
	{
        printf("Seleccion no valida.\n");
        while (getchar() != '\n');
        return;
    }
    getchar();

    char* embalseElegido = nombres[seleccion - 1];
    int anios[10] = {2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021};
    int volumenPorAnio[10] = {0};
    int maxValor = 0;

    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].embalse, embalseElegido) == 0) 
		{
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                volumenPorAnio[j] += embalses[i].volumen[j];
                if (volumenPorAnio[j] > maxValor) 
				{
                    maxValor = volumenPorAnio[j];
                }
            }
        }
    }

    // Determinar escala automáticamente
    int escala;
    if (maxValor > 1000) 
	{
        escala = 100;
    } else if (maxValor > 100) 
	{
        escala = 50;
    } else 
	{
        escala = 10;
    }

    printf("\n=== GRAFICO DE BARRAS PARA '%s' ===\n", embalseElegido);
    printf(" Cada '#' representa %d hm3 \n", escala);
    for (int i = 0; i < 10; i++) 
	{
        printf("%d | ", anios[i]);
        int altura = volumenPorAnio[i] / escala;
        for (int j = 0; j < altura; j++) 
		{
            printf("#");
        }
        printf(" (%d hm3)\n", volumenPorAnio[i]);
    }

    for (int i = 0; i < totalEmbalses; i++) free(nombres[i]);
    free(nombres);
}

// Esta función pide al usuario  una cuenca y muestra el embalse que ha acumulado más agua
void embalseConMasAguaPorCuenca(Embalse* embalses, int nEmbalses) 
{
    int capacidad = 10;
    int totalCuencas = 0;
    char** cuencas = (char**)malloc(capacidad * sizeof(char*));

    // Construir una lista dinámica con todas las cuencas únicas
    for (int i = 0; i < nEmbalses; i++) 
	{
        int existe = 0;
        for (int j = 0; j < totalCuencas; j++) 
		{
            if (strcmp(cuencas[j], embalses[i].cuenca) == 0)
			{
                existe = 1;
                break;
            }
        }
        if (!existe) {
            if (totalCuencas == capacidad) 
			{
                capacidad *= 2;
                cuencas = (char**)realloc(cuencas, capacidad * sizeof(char*));
            }
            cuencas[totalCuencas++] = strdup(embalses[i].cuenca);
        }
    }

    // Mostrar el menú de cuencas disponibles
    printf("\n=== CUENCAS DISPONIBLES ===\n");
    for (int i = 0; i < totalCuencas; i++) 
	{
        printf("%d. %s\n", i + 1, cuencas[i]);
    }

    // Leer la selección del usuario
    int seleccion = 0;
    printf("Selecciona el numero de la cuenca: ");
    if (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > totalCuencas) 
	{
        printf("Seleccion no valida.\n");
        while (getchar() != '\n');
        return;
    }
    getchar(); 

    char* cuencaElegida = cuencas[seleccion - 1];

    // Variables para rastrear el embalse con mayor volumen total
    char* embalseMax = NULL;
    int volumenMax = -1;

    // Recorrer todos los embalses para encontrar el que más agua acumuló
    for (int i = 0; i < nEmbalses; i++) 
	{
        if (strcmp(embalses[i].cuenca, cuencaElegida) == 0) 
		{
            int suma = 0;
            for (int j = 0; j < embalses[i].nVolumenes; j++) 
			{
                suma += embalses[i].volumen[j];
            }
            if (suma > volumenMax) 
			{
                volumenMax = suma;
                embalseMax = embalses[i].embalse;
            }
        }
    }

    // Mostrar el resultado
    if (embalseMax != NULL) 
	{
        printf("El embalse con mas agua estancada en la cuenca '%s' es: '%s' con %d hecometros cubicos acumulados.\n",
               cuencaElegida, embalseMax, volumenMax);
    } 
	else 
	{
        printf("No se encontraron datos para esa cuenca.\n");
    }

    // Liberar memoria de la lista de cuencas
    for (int i = 0; i < totalCuencas; i++) 
	{
        free(cuencas[i]);
    }
    free(cuencas);
}
// Función para liberar datos
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
    
// Función principal del programa: carga los datos, muestra el menú y permite al usuario elegir análisis a realizar.
int main(void) 
{
    setlocale(LC_ALL, "Spanish");

    Embalse* embalses = NULL;
    int nEmbalses = 0;
    int nVolumenes = 0;
    char nombreFichero[256];
    
    // Bienvenida al codigo
    printf(" BIENVENIDO AL PROGRAMA DE ANALISIS DE DATOS DE MITECO ----> CuencaStast \n");
    printf("\n Hemos registrado los datos relativos a los volumenes de agua almacenada, en los embalses espanoles, agrupados por cuencas hidrograficas.\n");
    printf("\n Y a partir de este programa, creado por :\n ");
    printf("\n 1-Marcos Tejero Monedero \n");
    printf("\n 2-Miguel Martinez-Lacaci \n");
    printf("\n 3-Juan Moreno Aymerich \n");
    printf("\n 4-Manuel Lopez Romera \n");
    printf("\n Podras elegir diferentes opciones, para estudiar este conjunto de datos, de manera exacta y efectiva. \n");
    printf("\n Estas listo? \n");
    printf("\n \n Para ello, deberas de introducir el nombre del fichero CSV que le hemos proporcinado (dataset.csv): ");
    fgets(nombreFichero, sizeof(nombreFichero), stdin);
    nombreFichero[strcspn(nombreFichero, "\n")] = 0;

    embalses = cargarDatos(nombreFichero, &nEmbalses, &nVolumenes);
    if (!embalses) 
	{
        printf("No se pudo abrir el archivo o esta vacio.\n");
        return 1;
    }

    printf("Archivo cargado correctamente (%d registros).\n", nEmbalses);
    printf("\n -------BIENVENIDO------ \n");

    char continuar = 's';
    while (continuar == 's' || continuar == 'S') 
	{
        int opcion;
        printf("\n=== MENU DE OPCIONES ===\n");
        printf("1. Calcular media mensual por cuenca\n");
        printf("2. Calcular media anual por cuenca\n");
        printf("3. Calcular media (Anual o Mensual) por embalse\n");
        printf("4. Calcular evolucion del agua estancada a lo largo del tiempo\n");
        printf("5. Mostrar embalse con mas agua estancada en una cuenca\n");
	    printf("6. Detectar periodos anomalos\n");
 	    printf("7. Calcular moda por embalse\n");
        printf("8. Calcular desviacion tipica por embalse\n");
        printf("9. Calcular desviacion tipica por cuenca\n");
	    printf("10. Volumen total acumulado por embalse\n");
        printf("11. Graficar evolucion anual de un embalse\n");
        printf("12. Salir\n");
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
                Evolucion_de_agua_estancada(embalses, nEmbalses);
                break;
                
            case 5:
                embalseConMasAguaPorCuenca(embalses, nEmbalses);
                break;
                
            case 6:
                detectarAnomalias(embalses, nEmbalses, nVolumenes);
                break;
                
	        case 7:
		        calcularModa(embalses, nEmbalses);
	       	    break;
	       	    
            case 8:
                Desviaciontipica(embalses, nEmbalses);
                break;
                
            case 9:
		        Desviaciontipicaporcuenca(embalses, nEmbalses);
		        break;
		        
            case 10:
		        sumarVolumenTotalPorEmbalse(embalses, nEmbalses);
		        break;
		        
            case 11:
                graficarEvolucionAnualPorEmbalse(embalses, nEmbalses);
                break;
                
	         case 12:
		        liberarDatos(embalses, nEmbalses);
                printf("Programa finalizado.\n");
                system("pause");
                return 0;
                
            default:
                printf("Opcion no valida.\n");
        }

        printf("\n¿Deseas ejecutar otra funcion?: ");
        scanf(" %c", &continuar);
        getchar();
    }

    liberarDatos(embalses, nEmbalses);
    printf("Programa finalizado.\n");
    return 0;
}
