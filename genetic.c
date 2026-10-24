#include "stdio.h"
#include "stdlib.h" 
#include <string.h>
#include <time.h>

#define BINARY_STRING_SIZE sizeof(int) * 8
#define CROSSOVER_CHANCE 0.6

typedef struct {
	int number_of_items;
	char** items;
	int* items_points;
	int* items_weights;
	int max_capacity;
	int optimal;
	char* solution;
} KnapSack;

char* toBinaryString(int n) {
  int num_bits = BINARY_STRING_SIZE;
  char *string = malloc(num_bits + 1);
  if (!string) {
    return NULL;
  }
  for (int i = num_bits - 1; i >= 0; i--) {
    string[i] = (n & 1) + '0';
    n >>= 1;
  }
  string[num_bits] = '\0';
  return string;
}

char** duplicate_string_array(char **original, int count) {
	if (original == NULL || count <= 0) return NULL;

	char **copy = malloc(count * sizeof(char*));
	if (copy == NULL) return NULL;

	for (int i = 0; i < count; i++) {
		copy[i] = strdup(original[i]);

		// cleanup on allocation failure
		if (copy[i] == NULL) {
			for (int j = 0; j < i; j++) {
				free(copy[j]);
			}
			free(copy);
			return NULL;
		}
	}

	return copy;
}

/**N is the number of options 
@param chrom - The number of chromosomes in the algorithm*/
char** initialPhase(int number_of_options, int number_of_chromosomes){
	// initial array of binary representation of possibilities
	char** initial_options = malloc(number_of_chromosomes * sizeof(char *));

	for(int i = 0; i < number_of_chromosomes; i++){
		char* option_n = malloc((number_of_options+1) * sizeof(char));
		for(int j = 0; j < number_of_options; j++){
			int r = rand() % 2 + '0';
			option_n[j] = r; 
		}
		option_n[number_of_options] = '\0';
		initial_options[i] = option_n;
	}

	return initial_options;
}

float fitness(char* chromosome, KnapSack* problem){
	float res = 0;
	float weight = 0;
	for(int i = 0; i < problem->number_of_items; i++){
		if(chromosome[i] == '1'){
			res += problem->items_points[i];
			weight += problem->items_weights[i];
		}
	}

	if(weight > problem->max_capacity) return 0;

	res /= problem->optimal;

	return res;
}

int choose_chromosome(float* generation_fitness, int number_of_chromosomes, float fitness_sum){
	if (fitness_sum <= 0.0f) {
		return rand() % number_of_chromosomes; // fallback if total fitness is 0
	}

	float r = ((float)rand() / (float)RAND_MAX) * fitness_sum;
	for (int i = 0; i < number_of_chromosomes; i++) {
		r -= generation_fitness[i];
		if (r <= 0.0f) {
			return i;
		}
	}

	return number_of_chromosomes - 1; // safeguard
}

int crossover(char** next_gen, char* chr1, char* chr2, float crossover_chance, int number_of_options, int iter){
  char* new_chr1 = malloc((number_of_options + 1) * sizeof(char));
  char* new_chr2 = malloc((number_of_options + 1) * sizeof(char));
  if (new_chr1 == NULL || new_chr2 == NULL) return -1;
  
  strcpy(new_chr1, chr1);
  strcpy(new_chr2, chr2);

  float roll = (float)rand() / (float)RAND_MAX;

  if (roll < crossover_chance) {
    int crossover_point = (rand() % (number_of_options - 1)) + 1;

    for (int i = crossover_point; i < number_of_options; i++) {
      char aux = new_chr1[i];
      new_chr1[i] = new_chr2[i];
      new_chr2[i] = aux;
    }
  }

  next_gen[iter] = new_chr1;
  next_gen[iter + 1] = new_chr2;

  return 0;
}

void perGeneMutation(char** chromosomes, float mutation, int number_of_chromosomes, int items){
	for(int i = 0; i < number_of_chromosomes; i++){
		for(int j = 0; j < items; j++){
			float r = (float)rand() / (float)RAND_MAX;
			if(r < mutation){
				if(chromosomes[i][j] == '0') chromosomes[i][j] = '1';
				else chromosomes[i][j] = '0';
			}
		}
	}
}

void showGeneration(char** chromosomes, int number_of_chromosomes, int number_of_options, int generation){
	printf("Generation %d\n", generation);
	for(int i = 0; i < number_of_chromosomes; i++){
		printf("Chromosome %d: ", i);
		for(int j = 0; j < number_of_options; j++){
			printf("%c", chromosomes[i][j]);
		}
		printf("\n");
	}
}

