#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERRO 0
#define OK 1

struct sNo {
	int val;
	struct sNo *esq;
	struct sNo *dir;
}typedef sNo;

sNo *inicializa();
sNo *cria(int valor, sNo *sae, sNo *sad, int *res);
void compactar(char *arquivo);





/*
int main(int argc, char *argv[]) {
	if (argc > 1) {
		if ((strcmp(argv[1], "-c") == 0) && (argc == 3)) {
			compactar(argv[2]);
			getch();
		}
		else if ((strcmp(argv[1], "-d") == 0) && (argc == 3)) {
			printf("descompactar");
			getch();
		}
		else {
			printf("\nArgumento invalido!\n");
			printf("Uso correto:\n");
			printf("huff -c arquivo.txt    COMPACTAR ARQUIVO\n");
			printf("huff -d arquivo.huff   DESCOMPACTAR ARQUIVO\n");
		}
	}
	else {
		printf("\nArgumento invalido!\n");
		printf("Uso correto:\n");
		printf("huff -c arquivo.txt    COMPACTAR ARQUIVO\n");
		printf("huff -d arquivo.huff   DESCOMPACTAR ARQUIVO\n");
	}
}
*/

int main() {
	char in[BUFSIZ];

	gets(in);
	compactar(in);

}

void compactar(char *arquivo) {

	FILE *arq;
	int freq[256], counttotal;
	int i;
	char *in;

	printf("Arquivo %s\n\n", arquivo);


	for (i = 0; i < 256; i++) {
		freq[i] = 0;
	}

	if ((arq = fopen(arquivo, "rb")) != NULL) {
		
		
		fseek(arq, 0, SEEK_END);
		counttotal = ftell(arq);
		fseek(arq, 0, SEEK_SET);
		in = (char*) calloc(counttotal+1, sizeof(char));
		if (in == NULL) {
			return 0;
		}

		for (i = 0; i < counttotal; i++) {
			char c;
			c = fgetc(arq);
			in[i] = c;
			freq[c]++;
		}

		printf("Conteudo:\n");
		printf("%s\n\n", in);

		printf("Lista de frequencias:\n");

		for (i = 0; i < 256; i++) {
			if (freq[i] != 0) {
				printf("%c -> %d\n", i, freq[i]);
			}
		}
	}
	else {
		printf("Nao foi possivel abrir o arquivo\n");
	}

}

sNo *inicializa(void) {
	return NULL;
}

sNo *cria(int valor, sNo *sae, sNo *sad, int *res) {
	sNo *no;
	no = (sNo *)malloc(sizeof(sNo));
	if (no == NULL) *res = ERRO;
	else {
		no->esq = sae;
		no->dir = sad;
		no->val = valor;
		*res = OK;
	}
	return no;
}