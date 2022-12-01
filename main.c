#include <math.h>
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include "list.h"
#include "treemap.h"
//#include "combate.h"
//#include "grafo.h"

//Declaracion Struct

typedef struct{ //estructura con las estadisticas de objeto

    char Nombre_Item[30];
    char Tipo[30];
    int Dano_vida_Armadura;
    int Nivel_o;

}Objeto;

typedef struct{ // estructura para el personaje

    char Nombre_Usuario[30];
    int Vida;
    int Oro;
    Objeto* Arma;
    Objeto* Armadura;

}Personaje;

typedef struct{  //estructura para guardar informacion del usuario y mostarla en los marcadores

    char Nombre_Usuario[30];
    int Oro;

}Usuario;

typedef struct{ //estructura para estadisticas de enemigos

    char Nombre_Enemigo[30];
    int Vida;
    int Dano;
    int Nivel_e;

}Enemigo;

typedef struct NodoGrafo NodoGrafo;

struct NodoGrafo{ // estructura para poder determinar los eventos si se encuentra un arma o un enemigo

    Enemigo* enemigo;
    Objeto* objeto;

};

typedef struct{ //para poder navegar el grafo-mapa
    
    TreeMap* Niveles;
    NodoGrafo* nodoactual;

}Grafo;

typedef struct{ // estructura para poder saber que ruta se esta tomando

    char nombreNivel[30];
    char camino_der[30];
    char camino_izq[30];

}Nivel;

//Funcion para leer el k-esimo elemento de un string (separado por comas)
char *get_csv_field(char *linea, int indice){
    char *campo = (char *) malloc(100 * sizeof(char *)); // Guarda el string a retornar
    int i = 0; // Recorre la linea
    int k = 0; // Cuenta las comas
    int n = 0; // Recorre el campo
    bool coma = false;
    while(linea[i] != '\0'){
        if(linea[i] == '\"'){
            coma = !coma;
        }
        if(k == indice){
            if(linea[i] != '\"'){
                campo[n] = linea[i];
                n++;
            }
        }
        i++;
        if(linea[i] == ',' && !coma){
            k++;
            i++;
        }
        if(k > indice || linea[i] == '\0' || linea[i] == '\n'){
            campo[n] = '\0';
            return campo;
        }
    }
    return NULL;
}

//menus que contienen las opciones que el usuario pueda saber que comandos hace cada opcion
void menu();
void submenu();
void submenuinventario();
void submenucombate();

int lower_than_int(void* , void* ); // para saber si un valor es mayor que o menor que

//cargar objetos y enemigos, despues cargar los niveles
void Cargarenemigos(HashMap*);
void Cargarobjetos(HashMap*);
void CargarNiveles(Grafo*);

void agregarJugador(TreeMap*, void*, Usuario*);
void mostrarmarcadores(TreeMap *, Pair *, Pair* (avanzar)(TreeMap *mapa));
void exportarmarcadores(TreeMap*);
void avanzar(Pair*);
void Mostrarinventario();
void Mostrarestado();
bool esenemigo(Grafo*);
void combatir(Enemigo*);
void tomarobjeto();
void CargarMarcadores(TreeMap*);

//tdas y estructuras de datos
Grafo* grafo;
HashMap* Mapaenemigo;
HashMap* Mapaobjeto;
HashMap* Mapainventario;
Personaje* personaje;
Enemigo* jefe;

