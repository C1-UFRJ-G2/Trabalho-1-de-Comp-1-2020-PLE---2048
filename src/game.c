#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "game.h"
#include "interface.h"
#include "music.h"

#define VITORIA 1
#define DERROTA 0

unsigned int high_score, score = 0;

/*Sobrescreve o antigo high_score caso ele tenha sido superado*/
void novoHighScore(void) {
    FILE *score_data = fopen("high_score.bin", "wb");
    high_score = score;
    fwrite(&score, sizeof(int), 1, score_data);
    fclose(score_data);
}

/*Recebe o high_score caso ele esteja definido para algum valor, se não,
cria o arquivo high_score.bin para guardar o high_score e coloca 0 como
atual high_score*/
void getHighScore(void) {
    FILE *score_data = fopen("high_score.bin", "rb");
    if (!score_data) {
        novoHighScore();
    } else {
        fread(&high_score, sizeof(int), 1, score_data);
        fclose(score_data);
    }
}

/*Retorna true ou false caso o jogo deva começar, iniciando o processo
de restart caso o jogador tenha perdido ou ganhado*/
short int oJogoContinua(short int matrix[SIZE][SIZE]) {
    short int i, j;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (matrix[i][j] == 2048) {
                return restart(matrix, VITORIA);
            }
        }
    }

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (matrix[i][j] == 0) 
                return 1;
            if (i > 0 && i < SIZE - 1) {
                if (matrix[i][j] == matrix[i + 1][j] || matrix[i][j] == matrix[i - 1][j])
                    return 1;
            }

            if (j > 0 && j < SIZE - 1) {
                if (matrix[i][j] == matrix[i][j + 1] || matrix[i][j] == matrix[i][j - 1])
                    return 1;
            }
        }
    }

    return restart(matrix, DERROTA);
}

/*Percorre a matriz equivalente ao grid do jogo, encontrando os valores iguais a zero
e guardando seus endereços, depois disso, adiciona um novo valor (90% de chance de ser
um 2 e 10% de ser um 4) em um valor igual a zero aleatório*/
void adicionaNovoValor(short int matrix[SIZE][SIZE]) {
    short int i, j, counter = 0,
    **vazios = (short int **) malloc(sizeof(short int *) * SIZE * SIZE);

    for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (matrix[i][j] == 0) {
				*(vazios + counter) = &matrix[i][j];
				counter++;
			}
		}
	}

    if (counter > 0) {
        int i = rand() % counter;
        *vazios[i] = (rand() % 10) ? 2 : 4;
    }

    free(vazios);
}

/*Define o novo high_score caso o jogador o tenha batido e encerra a interface*/
void fimDeJogo(void) {
    if (score > high_score) novoHighScore();
    stop();
    encerraInterface();
    keypad(stdscr, 0);
}

/*Inicia a interface, pega o high_score guardado e adiciona os valores
do inicio do jogo*/
void novoJogo(short int matrix[SIZE][SIZE]) {
    short int i, j;
    score = 0;
    iniciaInterface();
    printInterface(matrix); /* Sem essa função aqui, ao reiniciar o jogo,
                                a interface não aparece até receber input
                                do usuário */
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            matrix[i][j] = 0;
        }
    }
    /*Desativa o comportamento padrão dos direcionais no terminal, e permite usa-los
    como controle*/
    keypad(stdscr, 1);
    getHighScore();
    adicionaNovoValor(matrix);
    adicionaNovoValor(matrix);
    printInterface(matrix);
}

/*Pergunta se o jogador deseja encerrar o jogo iniciando um novo jogo caso
sim, ou realizando os processos de fim de jogo caso não, geralmente é chamado pela função
oJogoContinua(), que recebe o valor que ela retornará o valor que ela retornar, entre 0 e 1,
definindo se o jogo continuará ou não*/
short int restart(short int matrix[SIZE][SIZE], int status) {
    printInterface(matrix);
    move(11, 0); /*Move o ponteiro para o começo da linha onde se encontram os controles*/
    clrtoeol(); /*Apaga essa linha*/

    if (status == VITORIA) printw("Você venceu!\n");
    else printw("Você perdeu!\n");
    printw("Pressione r para jogar novamente ou q para sair\n");

    while (1) {
        char input = getch();

        switch (input) {
            case 'r':
                fimDeJogo();
                novoJogo(matrix);
                return 1;

            case 'q':
                return 0;

            default:
                break;
        }
    }
}
