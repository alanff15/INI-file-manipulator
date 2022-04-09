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
    ini_write_value(file_name, "sec1", "ch1", "001");
    ini_write_value(file_name, "sec1", "ch2", "002");
    ini_write_value(file_name, "sec1", "ch3", "003");
    ini_write_value(file_name, "sec1", "ch4", "004");
    ini_write_value(file_name, "sec2", "cha", "a01");
    ini_write_value(file_name, "sec2", "chb", "a02");
    ini_write_value(file_name, "sec3", "ch", "0.05");
}

void read_example_ini_file(char *file_name) {
    char sec[64], key[64], val[64];
    int n_sec, n_key;
    //varrer seções
    n_sec = ini_count_sections(file_name);
    for (int j, i = 0; i < n_sec; i++) {
        ini_read_section_name(file_name, i, sec);
        printf("  [%s]\n", sec);
        //varrer chaves
        n_key = ini_count_keys(file_name, sec);
        for (j = 0; j < n_key; j++) {
            ini_read_key_name(file_name, sec, j, key);
            ini_read_value(file_name, sec, key, val);
            printf("   %s = %s\n", key, val);
        }
    }
}

int main(int argc, char** argv) {

    char *fname = "teste.ini";

    //criar arquivo
    if (!ini_exist(fname)) {
        create_example_ini_file(fname);
        printf("created file: \"%s\"\n\n", fname);
    }
    //abrir arquivo
    printf("read: %s\n", fname);
    read_example_ini_file(fname);

    //pause
    getchar();
    return (EXIT_SUCCESS);
}