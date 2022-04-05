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

void create_example_ini_file(char *file_name) {
    ini_write(file_name, "sec1", "ch1", "001");
    ini_write(file_name, "sec1", "ch2", "002");
    ini_write(file_name, "sec1", "ch3", "003");
    ini_write(file_name, "sec1", "ch4", "004");
    ini_write(file_name, "sec2", "cha", "a01");
    ini_write(file_name, "sec2", "chb", "a02");
    ini_write(file_name, "sec3", "ch", "0.05");
    printf("Arquivo \"%s\" criado\n", file_name);
}

/*
void open_example_ini_file(char *file_name) {
    char **sec, **key, valor[64];
    int n, m;
    //mostrar arquivo
    printf("%s\n", file_name);
    //contar seções
    n = count_sections_ini(file_name);
    //alocar memória seções
    sec = (char**) malloc(sizeof (char*) * n);
    for (int i = 0; i < n; i++) sec[i] = (char*) malloc(sizeof (char) * 64);
    //ler seções
    read_sections_ini(file_name, sec, &n);
    //varrer seções
    for (int j, i = 0; i < n; i++) {
        printf("\t%s\n", sec[i]);
        ///////////////////////////////////////////////////////////////
        //contar chaves
        m = count_keys_ini(file_name, sec[i]);
        //alocar memória chaves
        key = (char**) malloc(sizeof (char*) * m);
        for (j = 0; j < m; j++) key[j] = (char*) malloc(sizeof (char) * 64);
        //ler chaves
        read_keys_ini(file_name, sec[i], key, &m);
        //varrer chaves
        for (j = 0; j < m; j++) {
            printf("\t\t%s", key[j]);
            //mostrar valores
            read_ini(file_name, sec[i], key[j], valor);
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
 */

void open_example_ini_file(char *file_name) {
    char sec[64], key[64], val[64];
    int n, m;
    //mostrar arquivo
    printf("%s:\n", file_name);
    //varrer seções
    n = ini_count_sections(file_name);
    for (int j, i = 0; i < n; i++) {
        ini_read_section_name(file_name, i, sec);
        printf("  [%s]\n", sec);
        //varrer chaves
        m = ini_count_keys(file_name, sec);
        for (j = 0; j < m; j++) {
            ini_read_key_name(file_name, sec, j, key);
            ini_read(file_name, sec, key, val);
            printf("   %s = %s\n", key, val);
        }
    }
}

int main(int argc, char** argv) {
    if (!ini_exist("teste.ini")) {
        create_example_ini_file("teste.ini"); //criar arquivo
    }
    open_example_ini_file("teste.ini"); //abrir arquivo

    //pause
    getchar();
    return (EXIT_SUCCESS);
}

