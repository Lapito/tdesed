#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERRO 0
#define OK 1

struct sNo {
	int val;
	int freq;
	struct sNo *esq;
	struct sNo *dir;
}typedef sNo;

struct tSim {
	int val;
	char cod[256];
}typedef tSim;

sNo *inicializa();
sNo *cria(int valor, int frequencia, sNo *sae, sNo *sad);
void compactar(char *arquivo);
void ordena(sNo *fila, int tam);
sNo *tiraUltimo(sNo *fila, int tamanho);
int tamanhoFila(sNo *fila);
//sNo *criaHuffman(sNo *no1, sNo *no2);
sNo *criaHuffman(sNo *fila, int tamanho);
void imprime_textual(sNo *a) {
	if (a == NULL)
		printf("<>");
	else {
		printf("<%c/%d", a->val, a->freq);
		imprime_textual(a->esq);
		imprime_textual(a->dir);
		printf(">");
	}
}

void criaTsim(tSim *tsim) { //Tabela de simbolos
	tSim *aux;
	static char trail[256];



}


//FAZER FUNCAO PARA PEGAR O TAMANHO DO VETOR


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
	int freq[256], counttotal, tam;
	int i, j=0;
	char *in;
	//sNo *fila;

	static sNo fila[256];
	tSim tsim[256];

	//fila[256];

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
		fclose(arq);
		//fila = (sNo *)calloc(256, sizeof(sNo));

		printf("Conteudo:\n");
		printf("%s\n\n", in);

		printf("Lista de frequencias:\n");

		for (i = 0; i < 256; i++) {
			if (freq[i] != 0) {
				printf("%c -> %d\n", i, freq[i]);
			}
		}

		for (i = 0; i < 256; i++) {
			if (freq[i] != 0) {
				fila[j] = *cria(i, freq[i], inicializa(), inicializa());
				j++;
			}
		}
		tam = tamanhoFila(&fila);
		printf("\n\nTAMANHO %d\n\n", tam);
		ordena(&fila, tam);

		for (i = 0; i < tam; i++) {
			printf("%d->%c\n", fila[i].freq,fila[i].val);
		}
		
		sNo hufftree;

		printf("\n\n%x\n\n", &fila); //imprime local da fila so pq nn sei queria ver no q dava

		printf("\n\n");
		hufftree = *criaHuffman(&fila, tam);
		imprime_textual(&hufftree);


	}
	else {
		printf("Nao foi possivel abrir o arquivo\n");
	}

}

sNo *inicializa(void) {
	return NULL;
}

sNo *cria(int valor, int frequencia, sNo *sae, sNo *sad) {
	sNo *no;
	no = (sNo *)malloc(sizeof(sNo));
	//if (no == NULL) *res = ERRO;
	//else {
		no->esq = sae;
		no->dir = sad;
		no->val = valor;
		no->freq = frequencia;
		//*res = OK;
	//}
	return no;
}

void ordena(sNo *fila, int tam) {
	int i, j;
	sNo *aux;
	aux = cria(0, 0, inicializa(), inicializa());
	for (i = 1; i < tam; i++) {
		*aux = fila[i];
		j = i - 1;
		while (j >= 0 && fila[j].freq < aux->freq) {
			fila[j + 1] = fila[j];
			j--;
		}
		fila[j + 1] = *aux;
	}
	free(aux);
}

sNo *tiraUltimo(sNo *fila, int tamanho) {
	sNo *aux;
	int i;
	aux = cria(0, 0, inicializa(), inicializa());
	*aux = fila[tamanho - 1];
	fila[tamanho - 1] = *cria(0, 0, inicializa(), inicializa());
	return aux;
}

int tamanhoFila(sNo *fila) {
	int count=0;
	while (fila[count].val != NULL) {
		count++;
	}
	return count;
}
/*
sNo *criaHuffman(sNo *no1, sNo *no2) {
	sNo *notemp;
	notemp = (sNo *)calloc(1, sizeof(sNo));
	notemp = cria(0, (no1->freq) + (no2->freq), no1, no2);
	return notemp;
}*/

sNo *criaHuffman(sNo *fila, int tamanho) {
	int tam = tamanho;
	sNo *hufftree, *notemp1, *notemp2;
	hufftree = (sNo*)calloc(1, sizeof(sNo));
	notemp1 = (sNo*)calloc(1, sizeof(sNo));
	notemp2 = (sNo*)calloc(1, sizeof(sNo));

	while (tam > 1) {
		notemp1 = tiraUltimo(&*fila, tam);
		tam--;
		notemp2 = tiraUltimo(&*fila, tam);
		tam--;
		hufftree = cria(0, (notemp1->freq) + (notemp2->freq), notemp1, notemp2);
		fila[tam] = *hufftree;
		tam++;
		ordena(&*fila, tam);
	}

	//free(notemp1);
	//free(notemp2);
	
	return hufftree;

}