int main(){ // se inicia el programa
     // declarion de variables
    int opcion_p;
    
    TreeMap* Marcadores = createTreeMap(lower_than_int);
    Mapaenemigo = createMap(55);
    Mapaobjeto = createMap(55);
    CargarMarcadores(Marcadores);
    Cargarenemigos(Mapaenemigo);
    Cargarobjetos(Mapaobjeto);
    grafo = (Grafo*)calloc(1, sizeof(Grafo));
    grafo->nodoactual = (NodoGrafo*)calloc(1, sizeof(NodoGrafo));
    grafo->Niveles = NULL;
    CargarNiveles(grafo);
    personaje = (Personaje*)malloc(sizeof(Personaje));
    Usuario* u = (Usuario*)malloc(sizeof(Usuario));

    menu(); // no se introduce un numero o opcion valida
    scanf("%d",&opcion_p);
    while(opcion_p<1||opcion_p>3){
        printf("Introdusca un numero valido: ");
        scanf("%d", &opcion_p);
    }
    Pair* aux_avan;
    while (1){ // cuando se obtiene un valor valido
        switch (opcion_p){
	        case 1:{ //iniciar partida
                int opcion_j;
                char ing_nombre[30];
                
                Mapainventario = createMap(55);

                Objeto* aux_objeto1 = (Objeto*)malloc(sizeof(Objeto));
                Pair_2* aux_pairO = (Pair_2*)malloc(sizeof(Pair_2));
               
               // se agrega el equipamiento basico al jugador
                aux_pairO = searchMap(Mapaobjeto, "Ropa de campesino");
                aux_objeto1 = aux_pairO->value;
                personaje->Armadura = aux_objeto1;
                insertMap(Mapainventario, aux_objeto1->Nombre_Item, aux_objeto1);

                aux_pairO = searchMap(Mapaobjeto, "Punos");
                aux_objeto1 = aux_pairO->value;
                personaje->Arma = aux_objeto1;
                insertMap(Mapainventario, aux_objeto1->Nombre_Item, aux_objeto1);

                aux_pairO = searchMap(Mapaobjeto, "Pan");
                aux_objeto1 = aux_pairO->value;
                insertMap(Mapainventario, aux_objeto1->Nombre_Item, aux_objeto1);

                personaje->Vida = 100;
                personaje->Oro = 0;

                printf("Ingrese nombre personaje: ");
                fflush(stdin);
                scanf("%[^\n]", ing_nombre);
                strcpy(personaje->Nombre_Usuario, ing_nombre);
                printf("\n");

                submenu(); // submenu para poder dar las opciones para empezar a jugar
                fflush(stdin);
                scanf("%d", &opcion_j);

                aux_avan = firstTreeMap(grafo->Niveles);

                while(1){
                    switch (opcion_j){
                        case 1:{   // se avanza por el mapa 
                            avanzar(aux_avan);
                            aux_avan = nextTreeMap(grafo->Niveles);
                            break;
                        }
                        case 2:{ // abre el inventario
                            Mostrarinventario();
                            break;
                        }
                        case 3:{ //  muestra la vida, arma, armadura y oro del personaje
                            Mostrarestado();
                            break;
                        }
                    }
                        // matas al jefe final y se termina la partida
                    if (strcmp(jefe->Nombre_Enemigo, "cuecue") == 0 && jefe->Vida <= 0){
                        personaje->Oro += 1000;
                        strcpy(u->Nombre_Usuario,personaje->Nombre_Usuario);
                        u->Oro = personaje->Oro;
                        agregarJugador(Marcadores, &(u->Oro), u);
                        exportarmarcadores(Marcadores);
                        printf("Derrotaste al jefe\n");
                        printf("Aventura finalizada");
                        return 0;
                    }

                        // cuando tu vida llega a cero mueres y termina la partida
                    if (opcion_j == 4 || (personaje->Vida <= 0)){ 
                        strcpy(u->Nombre_Usuario,personaje->Nombre_Usuario);
                        u->Oro = personaje->Oro;
                        agregarJugador(Marcadores, &(u->Oro), u);
                        exportarmarcadores(Marcadores);
                        printf("Aventura finalizada");
                        aux_avan = firstTreeMap(grafo->Niveles);
                        return 0;
                    }
                    submenu(); //se vuelve a proyectar el submenu
                   
                    scanf("%d", &opcion_j); // no se introduce una opcion valida
                    while(opcion_j<1||opcion_j>4){
                        printf("Introdusca un numero valido: ");
                        scanf("%d", &opcion_j);
                    }
                }
                break;
            }
            case 2:{ // mostrar marcadores se refiere a las puntuaciones con el nombre como si de un arcade se tratara

                mostrarmarcadores(Marcadores, firstTreeMap(Marcadores), nextTreeMap);
                break;

            }
        }
        if (opcion_p==3){ //se sale del juego
            printf("Programa finalizado");
            break;
        }
        printf("\n");
        menu();
        fflush(stdin);
        scanf("%d", &opcion_p);
        while(opcion_p<1||opcion_p>3){ // no se ingresa una opcion valida
            printf("Introdusca un numero valido: ");
            scanf("%d", &opcion_p);
        }
    }
    
    return 0;
}

