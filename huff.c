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
sNo *cria(int valor, int frequencia, sNo *sae, sNo *sad, int * stat);
void compactar(char *arquivo);
void descompactar(char *arquivo);
void ordena(sNo *fila, int tam);
sNo *tiraUltimo(sNo *fila, int tamanho, int * stat);
int checaHuf(FILE *arq);
sNo *criaHuffman(sNo *fila, int tamanho, int *stat);
void imprime_textual(sNo *a);
void criaTsim(tSim *tsim, sNo *a, int *stat);



int main(int argc, char *argv[]) {
	if (argc > 1) {
		if ((strcmp(argv[1], "-c") == 0) && (argc == 3)) {
			compactar(argv[2]);
		}
		else if ((strcmp(argv[1], "-d") == 0) && (argc == 3)) {
			descompactar(argv[2]);
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

void compactar(char *arquivo) {

	FILE *arq;
	int freq[256], counttotal, tam, stat;
	int i, j=0,k;
	char *in;

	sNo fila[256]; /*Fila que guarda os nos pra criar a arvore de huffman*/
	static tSim tsim[256]; /*Vetor que guarda os simbolos de cada caracter*/

	printf("Arquivo <%s>, ", arquivo);


	for (i = 0; i < 256; i++) { /*Remove o lixo de memoria da tabela de freqeuncias*/
		freq[i] = 0;
	}

	if ((arq = fopen(arquivo, "rb")) != NULL) {


		fseek(arq, 0, SEEK_END);
		counttotal = ftell(arq); /*Vai ate o fim do arquivo e pega a quantidade total de bytes do arquivo*/
		printf("%d\n\n", counttotal);
		fseek(arq, 0, SEEK_SET);
		in = (char*)calloc(counttotal + 1, sizeof(char)); /*Aloca o vetor que guardara o arquivo e poupar o arquivo original*/
		if (in == NULL) {
			printf("\nHouve um erro\n");
			return 0;
		}

		for (i = 0; i < counttotal; i++) { /*Percorre o arquivo incrementando as ocorrencias dos caracteres*/
			char c;						   /*e colocando os caracteres numa string dinamica*/
			c = fgetc(arq);
			in[i] = c;
			freq[c]++;
		}
		fclose(arq);

		printf("Conteudo:\n");
		printf("%s\n\n", in);

		printf("Lista de frequencias:\n");

		for (i = 0; i < 256; i++) { /*Imprime caracter seguido da sua frequencia*/
			if (freq[i] != 0) {
				printf("%c -> %d\n", i, freq[i]);
			}
		}

		for (i = 0; i < 256; i++) {
			if (freq[i] != 0) {
				fila[j] = *cria(i, freq[i], inicializa(), inicializa(), &stat); /*Para cada caractere da tabela de frequencia, um no sem filhos eh criado*/
				if (stat == ERRO) {
					printf("\nOcorreu um erro\n");
					return 0;
				}
				j++;
			}
		}

		sNo hufftree; /*Variavel que guardara a arvore de huffman*/

		printf("\nArvore de Huffman:\n");
		hufftree = *criaHuffman(&fila, j, &stat); /*Cria arvore de huffman*/
		if (stat == ERRO) {
			printf("\nOcorreu um erro\n");
			return 0;
		}
		imprime_textual(&hufftree);

		criaTsim(&tsim, &hufftree, &stat); /*Cria tabela de simbolos*/
		if (stat == ERRO) {
			printf("\nOcorreu um erro\n");
			return 0;
		}
		printf("\n\nTabela de simbolos:\n"); /*Imprime o caracter seguido do seu codigo*/
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

		long int bitcount = 0, bitatual=0;

		printf("\nQuantidade de bits:\n"); /*Calcula quantidade de bits do arquivo compactado*/
		for (i = 0; i < 256; i++) {

			if (freq[i] != 0) {
				printf("%c - %d => ", i, freq[i]);
				bitatual = strlen(&tsim[i].cod);
				bitatual *= freq[i];
				bitcount += bitatual;
				printf("%ld, acumulando %ld\n", bitatual, bitcount);

			}

		}

		char *bitsout;
		bitsout = (char *)calloc(bitcount + 1, sizeof(char)); /*Aloca a string que guardara os bits baseado na conta feita acima*/
		j = 0;                                                /*e coloca coloca os bits de cada letra na string*/

		if (bitsout == NULL) {
			printf("\nOcorreu um erro\n");
			return 0;
		}

		for (i = 0; i < counttotal; i++) {
			for (k = 0; k < strlen(&tsim[in[i]].cod); k++) {
				bitsout[j] = tsim[in[i]].cod[k];
				j++;
			}
		}

		printf("\nConjunto de bits de saida:\n%s\n", bitsout);

		//printf("%d %d", bitcount, counttotal);
		printf("\nTaxa de compressao: %.2f%c\n", (100 - (((float)bitcount / ((float)counttotal * 8)) * 100)), (char)'%');
		printf("Tamanho original: %d bytes -> %d bits\n", counttotal, (counttotal*8));
		printf("Tamanho comprimido: %d bytes -> %d bits\n", ((bitcount % 8) != 0) ? ((bitcount / 8) + 1) : (bitcount / 8), bitcount);

		/*Monta os bits para serem gravados*/

		unsigned char bittemp, mask;
		unsigned char *saida;

		int desloc, andar = 0;
		saida = (char *)calloc(bitcount, sizeof(char));

		for (i = 0; i < (((bitcount % 8) != 0) ? ((bitcount / 8) + 1) : (bitcount / 8)); i++) { /*Se a quantidade de bits mod 8 for diferente de 0, adiciona um byte extra;*/
			if ((bitsout[andar + 8] == NULL) && (bitcount%8 != 0)) { /*Caso for necessario, completa o ultimo byte com zeros*/
				desloc = 0;
				bittemp = 0x0;
				for (j = 0; j < (8-(bitcount%8)); j++) {
					if (bitsout[j+andar] == '1') {
						mask = 0x80; /*Mascara eh 10000000*/
						mask >>= desloc; /*Desloca com cada iteracao*/
						bittemp |= mask; /*Adiciona ao byte final*/
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
				for (j = 0; j < 8; j++) {
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
		for (i = 0; i < (((bitcount % 8) != 0) ? ((bitcount / 8) + 1) : (bitcount / 8)); i++) { /*Imprime bytes de saida em hexadecimal*/
			printf(" %x", saida[i]);
		}


		/*SALVA O ARQUIVO*/

		FILE *out;
		char nomeout[BUFSIZ];
		
		strcpy(nomeout, arquivo);
		nomeout[strlen(arquivo) - 1] = 'f'; /*Substitui o formato original por .huf*/
		nomeout[strlen(arquivo) - 2] = 'u';
		nomeout[strlen(arquivo) - 3] = 'h';

		if ((out = fopen(nomeout, "wb")) != NULL) { /*Abre arquivo para gravacao com extencao .huf*/
			fprintf(out, "PFNBDH\n"); /*Identificador do arquivo*/
			fprintf(out, "%d\n", counttotal); /*Salva a quantidade de bytes do arquivo original*/
			fprintf(out, "%s\n", arquivo); /*Salva a tabela de frequencia*/
			for (i = 0; i < 256; i++) {
				if (freq[i] != 0) {
					fprintf(out, "%c%d", i, freq[i]);
				}
			}
			fprintf(out, "%c\n",17); /*Aqui foi decicido colocar esse caractere delimitador para ajudar na descompactacao*/
			fwrite(saida, sizeof(char), (((bitcount % 8) != 0) ? ((bitcount / 8) + 1) : (bitcount / 8)), out); /*Salva bytes comprimidos*/
			printf("\n\nArquivo <%s> compactado para <%s>\n", arquivo, nomeout);
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
	int tamoriginal, freq[256], freqlido, bitslidos = 0, stat;
	int i, j;
	sNo fila[256];
	sNo hufftree;

	for (i = 0; i < 256; i++) {
		freq[i] = 0;
	}

	if ((arq = fopen(arquivo, "rb")) != NULL) {
		if (checaHuf(arq)) { /*Verifica o identificador do arquivo*/
			fseek(arq, 7, SEEK_SET);
			fscanf(arq, "%d", &tamoriginal); /*Le o nome e tamanho original*/
			fgets(BUFFER, 15, arq);
			fscanf(arq, "%s", nomeorig);
			fgets(BUFFER, 15, arq);
			if ((out = fopen(nomeorig, "wb")) != NULL) {

				while (1) {
					fscanf(arq, "%c", &letralida); /*Le a tabela de frequencia salva e a remonta*/
					if (letralida != 17) { /*Ate o caractere delimitador*/
						fscanf(arq, "%d", &freqlido);
						freq[letralida] = freqlido;
					}
					else {
						break;
					}

				}

				for (i = 0; i < 256; i++) { /*Imprime tabela de simbolos*/
					if (freq[i] != 0) {
						printf("%c -> %d\n", i, freq[i]);
					}
				}
				
				j = 0;
				for (i = 0; i < 256; i++) {
					if (freq[i] != 0) {
						fila[j] = *cria(i, freq[i], inicializa(), inicializa(), &stat); /*Cria nos sem filhos com caracteres*/
						if (stat == ERRO) {
							printf("\nOcorreu um erro\n");
							return 0;
						}
						j++;
					}
				}

				printf("\nArvore de Huffman:\n");
				hufftree = *criaHuffman(&fila, j, &stat); /*Recria a arvore de huffman*/
				if (stat == ERRO) {
					printf("\nOcorreu um erro\n");
					return 0;
				}
				imprime_textual(&hufftree);
				printf("\n\n");

				fgets(BUFFER, 15, arq); /*Pula linha*/

				unsigned char mask, bittemp;
				int desloc, locatual, locatotal;
				sNo *hufftemp;
				hufftemp = &hufftree; /*Ponteiro para a arvore original*/

				locatual = ftell(arq);
				fseek(arq, 0, SEEK_END);
				locatotal = ftell(arq);
				fseek(arq, locatual, SEEK_SET); /*Delimita ate onde ler o arquivo baseado no ultimo byte*/

				while (ftell(arq) != locatotal) { /*Ira ler o arquivo ate o fim*/
					letralida = fgetc(arq);
					desloc = 0;
					for (i = 0; i < 8; i++) { /*Desmontara o byte bit a bit e andara na arvore de huffman*/
						bittemp = letralida;  /*Sendo para 0, ir para a esquerda e 1 para direita.*/
						mask = 0x80;
						mask >>= desloc;
						bittemp &= mask;
						bittemp <<= desloc;
						if (bitslidos != tamoriginal) {
							if (bittemp == 0x80) { /*QUANDO FOR 1, MAS 1000 0000 E NAO 0000 0001*/
								hufftemp = hufftemp->dir; /*Anda para a direita caso 1*/
								if ((hufftemp->esq == NULL) && (hufftemp->dir == NULL)) { /*Caso encontre uma folha, imprime a letra correspondente*/
									fputc(hufftemp->val, out);
									bitslidos++;
									hufftemp = &hufftree; /*Volta para o topo da arvore*/
								}
								desloc++; /*Incrementa deslocamento da mascara*/
							}
							else {
								hufftemp = hufftemp->esq; /*Anda para a esqueda caso 0*/
								if ((hufftemp->esq == NULL) && (hufftemp->dir == NULL)) {
									fputc(hufftemp->val, out);
									bitslidos++;
									hufftemp = &hufftree;
								}
								desloc++;
							}
						}
						else {
							break;
						}
					}
				}
				
				fclose(out); /*Fecha e salva o arquivo descompactado*/
				printf("\n<%s> descompactado com sucesso!\n", nomeorig);
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

sNo *inicializa(void) { /*Inicializa no nulo*/
	return NULL;
}

sNo *cria(int valor, int frequencia, sNo *sae, sNo *sad, int *stat) { /*Cria arvore de acordo com oque for especificado*/
	sNo *no;
	no = (sNo *)malloc(sizeof(sNo));
	if (no == NULL) *stat = ERRO;
	else {
		no->esq = sae;
		no->dir = sad;
		no->val = valor;
		no->freq = frequencia;
		*stat = OK;
	}
	return no;
}

void ordena(sNo *fila, int tam) {
	int i, j, stat;
	sNo *aux;
	aux = cria(0, 0, inicializa(), inicializa(), &stat);
	if (stat == ERRO) {
		printf("\nOcorreu um erro\n");
		return 0;
	}
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

sNo *tiraUltimo(sNo *fila, int tamanho, int *stat) { /*Retira o ultimo da fila de subarvores*/
	sNo *aux;
	int i, status;
	aux = cria(0, 0, inicializa(), inicializa(), &status);
	if (status == ERRO) {
		printf("\nOcorreu um erro\n");
		*stat = ERRO;
		return 0;
	}
	*aux = fila[tamanho - 1];
	fila[tamanho - 1] = *cria(0, 0, inicializa(), inicializa(), &status); /*Substitui por no vazio*/
	if (stat == ERRO) {
		printf("\nOcorreu um erro\n");
		*stat = ERRO;
		return 0;
	}
	*stat = OK;
	return aux;
}

int checaHuf(FILE *arq) { /*Checa identificador do arquivo*/
	char buffer[BUFSIZ];
	fgets(buffer, 15, arq);
	if (strcmp(buffer, "PFNBDH\n") == 0) {
		return OK;
	}
	else {
		return ERRO;
	}
}

sNo *criaHuffman(sNo *fila, int tamanho, int *stat) {
	int tam = tamanho, status;
	sNo *hufftree, *notemp1, *notemp2;
	hufftree = (sNo*)calloc(1, sizeof(sNo));
	notemp1 = (sNo*)calloc(1, sizeof(sNo));
	notemp2 = (sNo*)calloc(1, sizeof(sNo)); /*Retira os dois caracteres com menor frequencia*/

	if ((hufftree == NULL) || (notemp1 == NULL) || (notemp2 == NULL)) {
		printf("\nOcorreu um erro\n");
		*stat = ERRO;
		return NULL;
	}

	while (tam > 1) { /*Enquanto o tamanho for maior que 1*/
		ordena(&*fila, tam);
		notemp1 = tiraUltimo(&*fila, tam, &status);
		tam--;
		notemp2 = tiraUltimo(&*fila, tam, &status);
		tam--;
		hufftree = cria(0, (notemp1->freq) + (notemp2->freq), notemp1, notemp2, &status); /*Cria um no com as frequencias somadas dos nos temporarios e aponta para eles*/
		if (status == ERRO) {
			printf("\nOcorreu um erro\n");
			*stat = ERRO;
			return NULL;
		}
		fila[tam] = *hufftree;
		tam++;
	}

	*stat = OK;
	return hufftree; /*Retorna arvore final*/

}

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

void criaTsim(tSim *tsim, sNo *a, int *stat) {
	static int i = 0;
	int k;
	static char trail[256];
	tSim *aux;

	aux = (tSim*)calloc(1, sizeof(tSim));

	if (aux == NULL) {
		printf("\nOcorreu um erro\n");
		*stat = ERRO;
		return 0;
	}

	if (a == NULL)
		return NULL;
	if (a->esq != NULL) {
		trail[i] = '0';
		i++;
		criaTsim(tsim, a->esq, &*stat);
	}
	if (a->dir != NULL) {
		trail[i] = '1';
		i++;
		criaTsim(tsim, a->dir, &*stat);
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
			*stat = OK;
			return 1;
		}

		trail[i - 1] = NULL;
		i--;

	}
}
