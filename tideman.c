// Dependency libraries -
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// structs

struct ranked_pairs {
  char* winner;
  char* loser;
};

struct candidate_pair {
  char* winner;
  char* loser;
  int vote_count;
};

/* A very simple struct that holds the two things.
A name of the candidate and the number of
points poiting to the table.
*/

struct candidate_tideman_node {
    char* name;
    int in_degree;
};

struct candidate_tally {
    char* name;
    int votes_received;
};

struct pairs {
    struct candidate_tally* one;
    struct candidate_tally* two;
};

// Prototype
char* get_user_input();

void all_candidate_permutations(struct candidate_pair* current_list_of_candidate_pairs,char** the_candidates, int candidate_count);

int permutations_calculator(int totals,int n);

int combinations(int totals, int n);

struct ranked_pairs* ranked_pairs_func(char** candidates, struct ranked_pairs* current_ranks, int number_of_candidates);

void tally(struct candidate_pair* current_candidate_pair_array, struct ranked_pairs* current_ranked_pairs_array, int permutations_size, int combinations_size);

struct candidate_pair* bubble_sort_candidate_pair(struct candidate_pair* unordered, int permutation_count);

void per_canidate_in_degree(struct candidate_pair* ordered_candidate_pairs, struct candidate_tideman_node* in_degree_holder, size_t ordered_candidate_pairs_size, size_t candidate_count);

void permutational_pairs(char** current_candidates,struct candidate_pair* ordered_candidate_pairs,struct pairs* current_pairs,int magic_number,int candidate_size);


// Demarcating the start of the int function.