void menu(){ // muestra al usuario las opciones de pantalla de inicio
    printf("1. Iniciar Partida\n");
    printf("2. Ver Macadores\n");
    printf("3. Salir\n");
    printf("Introdusca un numero: ");
}

void submenu(){ // ya cuando se inicia el juego al iniciar partida estas opciones se mostraran
    printf("1. Avanzar\n");
    printf("2. Mostrar inventario\n");
    printf("3. Mostrar estado\n");
    printf("4. Rendirse\n");
    printf("Introdusca un numero: ");
}

void submenucombate(){ // como su nombre lo indica son las opciones de combate
    printf("1. Atacar\n");
    printf("2. Curar\n");
    printf("3. Huir\n");
    printf("Introdusca un numero: ");
}

void submenuinventario(){ //al usar la opcion mostrar inventario estas nuevas opciones saldran
    printf("1. Equipar arma\n");
    printf("2. Equipar armadura\n");
    printf("3. Curar\n");
    printf("4. salir del inventario\n");
    printf("Introdusca un numero: ");
}

int lower_than_int(void* key1, void* key2){
    int k1 = *((int*) (key1));
    int k2 = *((int*) (key2));
    return k1<k2;
}

void agregarJugador(TreeMap* mapa, void* clave, Usuario* p) { //ingresa el nombre del usuario para poder guardarlo en los marcadores
    Pair *pair = searchTreeMap(mapa, clave);
    List *listajugador = NULL;
    // No existe la clave en el mapa
    if (pair == NULL) {
        listajugador = createList();
        insertTreeMap(mapa, clave, listajugador);
    }
    // Si existe 
    else {
        listajugador = pair->value;
    }
    pushBack(listajugador, p);
}
//funcion para poder mostrar los marcadores al finalizar partia como si de un arcade se tratase
// nombre puntaje
void mostrarmarcadores(TreeMap *mapa, Pair *primero, Pair* (avanzar)(TreeMap *mapa)) {
    Pair *pair = primero;
    while(pair) {
        List *lista = pair->value;
        Usuario* p = firstList(lista);
        while(p) {
            printf("%s / %d\n", p->Nombre_Usuario, p->Oro);
            p = nextList(lista);
        }
        pair = avanzar(mapa);
    }
}

bool esenemigo(Grafo* n){ //cuando se inicia el combate con un enemigo
    if(n->nodoactual->enemigo== NULL){
        return false;
    }
    return true;
}

void exportarmarcadores(TreeMap* m){
    char nombre_archivo2[30];
    strcpy(nombre_archivo2, "Marcadores.csv");
    FILE *fp = fopen (nombre_archivo2, "w");
    fprintf(fp,"Nombre,Puntaje\n");
    Pair *pair = lastTreeMap(m);
    while(pair) {
        List *lista = pair->value;
        Usuario* p = firstList(lista);
        while(p) {
            fprintf(fp,"%s,%d\n", p->Nombre_Usuario, p->Oro);
            p = nextList(lista);
        }
        pair = prevTreeMap(m);
    }
    fclose(fp);
    printf("\n");
}

