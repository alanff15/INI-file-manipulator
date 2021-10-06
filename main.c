/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Alan
 *
 * Created on 29 de Junho de 2021, 21:10
 */

#include <stdio.h>
#include <stdlib.h>

#include "read_write_ini.h"

/*
 * 
 */
int main(int argc, char** argv) {
    char file[] = "teste.ini";
    char **sec, **key, valor[64];
    int n, m;

    if (!exist_ini(file)) {
        //gerar arquivo de teste
        write_ini(file, "sec1", "ch1", "001");
        write_ini(file, "sec1", "ch2", "002");
        write_ini(file, "sec1", "ch3", "003");
        write_ini(file, "sec1", "ch4", "004");
        write_ini(file, "sec2", "cha", "a01");
        write_ini(file, "sec2", "chb", "a02");
        write_ini(file, "sec3", "ch", "0.05");
        printf("Arquivo \"%s\" criado\n", file);
    } else {
        //mostrar arquivo
        printf("%s\n", file);
        //contar seções
        n = count_sections_ini(file);
        //alocar memória seções
        sec = malloc(sizeof (char*) * n);
        for (int i = 0; i < n; i++) sec[i] = malloc(sizeof (char) * 64);
        //ler seções
        read_sections_ini(file, sec, &n);
        //varrer seções
        for (int j, i = 0; i < n; i++) {
            printf("\t%s\n", sec[i]);
            ///////////////////////////////////////////////////////////////
            //contar chaves
            m = count_keys_ini(file, sec[i]);
            //alocar memória chaves
            key = malloc(sizeof (char*) * m);
            for (j = 0; j < m; j++) key[j] = malloc(sizeof (char) * 64);
            //ler chaves
            read_keys_ini(file, sec[i], key, &m);
            //varrer chaves
            for (j = 0; j < m; j++) {
                printf("\t\t%s", key[j]);
                //mostrar valores
                read_ini(file, sec[i], key[j], valor);
                printf(" = %s\n", valor);
            }
            //liberar memória chaves
            for (j = 0; j < m; j++) free(key[j]);
            free(key);
            ///////////////////////////////////////////////////////////////
        }
        //liberar memória seções
        for (int i = 0; i < n; i++) free(sec[i]);
        free(sec);
    }

    //pause
    getchar();

    //retorno
    return (EXIT_SUCCESS);
}

