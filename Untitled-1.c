#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pasajero {
    char genero;  // 'F', 'M', 'N'
    char apellido[50];
    struct pasajero *sig;
} pasajero;

// Variables globales
pasajero *lista_tiquetes = NULL;
pasajero *cola_tiquetes = NULL;
int capacidad_maxima = 0;
int tiquetes_vendidos = 0;
int abordaje_iniciado = 0;

// Prototipos de funciones
void establecer_capacidad();
void vender_tiquete();
void iniciar_abordaje();
void ver_abordados();
void ver_no_abordados();
void liberar_memoria();

int main() {
    int opcion;
    
    printf("=== SISTEMA DE GESTIÓN DE TIQUETES DE AVIÓN ===\n");
    
    do {
        printf("\n=== MENÚ PRINCIPAL ===\n");
        printf("1. Establecer Capacidad del Avión\n");
        printf("2. Vender Tiquete\n");
        printf("3. Iniciar Abordaje\n");
        printf("4. Ver Pasajeros Abordados\n");
        printf("5. Ver Pasajeros No Abordados\n");
        printf("6. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        
        switch(opcion) {
            case 1:
                establecer_capacidad();
                break;
            case 2:
                vender_tiquete();
                break;
            case 3:
                iniciar_abordaje();
                break;
            case 4:
                ver_abordados();
                break;
            case 5:
                ver_no_abordados();
                break;
            case 6:
                printf("Saliendo del sistema...\n");
                break;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 6);
    
    liberar_memoria();
    return 0;
}

void establecer_capacidad() {
    if (capacidad_maxima > 0) {
        printf("La capacidad ya fue establecida anteriormente.\n");
        return;
    }
    
    printf("Ingrese la capacidad máxima de pasajeros del avión: ");
    scanf("%d", &capacidad_maxima);
    
    if (capacidad_maxima <= 0) {
        printf("La capacidad debe ser un número positivo.\n");
        capacidad_maxima = 0;
        return;
    }
    
    printf("Capacidad establecida en: %d pasajeros\n", capacidad_maxima);
    printf("Límite de sobreventa (10%%): %d tiquetes\n", (int)(capacidad_maxima * 1.1));
}

void vender_tiquete() {
    if (capacidad_maxima == 0) {
        printf("ERROR: Primero debe establecer la capacidad del avión.\n");
        return;
    }
    
    if (abordaje_iniciado) {
        printf("ERROR: El abordaje ya inició. No se pueden vender más tiquetes.\n");
        return;
    }
    
    int limite_sobreventa = (int)(capacidad_maxima * 1.1);
    if (tiquetes_vendidos >= limite_sobreventa) {
        printf("ERROR: Límite de sobreventa alcanzado (%d tiquetes).\n", limite_sobreventa);
        return;
    }
    
    // Crear nuevo pasajero
    pasajero *nuevo = (pasajero*)malloc(sizeof(pasajero));
    
    printf("\n--- VENTA DE TIQUETE ---\n");
    
    // Solicitar género
    printf("Género (F=Femenino, M=Masculino, N=No Binario): ");
    scanf(" %c", &nuevo->genero);
    
    // Validar género
    if (nuevo->genero != 'F' && nuevo->genero != 'M' && nuevo->genero != 'N') {
        printf("ERROR: Género no válido. Use F, M o N.\n");
        free(nuevo);
        return;
    }
    
    // Solicitar apellido
    printf("Primer apellido: ");
    scanf("%s", nuevo->apellido);
    
    nuevo->sig = NULL;
    
    // Agregar a la lista enlazada (al final para mantener orden de venta)
    if (lista_tiquetes == NULL) {
        lista_tiquetes = nuevo;
        cola_tiquetes = nuevo;
    } else {
        cola_tiquetes->sig = nuevo;
        cola_tiquetes = nuevo;
    }
    
    tiquetes_vendidos++;
    printf("Tiquete vendido exitosamente a: %s\n", nuevo->apellido);
    printf("Tiquetes vendidos: %d/%d\n", tiquetes_vendidos, limite_sobreventa);
}

void iniciar_abordaje() {
    if (capacidad_maxima == 0) {
        printf("ERROR: Primero debe establecer la capacidad del avión.\n");
        return;
    }
    
    if (abordaje_iniciado) {
        printf("El abordaje ya fue iniciado anteriormente.\n");
        return;
    }
    
    abordaje_iniciado = 1;
    printf("\n=== INICIANDO PROCESO DE ABORDAJE ===\n");
    printf("Capacidad del avión: %d pasajeros\n", capacidad_maxima);
    printf("Tiquetes vendidos: %d\n", tiquetes_vendidos);
    printf("\n");
    
    pasajero *actual = lista_tiquetes;
    int pasajeros_abordados = 0;
    
    // Abordar pasajeros en orden de venta hasta alcanzar capacidad máxima
    while (actual != NULL && pasajeros_abordados < capacidad_maxima) {
        printf("Abordando: %s (%c)\n", actual->apellido, actual->genero);
        pasajeros_abordados++;
        actual = actual->sig;
    }
    
    printf("\n=== ABORDAJE COMPLETADO ===\n");
    printf("Pasajeros que abordaron: %d\n", pasajeros_abordados);
    printf("Pasajeros que no pudieron abordar: %d\n", tiquetes_vendidos - pasajeros_abordados);
}

void ver_abordados() {
    if (!abordaje_iniciado) {
        printf("ERROR: El abordaje no ha iniciado.\n");
        return;
    }
    
    printf("\n=== LISTA DE PASAJEROS ABORDADOS ===\n");
    
    pasajero *actual = lista_tiquetes;
    int contador = 0;
    
    while (actual != NULL && contador < capacidad_maxima) {
        printf("%d. %s (%c)\n", contador + 1, actual->apellido, actual->genero);
        contador++;
        actual = actual->sig;
    }
    
    if (contador == 0) {
        printf("No hay pasajeros abordados.\n");
    } else {
        printf("Total: %d pasajeros\n", contador);
    }
}

void ver_no_abordados() {
    if (!abordaje_iniciado) {
        printf("ERROR: El abordaje no ha iniciado.\n");
        return;
    }
    
    if (tiquetes_vendidos <= capacidad_maxima) {
        printf("Todos los pasajeros con tiquete pudieron abordar el avión.\n");
        return;
    }
    
    printf("\n=== LISTA DE PASAJEROS NO ABORDADOS ===\n");
    
    pasajero *actual = lista_tiquetes;
    int contador = 0;
    
    // Avanzar hasta el primer pasajero no abordado
    while (actual != NULL && contador < capacidad_maxima) {
        contador++;
        actual = actual->sig;
    }
    
    int no_abordados = 1;
    while (actual != NULL) {
        printf("%d. %s (%c)\n", no_abordados, actual->apellido, actual->genero);
        no_abordados++;
        actual = actual->sig;
    }
    
    printf("Total: %d pasajeros\n", no_abordados - 1);
}

void liberar_memoria() {
    pasajero *actual = lista_tiquetes;
    pasajero *temp;
    
    while (actual != NULL) {
        temp = actual;
        actual = actual->sig;
        free(temp);
    }
    
    lista_tiquetes = NULL;
    cola_tiquetes = NULL;
}