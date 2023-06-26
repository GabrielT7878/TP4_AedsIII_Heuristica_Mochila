#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <cstdlib>
#include <time.h>

using namespace std;

typedef struct {
    unsigned int peso;
    int beneficio;
} type_item;


vector<bool*> zombieSurvivalOptimization(int n_items, type_item * items,int numberOfZombies){
    //Incialize N zombies in search space
    vector<bool*> zombies;
    bool * temp;
    srand(time(NULL));

    for (int i=0; i < numberOfZombies;i++){
        temp = new bool[n_items];
        for(int j=0; j < n_items;j++){
            temp[j] = false;
            int random = rand() % n_items;
            temp[random] = true;
        }
        zombies.push_back(temp);
    }
    return zombies;
}

type_item *ler_items(char *filename, int *n_items, int *capacidad) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        puts("Não foi possível abrir o arquivo");
        exit(0);
    }

    fscanf(fp, "%d %d", n_items, capacidad);
    printf("Número de elementos: %d, Capacidade: %d\n", *n_items, *capacidad);

    type_item *items = (type_item *)malloc(*n_items * sizeof(type_item));

    for (int i = 0; i < *n_items; i++) {
        fscanf(fp, "%d %d", &items[i].beneficio, &items[i].peso);
    }

    fclose(fp);

    return items;
}

void imprimir_items(type_item *items, int n) {
    int i;
    for (i = 0; i < n; i++) {
        printf("Beneficio: %d, Peso: %d\n", items[i].beneficio, items[i].peso);
        //items += 1;
    }
}

int main(int argc, char *argv[]) {
    int n_items, capacidad;

    if (argc != 2) {
        printf("Uso: %s <nome do arquivo de entrada>", argv[0]);
        return -1;
    }

    type_item *items = ler_items(argv[1], &n_items, &capacidad);
    imprimir_items(items, n_items);

    vector<bool*> teste = zombieSurvivalOptimization(n_items,items,4);
    for(int i=0;i<teste.size();i++){
        for(int j=0;j<n_items;j++){
            cout << teste[i][j] << " ";
        }
        cout << endl;
    }

    free(items);
    return 0;
}