void combatir(Enemigo* e){ // funcion que determina como se hace un combate
    char aux_objeto[30];
    int opcion_com, dano;
    Objeto* pocion = (Objeto*)malloc(sizeof(Objeto));
    Pair_2* pair_o = (Pair_2*)malloc(sizeof(Pair_2));
    jefe = (Enemigo*)malloc(sizeof(Enemigo));
    e = grafo->nodoactual->enemigo;

    while(personaje->Vida>0 && e->Vida>0){ // se acabara cuando el enemigo o la vida del personaje llegue a cero

        printf("Vida personaje: %d\n", personaje->Vida);
        printf("Nombre: %s / Vida: %d\n", e->Nombre_Enemigo, e->Vida);

        submenucombate();
        fflush(stdin);
        scanf("%d", &opcion_com);
        while(opcion_com<1||opcion_com>3){
            printf("Introdusca un numero valido: ");
            scanf("%d", &opcion_com);
        }
        switch (opcion_com){
            case 1:{ // opcion atacar
                e->Vida -= personaje->Arma->Dano_vida_Armadura;
                if (e->Vida <= 0){ //si el enemigo es derrotado
                    printf("Enemigo derrotado\n\n");
                    break;
                } 
                else{  //verifica si tienes mas armadura que el danio del enemigo por lo que no hace nada
                    if ((e->Dano - personaje->Armadura->Dano_vida_Armadura) < 0){
                        break;
                    }
                    else{ // ataque del enemigo
                       personaje->Vida -= (e->Dano - personaje->Armadura->Dano_vida_Armadura); 
                       break;
                    }
                }
            }
            case 2:{ //opcion para poder curarse
                printf("Introduzca el nombre de la pocion: ");//ingresar la pocion a usar
                fflush(stdin);
                scanf("%[^\n]", aux_objeto);

                pair_o = searchMap(Mapainventario, aux_objeto); //si notienes la pocion no te curas
                if(pair_o == NULL){
                    printf("Consumible no encontrado\n");
                    break;
                }
                else{ //utilizas la pocion, te curas y se remueve del inventario
                    pocion = pair_o->value;
                    personaje->Vida += pocion->Dano_vida_Armadura;
                    eraseMap(Mapainventario, aux_objeto);
                    break;
                }
            }
        }
        if (personaje->Vida<=0){ // mueres
            printf("You Dead\n\n");
        }
        if (opcion_com == 3){ // has escapado del combate
            printf("Cobarde...\n\n");
            personaje->Oro -= 100; //pierdes oro
            break;
        }
    }
    //estructura del jefe
    strcpy(jefe->Nombre_Enemigo, e->Nombre_Enemigo);
    jefe->Vida = e->Vida;
    jefe->Dano = e->Dano;
    jefe->Nivel_e = e->Nivel_e;
    personaje->Oro += 100;
}

void tomarobjeto(){ //funcion para añadir un objeto a tu inventario
    insertMap(Mapainventario, grafo->nodoactual->objeto->Nombre_Item, grafo->nodoactual->objeto);
}

void avanzar(Pair* aux_avan){ //cuando se ejecute la opcion avanzar
    int opcion_avan;
    Nivel* niveles = aux_avan->value;
    Pair_2* aux_pair = (Pair_2*)calloc(1,sizeof(Pair_2));
    Objeto* aux_obj = (Objeto*)calloc(1,sizeof(Objeto));
    Enemigo* aux_ene = (Enemigo*)calloc(1,sizeof(Enemigo));

    printf("Eliga un camino 1. Izquierda o 2. Derecha: ");
    fflush(stdin);
    scanf("%d", &opcion_avan);
    while (opcion_avan<1 || opcion_avan>2){
        printf("Introdusca un numero valido: ");
        fflush(stdin);
        scanf("%d", &opcion_avan);
    }
    

    if (opcion_avan == 1){ //carga grafo izquierdo

        aux_pair = searchMap(Mapaenemigo,niveles->camino_izq);
        if(aux_pair == NULL){

            aux_pair = searchMap(Mapaobjeto,niveles->camino_izq);
            aux_obj = aux_pair->value;
            grafo->nodoactual->objeto = aux_obj;

        }
        else{

            aux_ene = aux_pair->value;
            grafo->nodoactual->enemigo = aux_ene;

        }

    }
    else{
        //carga el grafo derecho o camino derecho
        aux_pair = searchMap(Mapaenemigo,niveles->camino_der);
        if(aux_pair==NULL){

            aux_pair = searchMap(Mapaobjeto,niveles->camino_der);
            aux_obj = aux_pair->value;
            grafo->nodoactual->objeto=  aux_obj;

        }
        else{

            aux_ene = aux_pair->value;
            grafo->nodoactual->enemigo= aux_ene;
            
        }

    }
    if(esenemigo(grafo)){ // si te topas con un enemigo se ejecuta el combate
        printf("Te has encontrado con un enemigo\n\n");
        combatir(grafo->nodoactual->enemigo);
        grafo->nodoactual->enemigo = NULL;
    }
    else{ // si te topas con un objeto te lo guardas al inventario 
        printf("Te has encontrado con un objeto\n\n");
        tomarobjeto();
    }
}

