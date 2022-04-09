/*
 * File:   read_write_ini.c
 * Author: alan.andrade
 *
 * Created on August 14, 2019, 1:09 PM
 */

#include "read_write_ini.h"

int ini_exist(char* file) {
    int ret = 0;
    FILE *arq = fopen(file, "r");
    ret = (arq != NULL);
    fclose(arq);
    return ret;
}

int ini_count_sections(char* file) {
    FILE *arq;
    int count = 0;
    arq = fopen(file, "r");
    if (arq == NULL) return INI_FILE_NOT_FOUND;
    //varrer caracteres do arquivo
    for (char c = getc(arq); c != EOF; c = getc(arq)) {
        //ignorar comentários
        while (c == ';') {
            for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
            c = getc(arq); //lê primeiro caractere da próxima linha
        }
        //procurar seção
        if (c == '[') {
            //ignora espaços iniciais
            do c = getc(arq); while (c == ' ');
            //ler nome
            for (; c != EOF && c != 0x0A && c != ' ' && c != ']'; c = getc(arq));
            //ignora espaços finais, se houver
            if (c != ']') {
                do c = getc(arq); while (c == ' ');
            }
            //seção encontrada
            if (c == ']') {
                count++; //contar seções
                for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
            }
        }
    }
    fclose(arq);
    //retorno
    return count;
}

int ini_count_keys(char* file, char* section) {
    FILE *arq;
    int sec_flag = 0;
    int i_aux, len;
    arq = fopen(file, "r");
    if (arq == NULL) return INI_FILE_NOT_FOUND;
    //zerar contagem
    int count = 0;
    //varrer caracteres do arquivo
    for (char c = getc(arq); c != EOF; c = getc(arq)) {
        //ignorar comentários
        while (c == ';') {
            for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
            c = getc(arq); //lê primeiro caractere da próxima linha
        }
        //procurar seção
        if (c == '[') {
            if (sec_flag) break; //termina busca se chegar a próxima seção sem achar a chave
            do c = getc(arq); while (c == ' '); //ignora espaços
            for (i_aux = 0, len = strlen(section); c != EOF && i_aux < len;) {//lê o nome da seção
                if (c != section[i_aux++]) {
                    for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
                    break;
                }
                do c = getc(arq); while (c == ' '); //ignora espaços
            }
            //seção encontrada
            if (c == ']') {
                sec_flag = 1; //flag de seção encontrada
                for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
            }
        }
        //procurar chave
        if (sec_flag) {
            //ignora espaços iniciais
            while (c == ' ' || c == 0x0A) c = getc(arq);
            //se for a próxima seção
            if (c == '[') break;
            //zerar ponteiro de nome da chave
            i_aux = 0;
            //ler nome da chave
            for (; c != ' ' && c != 0x0A && c != EOF; c = getc(arq));
            //incrementar contagem de chave
            count++;
            //ignora resto da linha
            while (c != 0x0A && c != EOF) c = getc(arq);
        }
    }
    fclose(arq);
    //retorno
    return count;
}

ini_status_t ini_read(char* file, char* section, char* key, char* value) {
    FILE *arq;
    int sec_flag = 0;
    int key_flag = 0;
    int i_aux, i_result = 0, len;
    arq = fopen(file, "r");
    if (arq == NULL) return INI_FILE_NOT_FOUND;
    //varrer caracteres do arquivo
    for (char c = getc(arq); c != EOF; c = getc(arq)) {
        //ignorar comentários
        while (c == ';') {
            for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
            c = getc(arq); //lê primeiro caractere da próxima linha
        }
        //procurar seção
        if (c == '[') {
            if (sec_flag) break; //termina busca se chegar a próxima seção sem achar a chave
            do c = getc(arq); while (c == ' '); //ignora espaços
            for (i_aux = 0, len = strlen(section); c != EOF && i_aux < len;) {//lê o nome da seção
                if (c != section[i_aux++]) {
                    for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
                    break;
                }
                do c = getc(arq); while (c == ' '); //ignora espaços
            }
            //seção encontrada
            if (c == ']') {
                sec_flag = 1; //flag de seção encontrada
                for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
            }
        }
        //procurar chave
        if (sec_flag) {
            for (i_aux = 0, len = strlen(key); c != EOF && i_aux < len;) {//lê o nome da chave
                if (c != key[i_aux++]) {
                    for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
                    break;
                }
                do c = getc(arq); while (c == ' '); //ignora espaços
            }
            //chave encontrada
            if (c == '=') {
                key_flag = 1; //flag de chave encontrada
                do c = getc(arq); while (c == ' '); //ignora espaços
                for (i_result = 0; c != EOF && c != 0x0A && c != ';';) { //ler resto da linha até encontrar comentário
                    value[i_result++] = c;
                    //do c = getc(arq); while (c == ' '); //ignora espaços
                    c = getc(arq);
                }
            }
        }
        if (key_flag) break;
    }
    fclose(arq);
    //saida
    value[i_result] = '\0';
    //retorno
    if (key_flag && sec_flag) return INI_OK;
    else if (sec_flag) return INI_SEC_FOUND;
    else if (key_flag) return INI_ERROR;
    else return INI_NOTHING_FOUND;
}

