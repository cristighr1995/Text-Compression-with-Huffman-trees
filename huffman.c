#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define MES_LEN_CAP 10000000

typedef struct TagHuffmanNode {
	unsigned char value;
	int16_t left;
	int16_t right;
} __attribute__ ((__packed__)) TagHuffmanNode;

typedef struct binary_tree {
	unsigned char character;
	// 0 left, 1 right
	unsigned char index;
	int HuffmanIndex;
	float frequency;
	int been_there;
	int inside_node;
	struct binary_tree * left;
	struct binary_tree * right;
} binary_tree;

typedef struct List_of_Trees {
	struct List_of_Trees * next;
	binary_tree * arbore;
} List_of_Trees;

typedef struct Char_Freq_Vector {
	unsigned char character;
	float frequency;
	int number_of_bits;
	int code;
	int bits_vector[16];
} Char_Freq_Vector;

typedef struct Auxiliar_List {
	struct binary_tree * arbore;
	struct Auxiliar_List * next;
} Auxiliar_List;

uint32_t MESSAGE_LENGTH;

void readFromFile (FILE * fid, unsigned char * mesage) {
	int c;
	while((c = fgetc(fid)) != EOF) {
		mesage[MESSAGE_LENGTH] = c;
		MESSAGE_LENGTH++;
	}
}

binary_tree * init_binary_tree (binary_tree * arbore) {
	arbore = NULL;
	return arbore;
}

// Priority queue of trees
List_of_Trees * init_List_of_Trees (List_of_Trees * queue) {
	queue = NULL;
	return queue;    
}

List_of_Trees * add_to_Back_in_List (List_of_Trees * cap, binary_tree * arbore) {
	List_of_Trees * nou = (List_of_Trees *) malloc(sizeof(List_of_Trees));
	List_of_Trees * aux;

	if(cap == NULL) {
		nou->arbore = arbore;
		nou->next = NULL;
		cap = nou;
	}
	else {
		aux = cap;
		while(aux->next != NULL)
			aux = aux->next;
		nou->arbore = arbore;
		aux->next = nou;
		nou->next = NULL;
	}
	return cap;
}

List_of_Trees * add_Trees_Leaves (List_of_Trees * queue, Char_Freq_Vector * vect, int length) {
	int i;
	for(i = 0; i < length; i++) {
		binary_tree * arbore = (binary_tree *) malloc(sizeof(binary_tree));
		arbore->character = vect[i].character;
		arbore->frequency = vect[i].frequency;
		arbore->left = NULL;
		arbore->right = NULL;
		arbore->inside_node = 0;
		queue = add_to_Back_in_List(queue, arbore);
	}
	return queue;
}

void print_List_of_Trees (List_of_Trees * cap) {
	while(cap != NULL) {
		printf("%c %f\n", cap->arbore->character, cap->arbore->frequency);
		cap = cap->next;
	}
}

List_of_Trees * new_node_Binary_Tree (List_of_Trees * queue) {
	binary_tree * nou = (binary_tree *) malloc(sizeof(binary_tree));
	List_of_Trees * aux;
	List_of_Trees * new_node = (List_of_Trees *) malloc(sizeof(List_of_Trees));

	aux = queue;

	nou->frequency = queue->arbore->frequency + queue->next->arbore->frequency;
	nou->left = queue->arbore;
	nou->right = queue->next->arbore;
	nou->inside_node = 1;
	
	// if there are 2 trees in the list
	if(aux->next->next == NULL) {
		new_node->arbore = nou;
		new_node->next = NULL;
		return new_node;
	}

	// the first 2 nodes have been added into a tree so move on with 2 positions
	aux = aux->next->next;

	// add at the beginning
	if(aux->arbore->frequency >= nou->frequency) {
		new_node->arbore = nou;
		new_node->next = aux;
		return new_node;
	}

	// add at the end or intermediate
	while(aux->next != NULL && aux->next->arbore->frequency < nou->frequency) {
		aux = aux->next;
	}
	if(aux->next == NULL) {
		new_node->arbore = nou;
		aux->next = new_node;
		new_node->next = NULL;
		return queue->next->next;
	}
	else {
		List_of_Trees * temporar;
		temporar = aux->next;
		new_node->arbore = nou;
		aux->next = new_node;
		new_node->next = temporar;
		return queue->next->next;
	}
}

