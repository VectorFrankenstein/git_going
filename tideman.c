// Dependency libraries -
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Structs

/*
Name:- candidate

A candidate in the election and their votes tally.

Fields:-
1. Name: Char*
2. votes: Int
*/

struct candidate {
    char* name;
    int votes;
};

/*
Name:- candidate_pair.

Permutation pair of the candidates. 

Fields:-
1. First Candidate: candidate
2. Second Candidate: candidate
2. Vote difference: Int
*/
struct candidate_pair {
    struct candidate one;
    struct candidate two;
    int vote_difference;
    char* winner;
    char* loser;
    bool draw;
};

/*
Name:- ranked_pair

Will be used when ranking votes and to create pairs of ranked pair of candidates.

Fields:-
winner: Char*
Loser: Char*
*/
struct ranked_pair{
    char* winner;
    char* loser;
};

/*
Name:- candidate_graph_node

Usecase:- This represent a node in the graph. An array of these will represent the graph.

Fields:-
1. name, the name of the candidate. Type:- char*
2. in_degree, the number of arrows pointing to the this candidate. Type:- int
*/
struct candidate_graph_node {
    char* name;
    int in_degree;
};


// Prototype
char* get_user_input();

int permutations(int totals,int n);

int combinations(int totals, int n);

void all_candidate_combinations(struct candidate_pair* combinations_of_candidates, int n_combinations_of_candidates,char** all_candidates, int n_all_candidates);

struct ranked_pair* rank_pairs(struct ranked_pair* rank_pair_holder,int n_combinations,char ** voters_rankings,int candidate_count);

void tally_election(struct candidate_pair* combinations_of_candidates, struct ranked_pair* current_ranked_pair, int n_size);

void qualify_electoral_pair(struct candidate_pair* current_electoral_pair, int n_current_electoral_pair);

void bubble_sort_election_tally(struct candidate_pair* current_electoral_pair, int n_current_electoral_pair);

void populate_candidate_graph(struct candidate_pair* current_electoral_pair, int n_current_electoral_pair,struct candidate_graph_node* current_candidate_graph, int n_candidates);
// ---

int main(int argc, char* argv[]){

    // Calculate how many combinations and permutations there are going to be for the -
    // whole program.

    int candidate_count = argc - 1; // Under the assumption that everything other than the program name is a candidate.

    char** all_candidates = malloc(candidate_count * sizeof(char*));

    for(int i = 0;i < candidate_count;i++){ 
        all_candidates[i] = argv[i + 1]; // i vs i + 1 because argv has the program name as the first element where as all_candidates does not.
    }

    int n_combinations = combinations(candidate_count,2); // the second argument will have to be a magic number. but is is the size of a combination.

    int n_permutations = permutations(candidate_count,2); // The second argument will have to be a magic number. Is is the size of a permutation.

    struct candidate_pair* combinations_of_candidates = (struct candidate_pair*)malloc(n_combinations * sizeof(struct candidate_pair));

    // Now make all the combinations of the candidates
    all_candidate_combinations(combinations_of_candidates,n_combinations,all_candidates,candidate_count);


    /* Now that the combinations are made -
    get each voter's preference.
    */

    // First get the voter count.

    int n_voters;
    printf("What is the number of voters? ");
    scanf("%d",&n_voters);
    
    // Now for each voter get their preference
    for(int i = 1; i <= n_voters; i++){ // i is initalized to 1 because i is a count.

        // this voter's ranked pairs

        struct ranked_pair* rank_pair_holder = (struct ranked_pair*)malloc(n_combinations * sizeof(struct ranked_pair));

        char** voters_rankings = malloc(candidate_count * sizeof(char*));

        for(int i = 0;i < candidate_count;i++){
            voters_rankings[i] = malloc(64 * sizeof(char));
        }

        printf("Please rank the candidates for voter number %i :- \n",i);

        for(int j = 0; j < candidate_count; j++){
            printf("Rank number %i:- ",j+1);
            scanf("%s",voters_rankings[j]);
        } // here you have the current voter's ranks.

        struct ranked_pair* current_ranked_pair = rank_pairs(rank_pair_holder,n_combinations,voters_rankings,candidate_count);

        // Now that you have the ranked pairs send this iteration's -
        // ranked pairs to the struct candidate_pair* to be tallied -
        // up.

        tally_election(combinations_of_candidates,current_ranked_pair, n_combinations); // n_combination is the size of both combinations_of_candidates and current_ranked_pair

        free(rank_pair_holder);

        for(int i = 0;i < candidate_count;i++){
            free(voters_rankings[i]);
        }

        free(voters_rankings);

    }

    // Qualify the current electoral pairs array

    qualify_electoral_pair(combinations_of_candidates,n_combinations);

    // Sort the current current election tally.
    bubble_sort_election_tally(combinations_of_candidates,n_combinations);

    // Now that we ordered array of electoral pairs -
    // we need to work on the graph
    struct candidate_graph_node* candidate_graph = (struct candidate_graph_node*)malloc(n_voters * sizeof(struct candidate_graph_node));

    for(int i = 0; i < n_voters;i++){ //
        candidate_graph[i].name = all_candidates[i];
        candidate_graph[i].in_degree = 0;
    }

    populate_candidate_graph(combinations_of_candidates,n_combinations,candidate_graph,candidate_count);


    //for(int i = 0; i < n_combinations; i++){ // < because n_combinations is a count and i is an index.
    //    printf("The first candidate is %s with %i vote/s and the second candidate is %s with %i vote/s and the winner is %s\n", combinations_of_candidates[i].one.name, combinations_of_candidates[i].one.votes, combinations_of_candidates[i].two.name, combinations_of_candidates[i].two.votes, combinations_of_candidates[i].winner);
    //}

    for(int i = 0; i < n_voters;i++){
        printf("The candidate is %s and the in-degree is %i\n", candidate_graph[i].name,candidate_graph[i].in_degree);
    }


    // Free the dynamically allocated memory.
    free(all_candidates);

    free(combinations_of_candidates);   

    free(candidate_graph);
}

