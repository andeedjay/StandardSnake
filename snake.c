/* Programarea Calculatoarelor, seria CC
 * Tema2 - Snake
 */
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <ctype.h>

#define FOREVER 	1
#define INIT_ROW 	20
#define INIT_COL 	20

typedef struct sarpe
{
	int x, y;
}sarpe;

typedef struct hrana
{
	int x, y;
}hrana;

void deseneaza_chenar(int nrows, int ncols)
{
	int i;
	attron(COLOR_PAIR(3));
	for(i = 0; i < ncols; i++)
	{
		mvaddstr(7, i, "$");
		mvaddstr(nrows-1, i, "$");
	}

	for(i = 8; i < nrows; i++)
	{
		mvaddstr(i ,0, "$");
		mvaddstr(i, ncols-1, "$");
	}
	attroff(COLOR_PAIR(3));
}

void genereaza_hrana(hrana *hrana, int nrows, int ncols)
{
	srand(time(NULL));
	do
	{
		hrana->x = rand() % nrows;
		hrana->y = rand() % ncols;
		if ( ((mvinch(hrana->x, hrana->y) & A_CHARTEXT) == ' ') && (hrana->x > 8))
			break;
	}
	while (1);
	attron(COLOR_PAIR(5));
	mvaddch(hrana->x, hrana->y, 'O');
	attroff(COLOR_PAIR(5));
}

void genereaza_obstacole(int nrows, int ncols)
{
	attron(COLOR_PAIR(3));
	srand(time(NULL));
	int nr_obstacole = 0, i, x, y;
	while (nr_obstacole <= 10)
		nr_obstacole = rand() % 15;

	for (i = 0; i < nr_obstacole; i++)
	{
		do
		{
			x = rand() % nrows;
			y = rand() % ncols;
			if ( ((mvinch(x, y) & A_CHARTEXT) == ' ') && (x > 8))

				break;
		}
		while (1);
		mvaddch(x, y, '$');
	}
	attroff(COLOR_PAIR(3));
}


