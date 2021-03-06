#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include <string.h>

enum selection_t { MPF, ELIT };
enum cases_t { SEND, EAT, CROSS, COCA, DONALD };

void generatePop(int pop[][11], int pop_size);
void getMinMaxAv(int pop[][11], int pop_size);
void sortRandom(int array[]);
void sortOrderAv(int array[][11], unsigned int array_size);
void getAv(int array[]);
void tour(int parents[][11],int pop_size, int p_index[], int tour_size, int n_sons);
void crossOverAll(int parents[][11], int p_index[], int sons[][11], int n_sons,
                  int mutate_percent);
void crossOver(int parent1[], int parent2[], int son1[], int son2[],
               int mutate_percent);
void mutation(int array[]);
void updatePop(int parents[][11], int p_size, int sons[][11], int n_sons,
               int type);
void setRoulette(int parents[][11], int p_size, double roulette[]);
void spinRoulette(double roulette[], int p_size, int p_index[], int n_sons);

int av_type = CROSS;
int av_min = 1000000000, av_max = 0;
unsigned long int roulette_inv = 100000;

int main() {
  // sendmory
  srand((unsigned)666);

  // PARAMETROS DO AG
  static int pop_size = 300;
  double cros_over = 0.8;
  int n_sons = pop_size * cros_over;
  static int n_ger =30;
  static int mutate_percent = 90;

  // PAIS E FILHOS
  int p[pop_size][11];
  int p_index[n_sons];
  int sons[n_sons][11];

  // Numero de execucoes
  int n_execucao = 1000;
  int n_zero = 0;

  double roulette[pop_size];

  for (int ag_n = 0; ag_n < n_execucao; ag_n++) {
    av_min = 10000000, av_max = 0;

    generatePop(p, pop_size);
    getMinMaxAv(p, pop_size);

    for (size_t i = 0; i < n_ger; i++) {
      tour(p, pop_size, p_index, 2, n_sons);
      // setRoulette(p, pop_size, roulette);
      // spinRoulette(roulette, pop_size, p_index, n_sons);
      crossOverAll(p, p_index, sons, n_sons, mutate_percent);
      updatePop(p, pop_size, sons, n_sons, ELIT);
      getMinMaxAv(p, pop_size);
    }
    printf("\n Melhor avalicao na execucao %i foi: %i", ag_n, av_min);

    if (av_min == 0) {
      n_zero++;
    }
  }

  printf("\nPorcentagem de sucesso %lf\n", (double)n_zero / n_execucao);
}