List_of_Trees * create_Binary_Tree (List_of_Trees * queue) {
	while(queue->next != NULL) {
		queue = new_node_Binary_Tree(queue);
	}
	return queue;
}

 // indexes are used to determine the way to a node
 // 0 left, 1 right
void put_Indexes (binary_tree * cap) {
	if(cap == NULL)
		return;
	if(cap->left != NULL) {
		cap->left->index = 0;
		put_Indexes(cap->left);
	}
	if(cap->right != NULL) {
		cap->right->index = 1;
		put_Indexes(cap->right);
	}
}

int nr_Leaves;

void create_Vector_Char_Freq (unsigned char * initial_mesage, Char_Freq_Vector * vect) {
	int i = 0, length = MESSAGE_LENGTH, count = 0, nr = 0;
	int already = 0, j, k;
	float frequency;

  	// calculate the frequency of each letter from initial mesage
	for(i = 0; i < length - 1; i++) {
		for(j = i + 1; j < length; j++)
			if(initial_mesage[i] == initial_mesage[j]) 
				count++;
		for(k = 0; k < nr_Leaves; k++)
			// verify if already added
			if(vect[k].character == initial_mesage[i]) {
				already = 1;
				break;
			}
		if(already == 0) {
			vect[nr_Leaves].character = initial_mesage[i];
			count++;
			frequency = (float)count / length;
			vect[nr_Leaves].frequency = frequency;
			nr_Leaves++;
		}
		already = 0;
		count = 0;
	}

	already = 0;

	 // verify if last character is '\n'
	 // if true, verify if it has been already added
	 // if false, his frequency will be 1 / length(initial mesage)
	for(i = 0; i < nr_Leaves; i++)
		if(vect[i].character == initial_mesage[length - 1]) {
			already = 1;
			break;
		}
	if(already == 0) {
		vect[nr_Leaves].character = initial_mesage[length - 1];
		int one = 1;
		frequency = (float)one / length;
		vect[nr_Leaves].frequency = frequency;
		nr_Leaves++;
	}
} 

// ascending sort of frequency vector using bubble sort
Char_Freq_Vector * sortAscending_Char_Freq_Vector (Char_Freq_Vector * vect, int length) {
	int i, j, found = 1;
	unsigned char aux_character;
	float aux_frequency;
    
    for(i = 0; (i < length) && found; i++) {
      	found = 0;
      	for (j = 0; j < length -1 ; j++) {
               if (vect[j + 1].frequency < vect[j].frequency) { 
               		// swap frequencies
                    aux_frequency = vect[j].frequency;
                    vect[j].frequency = vect[j + 1].frequency;
                    vect[j + 1].frequency = aux_frequency;

                    // swap characters
               		aux_character = vect[j].character;
               		vect[j].character = vect[j + 1].character;
               		vect[j + 1].character = aux_character;
               		found = 1;
               }
        }
    }
	return vect;
}

int length_global;
int bits_global[16];

// search a node into a tree
binary_tree * search (binary_tree * arbore, float frequency, unsigned char character) {
    binary_tree * found = NULL;

    if(arbore == NULL)
        return NULL;

    if(arbore->frequency == frequency && arbore->character == character)
        return arbore;

     // bits_global knows the path to the that node, which is a leaf
     // because of recursive call, and the attribution takes place after the recursive call
     // the vector needs to be reversed
    found = search(arbore->left, frequency, character);
    if (found) {
    	bits_global[length_global] = 0;
    	length_global++;
        return found;
    }

    found = search(arbore->right, frequency, character);
    if (found) {
    	bits_global[length_global] = 1;
    	length_global++;
       return found;
    }
    return NULL;
}

