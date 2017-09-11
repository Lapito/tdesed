#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OVERFLOW 0;
#define UNDERFLOW 0;

typedef struct Pixel {
	int bg;
	int flag;
}pixel;

typedef struct Pile {
	int i;
	int j;
	struct Pile *next;
}pile;

typedef struct Pile_info {
	int quant;
	struct Pile *top;
}pileInfo;

int fileCheck(FILE *img);
int processImg(FILE *img);
void checkObjects(pixel **pixel, int i, int j, int line, int row, pileInfo *info);
void startPile(pileInfo *info);
int push(pileInfo *info, int i, int j);
int pop(pileInfo *info, int *i, int *j);


int main() {

	FILE *img;
	char kin[20];
	pixel **px;
	int a = 0;

	printf("Input image filename to be analyzed: ");
	scanf("%s", kin);
	/*Abre arquivo, checa se é ppm e segue para o processamento de imagem*/
	if ((img = fopen(kin, "rb")) != NULL) {
		if (fileCheck(img) == 1) {
			if (processImg(img) == 1) {
				
				getch();
				return 0;
			}
			else {
				printf("There was a problem processing the image\n");
				fclose(img);
				getch();
				return 0;
			}
		}
		else {
			printf("File is not an valid image\n");
			fclose(img);
			getch();
			return 0;
		}
	}
	else {
		printf("There was a problem opening the file\n");
		getch();
		return 0;
	}



}

/*CHECKS FOR PPM FILE*/
int fileCheck(FILE *img) {

	char buffer[BUFSIZ];
	fgets(buffer, 15, img);
	if (strcmp(buffer, "P6\n") == 0) {
		fgets(buffer, 15, img);
		fgets(buffer, 15, img);
		if (strcmp(buffer, "255\n") == 0)
			return 1;
	}
	else {
		return 0;
	}

}

int processImg(FILE *img) {
	char buffer[BUFSIZ];
	int line, row;
	pixel **px;
	int i = 0, j = 0;

	/*JUMP TO THE SECOND LINE*/
	fseek(img, 3, SEEK_SET);

	/*CHECK FOR IMAGE DIMENSIONS*/
	fscanf(img, "%d", &row);
	fscanf(img, "%d", &line);

	printf("width: %d height:%d\n", row, line);

	/*ALLOCATE PIXEL MATRIX*/
	if ((px = (pixel**)calloc(line, sizeof(pixel))) != NULL) {
		for (i = 0; i < line; i++) {
			if ((px[i] = (pixel*)calloc(row, sizeof(pixel))) == NULL)
				break;
		}

		/*PROCESSING*/
		int r = 0, g = 0, b = 0;
		int rbg, gbg, bbg;
		int lastpos;

		/*JUMPS LINES ULTIL IT REACHES THE PIXELS*/
		fseek(img, 3, SEEK_SET);
		fscanf(img, "%s", buffer);
		fscanf(img, "%s", buffer);
		fscanf(img, "%s", buffer);
		fgetc(img);
		lastpos = ftell(img);
		
		/*READS BACKGROUND COLOR*/
		rbg = fgetc(img);
		gbg = fgetc(img);
		bbg = fgetc(img);
		printf("Background is %d %d %d\n\n", rbg, gbg, bbg);
		fseek(img, lastpos, SEEK_SET);

		for (i = 0; i < line; i++) {
			for (j = 0; j < row; j++) {
				
				/*CHECKS IF THE PIXEL IS EQUAL TO THE BACKGROUND OR NOT*/
				fscanf(img, "%c%c%c", &r, &g, &b);
				if ((r == rbg) && (g == gbg) && (b == bbg)) {
					px[i][j].bg = 1;
					//printf("#");
				}
				else {
					px[i][j].bg = 0;
					//printf("*");
				}
			}
			//printf("\n");
		}
		fclose(img);

		/*SEARCH FOR OBJECTS*/


		struct Pile_info info;
		/*INITIALIZES PILE*/
		startPile(&info);

		int objects = 0, ip = 0, jp = 0;

		for (i = 0; i < line; i++) {
			for (j = 0; j < row; j++) {
				if ((px[i][j].bg == 0) && (px[i][j].flag != 1)) {
					/*CHECKS IF THE PIXEL ISN'T BACKGROUND AND PUSHES IT TO THE PILE*/
					push(&info, i, j);
					checkObjects(px, i, j, line, row, &info);

					/*CHECKS FOR THE PIXELS THAT HAVEN'T BEEN CHECKED UNTIL THE PIXELS REACH 0 ITEMS*/
					while (info.quant != 0) {
						if (pop(&info, &ip, &jp) != 0) {
							if ((px[ip][jp].flag == 0)) {
								px[ip][jp].flag = 1; /*FLAGS THE POPPED PIXEL*/
								checkObjects(px, ip, jp, line, row, &info);

							}
						}
						else {
							printf("There was a problem popping from the pile\n");
							return 0;
						}
					}
					/*AFTER IT FINISHES THE PILE, THE VARIABLE OBJECTS IS INCREMENTED*/
					objects++;
				}
			}
		}

		printf("There is(are) %d object(s) in this image\n", objects);
		
		/*FREE THE MATRIX*/
		for (i = 0; i < line; i++) {
			free(px[i]);
		}
		free(px);
		return 1;
	}
	printf("There was a problem allocating memory\n");
	fclose(img);
	return 0;
}

