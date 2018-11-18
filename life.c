/*
Este programa es un simulador de células autómatas, conocido como El juego de la Vida.
Consiste de un mundo 2D dividido en celdas (delimitado por una matriz de 10x10). 
Cada celda contiene una célula, la cual tiene dos estados: viva o muerta, en cada generación.
El juego consiste de una serie de reglas que describen cómo estas células evolucionan generación a generación.

Estas reglas calculan el estado de una célula para la próxima generación como función del estado de las células colindantes (vecinas) en la generación actual:
a) Una célula viva con menos de 2 vecinas, muere;
b) Una célula con más de 3 vecinas, muere;
c) Una célula viva con exactamente 2 o 3 vecinas, sobrevive;
d) Una célula muerta con exactamente 3 vecinas, obtiene vida nuevamente.
*/

/*
Grupo 5:
Smolkin, Pablo
Matorras Bratsche, Belén
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "disdrv.h"
#include "termlib.h"
#define FILAS 16
#define COLUMNAS 16
#define DEAD 0 //' '
#define ALIVE 1 //'*'
#define CONTINUE 0
#define END 1

//La funcion initworlds inicia las dos matrices old_world y new_world con celulas vivas o muertas segun corresponda.
int initworlds(char old_world[FILAS][COLUMNAS], char new_world[FILAS][COLUMNAS]);

//La funcion dispworld muestra en consola el estado actual de la matriz pasada como parametro.
int dispworld(char old_world[FILAS][COLUMNAS]);

//La funcion set_state determina el estado de una celula en la proxima generacion en base a su estado actual y el de sus vecinas.
char set_state(int x, int y, char old_world[FILAS][COLUMNAS]);

//La funcion isinrange evalua si la celula vecina a analizar esta dentro del rango permitido.
int isinrange(int xcoord, int ycoord, int filas, int columnas);

//La funcion get_state determina si una celula esta viva o muerta.
char get_state(int i, int j, char old_world[FILAS][COLUMNAS]);

//La funcion evolve llena la matriz new_world con las celulas de la proxima generacion.
int evolve(char old_world[FILAS][COLUMNAS], char new_world[FILAS][COLUMNAS]);

//La funcion finalize_evolution convierte la matriz new_world en la actual para luego mostrarla en pantalla.
int finalize_evolution(char old_world[FILAS][COLUMNAS], char new_world[FILAS][COLUMNAS]);

//La funcion life_eval verifica que exista por lo menos una celula viva en el mundo.
char life_eval(char world[FILAS][COLUMNAS]);

//La funcion end_eval evalua si la matriz actual y la nueva son iguales (lo cual significaria que se entro en un loop).
int end_eval(char old_world[FILAS][COLUMNAS], char new_world[FILAS][COLUMNAS]);

//La funcion delay genera una espera para visualizar mejor las generaciones en consola.
int delay(void);

int main(void)
{
	char old_world[FILAS][COLUMNAS];
	char new_world[FILAS][COLUMNAS];

	initworlds(old_world, new_world);
	display_init();

	do  			// El ciclo principal del programa, corre hasta que end_eval o life_eval le indiquen lo contrario.
	{
		evolve(old_world, new_world);
		if(end_eval(old_world, new_world) == END)
			return 0;
		system("clear"); // Esta instruccion hace un Ctrl+l para limpiar la consola y lograr una visualizacion mas clara. No afecta el funcionamiento de manera significativa.
		finalize_evolution(old_world, new_world);
		dispworld(old_world);
		//delay();
	}
	while(life_eval(old_world) == ALIVE);

	return 0;
}


int initworlds(char old_world[FILAS][COLUMNAS], char new_world[FILAS][COLUMNAS]) // Esta funcion determina las condiciones iniciales de los mundos.
{
	int i,j;

	for(i = 0; i < FILAS; ++i)
	{
		for(j = 0; j < COLUMNAS; ++j)
			old_world[i][j] = new_world[i][j] = DEAD;	// Ambas matrices se llenan de celulas muertas.
	}

	old_world[0][0] = old_world[1][1] = old_world[2][2] = old_world[3][3] = old_world[4][4] = ALIVE;
	old_world[5][5] = old_world[6][6] = old_world[7][7] = old_world[8][8] = old_world[9][9] = ALIVE;
	old_world[0][9] = old_world[1][8] = old_world[2][7] = old_world[3][6] = old_world[4][5] = ALIVE;
	old_world[5][4] = old_world[6][3] = old_world[7][2] = old_world[8][1] = old_world[9][0] = ALIVE;	// La matriz old_world recibe sus celulas vivas iniciales.
	old_world[8][2] = old_world[3][1] = old_world[6][4] = old_world[3][3] = old_world[8][8] = ALIVE;
	old_world[3][7] = old_world[7][2] = old_world[9][4] = old_world[2][2] = old_world[2][4] = ALIVE;
	old_world[3][6] = old_world[2][8] = old_world[2][5] = old_world[6][8] = old_world[5][8] = ALIVE;

	return 0;
}

int dispworld(char world[FILAS][COLUMNAS])	// Esta funcion muestra en consola la matriz que recibe como parametro.
{
	int i,j;

	for(i = 0; i < FILAS; ++i)
	{
		for(j = 0; j < COLUMNAS; ++j)
			(world[i][j])? display_write(i,j,D_ON) : display_write(i,j,D_OFF);
	}
	display_update();

	return 0;
}

char set_state(int x, int y, char old_world[FILAS][COLUMNAS]) // Esta función retorna el estado que deberá tener una célula en la próxima generación
{
	int i, count = 0;
	int dir[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}}; // Posiciones de células colindantes a una célula dada
	int xcoord, ycoord;
	char cell = get_state(x, y, old_world); // Célula principal a analizar

	for (i = 0; i < 8; ++i) // Recorro las 8 células colindantes a la célula principal
	{
		xcoord = x+dir[i][0];
		ycoord = y+dir[i][1];

		if (isinrange(xcoord, ycoord, FILAS, COLUMNAS) && old_world[xcoord][ycoord] == ALIVE) // Si están en rango y vivas, las cuento como vecinas
			count++;
	}

	switch(cell) // Analizo qué sucederá con la célula principal según las reglas del juego y devuelvo au estado correspondiente
	{
		case ALIVE:
			if (count < 2 || count > 3)
				return DEAD;
			else
				return ALIVE;
			break;
		case DEAD:
			if(count == 3)
				return ALIVE;
			else
				return DEAD;
			break;
	}
	return 0;
}

int isinrange(int xcoord, int ycoord, int filas, int columnas) // Esta función indica si un par de coordenadas están dentro de la matriz dada
{
	enum boolean {false, true}; //si se puede incluir stdbool.h esto no hace falta, pero habria que cambiar a bool el tipo de dato que devuelve isinrange
	
	if (xcoord < 0 || xcoord >= columnas)
		return false;
	else if(ycoord < 0 || ycoord >= filas)
		return false;
	else
		return true; // Si ambas coordenadas están dentro de la matriz, entonces están dentro del rango de análisis
}

char get_state(int i, int j, char old_world[FILAS][COLUMNAS]) // Esta función devuelve el estado de una célula
{
	if(old_world[i][j] == ALIVE)
		return ALIVE;
	else
		return DEAD;
}

int evolve(char old_world[FILAS][COLUMNAS], char new_world[FILAS][COLUMNAS]) // Esta función setea las células evolucionadas en la matriz de la nueva generación
{
	int i,j;

	for(i = 0; i < FILAS; ++i) 
	{
		for(j = 0; j < COLUMNAS; ++j)
			new_world[i][j] = set_state(i,j,old_world);
	}
	return 0;
}

int finalize_evolution(char old_world[FILAS][COLUMNAS], char new_world[FILAS][COLUMNAS]) // Esta función pasa cada elemento de la matriz new_world a la actual
{
	int i,j;

	for(i = 0; i < FILAS; ++i)
	{
		for(j = 0; j < COLUMNAS; ++j)
		{
			old_world[i][j] = new_world[i][j];
			new_world[i][j] = DEAD; // Y limpia la matriz new_world para ser utilizada en la próxima evolución
		}
	}
	return 0;
}

char life_eval(char world[FILAS][COLUMNAS]) // Esta funcion evalua si hay celulas vivas en el mundo para evitar caer en un loop.
{
	int i,j;
	char cell = DEAD;

	for(i = 0; i < FILAS; ++i)
	{
		for(j = 0; j < COLUMNAS; ++j)
		{
			if(world[i][j] == ALIVE) // Si encuentra por lo menos una celula viva devuelve ALIVE, si no devuelve DEAD.
				cell = ALIVE;
		}
	}
	return cell;
}

int end_eval(char old_world[FILAS][COLUMNAS], char new_world[FILAS][COLUMNAS])	// Esta funcion evalua si las matrices old_world y new_world son iguales.
{
	int i, j;
	
	for(i = 0; i < FILAS; ++i)
	{
		for(j = 0; j < COLUMNAS; ++j)
		{
			if(old_world[i][j] != new_world[i][j]) // Si cualquier celula con las mismas coordenadas en old_world y new_world es distinto, devuelve CONTINUE.
				return CONTINUE;
		}
	}
	return END;
}

int delay(void)	// Esta funcion genera un tiempo de espera para facilitar la visualizacion del juego.
{
	unsigned long i,j;

	for(i = 0; i <= 5; ++i)
		for(j = 0; j < 19000000; ++j);	// Estos ciclos for no ejecutan ninguna instruccion, solo pasan el tiempo.

	return 0;
} 
