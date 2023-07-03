#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <thread>
#include <algorithm>

using namespace std;

typedef struct {
    unsigned int peso;
    int beneficio;
} type_item;

int bestSolution = 0;
int bestZombie = 0;
vector<bool*> humans;

int findClosestHuman(bool * zombie,int n_items){
    vector<int> distances;
    for(int i=0;i<humans.size();i++){
        int count = 0;
        for(int j=0;j<n_items;j++){
            if(humans[i][j] != zombie[j]){
                count++;
            }
        }
        distances.push_back(count);
    }
    int min = distances[0];
    int index = 0;
    for(int i=0;i<distances.size();i++){
        if(distances[i] < min){
            min = distances[i];
            index = i;
        }
    }
    return index;
}

int evaluateFitness(bool * zombie, int n_items,type_item * items,int capacity){
    int fitness = 0;
    int peso = 0;
    for(int i=0;i<n_items;i++){
        if(zombie[i]){
            // if(peso + items[i].peso > capacity){
            //     return -1;
            // }
            peso += items[i].peso;
            fitness += items[i].beneficio;
        }
    }
    if(peso > capacity){
        fitness = fitness * 0.01;
    }
    return fitness;
}

void gradientAscedentSearch(bool * human, int n_items,type_item * items,int capacity){
    int fitness=0,lastFitness=0,index=0;
    bool value = false;
    vector<int> neighboors;
    do{
        lastFitness = evaluateFitness(human,n_items,items,capacity);
        for(int i=0;i<n_items;i++){
            human[i] = !human[i];
            neighboors.push_back(evaluateFitness(human,n_items,items,capacity));
            human[i] = !human[i];
            if(neighboors[i] > lastFitness){
                fitness = neighboors[i];
                index = i;
                value = !human[i];
            }
        }
        if(fitness > lastFitness){
            lastFitness = fitness;
            human[index] = value;
        }
    }while(fitness > lastFitness);

    if(lastFitness > bestSolution){
        bestSolution = lastFitness;
    }
}

void changeZombie(bool ** zombie, int n_items,type_item * items, int * definedFitness,int capacity,int velocity){
    int random = rand() % n_items;
    if(humans.size()){
        //int index = findClosestHuman((*zombie),n_items);
        for(int i=0;i<velocity;i++){
            random = rand() % n_items;
            (*zombie)[random] = humans[0][random];
        } 
    }else{
        for(int i=0;i<velocity;i++){
            random = rand() % n_items;
            (*zombie)[random] = !(*zombie)[random];
        }
        int fitness = evaluateFitness((*zombie),n_items,items,capacity);
        if (fitness > bestZombie){
            bestZombie = fitness;
        }
        if(fitness >= *definedFitness){
            int temp = *definedFitness;
            *definedFitness = fitness * 1.1;
            humans.push_back((*zombie));
            gradientAscedentSearch((*zombie),n_items,items,capacity);
            *definedFitness = temp;
            humans.pop_back();
        }
    }
    
}

void zombieSurvivalOptimization(int n_items, type_item * items,int numberOfZombies, int generations, int definedFitness,int capacity){
    //Incialize N zombies in search space
    vector<bool*> zombies;
    bool * temp;
    srand(time(NULL));
    int velocity = 1,increase=0.2;

    for (int i=0; i < numberOfZombies;i++){
        temp = new bool[n_items];
        for(int j=0; j < n_items;j++){
            temp[j] = false;
            int random = rand() % n_items;
            temp[random] = true;
        }
        zombies.push_back(temp);
    }
    vector<thread> zombieThreads; 

    for (int i=0;i<generations;i++){
        for(int j=0;j<numberOfZombies;j++){
            zombieThreads.push_back(thread(changeZombie,&zombies[j],n_items,items,&definedFitness,capacity,velocity));
        }
        for(int j=0;j<numberOfZombies;j++){
            zombieThreads[j].join();
        }
        zombieThreads.clear();
        if(bestSolution == 0 && i > generations*increase){
            velocity ++;
            increase += 0.1;
        }
    }
    
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

bool compararItens(type_item a, type_item b) {
    return a.beneficio < b.beneficio;
}

double mochilaGulosa(type_item * itens, double pesoMaximo, int n_items) {
    sort(&itens[0], &itens[n_items], compararItens);
    double pesoAtual = 0.0;
    double beneficioTotal = 0.0;
    for (int i = 0; i < n_items; i++) {
        if (pesoAtual + itens[i].peso <= pesoMaximo) {
            pesoAtual += itens[i].peso;
            beneficioTotal += itens[i].beneficio;
        }
    }
    return beneficioTotal;
}

int main(int argc, char *argv[]) {
    int n_items, capacidad;

    if (argc != 2) {
        printf("Uso: %s <nome do arquivo de entrada>", argv[0]);
        return -1;
    }

    type_item *items = ler_items(argv[1], &n_items, &capacidad);

    int definedFitness = mochilaGulosa(items, capacidad, n_items) * 0.85;

    cout << "Fitness definido: " << definedFitness << endl;

    zombieSurvivalOptimization(n_items,items,200,300,definedFitness,capacidad);

    cout << "Melhor solução encontrada: " << bestSolution << endl;

    return 0;
}