all:Mirr

Mirr:mirr_functions.o mirr_main.o
	gcc -o mirr mirr_functions.o mirr_main.o

mirr_main.o:mirr_main.c
	gcc -c mirr_main.c

mirr_functions.o:mirr_functions.c
	gcc -c mirr_functions.c
