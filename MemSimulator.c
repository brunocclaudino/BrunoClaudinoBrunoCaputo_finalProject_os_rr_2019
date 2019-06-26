#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>


void AddNewPage(char value[9]);
void LRU(char value[9]);
void FIFO(char value[9]);
void Random(char value[9]);
bool Find(char value[9]);
void ReplacePage(char value[9]);
void WriteAddress(char value[9]);
void FreeMemory();

typedef struct Page
{
	char address[9];
	struct Page *next;
} Page;

FILE *file;
Page *first, *last;
char *alg, *filePath, line[20], tmpAddress[9];
int pageSize, memSize, numPages;
int operations = 0, reads = 0, writes = 0, hits = 0, misses = 0, faults = 0, usedPages = 0;

int main(int argc, char *argv[]){
	alg = argv[1];
	filePath = argv[2];
	pageSize = atoi(argv[3]);
	memSize = atoi(argv[4]);

	if (pageSize < 2 || pageSize > 64){
		printf("ERRO: O tamanho de cada pagina deve estar entre 2 e 64.");
		return 0;
	}

	if (memSize < 128 || memSize > 16384){
		printf("ERRO: O tamanho da memoria deve estar entre 128 e 16384.");
		return 0;
	}

	if (strcmp(alg, "lru") && strcmp(alg, "fifo") && strcmp(alg, "random")){
		printf("ERRO: O algoritmo deve ser lru, fifo ou random.");
		return 0;
	}

	numPages = memSize / pageSize;

	if (strlen(filePath) > 0){
		file = fopen(filePath, "r");
		while (fgets(line, 20, file) != NULL){
			operations++;
			strncpy(tmpAddress, line, 8);
			tmpAddress[8] = '\0';
			if (line[9] == 'W' || line[9] == 'w'){
				WriteAddress(tmpAddress);
			}
			else if (line[9] == 'R' || line[9] == 'r'){
				if (Find(tmpAddress)){
					hits++;
				}
				else{
					misses++;
					WriteAddress(tmpAddress);
				}
				reads++;
			}
			else{
				printf("ERRO: Os dados do arquivo de entrada estao em formato incorreto.");
				return 0;
			}
		}
	}
	else{
		printf("ERRO: Arquivo de entrada inválido.");
		return 0;
	}

	printf("\nExecutando o simulador...\n");
	printf("Arquivo de entrada: %s\n", filePath);
	printf("Tamanho da memoria: %iKB\n", memSize);
	printf("Tamanho das paginas: %iKB\n", pageSize);
	printf("Tecnica de reposicao: %s\n", alg);
	printf("Paginas lidas:%i \n", faults);
	printf("Paginas escritas:%i \n", writes);

	FreeMemory();

	return 0;
}


void AddNewPage(char value[9]){
	Page *current = (Page *)malloc(sizeof(Page));
	strcpy(current->address, value);
	current->next = NULL;

	if (usedPages == 0){
		first = current;
		last = first;
	}
	else{
		last->next = current;
		last = current;
	}

	if (usedPages < numPages)
		usedPages++;

	writes++;
}

void LRU(char value[9]){
	AddNewPage(value);
	if (usedPages == numPages)
		first = first->next;
}

void FIFO(char value[9]){
	AddNewPage(value);
	if (usedPages == numPages)
		first = first->next;
}

void Random(char value[9]){
	writes++;
	srand(time(NULL));
	int index = rand() % usedPages;
	Page *tmp = first;
	for (int i = 0; i < index; i++){
		tmp = tmp->next;
	}
	strcpy(tmp->address, value);
}

bool Find(char value[9]){
	Page *tmp = first, *prev = NULL;
	while (tmp != NULL){
		if (strcmp(tmp->address, value) == 0){
			if (strcmp(alg, "lru") == 0){
				if (prev != NULL){
					if (tmp->next != NULL)
						prev->next = tmp->next;
				}
				else{
					first = first->next;
				}
				last->next = tmp;
				last = tmp;
				tmp->next = NULL;
			}

			return true;
		}
		prev = tmp;
		tmp = tmp->next;
	}
	return false;
}

void ReplacePage(char value[9]){
	if (strcmp(alg, "lru") == 0){
		LRU(value);
	}
	else if (strcmp(alg, "random") == 0){
		Random(value);
	}
	else if (strcmp(alg, "fifo") == 0){
		FIFO(value);
	}
}

void WriteAddress(char value[9]){
	if (usedPages < numPages){
		AddNewPage(tmpAddress);
	}
	else{
		faults++;
		ReplacePage(tmpAddress);
	}
}

void FreeMemory(){
	Page *tmp = first;
	Page *tmp2;
	while (tmp != NULL){
		tmp2 = tmp;
		tmp = tmp->next;
		free(tmp2);
	}
	fclose(file);
}