void checkObjects(pixel **pixel, int i, int j, int line, int row, pileInfo *info) {
	/*CHECKS FOR PIXELS IN THE SURROUNDINGS AND PUSHES THEM IF THEY AREN'T BACKGROUND*/
	if ((i + 1 < line)) {
		if (pixel[i + 1][j].bg == 0) {
			if (push(info, i + 1, j) != 1) {
				printf("There was a problem pushing the pixel %d%d\n", i+1, j);
			}
		}
	}

	if (i - 1 >= 0) {
		if (pixel[i - 1][j].bg == 0) {
			if (push(info, i - 1, j) != 1) {
				printf("There was a problem pushing the pixel %d%d\n", i+1, j);
			}
		}
	}

	if ((j + 1 < row)) {
		if (pixel[i][j + 1].bg == 0) {
			if ((push(info, i, j + 1)) != 1) {
				printf("There was a problem pushing the pixel %d%d\n", i, j + 1);
			}
		}
	}

	if ((j - 1 >= 0)) {
		if (pixel[i][j - 1].bg == 0) {
			if ((push(info, i, j - 1) != 1)) {
				printf("There was a problem pushing the pixel %d%d\n", i, j - 1);
			}
		}
	}
}



int push(pileInfo *info, int i, int j) {

	pile *aux = (pile*)calloc(1, sizeof(pile)); /*ALLOCATES NEW POSITION FOR THE PILE*/

	if (aux != NULL) {
		aux->i = i;/*CHANGES THE VALUES*/
		aux->j = j;
		if (info->quant == 0) {
			aux->next = NULL;
		}
		else {
			aux->next = info->top;/*SETS NEW NEXT*/
		}
		info->quant++;/*INCREMENTS PILE QUANTITY*/
		info->top = aux;/*SETS NEW TOP*/
		return 1;
	}
	else {
		printf("There was a pile overflow\n");
		return OVERFLOW;
	}

}

int pop(pileInfo *info, int *i, int *j) {

	pile *aux;

	if (info->quant == 0) {
		printf("There was a pile underflow\n");
		return UNDERFLOW;
	}
	else {
		aux = info->top;
		info->top = info->top->next;
		*i = aux->i;/*SETS VALUES FOR THE VARIABLES*/
		*j = aux->j;
		info->quant--;
		free(aux);/*FREE PILE POSITION*/
		return 1;
	}
}

void startPile(pileInfo *info) {
	info->quant = 0;
	info->top = NULL;
}
