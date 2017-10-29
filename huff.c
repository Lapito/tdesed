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
void descompactar(char *arquivo);
void ordena(sNo *fila, int tam);
sNo *tiraUltimo(sNo *fila, int tamanho);
int checaHuf(FILE *arq);
int tamanhoCod(char *codigo);
//int tamanhoFila(sNo *fila);
//sNo *criaHuffman(sNo *no1, sNo *no2);
sNo *criaHuffman(sNo *fila, int tamanho);
void imprime_textual(sNo *a) {
	if (a == NULL)
		printf("<>");
	else {
		a->val == 0 ? printf("<__/%d", a->freq) : printf("<%c/%d", a->val, a->freq);
		//printf("<%c/%d", a->val, a->freq);
		imprime_textual(a->esq);
		imprime_textual(a->dir);
		printf(">");
	}
}

void criaTsim(tSim *tsim, sNo *a) {
	static int i = 0;
	int k;
	static char trail[256];
	tSim *aux;

	aux = (tSim*)calloc(1, sizeof(tSim));

	if (a == NULL)
		return NULL;
	if (a->esq != NULL) {
		trail[i] = '0';
		i++;
		criaTsim(tsim, a->esq);
	}
	if (a->dir != NULL) {
		trail[i] = '1';
		i++;
		criaTsim(tsim, a->dir);
	}
	if ((a->esq == NULL) && (a->dir == NULL)) {
		for (k = 0; k < i; k++) {
			aux->cod[k] = trail[k];
		}
		aux->val = a->val;
		tsim[aux->val] = *aux;
		trail[i - 1] = NULL;
		i--;
	}
	else {
		if (i <= 0) {
			return 1;
		}
		
		trail[i - 1] = NULL;
		i--;
		
	}
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
	descompactar(in);

}

void compactar(char *arquivo) {

	FILE *arq;
	int freq[256], counttotal, tam;
	int i, j=0,k;
	char *in;
	//sNo *fila;

	static sNo fila[256];
	static tSim tsim[256];

	//fila[256];

	printf("Arquivo %s\n\n", arquivo);


	for (i = 0; i < 256; i++) {
		freq[i] = 0;
	}

	if ((arq = fopen(arquivo, "rb")) != NULL) {


		fseek(arq, 0, SEEK_END);
		counttotal = ftell(arq);
		fseek(arq, 0, SEEK_SET);
		in = (char*)calloc(counttotal + 1, sizeof(char));
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
		//tam = tamanhoFila(&fila);
		//ordena(&fila, j);
		/*
		for (i = 0; i < j; i++) {
			printf("%d->%c\n", fila[i].freq,fila[i].val);
		}
		*/

		sNo hufftree;

		printf("\nArvore de Huffman:\n");
		hufftree = *criaHuffman(&fila, j);
		imprime_textual(&hufftree);

		criaTsim(&tsim, &hufftree);
		printf("\n\nTabela de simbolos:\n");
		for (int a = 0; a < 256; a++) {
			if (tsim[a].val != NULL) {
				printf("%c - ", a);
			}
			for (int b = 0; b < 256; b++) {
				if (tsim[a].cod[b] != '\0') {
					printf("%c", tsim[a].cod[b]);
				}
			}
			if (tsim[a].val != NULL) {
				printf("\n");
			}
		}

		int bitcount = 0, bitatual=0;

		printf("\nQuantidade de bits:\n");
		for (i = 0; i < 256; i++) {

			if (freq[i] != 0) {
				printf("%c - %d => ", i, freq[i]);
				bitatual = tamanhoCod(&tsim[i].cod);
				bitatual *= freq[i];
				bitcount += bitatual;
				printf("%d, acumulando %d\n", bitatual, bitcount);

			}

		}

		char *bitsout;
		bitsout = (char *)calloc(bitcount + 1, sizeof(char));
		j = 0;

		for (i = 0; i < counttotal; i++) {
			for (k = 0; k < tamanhoCod(&tsim[in[i]].cod); k++) {
				bitsout[j] = tsim[in[i]].cod[k];
				j++;
			}
		}

		printf("\nConjunto de bits de saida:\n%s\n", bitsout);

		//printf("%d %d", bitcount, counttotal);
		printf("\nTaxa de compressao: %.2f%c\n", (100 - (((float)bitcount / ((float)counttotal * 8)) * 100)), (char)'%');
		printf("Tamanho original: %d bytes -> %d bits\n", counttotal, (counttotal*8));
		printf("Tamanho comprimido: %d bytes -> %d bits\n", ((bitcount % 8) != 0) ? ((bitcount / 8) + 1) : (bitcount / 8), bitcount);

		unsigned char bittemp, mask;
		unsigned char *saida;

		int desloc, andar = 0;
		saida = (char *)calloc(bitcount, sizeof(char));

		for (i = 0; i < (((bitcount % 8) != 0) ? ((bitcount / 8) + 1) : (bitcount / 8)); i++) {
			if ((bitsout[andar + 8] == NULL) && (bitcount%8 != 0)) {
				desloc = 0;
				bittemp = 0x0;
				for (j = 0; j <= (8-(bitcount%8)); j++) {
					if (bitsout[j+andar] == '1') {
						mask = 0x80;
						mask >>= desloc;
						bittemp |= mask;
						desloc++;
					}
					else {
						desloc++;
					}
				}
				saida[i] = bittemp;
			}
			else {
				desloc = 0;
				bittemp = 0x0;
				for (j = 0; j <= 8; j++) {
					if (bitsout[j+andar] == '1') {
						mask = 0x80;
						mask >>= desloc;
						bittemp |= mask;
						desloc++;
					}
					else {
						desloc++;
					}
				}
				
			}
			saida[i] = bittemp;
			andar += 8;
		}
		printf("\nBytes de saida:\n");
		for (i = 0; i < (((bitcount % 8) != 0) ? ((bitcount / 8) + 1) : (bitcount / 8)); i++) {
			printf(" %x", saida[i]);
		}

		//SALVA O ARQUIVO

		FILE *out;
		char nomeout[BUFSIZ];
		
		strcpy(nomeout, arquivo);
		nomeout[strlen(arquivo) - 1] = 'f';
		nomeout[strlen(arquivo) - 2] = 'u';
		nomeout[strlen(arquivo) - 3] = 'h';

		//strncat(nomeout, "huf", (strlen(arquivo) - 4));
		//strcat(nomeout-3, "huf");
		


		if ((out = fopen(nomeout, "wb")) != NULL) {
			fprintf(out, "PFNBDH\n");
			fprintf(out, "%d\n", counttotal);
			fprintf(out, "%s\n", arquivo);
			for (i = 0; i < 256; i++) {
				if (freq[i] != 0) {
					fprintf(out, "%c%d", i, freq[i]);
				}
			}
			fprintf(out, "%c\n",17);
			//memcpy(&out, &saida, (((bitcount % 8) != 0) ? ((bitcount / 8) + 1) : (bitcount / 8)));
			fwrite(saida, sizeof(char), (((bitcount % 8) != 0) ? ((bitcount / 8) + 1) : (bitcount / 8)), out);
		}
		else {
			printf("\nNao foi possivel salvar o arquivo .huf\n");
		}
	
		fclose(out);
	}
	else {
		printf("Nao foi possivel abrir o arquivo\n");
	}

}

