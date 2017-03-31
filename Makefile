# Programarea Calculatoarelor, seria CC
# Tema2 - Snake

build: snake1 snake2 select rand

snake1: snake1.c
	gcc -Wall snake1.c -o snake1 -lcurses

snake2: snake2.c
	gcc -Wall snake2.c -o snake2 -lcurses

select: select.c
	gcc -Wall select.c -o select

rand: rand.c
	gcc -Wall rand.c -o rand

.PHONY:

clean:
	rm -f snake1 snake2 rand select

run_snake1:
	./snake1

run_snake2:
	./snake2

run_select:
	./select

run_rand:
	./rand