void freePopulation(char** pop, int count) {
    if (pop == NULL) return;
    for (int i = 0; i < count; i++) {
        free(pop[i]);
    }
    free(pop);
}

char* geneticAlgorithm(char** chromosomes, float mutation, KnapSack* problem, int number_of_chromosomes, float acceptance, int max_loops) {
  int best_index = -1;
  int generation = 0;

  showGeneration(chromosomes, number_of_chromosomes, problem->number_of_items, generation);

  while (max_loops > 0 && best_index < 0) {
    float fitness_sum = 0;
    float* generation_fitness = malloc(number_of_chromosomes * sizeof(float));
    
    for (int i = 0; i < number_of_chromosomes; i++) {
      float fit = fitness(chromosomes[i], problem);
      generation_fitness[i] = fit;
      if (fit >= acceptance && best_index < 0) {
        best_index = i;
      }
      fitness_sum += fit;
    }

    if (best_index >= 0) {
      char* result = strdup(chromosomes[best_index]);
      free(generation_fitness);
      freePopulation(chromosomes, number_of_chromosomes);
      return result;
    }

    char** next_gen = malloc(number_of_chromosomes * sizeof(char*));
    for (int i = 0; i < number_of_chromosomes; i += 2) {
      int chr1 = choose_chromosome(generation_fitness, number_of_chromosomes, fitness_sum);
      int chr2 = choose_chromosome(generation_fitness, number_of_chromosomes, fitness_sum);
      crossover(next_gen, chromosomes[chr1], chromosomes[chr2], CROSSOVER_CHANCE, problem->number_of_items, i);
    }

    free(generation_fitness);
    perGeneMutation(next_gen, mutation, number_of_chromosomes, problem->number_of_items);

    freePopulation(chromosomes, number_of_chromosomes);
    chromosomes = next_gen;
    max_loops--;
    generation++;

    showGeneration(chromosomes, number_of_chromosomes, problem->number_of_items, generation);
  }

  // fallback if max loops reached
  char* result = strdup(chromosomes[0]);
  freePopulation(chromosomes, number_of_chromosomes);
  return result;
}

int main(){
	srand(time(NULL));
		/*
	capacity: 10
	options:
	first aid kit: 10p, 1w
	survival radio: 24p, 2w
	sleeping bag: 30p, 3w
	laptop: 36p, 4w
	tent: 40p, 5w
	portable generator: 63p, 7w

	solution: 1, 1, 1, 1, 0, 0
	optimal: 100p
	*/
	int number_of_chromosomes = 16; // needs to be an even number
	int items = 6;
	float mutation = 0.03f;
	float acceptance = 0.99f;
	KnapSack* problem = malloc(sizeof(KnapSack));
	problem->number_of_items = items;
	
	problem->items = malloc(number_of_chromosomes * sizeof(char*));
	problem->items[0] = "first aid kit";
	problem->items[1] = "survival radio";
	problem->items[2] = "sleeping bag";
	problem->items[3] = "laptop";
	problem->items[4] = "tent";
	problem->items[5] = "portable generator";

	problem->items_points = malloc(number_of_chromosomes * sizeof(int));
	problem->items_points[0] = 10;
	problem->items_points[1] = 24;
	problem->items_points[2] = 30;
	problem->items_points[3] = 36;
	problem->items_points[4] = 40;
	problem->items_points[5] = 63;

	problem->items_weights = malloc(number_of_chromosomes * sizeof(int));
	problem->items_weights[0] = 1;
	problem->items_weights[1] = 2;
	problem->items_weights[2] = 3;
	problem->items_weights[3] = 4;
	problem->items_weights[4] = 5;
	problem->items_weights[5] = 7;

	problem->max_capacity = 10;
	problem->optimal = 100;
	
	problem->solution = malloc(number_of_chromosomes * sizeof(char));
	problem->solution[0] = '1';
	problem->solution[1] = '1';
	problem->solution[2] = '1';
	problem->solution[3] = '1';
	problem->solution[4] = '0';
	problem->solution[5] = '0';

	char** initial_generation = initialPhase(problem->number_of_items, number_of_chromosomes);
	char* solution = geneticAlgorithm(initial_generation, mutation, problem, number_of_chromosomes, acceptance, 100);

	printf("\nsolution: ");
	for(int i = 0; i < problem->number_of_items; i++){
		printf("%c", problem->solution[i]);
	}
	printf("\nalgorithm solution: ");
	for(int i = 0; i < problem->number_of_items; i++){
		printf("%c", solution[i]);
	}

	return 0;
}