ini_status_t ini_write(char* file, char* section, char* key, char* value) {
    FILE *arq;
    int flag, sec_flag = 0, key_flag = 0;
    int n, pos = 0, pos_value = 0, i_aux, len;
    char c, *data;
    char aux[64];
    //avaliar
    flag = ini_read(file, section, key, aux);
    if (flag == INI_FILE_NOT_FOUND) {
        arq = fopen(file, "w");
        if (arq == NULL) return INI_ERROR;
        fclose(arq);
        flag = INI_NOTHING_FOUND;
    }
    switch (flag) {
        case INI_NOTHING_FOUND:
            //contar caracteres
            arq = fopen(file, "r");
            for (n = 0, c = getc(arq); c != EOF; n++, c = getc(arq));
            fclose(arq);
            //gravar chave nova ao final do arquivo
            arq = fopen(file, "a");
            if (n > 0) fprintf(arq, "\n\n"); //chave
            fprintf(arq, "[%s]\n", section);
            fprintf(arq, "%s = %s", key, value);
            fclose(arq);
            break;

        case INI_SEC_FOUND:
            //contar caracteres
            arq = fopen(file, "r");
            for (n = 0, c = getc(arq); c != EOF; n++, c = getc(arq));
            fclose(arq);
            //ler arquivo num vetor
            data = (char*) malloc(n);
            arq = fopen(file, "r");
            for (int i = 0; i < n; i++) data[i] = getc(arq);
            fclose(arq);
            //encontrar posição de inserção
            pos = 0;
            for (c = data[pos++]; pos < n; c = data[pos++]) {
                //ignorar comentários
                while (c == ';') {
                    for (; pos < n && c != 0x0A; c = data[pos++]); //ignorar resto da linha
                    c = data[pos++]; //lê primeiro caractere da próxima linha
                }
                //procurar seção
                if (c == '[') {
                    if (sec_flag) break; //termina busca se chegar a próxima seção sem achar a chave
                    do c = data[pos++]; while (c == ' '); //ignora espaços
                    for (i_aux = 0, len = strlen(section); pos < n && i_aux < len;) {//lê o nome da seção
                        if (c != section[i_aux++]) {
                            for (; pos < n && c != 0x0A; c = data[pos++]); //ignorar resto da linha
                            break;
                        }
                        do c = data[pos++]; while (c == ' '); //ignora espaços
                    }
                    //seção encontrada
                    if (c == ']') {
                        sec_flag = 1; //flag de seção encontrada
                        for (; pos < n && c != 0x0A; c = data[pos++]); //ignorar resto da linha
                    }
                }
            }
            if (pos != n) {
                pos--;
                do pos--; while (data[pos] == ' ' || data[pos] == 0x0A);
                pos++;
            }
            //gravar arquivo
            arq = fopen(file, "w");
            for (int i = 0; i < pos; i++) fprintf(arq, "%c", data[i]); //antes
            if (pos > 0) fprintf(arq, "\n"); //chave
            fprintf(arq, "%s = %s", key, value); //chave
            if (pos < n) fprintf(arq, "\n"); //chave
            for (int i = pos; i < n; i++) fprintf(arq, "%c", data[i]); //depois
            fclose(arq);
            //liberar memória
            free(data);
            break;

        case INI_OK:
            //contar caracteres
            arq = fopen(file, "r");
            for (n = 0, c = getc(arq); c != EOF; n++, c = getc(arq));
            fclose(arq);
            //ler arquivo num vetor
            data = (char*) malloc(n);
            arq = fopen(file, "r");
            for (int i = 0; i < n; i++) data[i] = getc(arq);
            fclose(arq);
            //encontrar posição de inserção
            pos = 0;
            for (c = data[pos++]; pos < n; c = data[pos++]) {
                //ignorar comentários
                while (c == ';') {
                    for (; pos < n && c != 0x0A; c = data[pos++]); //ignorar resto da linha
                    c = data[pos++]; //lê primeiro caractere da próxima linha
                }
                //procurar seção
                if (c == '[') {
                    if (sec_flag) break; //termina busca se chegar a próxima seção sem achar a chave
                    do c = data[pos++]; while (c == ' '); //ignora espaços
                    for (i_aux = 0, len = strlen(section); pos < n && i_aux < len;) {//lê o nome da seção
                        if (c != section[i_aux++]) {
                            for (; pos < n && c != 0x0A; c = data[pos++]); //ignorar resto da linha
                            break;
                        }
                        do c = data[pos++]; while (c == ' '); //ignora espaços
                    }
                    //seção encontrada
                    if (c == ']') {
                        sec_flag = 1; //flag de seção encontrada
                        for (; pos < n && c != 0x0A; c = data[pos++]); //ignorar resto da linha
                    }
                }
                //procurar chave
                if (sec_flag) {
                    for (i_aux = 0, len = strlen(key); pos < n && i_aux < len;) {//lê o nome da chave
                        if (c != key[i_aux++]) {
                            for (; pos < n && c != 0x0A; c = data[pos++]); //ignorar resto da linha
                            break;
                        }
                        do c = data[pos++]; while (c == ' '); //ignora espaços
                    }
                    //chave encontrada
                    if (c == '=') {
                        key_flag = 1; //flag de chave encontrada
                    }
                }
                if (key_flag) break;
            }
            pos_value = pos;
            for (; pos < n && c != 0x0A && c != ';';) c = data[pos++]; //ignorar até o final da linha ou até encontrar comentário
            if (pos < n) pos--;
            //gravar arquivo
            arq = fopen(file, "w");
            for (int i = 0; i < pos_value; i++) fprintf(arq, "%c", data[i]); //antes
            fprintf(arq, " %s", value); //chave
            if (data[pos] == ';') fprintf(arq, " "); //chave
            for (int i = pos; i < n; i++) fprintf(arq, "%c", data[i]); //depois
            fclose(arq);
            //liberar memória
            free(data);
            break;

        default:
            return INI_ERROR;
            break;
    }
    return INI_OK;
}

