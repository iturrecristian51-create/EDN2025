#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==========================
   ESTRUCTURAS DE DATOS
   ========================== */

   
typedef struct Pedido {
    char destino[50];
    int cantidad;
    struct Pedido* sig;
} Pedido;

typedef struct NodoAVL {
    int fecha;                 // AAAAMMDD
    char producto[50];
    int stock;
    int altura;
    Pedido* cabeza_pedidos;    // Cola FIFO
    struct NodoAVL* izq;
    struct NodoAVL* der;
} NodoAVL;

/* ==========================
   FUNCIONES AUXILIARES AVL
   ========================== */

int max(int a, int b) {
    return (a > b) ? a : b;
}

int altura(NodoAVL* n) {
    return n ? n->altura : 0;
}

NodoAVL* nuevoNodo(int fecha, char* producto, int stock) {
    NodoAVL* n = (NodoAVL*)malloc(sizeof(NodoAVL));
    n->fecha = fecha;
    strcpy(n->producto, producto);
    n->stock = stock;
    n->altura = 1;
    n->cabeza_pedidos = NULL;
    n->izq = n->der = NULL;
    return n;
}

NodoAVL* rotarDerecha(NodoAVL* y) {
    NodoAVL* x = y->izq;
    NodoAVL* T2 = x->der;

    x->der = y;
    y->izq = T2;

    y->altura = max(altura(y->izq), altura(y->der)) + 1;
    x->altura = max(altura(x->izq), altura(x->der)) + 1;

    return x;
}

NodoAVL* rotarIzquierda(NodoAVL* x) {
    NodoAVL* y = x->der;
    NodoAVL* T2 = y->izq;

    y->izq = x;
    x->der = T2;

    x->altura = max(altura(x->izq), altura(x->der)) + 1;
    y->altura = max(altura(y->izq), altura(y->der)) + 1;

    return y;
}

int factorBalance(NodoAVL* n) {
    return n ? altura(n->izq) - altura(n->der) : 0;
}

/* ==========================
   INSERCIÓN AVL
   ========================== */

NodoAVL* insertarAVL(NodoAVL* nodo, int fecha, char* producto, int stock) {
    if (!nodo)
        return nuevoNodo(fecha, producto, stock);

    if (fecha < nodo->fecha)
        nodo->izq = insertarAVL(nodo->izq, fecha, producto, stock);
    else if (fecha > nodo->fecha)
        nodo->der = insertarAVL(nodo->der, fecha, producto, stock);
    else {
        printf("Fecha ya existe. No se puede registrar el producto.\n");
        return nodo;
    }

    nodo->altura = 1 + max(altura(nodo->izq), altura(nodo->der));

    int balance = factorBalance(nodo);

    if (balance > 1 && fecha < nodo->izq->fecha)
        return rotarDerecha(nodo);

    if (balance < -1 && fecha > nodo->der->fecha)
        return rotarIzquierda(nodo);

    if (balance > 1 && fecha > nodo->izq->fecha) {
        nodo->izq = rotarIzquierda(nodo->izq);
        return rotarDerecha(nodo);
    }

    if (balance < -1 && fecha < nodo->der->fecha) {
        nodo->der = rotarDerecha(nodo->der);
        return rotarIzquierda(nodo);
    }

    return nodo;
}

/* ==========================
   COLA FIFO
   ========================== */

void encolarPedido(NodoAVL* nodo, char* destino, int cantidad) {
    Pedido* nuevo = (Pedido*)malloc(sizeof(Pedido));
    strcpy(nuevo->destino, destino);
    nuevo->cantidad = cantidad;
    nuevo->sig = NULL;

    if (!nodo->cabeza_pedidos) {
        nodo->cabeza_pedidos = nuevo;
    } else {
        Pedido* aux = nodo->cabeza_pedidos;
        while (aux->sig)
            aux = aux->sig;
        aux->sig = nuevo;
    }
}

int contarPedidos(Pedido* p) {
    int c = 0;
    while (p) {
        c++;
        p = p->sig;
    }
    return c;
}

void liberarCola(Pedido* p) {
    while (p) {
        Pedido* tmp = p;
        p = p->sig;
        free(tmp);
    }
}

/* ==========================
   BÚSQUEDAS
   ========================== */

NodoAVL* buscarMin(NodoAVL* nodo) {
    while (nodo && nodo->izq)
        nodo = nodo->izq;
    return nodo;
}

