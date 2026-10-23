#include "stdio.h"
#include "stdlib.h" 

#define BINARY_STRING_SIZE sizeof(int) * 8

/*
capacity: 10
options:
first aid kit: 10p, 1w
survival radio: 24p, 2w
sleeping bag: 30p, 3w
laptop: 36p, 4w
tent: 40p, 5w
portable generator: 63p, 7w

optimal: 1, 1, 1, 1, 0, 0
optimal: 100p
*/

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

/**N is the number of options 
@param chrom - The number of chromosomes in the algorithm*/
char** initialPhase(int n, int chrom){
	// initial array of binary representation of possibilities
	char** initial_options = malloc(n * sizeof(char *));

	for(int i = 0; i < n; i++){
		char* option_n = malloc((n+1) * sizeof(char));
		for(int j = 0; j < n; j++){
			int r = rand() % 2;
			option_n[j] = r; 
		}
		initial_options[i] = option_n;
	}

	return initial_options;
}

float fitness(char* chromosome, KnapSack* problem){
	float res = 0;
	for(int i = 0; i < problem->number_of_items; i++){
		if(chromosome[i] == 1){
			res += problem->items_points[i];
		}
	}

	res /= problem->optimal;

	return res;
}

int choose_chromosome(float* generation_fitness, float fitness_sum){
	int i = 0;
	float r = ((float)rand() / (float)RAND_MAX) * fitness_sum;
	while(r > 0){
		r -= generation_fitness[i];
		i++; 
	}

	return i;
}

char* crossover(char* chr1, char* chr2, float crossover_chance, int n){
	float r = (float)rand() / (float)RAND_MAX;
	int crossover_point = n / 3;
	if(r < crossover_chance){
		for(int i = 0; i < n; i++){
		
		}
	}
}

char** geneticAlgorithm(char** chromosomes, float mutation, KnapSack* problem, int n){
	float* generation_fitness = malloc(problem->number_of_items * sizeof(float));
	float fitness_sum = 0;
	for(int i = 0; i < problem->number_of_items; i++){
		float fit = fitness(chromosomes[i], problem);
		generation_fitness[i] = fit;
		fitness_sum += fit;
	}
	// choosing pairs to crossover new chromosomes
	for(int i = 0; i < n; i++){
		int chr1 = choose_chromosome(generation_fitness, fitness_sum);
		int chr2 = choose_chromosome(generation_fitness, fitness_sum);


	}
}

int main(){
	int r = 0;

	for(int i = 0; i < 10; i++){
		r = rand() % 6;
		printf("\n%d", r);
	}

	return 0;
}