// General purpose functions
/*
Name:- get_user_input 

Function:- Letting the user supply their own
*/
char* get_user_input(){
    char* user_input;
    size_t size = 64 ;
    size_t used = 0;
    int c;

    user_input = (char *)malloc(size * sizeof(char));

    if(user_input == NULL){
        return NULL;
    }

    while((c = getchar()) != '\n' && c != EOF){

        if(used + 1 >= size){
            size *= 2;
            char* temp = (char *)realloc(user_input, size * sizeof(char));
            if(temp == NULL){
                free(user_input);
                return NULL;
            }
            user_input = temp;
        }
        user_input[used++] = c;
    }

    user_input[used] = '\0';

    return user_input;

}

/*
Name:- `factorial`
Goal:- Function to calculate factorials
Takes:- An integer
Returns:- A factorial for the integer
*/
unsigned long long factorial(int n) {
    if (n == 0 || n == 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

/*
Name:- `combinations`
Goal:- Functions to calculate combinations
Takes:- An integer
Returns:- A combinatorial value for the integer
*/
int combinations(int totals, int n) {
  
  int total_factorial = (int)factorial(totals);

  int n_factorial = (int)factorial(n);

  int total_minus_n_factorial = (int)factorial(totals - n);

  int combinations = total_factorial / (n_factorial * total_minus_n_factorial);

  return combinations;
}

/*
Name:- `permutations`
Goal:- Functions to calculate permutations for a given integer.
Takes:- An integer
Returns:- A combinatorial value for the integer
*/
int permutations(int totals, int n){

  int total_factorial = (int)factorial(totals);
  
  int total_minus_n_factorial = (int)factorial(totals - n);

  int permutations = total_factorial / (total_minus_n_factorial);

  return permutations;
    
}

void all_candidate_combinations(struct candidate_pair* combinations_of_candidates, int n_combinations_of_candidates,char** all_candidates, int n_all_candidates){

    int i = 0;

    for(int j = 0; j < n_all_candidates - 1; j++){ // minus 1 because j is an index, and n_all_candidates is count but you only need to go up to the penultimate item.
        for(int k = j + 1; k < n_all_candidates; k++){
            combinations_of_candidates[i].one.name = all_candidates[j];
            combinations_of_candidates[i].two.name = all_candidates[k];
            i++;
        }
    }
}


/*
Name:- rank_pairs
Goal:- Take a voter's preference, create ranked pairs for the preference.-
So, for candidates ranked as Foo Bar Boo the ranked pairs would be -
Foo Bar
Foo Boo
Bar Boo

Takes:- 
1. rank_pair_holder, the ranked pairs array for this iteration. Type:- struct ranked_pair* 
2. n_combinations, the size of the rank_pair_holder array. Type:- int
3. voters_rankings, the preference for this iteration. Type:- char**
4. candidate_count, the size for this voters_rankings. Type:- int
*/
struct ranked_pair* rank_pairs(struct ranked_pair* rank_pair_holder,int n_combinations,char ** voters_rankings,int candidate_count){

    int k = 0;

    for(int i = 0; i < candidate_count - 1; i++){ // i is an index and candidate_count is a count but we only need to go to the second to the last item in this array for this iteration.
        for (int j = i + 1; j < candidate_count; j++){ // < because j is an index and candidate_count is a count. 
            rank_pair_holder[k].winner = voters_rankings[i];
            rank_pair_holder[k].loser = voters_rankings[j];
            k++;
        }
    }

    return rank_pair_holder;
}

/*
Name:- tally_election

Goal:- Take one array of ranked_pairs and the current electoral tally and update the current electoral tally.

Takes:- 
1. combinations_of_candidates, the current electoral tally. Type:- struct candidate_pair*
2. current_ranked_pairs, one vote's ranked pairs. Type:- struct ranked_pair*
3. n_size, the size of both the combinations_of_candidates and current_ranked_pairs. Type:- int.

Returns:- (void)

Does:- 
Updates the current tally of the election.
*/
void tally_election(struct candidate_pair* combinations_of_candidates, struct ranked_pair* current_ranked_pair, int n_size){

    for(int i = 0; i < n_size; i++){ // < because i is an index and n_size is a count.
        /*
        Each current_ranked_pairs has winner and loser -
        And each item in the combinations_of_candidates will have one and two -
        We need to update the tally for the following two cases -
        winner = one and loser = two OR
        winner = two and loser = one
        Then update.
        */
       for(int j = 0; j < n_size;j++){

            if(strcmp(current_ranked_pair[i].winner,combinations_of_candidates[j].one.name) == 0 && strcmp(current_ranked_pair[i].loser,combinations_of_candidates[j].two.name) == 0){
                combinations_of_candidates[j].one.votes++;
            }

            if(strcmp(current_ranked_pair[i].winner,combinations_of_candidates[j].two.name) == 0 && strcmp(current_ranked_pair[i].loser,combinations_of_candidates[j].one.name) == 0){
                combinations_of_candidates[j].two.votes++;
            }

       }
    }
}

/*
Name:- qualify_electoral_pair

Goal:- Take an unqualified array of electoral pairs (type:- candidate_pair) and qualify it by calculating the vote differences and noting the winner, loser and other meta-data such as draws.

Takes:-
1. current_candidate_pair, a (current)electoral pair array. Type:- struct candidate_pair*
2. n_current_candidate_pair, the size of the current_candidate_pair. Type:- int
Returns:- (void)

Does:- 
Updates the array such that new information is added.
*/
void qualify_electoral_pair(struct candidate_pair* current_electoral_pair, int n_current_electoral_pair){

    for(int i = 0; i < n_current_electoral_pair;i++){
        
        // TODO
        
        // Apply a vote differential

        current_electoral_pair[i].vote_difference = abs(current_electoral_pair[i].one.votes - current_electoral_pair[i].two.votes);

        if(current_electoral_pair[i].vote_difference == 0){
            current_electoral_pair[i].draw = true;
        } else{
            current_electoral_pair[i].draw = false;
        }

        if(!current_electoral_pair[i].draw){

            if(current_electoral_pair[i].one.votes > current_electoral_pair[i].two.votes){
                current_electoral_pair[i].winner = current_electoral_pair[i].one.name;
                current_electoral_pair[i].loser = current_electoral_pair[i].two.name;
            } else if(current_electoral_pair[i].one.votes < current_electoral_pair[i].two.votes){
                current_electoral_pair[i].loser = current_electoral_pair[i].one.name;
                current_electoral_pair[i].winner = current_electoral_pair[i].two.name;
            }
        }

    }
}

/*
Name:- bubble_sort

Goal:- Take an array of candidate_pair that has already been tallied - this function assumes that the array has already been tallied and bubble sorts the array.

Takes:-
1. current_candidate_pair, a (current)electoral pair array. Type:- struct candidate_pair*
2. n_current_candidate_pair, the size of the current_candidate_pair. Type:- int
Returns:- (void)

Does:- 
Sorts the array such that it tallies the votes in descending order.
*/
void bubble_sort_election_tally(struct candidate_pair* current_electoral_pair, int n_current_electoral_pair){

    int swapped;

    do {

        for(int i = 0; i < n_current_electoral_pair - 1;i++){ // < n_current_electoral_pair is a count and i is an index and i only needs to go up to the penultimate element.
            swapped = 0;
            if (current_electoral_pair[i].vote_difference < current_electoral_pair[i+1].vote_difference){
                
                swapped = 1;
                
                struct candidate_pair temp = current_electoral_pair[i+1];

                current_electoral_pair[i+1] = current_electoral_pair[i];

                current_electoral_pair[i] = temp;

            } 
        } 
    } while(swapped == 1);
}

/*
Name:- populate_candidate_graph

Goal:- Populate the graph
*/

void populate_candidate_graph(struct candidate_pair* current_electoral_pair, int n_current_electoral_pair,struct candidate_graph_node* current_candidate_graph, int n_candidates){
    // TODO:- Count in_degrees for each candidate.
    for(int i = 0; i < n_candidates;i++){
        for(int j = 0; j < n_current_electoral_pair; j++){
            if(strcmp(current_candidate_graph[i].name,current_electoral_pair[j].loser) == 0){
                current_candidate_graph[i].in_degree++;
            }
        }
    }
}