void Mostrarinventario(){
    Pair_2* pair_o = firstMap(Mapainventario);
    Objeto* o = pair_o->value;

    printf("\n");
    while(pair_o != NULL){ //imprime el inventario por pantalla
        o = pair_o->value;
        printf("Nombre: %s / Tipo: %s / Valor: %d\n",o->Nombre_Item, o->Tipo, o->Dano_vida_Armadura);
         pair_o = nextMap(Mapainventario);
    }
    //DECLARACION DE VARIABLES
    int opcion_iven;
    char aux_objeto[30];
    Objeto* pocion = (Objeto*)malloc(sizeof(Objeto));
    pair_o = (Pair_2*)malloc(sizeof(Pair_2));

    submenuinventario();
    fflush(stdin);
    scanf("%d", &opcion_iven);
    printf("\n");

    while(1){
        switch (opcion_iven){
            case 1:{ // equipar arma
                printf("Introduzca el nombre del arma: ");
                fflush(stdin);
                scanf("%[^\n]", aux_objeto);

                pair_o = searchMap(Mapainventario, aux_objeto);
                if(pair_o == NULL){
                    printf("Objeto no encontrado\n");
                    break;
                }
                else{
                    personaje->Arma = pair_o->value;
                    printf("Arma equipada\n");
                    break;
                }
            }
            case 2:{ // equipar armadura
                printf("Introduzca el nombre de la armadura: ");
                fflush(stdin);
                scanf("%[^\n]", aux_objeto);

                pair_o = searchMap(Mapainventario, aux_objeto);
                if(pair_o == NULL){
                    printf("Objeto no encontrado\n");
                    break;
                }
                else{
                    personaje->Armadura = pair_o->value;
                    printf("Armadura equipada\n");
                    break;
                }
            }
            case 3:{ //curarse
                printf("Introduzca el nombre de la pocion: ");
                fflush(stdin);
                scanf("%[^\n]", aux_objeto);

                pair_o = searchMap(Mapainventario, aux_objeto);
                if(pair_o == NULL){
                    printf("No tienes pociones de vida\n");
                    break;
                }
                else{
                    pocion = pair_o->value;
                    personaje->Vida += pocion->Dano_vida_Armadura;
                    eraseMap(Mapainventario, aux_objeto);
                    printf("Vida restaurada\n");
                    break;
                }
            }
        }
        if (opcion_iven == 4){ //salir del menu inventario
            break;
        }
        printf("\n"); //luego de ejecutar una opcion se muestra eso por pantalla hasta usar opcion salir del menu
        submenuinventario();
        fflush(stdin);
        scanf("%d", &opcion_iven);
        while(opcion_iven<1||opcion_iven>4){ // no se ingreso una opcion valida
            printf("Introdusca un numero valido: ");
            scanf("%d", &opcion_iven);
        }
    }
}

