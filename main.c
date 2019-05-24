#include <stdio.h>
#include <memory.h>
#include <ctype.h>

#define INPUT_FILE_PATH "/Users/jeongwhanchoi/Projects/CLionProjects/HuffmanCoding/input.txt"
#define OUTPUT_FILE_PATH "/Users/jeongwhanchoi/Projects/CLionProjects/HuffmanCoding/output.txt"

#define CODE_SIZE 128   //
#define NUM_LETTERS 26 //the number of alphabets
#define BYTE_SIZE 8

typedef struct tree_node_s {
    char character;
    char code[CODE_SIZE];
    float frequency;
    struct tree_node_s *left;
    struct tree_node_s *right;
} tree_node_t;

tree_node_t *array[NUM_LETTERS];
tree_node_t *letters[NUM_LETTERS];

void find_minimums(tree_node_t **, float *, int *, float *, int *);
void encode(tree_node_t *, tree_node_t **, char, short, char*);
void printTree(tree_node_t *node);
void decode(char *str, int *index, tree_node_t *tree);

int main (){
    char str[CODE_SIZE];
    int file_read;
    int i;
    int j;
    int num_characters = 0;
    int index;
    int string_length = 0;
    int alphabet_length = 0;
    tree_node_t *tree;
    float min, secondMin;
    int minIndex, secondMinIndex;


    /*
     * Read an input.txt from INPUT_FILE_PATH
     * */
    FILE *fin;
    fin = fopen(INPUT_FILE_PATH, "r");
    FILE *fout;
    if(fin == NULL){
        printf("\nFile not found");
        return 0;
    }
    else{
        file_read = fgets(str, CODE_SIZE, fin);

        while(!feof(fin) || file_read){
            string_length = strlen(str);
            printf("Content of the input.txt file");
            printf("\n%s", str);
            printf("\n");

            for(i=0; i<string_length; i++){
                str[i] = toupper(str[i]);
                printf("\nstr[%d]: %d",i,str[i]);
                if (str[i] >= 'A' && str[i] <= 'Z'){
                    num_characters ++;
                    index = str[i] - 'A';
                    if (array[index]==NULL){
                        array[index] = malloc(sizeof(tree_node_t));
                        array[index]->character = str[i];
                        array[index]->frequency = 1;
                        array[index]->left = NULL;
                        array[index]->right = NULL;
                    }
                    else{
                        array[index]->frequency += 1;
                    }
                }
            }
            if (file_read){
                file_read = fgets(str, CODE_SIZE, fin) != NULL;
            }
        }
    }

    for(i=0; i<NUM_LETTERS; i++){
        letters[i] = array[i];
        if(array[i] != NULL){
            array[i]->frequency /= num_characters; //Computing the frequency.
            alphabet_length ++; //the number of involved letters which is going to be consumed in the do while loop's condition
        }
    }

    j = 1;
    do{
        /*
         * find minimum and second minimum
         * */
        find_minimums(array, &min, &minIndex, &secondMin, &secondMinIndex);


        if(minIndex != -1 && secondMinIndex != -1 && minIndex != secondMinIndex){
            tree_node_t *temp;
            /*
            * create a new node, its left child contains the minimum
            * and the righr child contains the second minimum
            * */
            tree = malloc(sizeof(tree_node_t));
            tree->frequency = array[minIndex]->frequency + array[secondMinIndex]->frequency;
            tree->character = j;
            tree->left = array[minIndex];

            temp = malloc(sizeof(tree_node_t));
            temp->character = array[secondMinIndex]->character;
            temp->frequency = array[secondMinIndex]->frequency;
            temp->left = array[secondMinIndex]->left;
            temp->right = array[secondMinIndex]->right;
            tree->right = temp;

            /*
             * minimum position points on the new node,
             * and the second minimum's array position points on NULL
             * */
            array[minIndex] = tree;
            array[secondMinIndex] = NULL;
        }
        j++;
    } while(j<alphabet_length);

    /*
     * Browse the array till the unique non NULL element is encountered
     * */
    for(i=0; i<NUM_LETTERS; i++){
        if(array[i] != NULL){
            char code[CODE_SIZE];
            strcpy(code, "");
            /*
             * build the encoding of each character
             * */
            encode(tree = array[i], letters, 0, 0, code);
            puts("\n\nSuccessful encoding");
            /*
             * display the letter and its characteristics
             * */
            printTree(array[i]);
        }
    }

    /*
     * Decode the content of the output file
     * */
    printf("\nInterpreting output file:\n");
    fout = fopen(OUTPUT_FILE_PATH, "r");
    file_read = fgets(str, CODE_SIZE, fout) != NULL;
    while(!feof(fout)||file_read){
        num_characters = strlen(str);
        i=0;
        while(i<num_characters){
            decode(str, &i, tree);
        }
        if(file_read){
            file_read = fgets(str, CODE_SIZE, fout) != NULL;
        }
    }
    printf("\n\nFile Size Comparison:");
    printf("\n\tFile size (only letters) of the input file: %d bits", num_characters*BYTE_SIZE);
    printf("\n\tFile size of the output file: %d bits", num_characters);

    fclose(fin);
    fclose(fout);

    puts("\n");
    return 0;
}

