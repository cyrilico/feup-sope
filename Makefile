all: clean gerador sauna

gerador: g_aux_functions.c gerador.c
	cc -Wall -o gerador gerador.c g_aux_functions.c

sauna: s_aux_functions.c sauna.c
	cc -Wall -o sauna sauna.c s_aux_functions.c

clean:
	rm gerador sauna