void Mostrarestado(){ // muestra los stats: vida, armadura, arma y oro actuales
    printf("\n");
    printf("Vida personaje: %d\n", personaje->Vida);
    printf("Nombre: %s / Proteccion: %d\n", personaje->Armadura->Nombre_Item, personaje->Armadura->Dano_vida_Armadura);
    printf("Nombre: %s / Dano: %d\n", personaje->Arma->Nombre_Item, personaje->Arma->Dano_vida_Armadura);
    printf("Oro: %d\n", personaje->Oro);
}

void Cargarenemigos(HashMap* Map){ //carga las estructuras de enemigos en un mapa global
    Enemigo* e = (Enemigo*)malloc(sizeof(Enemigo));
    int k=0;
    FILE *fp = fopen ("Enemigos.csv","r");
    // Cadena para guardar la linea completa del archivo csv
    char linea[1024];
    fgets (linea, 1023, fp);
    while (fgets (linea, 1023, fp) != NULL){ // Se lee la linea 

        strcpy(e->Nombre_Enemigo, get_csv_field(linea, 0)); 
        e->Vida = atoi(get_csv_field(linea, 1));
        e->Dano = atoi(get_csv_field(linea, 2));
        e->Nivel_e = atoi(get_csv_field(linea, 3));
                    
        insertMap(Map, e->Nombre_Enemigo, e);
                    
        e = (Enemigo*)malloc(sizeof(Enemigo));

        k++; 
        if(k==30) break;
    }
}

void Cargarobjetos(HashMap* Map){ //carga las estructuras de todos los objetos en un mapa global
    Objeto* o = (Objeto*)malloc(sizeof(Objeto));
    int k=0;
    FILE *fp = fopen ("Objetos.csv","r");
    // Cadena para guardar la linea completa del archivo csv
    char linea[1024];
    fgets (linea, 1023, fp);
    while (fgets (linea, 1023, fp) != NULL){ // Se lee la linea 

        strcpy(o->Nombre_Item, get_csv_field(linea, 0)); 
        strcpy(o->Tipo, get_csv_field(linea, 1)); 
        o->Dano_vida_Armadura = atoi(get_csv_field(linea, 2));
        o->Nivel_o = atoi(get_csv_field(linea, 3));
                    
        insertMap(Map, o->Nombre_Item, o);
                    
        o = (Objeto*)malloc(sizeof(Objeto));

        k++; 
        if(k==30) break;
    }
}

void CargarNiveles(Grafo* G){ //carga la estructura de los niveles
    Nivel* n = (Nivel*)malloc(sizeof(Nivel));
    TreeMap* auxtree = createTreeMap(lower_than_int);
    int k=0;
    FILE *fp = fopen ("Niveles.csv","r");
    // Cadena para guardar la linea completa del archivo csv
    char linea[1024];
    fgets (linea, 1023, fp);
    while (fgets (linea, 1023, fp) != NULL){ // Se lee la linea 

        strcpy(n->nombreNivel, get_csv_field(linea, 0));
        strcpy(n->camino_izq, get_csv_field(linea, 1));
        strcpy(n->camino_der, get_csv_field(linea, 2));
                    
        insertTreeMap(auxtree, n->nombreNivel, n);
        
        n = (Nivel*)malloc(sizeof(Nivel));

        k++; 
        if(k==30) break;
    }
    G->Niveles = auxtree;
}

void CargarMarcadores(TreeMap* map){ //carga los marcadores guardados 

    Usuario* P = (Usuario*)malloc(sizeof(Usuario));
    int k=0;
    FILE *fp = fopen ("Marcadores.csv","r");
    // Cadena para guardar la linea completa del archivo csv
    char linea[1024];
    fgets (linea, 1023, fp);
    while (fgets (linea, 1023, fp) != NULL){ // Se lee la linea 

        strcpy(P->Nombre_Usuario, get_csv_field(linea, 0));
        P->Oro = atoi(get_csv_field(linea, 1));
                    
        agregarJugador(map,&(P->Oro),P);
                    
        P = (Usuario*)malloc(sizeof(Usuario));
        k++; 
        if(k==5) break;
    }
}