NodoAVL* buscarFecha(NodoAVL* nodo, int fecha) {
    if (!nodo || nodo->fecha == fecha)
        return nodo;
    if (fecha < nodo->fecha)
        return buscarFecha(nodo->izq, fecha);
    return buscarFecha(nodo->der, fecha);
}

/* ==========================
   ELIMINACIÓN AVL
   ========================== */

NodoAVL* eliminarAVL(NodoAVL* raiz, int fecha) {
    if (!raiz)
        return raiz;

    if (fecha < raiz->fecha)
        raiz->izq = eliminarAVL(raiz->izq, fecha);
    else if (fecha > raiz->fecha)
        raiz->der = eliminarAVL(raiz->der, fecha);
    else {
        liberarCola(raiz->cabeza_pedidos);

        if (!raiz->izq || !raiz->der) {
            NodoAVL* temp = raiz->izq ? raiz->izq : raiz->der;
            if (!temp) {
                temp = raiz;
                raiz = NULL;
            } else
                *raiz = *temp;
            free(temp);
        } else {
            NodoAVL* temp = buscarMin(raiz->der);
            raiz->fecha = temp->fecha;
            strcpy(raiz->producto, temp->producto);
            raiz->stock = temp->stock;
            raiz->cabeza_pedidos = temp->cabeza_pedidos;
            raiz->der = eliminarAVL(raiz->der, temp->fecha);
        }
    }

    if (!raiz)
        return raiz;

    raiz->altura = 1 + max(altura(raiz->izq), altura(raiz->der));
    int balance = factorBalance(raiz);

    if (balance > 1 && factorBalance(raiz->izq) >= 0)
        return rotarDerecha(raiz);

    if (balance > 1 && factorBalance(raiz->izq) < 0) {
        raiz->izq = rotarIzquierda(raiz->izq);
        return rotarDerecha(raiz);
    }

    if (balance < -1 && factorBalance(raiz->der) <= 0)
        return rotarIzquierda(raiz);

    if (balance < -1 && factorBalance(raiz->der) > 0) {
        raiz->der = rotarDerecha(raiz->der);
        return rotarIzquierda(raiz);
    }

    return raiz;
}

/* ==========================
   REPORTE IN-ORDER
   ========================== */

void reporteInOrder(NodoAVL* raiz) {
    if (!raiz) return;
    reporteInOrder(raiz->izq);
    printf("Fecha: %d | Producto: %s | Stock: %d | Pedidos en espera: %d\n",
           raiz->fecha, raiz->producto, raiz->stock, contarPedidos(raiz->cabeza_pedidos));
    reporteInOrder(raiz->der);
}

/* ==========================
   MENÚ PRINCIPAL
   ========================== */

int main() {
    NodoAVL* raiz = NULL;
    int opcion;

    do {
        printf("\n--- SISTEMA LOGÍSTICO PUERTO BUENAVENTURA ---\n");
        printf("1. Recepción de mercancía\n");
        printf("2. Registrar pedido de despacho\n");
        printf("3. Baja de producto\n");
        printf("4. Reporte de estado\n");
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        if (opcion == 1) {
            int fecha, stock;
            char producto[50];
            printf("Fecha (AAAAMMDD): ");
            scanf("%d", &fecha);
            printf("Producto: ");
            scanf("%s", producto);
            printf("Cantidad: ");
            scanf("%d", &stock);
            raiz = insertarAVL(raiz, fecha, producto, stock);
        }
        else if (opcion == 2) {
            NodoAVL* n = buscarMin(raiz);
            if (!n) {
                printf("No hay productos disponibles.\n");
            } else {
                char destino[50];
                int cant;
                printf("Destino: ");
                scanf("%s", destino);
                printf("Cantidad solicitada: ");
                scanf("%d", &cant);
                if (cant <= n->stock) {
                    encolarPedido(n, destino, cant);
                    n->stock -= cant;
                } else {
                    printf("Stock insuficiente.\n");
                }
            }
        }
        else if (opcion == 3) {
            int fecha;
            printf("Fecha a eliminar: ");
            scanf("%d", &fecha);
            raiz = eliminarAVL(raiz, fecha);
        }
        else if (opcion == 4) {
            reporteInOrder(raiz);
        }

    } while (opcion != 0);

    return 0;
}