int main(void)
{
	int row = INIT_ROW, col = INIT_COL, new_row, new_col;
	int nrows, ncols, i;
	int aux_x[150], aux_y[150];
	int nfds=1, sel;
	int acceleratie = 500000;
	char c='a';
	sarpe sarpe[150];
	hrana hrana;

	fd_set read_descriptors;
	struct timeval timeout;
	FD_ZERO(&read_descriptors);
	FD_SET(0, &read_descriptors);
	timeout.tv_sec = 0;
	timeout.tv_usec = acceleratie;

	/* Se inițializează ecranul; initscr se va folosi în mod obligatoriu */
	WINDOW *wnd = initscr();
	start_color();
	init_pair(1, COLOR_BLUE, COLOR_BLUE);
	init_pair(2, COLOR_GREEN, COLOR_GREEN);
	init_pair(3, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(4, COLOR_RED, COLOR_BLUE);
	init_pair(5, COLOR_RED, COLOR_BLUE);
	/*Cu acestea se pot colora anumite zone, sunt niste seturi de culori*/

	/* getmaxyx - este un macro, nu o funcție, așă că nu avem adresă la parametri */
	/* Se va reține în nrows și ncols numărul maxim de linii și coloane */
	getmaxyx(wnd, nrows, ncols);
	/* Se șterge ecranul */
	clear();
	/* Se inhibă afișarea caracterelor introduse de la tastatură */
	noecho();
	/* Caracterele introduse sunt citite imediat - fără 'buffering' */
	cbreak();
	/* Se ascunde cursorul */
	curs_set(0);

	mvaddstr(0,0, "Doriti activarea obstacolelor?");
	mvaddstr(1,0, "1 - Da");
	mvaddstr(2,0, "2 - Nu");
	refresh();

	c = getchar();
	while (c != '1' && c != '2')
		c = getchar();
	clear();
	if (c == '1')
		genereaza_obstacole(nrows, ncols);


	/* Se va afișa un mesaj la poziția formată din primii doi parametri - (par1, par2) */
	mvaddstr(0, 0, "Puteti sa mutati sarpele folosind tastele:");
	mvaddstr(1, 2, "A - stanga");
	mvaddstr(2, 2, "D - dreapta");
	mvaddstr(3, 2, "W - sus");
	mvaddstr(4, 2, "S - jos");
	mvaddstr(5, 0, "Pentru iesire, apasati tasta Q.");

	deseneaza_chenar(nrows, ncols);

	int j = 0;
	for (i = col; i <= col + 4; i++)
	{
		attron(COLOR_PAIR(1));
		mvprintw(row, i, "*");
		sarpe[j].x = row;
		sarpe[j].y = i;
		aux_x[j] = row;
		aux_y[j] = i;
		j++;
		attroff(COLOR_PAIR(1));
	}
	genereaza_hrana(&hrana, nrows, ncols);
	/* Se reflectă schimbările pe ecran */
	refresh();

	int length = 5;
	int directie = 0;
	int scor = 0;

	/*
	directie are valoare:
	0 - stanga
	1 - dreapta
	2 - sus
	3 - jos
	*/

	int stop = 0;
	int start = 0;
	int coliziune = 0;

	/* Rămânem în while până când se primește tasta q */
	c='a';
	/*Sarpele porneste in directia mentionata*/
	while (FOREVER)
	{
		sel = select(nfds, &read_descriptors, NULL, NULL, &timeout);
		if(sel == 1)
		{
			stop = 0;
			c = getchar();
			/*Sarpele trebuie sa porneasca intr-una din directiile sus, jos, stanga pentru ca daca porneste dreapta s-ar manca singur*/
			if (c == 'd' && start == 0)
			{
				stop = 1;
				start = 1;
			}

			coliziune = 0;
			if (tolower(c) == 'q')
				break;

			if (!stop)
			{
				switch (tolower(c))
				{
					case 'a':
						if (directie == 1)
						{
							stop = 1;
							break;
						}
						new_row = row;
						if (col > 1)
						{
							new_col = col - 1;
							directie = 0;
						}
						else
							coliziune = 1;

						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '*' )
							coliziune = 1;

						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '$' )
							coliziune = 1;

						break;

					case 'd':
						if (directie == 0)
						{
							stop = 1;
							break;
						}
						new_row = row;
						if (col + 2 < ncols)
						{
							new_col = col + 1;
							directie = 1;
						}
						else
							coliziune = 1;
						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '*' )
							coliziune = 1;

						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '$' )
							coliziune = 1;

						break;

					case 'w':
						if (directie == 3)
						{
							stop = 1;
							break;
						}
						if (row > 8)
						{
							new_row = row - 1;
							directie = 2;
						}
						else
							coliziune = 1;

						new_col = col;
						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '*' )
							coliziune = 1;

						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '$' )
							coliziune = 1;

						break;

					case 's':
						if (directie == 2)
						{
							stop = 1;
							break;
						}
						if (row + 2 < nrows)
						{
							new_row = row + 1;
							directie = 3;
						}
						else
							coliziune = 1;

						new_col = col;
						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '*' )
							coliziune = 1;

						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '$' )
							coliziune = 1;

						break;
					}
			}
		}
		if(sel == 0)
		{
			stop = 0;
			if (c == 'd' && start == 0)
			{
				stop = 1;
				start = 1;
			}

			coliziune = 0;

			if (!stop)
			{
				switch (tolower(c))
				{
					case 'a':
						if (directie == 1)
						{
							stop = 1;
							break;
						}
						new_row = row;
						if (col > 1)
						{
							new_col = col - 1;
							directie = 0;
						}
						else
							coliziune = 1;

						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '*' )
							coliziune = 1;

						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '$' )
							coliziune = 1;

						break;

					case 'd':
						if (directie == 0)
						{
							stop = 1;
							break;
						}
						new_row = row;
						if (col + 2 < ncols)
						{
							new_col = col + 1;
							directie = 1;
						}
						else
							coliziune = 1;
						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '*' )
							coliziune = 1;

						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '$' )
							coliziune = 1;

						break;

					case 'w':
						if (directie == 3)
						{
							stop = 1;
							break;
						}
						if (row > 8)
						{
							new_row = row - 1;
							directie = 2;
						}
						else
							coliziune = 1;

						new_col = col;
						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '*' )
							coliziune = 1;

						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '$' )
							coliziune = 1;

						break;

					case 's':
						if (directie == 2)
						{
							stop = 1;
							break;
						}
						if (row + 2 < nrows)
						{
							new_row = row + 1;
							directie = 3;
						}
						else
							coliziune = 1;

						new_col = col;
						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '*' )
							coliziune = 1;

						if ( (mvinch(new_row, new_col) & A_CHARTEXT) == '$' )
							coliziune = 1;

						break;
					}
			}
		}

			if ( (mvinch(new_row, new_col) & A_CHARTEXT) == 'O' )
			{
				genereaza_hrana(&hrana, nrows, ncols);
				length++;
				scor++;
				acceleratie -= acceleratie / 8;
			}
			if (coliziune == 1)
				break;
			else
			{
				if (!stop)
				{
					for (i = 0; i < length; i++)
					{
						aux_x[i] = sarpe[i].x;
						aux_y[i] = sarpe[i].y;
					}
					sarpe[0].x = new_row;
					sarpe[0].y = new_col;
					for (i = 1; i < length; i++)
					{
						sarpe[i].x = aux_x[i-1];
						sarpe[i].y = aux_y[i-1];
					}
					for (i = 1; i < length; i++)
					{
						move(aux_x[i], aux_y[i]);
						addch(' ');
					}
					attron(COLOR_PAIR(1));
					for (i = 0; i < length; i++)
					{
						move(sarpe[i].x, sarpe[i].y);
						addch('*');
					}
					attroff(COLOR_PAIR(1));
				}
			}

		mvprintw(5, 40, "Scor: %d", scor);
		refresh();

		timeout.tv_sec = 0;
		timeout.tv_usec = acceleratie;
		FD_SET(0, &read_descriptors);

		row = new_row;
		col = new_col;

	}

	clear();
	mvprintw(12, 27, "Ai pierdut!");
	mvprintw(13, 27, "Ai facut ");
	mvprintw(14, 27, "%d ",scor);
	if(scor == 1)
		mvprintw(14, 30, "punct.");
	else
		mvprintw(14, 30, "puncte.");
	refresh();
	getchar();
	/* Se închide fereastra ncurses */
	endwin();

	return 0;
}