int least_multiplier_by_8 (int number) {
	int multiplier = 0;
	while(multiplier < number)
		multiplier += 8;
	return multiplier;
}

void text_compression (unsigned char * initial_mesage, Char_Freq_Vector * char_freq_vect, int length_List, FILE * file_output) {
	int i, j, k, l, length = MESSAGE_LENGTH, howManyBits = 0, count = 0;

	for(i = 0; i < length; i++) 
		for(j = 0; j < length_List; j++) 
			if(initial_mesage[i] == char_freq_vect[j].character) {
				int nr_bits = char_freq_vect[j].number_of_bits;
				howManyBits += nr_bits;
			}

	int multiplier = least_multiplier_by_8(howManyBits);
	// all necessary bits used for calculating the codes
	int * vector_of_compression = (int *) malloc(multiplier * sizeof(int));

	// stick together all bits in this vector
	for(i = 0; i < length; i++) 
		for(j = 0; j < length_List; j++) 
			if(initial_mesage[i] == char_freq_vect[j].character) {
				int nr_bits = char_freq_vect[j].number_of_bits;
				for(k = 0; k < nr_bits; k++) {
					vector_of_compression[count] = char_freq_vect[j].bits_vector[k];
					count++;
				}
			}

	// padding
	for(i = count; i < multiplier; i++)
		vector_of_compression[i] = 0;

	k = 0;
	unsigned char decimal_number = 0;
	count = 0;
	// divide in groups of 8 and calculate the codes
	while(k < multiplier) {
		for(i = k; i < k + 8; i++) {
			decimal_number += vector_of_compression[i] * pow(2, 8 - count - 1);
			count++;
		}
		k += 8;
		count = 0;
		fwrite(&decimal_number, sizeof(unsigned char), 1, file_output);
		decimal_number = 0;
	}

	free(vector_of_compression);
}

// next 3 functions are used for breadth first of tree
// calculate the Huffman Indexes
Auxiliar_List * push (Auxiliar_List * cap, binary_tree * arbore) {
	Auxiliar_List * nou = (Auxiliar_List *) malloc(sizeof(Auxiliar_List));
	Auxiliar_List * aux = cap;

	nou->arbore = arbore;
	nou->next = NULL;

	if(cap == NULL)
		return nou;
	else	
		while(cap->next != NULL)
			cap = cap->next;

	cap->next = nou;
	return aux;
}
// go to next node from queue
Auxiliar_List * go_to_next (Auxiliar_List * cap) {
	if(cap == NULL)
		return NULL;
	Auxiliar_List * p = cap->next;
	free(cap);
	return p;
}

void dealloc_Auxiliar_List (Auxiliar_List * cap) {	
	Auxiliar_List * aux;
	while(cap != NULL) {
		aux = cap->next;
		free(cap);
		cap = aux;
	}
	free(aux);
}

// Breadth first traversal
binary_tree * put_HuffMan_Indexes (binary_tree * radacina, Auxiliar_List * cap) {
	binary_tree * temp;
	int count = 0;

	if(radacina == NULL)
		return;

	cap = push(cap, radacina);

	while(cap != NULL) {
		// keap the root (head)
		temp = cap->arbore;
		temp->HuffmanIndex = count;
		count++;
		// if only one node, it will be null
		cap = go_to_next(cap);

		// add left son
		// if has also right son, add him
		if(temp->left != NULL)
			cap = push(cap, temp->left);
		if(temp->right != NULL)
			cap = push(cap, temp->right);
	}

	dealloc_Auxiliar_List(cap);
	return radacina;
}

binary_tree * find_HuffmanIndex (binary_tree * arbore, int index) {
	binary_tree * found = NULL;

    if(arbore == NULL)
        return NULL;

    if(arbore->HuffmanIndex == index)
        return arbore;

    found = find_HuffmanIndex(arbore->left, index);
    if (found) 
        return found;

    found = find_HuffmanIndex(arbore->right, index);
    if (found) 
       return found;
    
    return NULL;
}

