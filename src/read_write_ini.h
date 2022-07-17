/* 
 * File:   read_write_ini.h
 * Author: Alan
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
#include <string.h>

    /*
     * EN: Functions retun status
     * 
     * PT: Status de retorno das funções
     */
    typedef enum {
        INI_OK,
        INI_ERROR,
        INI_FILE_NOT_FOUND,
        INI_SEC_NOT_FOUND,
        INI_KEY_NOT_FOUND,
    } ini_status_t;

    /*
     * EN: Returns 1 if file exists or 0 otherwise
     * 
     * PT: Retorna 1 se o arquivo existir ou 0 caso contrário
     */
    int ini_exist(char* file);

    /*
     * EN: Counts the number of sections
     * 
     * PT: Conta o número se seções
     */
    int ini_count_sections(char* file);

    /*
     * EN: Counts the number of keys given the section name
     * 
     * PT: Conta a quantidade de chaves dado o nome da seção
     */
    int ini_count_keys(char* file, char* section);

    /*
     * EN: Reads the index-th section name
     * 
     * PT: Lê o indice-ésimo nome de seção
     */
    ini_status_t ini_read_section_name(char* file, int index, char *section_name);

    /*
     * EN: Reads the index-th key name in the given the section name
     * 
     * PT: Lê o indice-ésimo nome da chave dado o nome da seção
     */
    ini_status_t ini_read_key_name(char* file, char* section, int index, char *key_name);

    /*
     * EN: Guiven file, section and key names reades value
     * 
     * PT: Dados os nomes de arquivo, seção e chave lê o valor
     */
    ini_status_t ini_read_value(char* file, char* section, char* key, char* value);

    /*
     * EN: Edits or creates file, section, key and value
     * 
     * PT: Edita ou cria arquivo, seção, chave e valor
     */
    ini_status_t ini_write_value(char* file, char* section, char* key, char* value);

#ifdef __cplusplus
}
#endif

#endif /* READ_WRITE_INI_H */