void find_minimums(tree_node_t *array[], float *min, int *minIndex, float *secondMin, int *secondMinIndex){
    int i,k;
    k=0;
    *minIndex = -1;
    /*
     * Skipping all the NULL elements.
     */
    while(k < NUM_LETTERS && array[k] == NULL) {
        k++;
    }

    *minIndex = k;
    *min = array[k]->frequency;

    for(i=k; i<NUM_LETTERS; i++){
        if(array[i] != NULL && array[i]->frequency < *min){
            *min = array[i]->frequency;
            *minIndex = i;
        }
    }

    k=0;
    *secondMinIndex = -1;
    /*
     * Skip all the NULL elements.
     */
    while((k<NUM_LETTERS &*& array[k] == NULL) || (k==*minIndex && array[k] != NULL)){
        k++;
    }
    *secondMin = array[k]->frequency;
    *secondMinIndex = k;

    if(k == *minIndex){
        k++;
    }

    for(i=k; i<NUM_LETTERS; i++){
        if(array[i] != NULL && array[i]->frequency < *secondMin && i != *minIndex){
            *secondMin = array[i]->frequency;
            *secondMinIndex = i;
        }
    }
}

void encode(tree_node_t *node, tree_node_t **letters, char direction, short level, char* code){
    int n;
    if(node != NULL){
        if((n = strlen(code))<level){
            if(direction == 'R'){
                strcat(code, "1");
            }
            else{
                if(direction == 'L'){
                    strcat(code, "0");
                }
            }
        }
        else{
            if(n>=level){
                code[n-(n-level)-1] = 0;
                if(direction == 'R'){
                    strcat(code, "1");
                }
                else{
                    if(direction == 'L'){
                        strcat(code, "0");
                    }
                }
            }
        }
        if(node->character >= 'A' && node->character <= 'Z'){
            strcpy(node->code, code);
            strcpy(letters[node->character - 'A']->code, code);
        }
        encode(node->left, letters, 'L', level +1, code);
        encode(node->right, letters, 'R', level +1, code);
    }
}

void printTree(tree_node_t *node) {
    int n;
    if ( node != NULL ) {
        if (node->character >= 'A' && node->character <= 'Z') {
            printf("\t%c - frequency: %.10f\tencoding: %s\n", node->character, node->frequency, node->code);
        }
        printTree(node->left);
        printTree(node->right);
    }
}

void decode(char *str, int *index, tree_node_t *tree) {
    int n = strlen(str);
    if (tree->character >= 'A' && tree->character <= 'Z') {
        printf("%c ", tree->character);
        return ;
    } else {
        if ( *index < n ) {
            if (str[*index] == '0') {
                (*index) ++;
                decode(str, index, tree->left);
            } else {
                if (str[*index] == '1') {
                    (*index) ++;
                    decode(str, index, tree->right);
                }
            }
        }
    }
}