int main(int argc, char* argv[]){
  
    // Loop through the ballot and submit names to the candidates
    // get the user to specify the number of voters.

    char** current_candidates = malloc((argc - 1) * sizeof(char*));
    
    int j = 0;
    
    for(int i = 1; i < argc; i++){

        current_candidates[j++] = argv[i];
    }

    int candidate_count = argc - 1;

    int num_voters;
    printf("Number of Voters:");
    scanf("%d", &num_voters);


    // If more then move to the ranking function.

    // Allocate memory for serialized functions.

    
    // Now make the Ranked pairs -
    
    // An array to hold the ranked_pairs for the current run -
    // Combinatorics for the rankings
    
    // Get the permutations for the number

    int n_permutations = permutations_calculator(candidate_count, 2);

    int n_combinations = combinations(candidate_count,2);
    
    struct candidate_pair* current_list_of_candidate_pairs = (struct candidate_pair*)malloc(n_permutations * sizeof(struct candidate_pair));

    all_candidate_permutations(current_list_of_candidate_pairs,current_candidates,candidate_count);

    /*
    Now that you have made the permutations, go to make
    the tallies from each voter. I think it makes sense 
    to do it this way:
    1. let each voter vote
    2. Tally their pairs back to the original 
      permutations array.
    */
   for(int i = 1; i <= num_voters; i++){

    printf("Please rank the candidates for voter number %i:\n",i);

    int n = 2; // To make pairs in the combinatorics and permutations functions.
    int n_combinations = combinations(candidate_count,n); // We have two pairs for now.

    // A simple place-holder array that will be passed around.   
    struct ranked_pairs* ranked_pairs_array = (struct ranked_pairs*)malloc(n_combinations * sizeof(struct ranked_pairs));

    // get this voter's pair
    struct ranked_pairs* current_ranked_pairs = ranked_pairs_func(current_candidates,ranked_pairs_array,candidate_count);

    // Now you send this voter's pair to be tallied out.

    tally(current_list_of_candidate_pairs,current_ranked_pairs, n_permutations, n_combinations);

    // Free memory allocated for winner and loser
    for (int j = 0; j < n_combinations; j++) {
        free(current_ranked_pairs[j].winner);
        free(current_ranked_pairs[j].loser);
    }

    free(ranked_pairs_array); // Free here before re-creating.
   }

   // Create an unordered array as a place holder to receive the ordered array.
  struct candidate_pair* ordered_candidate_pairs = bubble_sort_candidate_pair(current_list_of_candidate_pairs, n_permutations);

  
  //We will have to tally the votes in pairs such 
  // they are combinatorial not permutational.
  struct pairs* current_pairs = (struct pairs*)malloc(n_combinations * sizeof(struct pairs*));

  for(int i = 0; i < n_combinations; i++) {
    current_pairs[i].one = (struct candidate_tally*)malloc(sizeof(struct candidate_tally));
    current_pairs[i].two = (struct candidate_tally*)malloc(sizeof(struct candidate_tally));
    }

  permutational_pairs(current_candidates,ordered_candidate_pairs,current_pairs,2,argc - 1);
  
  /*
  The `per_candidate_in_degree` function should be receiving an ordered pair.
  */
  
  struct candidate_tideman_node* in_degree_holder = (struct candidate_tideman_node*)malloc((argc - 1) * sizeof(struct candidate_tideman_node)); // A placeholder for the in-degree count.

  for(int i = 0; i < argc -1;i++){
      in_degree_holder[i].name = current_candidates[i];
      in_degree_holder[i].in_degree = 0;
  }
  
  per_canidate_in_degree(ordered_candidate_pairs,in_degree_holder,n_permutations, argc - 1);

  char* current_winner = NULL;

    current_winner = tideman(in_degree_holder,n_combinations);
    if(current_winner == NULL){
        current_winner = tideman_tiebreaker(

        );
    }

  /*
  Now that we have the tallies we can look for the one candidate that
  */  
  for(int i = 0; i < argc - 1; i++){
      printf("The candidate's name is %s and the amout of in-degree is %i\n",in_degree_holder[i].name,in_degree_holder[i].in_degree);
  }

  for(int i = 0; i < n_combinations;i++){
    printf("The first candidate is %s, with votes %i and the second candidate is %s, with votes %i\n",current_pairs[i].one->name,current_pairs[i].one->votes_received,current_pairs[i].two->name,current_pairs[i].two->votes_received);
  }

  // When cleaning up:
    for(int i = 0; i < n_combinations; i++) {
        free(current_pairs[i].one);
        free(current_pairs[i].two);
    }
    free(current_pairs);
    free(current_candidates);

    free(current_list_of_candidate_pairs);

    free(in_degree_holder);
  
    return 0; 
}

// Function:- `get_user_input`
// This is the general function for getting -
// the user's input.

char* get_user_input() {
    char *input;
    size_t size = 64;
    size_t used = 0;
    int c;

    input = (char *)malloc(size * sizeof(char));
    if (input == NULL) {
        return NULL; // Memory allocation failed
    }

    while ((c = getchar()) != '\n' && c != EOF) {
        if (used + 1 >= size) {
            size *= 2;
            char *temp = (char *)realloc(input, size * sizeof(char));
            if (temp == NULL) {
                free(input);
                return NULL; // Memory allocation failed
            }
            input = temp;
        }
        input[used++] = c;
    }

    input[used] = '\0'; // Null-terminate the string

    return input;
}

// Function:- `ranked_pairs`
// Goal:- return a pair of ranked pairs.
// Takes:- An array of candidates.
// Does:- Let's the user rank the candidates.
// Returns:- Pointer to a ranked pair.

struct ranked_pairs* ranked_pairs_func(char** candidates, struct ranked_pairs* current_ranks, int number_of_candidates) {
    char** serialized = malloc(number_of_candidates * sizeof(char*));
    for (int i = 0; i < number_of_candidates; i++) {
        serialized[i] = malloc(64 * sizeof(char)); // Allocate memory for each string
    }
    
    // Let the user rank their preference.
    int i = 1;
    while (i <= number_of_candidates) {
        printf("The candidate for Rank %i: ", i); // Because you start with rank 1 and not rank 0
        scanf("%s", serialized[i - 1]); // Store input in allocated space
        i++;
    }
    