// preorder traversal
void preordine (binary_tree * cap) {
	if(cap == NULL)
		return;
	printf("%c %f %d %d\n", cap->character, cap->frequency, cap->index, cap->HuffmanIndex);
	preordine(cap->left);
	preordine(cap->right);
}

// create Huffman vector
void create_Huffman (binary_tree * radacina, TagHuffmanNode * Huffman_Element, int index) {
	binary_tree * current = find_HuffmanIndex(radacina, index);

	if(current->inside_node != 1) {
		Huffman_Element->value = current->character;
		Huffman_Element->left = -1;
		Huffman_Element->right = -1;
	}
	else {
		Huffman_Element->left = current->left->HuffmanIndex;
		Huffman_Element->right = current->right->HuffmanIndex;
	}

	free(current);
}

uint16_t nr_Nodes;

void nr_Nodes_from_Tree (binary_tree * radacina) {
	if(radacina == NULL)
		return;
	nr_Nodes++;
	nr_Nodes_from_Tree(radacina->left);
	nr_Nodes_from_Tree(radacina->right);
}


int main (int arc, char ** argv) {
	if(strcmp(argv[1], "-c") == 0) {
		FILE * file_input = fopen(argv[2], "r");
		FILE * file_output = fopen(argv[3], "wb");
		Auxiliar_List * cap = NULL;
		int i, j;
		List_of_Trees * list = (List_of_Trees *) malloc(sizeof(List_of_Trees));

		unsigned char * initial_mesage = (unsigned char *) malloc(sizeof(unsigned char) * MES_LEN_CAP);

		readFromFile(file_input, initial_mesage);
		uint32_t initial_mesage_length = MESSAGE_LENGTH;


		Char_Freq_Vector * char_freq_vect = (Char_Freq_Vector *) malloc(MESSAGE_LENGTH * sizeof(Char_Freq_Vector));
		create_Vector_Char_Freq(initial_mesage, char_freq_vect);

		char_freq_vect = sortAscending_Char_Freq_Vector(char_freq_vect, nr_Leaves);

		list = init_List_of_Trees(list);
		list = add_Trees_Leaves(list, char_freq_vect, nr_Leaves);
		list = create_Binary_Tree(list);
		put_Indexes(list->arbore);

		for(i = 0; i < nr_Leaves; i++) {
			int code = 0, count = 0, j;
		
			binary_tree * found = search(list->arbore, char_freq_vect[i].frequency, char_freq_vect[i].character);

			int bits_correct[length_global];
			char_freq_vect[i].number_of_bits = length_global;

			/* Intorc vectorul bits_global, deoarece datorita recursivitatii ordinea bitilor se schimba */
			for(j = length_global - 1; j >= 0; j--) {
				bits_correct[count] = bits_global[j];
				count++;
			}
			/* Calculez codul fiecarei litere */
			for(j = 0; j < count; j++) {
				code += bits_correct[j] * pow(2, length_global - j - 1);
				char_freq_vect[i].bits_vector[j] = bits_correct[j];
			}

			char_freq_vect[i].code = code;
			length_global = 0;
		}
	
		nr_Nodes_from_Tree(list->arbore);

		TagHuffmanNode * Huffman_Vector = (TagHuffmanNode *) malloc(nr_Nodes * sizeof(TagHuffmanNode));

		list->arbore = put_HuffMan_Indexes(list->arbore, cap);

		for(i = 0; i < nr_Nodes; i++) 
			create_Huffman (list->arbore, &Huffman_Vector[i], i);

		fwrite(&initial_mesage_length, sizeof(uint32_t), 1, file_output);
		fwrite(&nr_Nodes, sizeof(uint16_t), 1, file_output);

		for(i = 0; i < nr_Nodes; i++)
			fwrite(&Huffman_Vector[i], sizeof(TagHuffmanNode), 1, file_output);
	
		text_compression(initial_mesage, char_freq_vect, nr_Leaves, file_output);
		fclose(file_input);
		fclose(file_output);
	}
	else if(strcmp(argv[1], "-d") == 0)
 		printf("Nu avem asa ceva -> ");

	return 0;
}