ini_status_t ini_read_section_name(char* file, int index, char *section_name) {
    FILE *arq;
    int i_aux;

    int count = 0;

    arq = fopen(file, "r");
    if (arq == NULL) return INI_FILE_NOT_FOUND;
    //varrer caracteres do arquivo
    for (char c = getc(arq); c != EOF; c = getc(arq)) {
        //ignorar comentários
        while (c == ';') {
            for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
            c = getc(arq); //lê primeiro caractere da próxima linha
        }
        //procurar seção
        if (c == '[') {
            //ignora espaços iniciais
            do c = getc(arq); while (c == ' ');
            //ler nome
            for (i_aux = 0; c != EOF && c != 0x0A && c != ' ' && c != ']'; c = getc(arq)) {
                if (section_name != NULL) {
                    section_name[i_aux++] = c;
                }
            }
            if (section_name != NULL) {
                section_name[i_aux++] = '\0';
            }
            //ignora espaços finais, se houver
            if (c != ']') {
                do c = getc(arq); while (c == ' ');
            }
            //seção encontrada
            if (c == ']') {
                if (count == index) {
                    return INI_OK;
                }
                count++; //contar seções
                for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
            }
        }
    }
    fclose(arq);
    //retorno
    section_name[0] = '\0';
    return INI_ERROR;
}

ini_status_t ini_read_key_name(char* file, char* section, int index, char *key_name) {
    FILE *arq;
    int sec_flag = 0;
    int i_aux, len;
    arq = fopen(file, "r");
    if (arq == NULL) return INI_FILE_NOT_FOUND;
    //zerar contagem
    int count = 0;
    //varrer caracteres do arquivo
    for (char c = getc(arq); c != EOF; c = getc(arq)) {
        //ignorar comentários
        while (c == ';') {
            for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
            c = getc(arq); //lê primeiro caractere da próxima linha
        }
        //procurar seção
        if (c == '[') {
            if (sec_flag) break; //termina busca se chegar a próxima seção sem achar a chave
            do c = getc(arq); while (c == ' '); //ignora espaços
            for (i_aux = 0, len = strlen(section); c != EOF && i_aux < len;) {//lê o nome da seção
                if (c != section[i_aux++]) {
                    for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
                    break;
                }
                do c = getc(arq); while (c == ' '); //ignora espaços
            }
            //seção encontrada
            if (c == ']') {
                sec_flag = 1; //flag de seção encontrada
                for (; c != EOF && c != 0x0A; c = getc(arq)); //ignorar resto da linha
            }
        }
        //procurar chave
        if (sec_flag) {
            //ignora espaços iniciais
            while (c == ' ' || c == 0x0A) c = getc(arq);
            //se for a próxima seção
            if (c == '[') break;
            //zerar ponteiro de nome da chave
            i_aux = 0;
            //ler nome da chave
            for (; c != ' ' && c != '=' && c != 0x0A && c != EOF; c = getc(arq)) {
                if (key_name != NULL) {
                    key_name[i_aux++] = c;
                }
            }
            if (key_name != NULL) {
                key_name[i_aux++] = '\0';
            }
            //incrementar contagem de chave
            if (count == index) {
                return INI_OK;
            }
            count++;
            //ignora resto da linha
            while (c != 0x0A && c != EOF) c = getc(arq);
        }
    }
    fclose(arq);
    key_name[0] = '\0';
    //retorno
    return INI_ERROR;
}