    int j = 0;
    
    for(int i = 0; i < number_of_candidates; i ++){
        int k = i + 1;
        
        while(k < number_of_candidates){
            // Allocate memory for winner and loser
            current_ranks[j].winner = malloc(64 * sizeof(char));
            current_ranks[j].loser = malloc(64 * sizeof(char));
            
            // Copy strings
            strcpy(current_ranks[j].winner, serialized[i]);
            strcpy(current_ranks[j].loser, serialized[k]);
            
            k++;
            j++;
        }
    }
    
    // Free serialized array and its elements
    for (int i = 0; i < number_of_candidates; i++) {
        free(serialized[i]);
    }
    free(serialized);
    
    return current_ranks;
}

/*
Function:- all_candidate_permutations

Goal:- A function to create all the possible permutations of the candidates.

Inputs:-

- A pointer to a candidate_pair array. Type: struct candidate_pair*

- The list of candidate names. Type: char**

- The number of permutations for the candidates. Type: int

Returns:-
- Void; The function takes a pointer and changes the candidate_pair array in place.
*/

void all_candidate_permutations(struct candidate_pair* current_list_of_candidate_pairs,char** the_candidates, int candidate_count){
  
  int k = 0;  
  for(int i = 0; i < candidate_count; i++){ // Using < because i is index and candidate_count is count
      for (int j = i + 1; j < candidate_count; j++){ // Using < because j is index and candidate_count is count
          if(i == j){
              continue;
          }else {
              current_list_of_candidate_pairs[k].winner = the_candidates[i];
              current_list_of_candidate_pairs[k].loser = the_candidates[j];
              
              // k needs to go up twice within the loop -
              // first here.              
              k++; 

              // second combination
              current_list_of_candidate_pairs[k].winner = the_candidates[j];
              current_list_of_candidate_pairs[k].loser = the_candidates[i];

              // k needs to go up twice within the loop -
              // second time here.
              k++;
          }
      }
  }
}

