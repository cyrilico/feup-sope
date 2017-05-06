all: clean gerador sauna

gerador: g_aux_functions.c gerador.c
	gcc gerador.c g_aux_functions.c -o gerador -D_REENTRANT -lpthread -lrt -Wall

sauna: s_aux_functions.c sauna.c
	gcc sauna.c s_aux_functions.c -o sauna -D_REENTRANT -lpthread -lrt -Wall

clean:
	rm -f gerador sauna