void descompactar(char *arquivo) {

	FILE *arq, *out;
	char BUFFER[BUFSIZ], nomeorig[BUFSIZ], letralida;
	int tamoriginal, freq[256], freqlido, bitslidos = 0;
	int i, j;
	sNo fila[256];
	sNo hufftree;

	for (i = 0; i < 256; i++) {
		freq[i] = 0;
	}

	if ((arq = fopen(arquivo, "rb")) != NULL) {
		if (checaHuf(arq)) {
			fseek(arq, 7, SEEK_SET);
			fscanf(arq, "%d", &tamoriginal);
			fgets(BUFFER, 15, arq);
			fscanf(arq, "%s", nomeorig);
			fgets(BUFFER, 15, arq);
			if ((out = fopen(nomeorig, "wb") != NULL)) {

				while (1) {
					fscanf(arq, "%c", &letralida);
					if (letralida != 17) {
						fscanf(arq, "%d", &freqlido);
						freq[letralida] = freqlido;
					}
					else {
						break;
					}

				}

				for (i = 0; i < 256; i++) {
					if (freq[i] != 0) {
						printf("%c -> %d\n", i, freq[i]);
					}
				}
				
				j = 0;
				for (i = 0; i < 256; i++) {
					if (freq[i] != 0) {
						fila[j] = *cria(i, freq[i], inicializa(), inicializa());
						j++;
					}
				}

				printf("\nArvore de Huffman:\n");
				hufftree = *criaHuffman(&fila, j);
				imprime_textual(&hufftree);



				fgets(BUFFER, 15, arq);

				char mask, bittemp;
				int desloc;
				sNo *hufftemp;

				while ((letralida = fgetc(arq)) != EOF) {
					bittemp = letralida;
					desloc = 0;
					for (i = 0; i < 8; i++) {
						bittemp = letralida;
						mask = 0x80;
						mask >>= desloc;
						bittemp ^= mask;
						//FAZER FUNCAO QUE ANDA NA ARVORE DO HUFFMAN
					}


				}

				printf("\n%s\n", BUFFER);
				printf("\n%d", ftell(arq));
				//fseek(arq, 7, SEEK_SET);
			}
			else {
				printf("O arquivo de saida nao pode ser criado\n");
			}
		}
		else {
			printf("O arquivo nao e do tipo .huf\n");
		}
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

int checaHuf(FILE *arq) {
	char buffer[BUFSIZ];
	fgets(buffer, 15, arq);
	if (strcmp(buffer, "PFNBDH\n") == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

int tamanhoCod(char *codigo) {
	int count=0;
	while (codigo[count] != NULL) {
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
		ordena(&*fila, tam);
		notemp1 = tiraUltimo(&*fila, tam);
		tam--;
		notemp2 = tiraUltimo(&*fila, tam);
		tam--;
		hufftree = cria(0, (notemp1->freq) + (notemp2->freq), notemp1, notemp2);
		fila[tam] = *hufftree;
		tam++;
	}

	//free(notemp1);
	//free(notemp2);
	
	return hufftree;

}