void generatePop(int pop[][11], int pop_size) {
  int n[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  for (int i = 0; i < pop_size; i++) {
    sortRandom(n);
    memcpy(pop[i], n, sizeof(int) * 10);
    getAv(pop[i]);
  }
}

void getMinMaxAv(int pop[][11], int pop_size) {
  for (int i = 0; i < pop_size; i++) {
    if (pop[i][10] < av_min) {
      av_min = pop[i][10];
    }

    if (pop[i][10] > av_max) {
      av_max = pop[i][10];
    }
  }
}

void sortRandom(int array[]) {
  for (int i = 9; i > 0; --i) {
    // index
    int w = rand() % i;
    // swap
    int t = array[i];
    array[i] = array[w];
    array[w] = t;
  }
}

void sortOrderAv(int array[][11], unsigned int array_size) {
  int a[11];
  for (int i = 0; i < array_size; ++i) {
    for (int j = i + 1; j < array_size; ++j) {
      if (array[i][10] > array[j][10]) {
        memcpy(a, array[i], sizeof(int) * 11);
        memcpy(array[i], array[j], sizeof(int) * 11);
        memcpy(array[j], a, sizeof(int) * 11);
      }
    }
  }
}

void getAv(int array[]) {
  unsigned int aux, sum;
  switch (av_type) {
  case SEND:

    sum = (array[0] * 1000 + array[1] * 100 + array[2] * 10 + array[3]) +
          (array[4] * 1000 + array[5] * 100 + array[6] * 10 + array[1]);

    aux = (1 + abs(array[7] - sum % 10)) *
          (1 + abs(array[1] - (sum / 10) % 10)) *
          (1 + abs(array[2] - (sum / 100) % 10)) *
          (1 + abs(array[5] - (sum / 1000) % 10)) *
          (1 + abs(array[4] - (sum / 10000) % 10));

    aux = aux - 1;
    roulette_inv = pow(10, 5) + 1;

    break;
  case EAT:
    sum = (array[0] * 100 + array[1] * 10 + array[2] * 1) +
          (array[2] * 1000 + array[3] * 100 + array[1] * 10 + array[2] * 1);

    aux = (1 + abs(array[0] - sum % 10)) *
          (1 + abs(array[5] - (sum / 10) % 10)) *
          (1 + abs(array[4] - (sum / 100) % 10)) *
          (1 + abs(array[4] - (sum / 1000) % 10)) *
          (1 + abs(array[1] - (sum / 10000) % 10));
    aux = aux - 1;
    roulette_inv = pow(10, 5) + 1;
    break;
  case CROSS:
    sum = (array[0] * 10000 + array[1] * 1000 + array[2] * 100 + array[3] * 10 +
           array[3] * 1) +
          (array[1] * 10000 + array[2] * 1000 + array[4] * 100 + array[5] * 10 +
           array[3] * 1);

    aux = (1 + abs(array[1] - sum % 10)) *
          (1 + abs(array[8] - (sum / 10) % 10)) *
          (1 + abs(array[7] - (sum / 100) % 10)) *
          (1 + abs(array[6] - (sum / 1000) % 10)) *
          (1 + abs(array[4] - (sum / 10000) % 10)) *
          (1 + abs(array[5] - (sum / 100000) % 10));
    aux = aux - 1;
    roulette_inv = pow(10, 6) + 1;
    break;
  case COCA:
    sum = (array[0] * 1000 + array[1] * 100 + array[0] * 10 + array[2] * 1) +
          (array[0] * 1000 + array[1] * 100 + array[3] * 10 + array[2] * 1);

    aux = (1 + abs(array[4] - sum % 10)) *
          (1 + abs(array[5] - (sum / 10) % 10)) *
          (1 + abs(array[4] - (sum / 100) % 10)) *
          (1 + abs(array[2] - (sum / 1000) % 10)) *
          (1 + abs(array[1] - (sum / 10000) % 10));
    aux = aux - 1;
    roulette_inv = pow(10, 5) + 1;
    break;
  case DONALD:
    sum = (array[0] * 100000 + array[1] * 10000 + array[2] * 1000 +
           array[3] * 100 + array[4] * 10 + array[0] * 1) +
          (array[5] * 100000 + array[6] * 10000 + array[7] * 1000 +
           array[3] * 100 + array[4] * 10 + array[0] * 1);

    aux = (1 + abs(array[9] - sum % 10)) *
          (1 + abs(array[7] - (sum / 10) % 10)) *
          (1 + abs(array[6] - (sum / 100) % 10)) *
          (1 + abs(array[8] - (sum / 1000) % 10)) *
          (1 + abs(array[1] - (sum / 10000) % 10)) *
          (1 + abs(array[7] - (sum / 100000) % 10));
    aux = aux - 1;
    roulette_inv = pow(10, 6) + 1;
    break;
  }
  array[10] = aux;
}

void tour(int parents[][11],int pop_size, int p_index[], int tour_size, int n_sons) {

  for (int i = 0; i < n_sons; i++) {
    int best_av = 1000000000;
    int best_index = -1;

    for (int n = 0; n < tour_size; n++) {
      int aux_i = rand() % pop_size;
      int aux_av = parents[aux_i][10];

      if (aux_av < best_av) {
        best_av = aux_av;
        best_index = aux_i;
      }

      p_index[i] = best_index;
    }
  }
}

void crossOverAll(int parents[][11], int p_index[], int sons[][11], int n_sons,
                  int mutate_percent) {
  for (size_t i = 0; i < n_sons; i += 2) {
    crossOver(parents[p_index[i]], parents[p_index[i + 1]], sons[i],
              sons[i + 1], mutate_percent);
  }

  // int mutation_size = n_sons*((double)mutate_percent/100);
  // for (int m = 0; m < mutation_size; m++) {
  //   mutation(sons[rand()%80]);
  // }
}

void crossOver(int parent1[], int parent2[], int son1[], int son2[],
               int mutate_percent) {
  unsigned int index = rand() % 10;
  unsigned int cycle[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  while (!cycle[index]) {
    cycle[index] = 1;
    for (size_t i = 0; i < 10; i++) {
      if (parent2[index] == parent1[i]) {
        index = i;
        break;
      }
    }
  }

  for (size_t j = 0; j < 10; j++) {
    if (cycle[j] == 1) {
      son1[j] = parent1[j];
      son2[j] = parent2[j];
    } else {
      son1[j] = parent2[j];
      son2[j] = parent1[j];
    }
  }
  if ((rand() % 101) < mutate_percent) {
    mutation(son1);
  }
  if ((rand() % 101) < mutate_percent) {
    mutation(son2);
  }

  getAv(son1);
  getAv(son2);
}

void mutation(int array[]) {
  int index = rand() % 10;
  int index2 = rand() % 10;
  int temp = array[index];
  array[index] = array[index2];
  array[index2] = temp;
}

void updatePop(int parents[][11], int p_size, int sons[][11], int n_sons,
               int type) {
  int parents_sons[p_size + n_sons][11];
  switch (type) {
  case MPF:
    memcpy(parents_sons[0], parents, sizeof(int) * p_size * 11);
    memcpy(parents_sons[p_size], sons, sizeof(int) * n_sons * 11);

    sortOrderAv(parents_sons, p_size + n_sons);

    memcpy(parents, parents_sons, p_size);
    break;
  case ELIT:
    sortOrderAv(parents, p_size);
    memcpy(parents[p_size-n_sons], sons, sizeof(int) * n_sons * 11);
    break;
  default:
    break;
  }
}

void setRoulette(int parents[][11], int p_size, double roulette[]) {
  long int av_total = 0;
  for (int i = 0; i < p_size; i++) {
    av_total += (roulette_inv - parents[i][10]);
  }
  long int mult_factor = 100000;
  roulette[0] =
      mult_factor * ((double)(roulette_inv - parents[0][10]) / av_total);
  // printf("\n%lf",roulette[0]);
  for (int j = 1; j < p_size; j++) {
    roulette[j] =
        (roulette[j - 1] +
         mult_factor * (roulette_inv - parents[j][10]) / (double)av_total);
    // printf("\n%lf",roulette[j]);
  }
}

void spinRoulette(double roulette[], int p_size, int p_index[], int n_sons) {
  for (int i = 0; i < n_sons; i++) {
    double chance = (rand() % 100001);
    for (int r_index = 0; r_index < p_size; r_index++) {
      if (chance <= roulette[r_index]) {
        p_index[i] = r_index;
        break;
      }
    }
    // printf("\n%i",p_index[i]);
  }
}
