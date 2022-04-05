/* 
 * File:   read_write_ini.h
 * Author: alan.andrade
 *
 * Created on August 14, 2019, 2:58 PM
 */

#ifndef READ_WRITE_INI_H
#define READ_WRITE_INI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

    //retornos

    enum {
        INI_OK,
        INI_ERROR,
        INI_FILE_NOT_FOUND,
        INI_NOTHING_FOUND,
        INI_SEC_FOUND,
    };

    /*arquivo*/
    //checar existência do arquivo
    uint8_t ini_exist(char* file);

    /*valores*/
    //ler ini
    uint8_t ini_read(char* file, char* section, char* key, char* value);
    //escrever ini
    uint8_t ini_write(char* file, char* section, char* key, char* value);

    /*seções*/
    //retorna quantidade de seções
    uint32_t ini_count_sections(char* file);
    //retorna nome da seção pelo índice
    uint8_t ini_read_section_name(char* file, uint32_t index, char *section_name);
    //retorna lista de nomes e quantidade de seções
    uint8_t ini_read_sections(char* file, char **sections, uint32_t* count);

    /*chaves*/
    //retorna quantidade de chaves
    uint32_t ini_count_keys(char* file, char* section);
    //retorna nome da chave pelo índice
    uint8_t ini_read_key_name(char* file, char* section, uint32_t index, char *key_name);
    //retorna lista de nomes e quantidade de chaves
    uint8_t ini_read_keys(char* file, char* section, char** keys, uint32_t* count);

#ifdef __cplusplus
}
#endif

#endif /* READ_WRITE_INI_H */