// Function
// Function:- `factorial`
// Takes:- An integer
// Returns:- A facorial for the integer
// Function to calculate factorials
unsigned long long factorial(int n) {
    if (n == 0 || n == 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

// Function:- `combinations`
// Takes:- An integer
// Returns:- A combination for the integer
// Functions to calculate combinations

int combinations(int totals, int n) {
  
  int total_factorial = (int)factorial(totals);

  int n_factorial = (int)factorial(n);

  int total_minus_n_factorial = (int)factorial(totals - n);

  int combinations = total_factorial / (n_factorial * total_minus_n_factorial);

  return combinations;
}

int permutations_calculator(int totals, int n){

  int total_factorial = (int)factorial(totals);
  
  int total_minus_n_factorial = (int)factorial(totals - n);

  int permutations = total_factorial / (total_minus_n_factorial);

  return permutations;
    
}

/*
Name:- Tally

Goal:- The tally function should take two things - an existing candidate pair array (permutational) 
and then take one voter's ranked pairs then tally up the votes in there.

Inputs:- 
- A candidate pair array. Type:- struct candidate_pair*.
- A ranked pair array of one voter. Type:- struct ranked_pairs*.
- permutations_size, the size of candidate pair array. Type:- int.
- combinations_size, the size of the ranked pair array. Type:- int.
*/

void tally(struct candidate_pair* current_candidate_pair_array, struct ranked_pairs* current_ranked_pairs_array, int permutations_size, int combinations_size){

  /*
  Both permutations_size and combinations_size are counts.
  So we need to adjust for index.
  */
  for(int i = 0;i < combinations_size;i++){
    // line up the winner and the loser of the ranked_pair
    for(int j = 0; j < permutations_size;j++){
      if(strcmp(current_ranked_pairs_array[i].winner,current_candidate_pair_array[j].winner) == 0){
        if(strcmp(current_ranked_pairs_array[i].loser,current_candidate_pair_array[j].loser) == 0){

          current_candidate_pair_array[j].vote_count++;
        }
      }
    }
  }
}

/*

Name:- bubble_sort_candidate_pairs

Goal:- Take the tallied candidate_pairs (permutations) and sort them.

Inputs:-
- A candidate pair array but unordered. Type:- struct candidate_pair*
- Combinations size. Type:- int

Returns:- 
- A candidate pair array but ordered for the size of the votes. Type:- struct candidate_pair*
*/

struct candidate_pair* bubble_sort_candidate_pair(struct candidate_pair* unordered, int permutation_count) {
    int swapped;
    do {
        swapped = 0;
        for (int i = 0; i < permutation_count - 1; i++) {
            if (unordered[i].vote_count < unordered[i + 1].vote_count) {
                // Swap elements
                struct candidate_pair temp = unordered[i];
                unordered[i] = unordered[i + 1];
                unordered[i + 1] = temp;
                swapped = 1;
            }
        }
    } while (swapped);

    return unordered;
}

void per_canidate_in_degree(struct candidate_pair* ordered_candidate_pairs, struct candidate_tideman_node* in_degree_holder, size_t ordered_candidate_pairs_size, size_t canidate_count){

    for(int i = 0; i < ordered_candidate_pairs_size;i++){ // < because i is index and ordered_candidate_pairs_size is size
        for(int j = 0; j < canidate_count;j++){
            if(strcmp(in_degree_holder[j].name,ordered_candidate_pairs[i].loser) == 0){
                if(ordered_candidate_pairs[i].vote_count != 0){ // if the vote-count of the "loser" is not zero then the in-degree count needs to go up
                    in_degree_holder[j].in_degree++;
                }
            }
        }
    }    
}

void permutational_pairs(char** current_candidates, struct candidate_pair* ordered_candidate_pairs, struct pairs* current_pairs, int magic_number, int candidate_size) {
    int n_combinations = combinations(candidate_size, magic_number);
    
    // Properly allocate memory for the candidate_tally structures
    int h = 0;
    for(int i = 0; i < candidate_size - 1; i++) {
        for(int j = i + 1; j < candidate_size; j++) {            
            // Initialize values
            current_pairs[h].one->name = current_candidates[i];
            current_pairs[h].two->name = current_candidates[j];
            current_pairs[h].one->votes_received = 0;
            current_pairs[h].two->votes_received = 0;
            
            h++;
        }
    }

    // Fix the comparison logic
    for(int i = 0; i < n_combinations; i++) {
        for(int j = 0; j < permutations_calculator(candidate_size, magic_number); j++) {
            if(strcmp(current_pairs[i].one->name, ordered_candidate_pairs[j].winner) == 0 && 
               strcmp(current_pairs[i].two->name, ordered_candidate_pairs[j].loser) == 0) {
                current_pairs[i].one->votes_received = ordered_candidate_pairs[j].vote_count;
            } 
            else if(strcmp(current_pairs[i].one->name, ordered_candidate_pairs[j].loser) == 0 && 
                    strcmp(current_pairs[i].two->name, ordered_candidate_pairs[j].winner) == 0) {
                current_pairs[i].two->votes_received = ordered_candidate_pairs[j].vote_count;
            }
        }
    }
}

char* tideman(struct candidate_tideman_node* current_graph,size_t current_pairs_size){

    char* winner = NULL;

    for(int i = 0; i < current_pairs_size; i++){
        if(current_graph[i].in_degree == 0 ){
            winner = current_graph[i].name;
            break;
        }
    }

    return winner;

}

char* tideman_tiebreaker(struct candidate_tideman_node* current_graph,size_t current_pairs_size,struct candidate_pair* ordered_candidate_pair, size_t ordered_candidate_pair_size){

    for(int i = ordered_candidate_pair - 1; i >= 0; i--){
        
    }
}