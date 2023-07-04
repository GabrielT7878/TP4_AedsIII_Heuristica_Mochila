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
            peso += items[i].peso;
            fitness += items[i].beneficio;
        }
    }
    if(peso > capacity){
        fitness = fitness * -0.5;
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
        int index = findClosestHuman((*zombie),n_items);
        try{
            for(int i=0;i<velocity;i++){
                random = rand() % n_items;
                (*zombie)[random] = humans[index][random];
            }
        }catch(const exception e){
            for(int i=0;i<velocity;i++){
                random = rand() % n_items;
                (*zombie)[random] = humans[0][random];
            } 
        }
    }else{
        for(int i=0;i<velocity;i++){
            random = rand() % n_items;
            (*zombie)[random] = !(*zombie)[random];
        }
        int fitness = evaluateFitness((*zombie),n_items,items,capacity);
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

void zombieSurvivalOptimization(int n_items, type_item * items,int numberOfZombies, int generations, bool * solution,int * definedFitness,int capacity){
    //Incialize N zombies in search space
    vector<bool*> zombies;
    bool * temp;
    srand(time(NULL));
    int velocity = 1,increase=0.2;

    for (int i=0; i < numberOfZombies;i++){
        temp = new bool[n_items];
        for(int j=0; j < n_items;j++){
            temp[j] = solution[j];
        }
        int random = rand() % n_items;
        for(int j=0;j<random;j++){
            random = rand() % n_items;
            if(random > n_items * 0.99){
                temp[random] = !temp[random];
            }
        }
        int peso = 0;
        zombies.push_back(temp);
    }
    vector<thread> zombieThreads; 

    for (int i=0;i<generations;i++){
        for(int j=0;j<numberOfZombies;j++){
            zombieThreads.push_back(thread(changeZombie,&zombies[j],n_items,items,definedFitness,capacity,velocity));
        }
        for(int j=0;j<numberOfZombies;j++){
            zombieThreads[j].join();
        }
        zombieThreads.clear();
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
    return a.beneficio/a.peso > b.beneficio/b.peso;
}

bool * mochilaGulosa(type_item * itens, double pesoMaximo, int n_items) {
    type_item copia[n_items];
    copy(&itens[0], &itens[n_items-1], &copia[0]);
    sort(&copia[0], &copia[n_items-1], compararItens);
    double pesoAtual = 0.0;
    double beneficioTotal = 0.0;
    int index = 0;
    for (int i = 0; i < n_items; i++) {
        if (pesoAtual + copia[i].peso <= pesoMaximo) {
            pesoAtual += copia[i].peso;
            beneficioTotal += copia[i].beneficio;
            index++;
        }
    }
    index--;
    vector<int> indexs;
    int pesos[index];
    int beneficios[index];
    for (int i = 0; i < index; i++) {
        for (int j = 0; j < n_items; j++) {
            if (copia[i].beneficio == itens[j].beneficio && copia[i].peso == itens[j].peso) {
                indexs.push_back(j);
                break;
            }
        }
    }
    bool * solution = new bool[n_items];
    for (int i = 0; i < n_items; i++) {
        solution[i] = false;
    }
    for (int i = 0; i < indexs.size()-1; i++) {
        solution[indexs[i]] = true;
    }

    int fitness = evaluateFitness(solution,n_items,itens,pesoMaximo);

    return solution;
}

int main(int argc, char *argv[]) {
    int n_items, capacidad;

    if (argc != 2) {
        printf("Uso: %s <nome do arquivo de entrada>", argv[0]);
        return -1;
    }

    type_item *items = ler_items(argv[1], &n_items, &capacidad);

    int definedFitness;
    bool * solution = mochilaGulosa(items, capacidad, n_items);
    definedFitness = evaluateFitness(solution,n_items,items,capacidad) * 0.5;

    cout << "Fitness definido: " << definedFitness << endl;

    zombieSurvivalOptimization(n_items,items,200,500,solution,&definedFitness,capacidad);

    cout << "Melhor solução encontrada: " << bestSolution << endl;

    return 0;
}