/*
 * File:   read_write_ini.c
 * Author: Alan
 *
 * Created on August 14, 2019, 1:09 PM
 */

#include "read_write_ini.h"

#define DEFAULT_STR_LENGTH 256

#define CHAR_COMMENT    ';'
#define CHAR_SEC_OPEN   '['
#define CHAR_SEC_CLOSE  ']'
#define CHAR_SET_VALUE  '='
#define CHAR_END_LINE   '\n'
#define CHAR_RETURN     '\r'

typedef enum {
    ST_NOT_FOUND = 0,
    ST_FINISH = 1,
    ST_SEARCH,
    ST_READING,
} state_t;

////////////////////////////////////////////////////////////////////////////////

long readFileToString(char *file, char **str) {
    long size;
    //open file
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        (*str) = NULL;
        return 0;
    }
    //read size and allocate memory
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp) + 1L;
    (*str) = malloc(size);
    //read file
    fseek(fp, 0L, SEEK_SET);
    fread((*str), size, 1, fp);
    (*str)[size - 1] = EOF;
    //close file
    fclose(fp);
    return size;
}

char getNextValidChar(char *str, long *index) {
    int newline;
    char c;
    //detect new line
    if ((*index) == 0 || str[(*index) - 1] == CHAR_END_LINE || str[(*index) - 1] == CHAR_RETURN) {
        newline = 1;
    } else {
        newline = 0;
    }
    //read next char
    c = str[(*index)++];
    //look for comment mark if it is the beginning of a line
    if (newline) {
        while (c == CHAR_COMMENT) {
            //ignore rest of line
            do {
                c = str[(*index)++];
            } while (c != EOF && c != CHAR_END_LINE && c != CHAR_RETURN);
        }
    }
    //return value
    return c;
}

int matchStr(char *str, long start, long end, char *compStr) {
    int ret;
    long compStart = 0;
    long compEnd = strlen(compStr);
    //ignore white spaces
    while (str[start] == ' ') start++;
    while (str[end - 1] == ' ') end--;
    while (compStr[compStart] == ' ') compStart++;
    while (compStr[compEnd - 1] == ' ') compEnd--;
    //compare chars
    if ((end - start) != (compEnd - compStart)) {
        ret = 0;
    } else {
        ret = 1;
        for (long i = start, j = compStart; i < end; i++, j++) {
            if (str[i] != compStr[j]) {
                ret = 0;
                break;
            }
        }
    }
    return ret;
}

void copyStr(char *str, long start, long end, char *dstStr) {
    //ignore white spaces
    while (str[start] == ' ') start++;
    while (str[end - 1] == ' ') end--;
    //copy string
    for (long i = 0, j = start; j < end; j++) {
        dstStr[i++] = str[j];
        dstStr[i] = '\0';
    }
}

void writeCharToFile(FILE** fp, char c) {
    fwrite(&c, 1, 1, (*fp));
}

void writeStringToFile(FILE** fp, char *str, long start, long end) {
    for (long i = start; i < end; i++) {
        writeCharToFile(fp, str[i]);
    }
}

int findNextSection(char *str, long index, long *start, long *end) {
    state_t state = ST_SEARCH;
    char c;
    (*start) = (*end) = index;
    for (; state != ST_FINISH && state != ST_NOT_FOUND;) {
        c = getNextValidChar(str, &index);
        switch (state) {
            case ST_FINISH:
            case ST_NOT_FOUND:
            case ST_SEARCH:
                if (c == CHAR_SEC_OPEN) {
                    state = ST_READING;
                    (*start) = index;
                }
                break;
            case ST_READING:
                if (c != CHAR_SEC_CLOSE) {
                } else {
                    state = ST_FINISH;
                    (*end) = index - 1L;
                }
                break;
        }
        if (c == EOF) {
            state = ST_NOT_FOUND;
        }
    }
    return state;
}

int findNextKey(char *str, long index, long *start, long *end) {
    state_t state = ST_SEARCH;
    char c;
    (*start) = (*end) = index;
    for (; state != ST_FINISH && state != ST_NOT_FOUND;) {
        c = getNextValidChar(str, &index);
        switch (state) {
            case ST_FINISH:
            case ST_NOT_FOUND:
            case ST_SEARCH:
                state = ST_READING;
                if (c == CHAR_SET_VALUE) {
                    break;
                }
            case ST_READING:
                if (c == CHAR_END_LINE || c == CHAR_RETURN) {
                    (*start) = index;
                } else if (c == CHAR_SET_VALUE || c == CHAR_SEC_OPEN) {
                    state = ST_FINISH;
                    (*end) = index - 1L;
                }
                break;
        }
        if (c == EOF || c == CHAR_SEC_OPEN) {
            state = ST_NOT_FOUND;
        }
    }
    return state;
}

int findNextValue(char *str, long index, long *start, long *end) {
    state_t state = ST_SEARCH;
    char c;
    (*start) = (*end) = index;
    for (; state != ST_FINISH && state != ST_NOT_FOUND;) {
        c = getNextValidChar(str, &index);
        switch (state) {
            case ST_FINISH:
            case ST_NOT_FOUND:
            case ST_SEARCH:
                state = ST_READING;
                (*start) = index;
            case ST_READING:
                if (c == EOF || c == CHAR_END_LINE || c == CHAR_RETURN) {
                    state = ST_FINISH;
                    (*end) = index - 1L;
                }
                break;
        }
        if (c == EOF) {
            state = ST_FINISH;
        }
    }
    return state;
}

////////////////////////////////////////////////////////////////////////////////

int ini_exist(char* file) {
    int ret;
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        ret = 0;
    } else {
        fclose(fp);
        ret = 1;
    }
    return ret;
}

int ini_count_sections(char* file) {
    int count = 0;
    char *str;
    long n, start, end = 0;
    //read file
    n = readFileToString(file, &str);
    if (n > 0) {
        //count
        while (findNextSection(str, end, &start, &end)) {
            count++;
        }
        free(str);
    }
    return count;
}

int ini_count_keys(char* file, char* section) {
    int count = 0;
    char *str;
    long n, start, end = 0;
    //read file
    n = readFileToString(file, &str);
    if (n > 0) {
        //search section
        while (findNextSection(str, end, &start, &end)) {
            if (matchStr(str, start, end, section)) {
                //count
                while (findNextKey(str, end, &start, &end)) {
                    count++;
                }
                break;
            }
        }
        free(str);
    }
    return count;
}

ini_status_t ini_read_section_name(char* file, int index, char *section_name) {
    ini_status_t ret = INI_SEC_NOT_FOUND;
    int count = 0;
    char *str;
    long n, start, end = 0;
    //read file
    n = readFileToString(file, &str);
    if (n > 0) {
        //search section
        while (findNextSection(str, end, &start, &end)) {
            if (index == count) {
                copyStr(str, start, end, section_name);
                ret = INI_OK;
                break;
            } else {
                count++;
            }
        }
        free(str);
    }
    return ret;
}

ini_status_t ini_read_key_name(char* file, char* section, int index, char *key_name) {
    ini_status_t ret;
    int count = 0;
    char *str;
    long n, start, end = 0;
    //read file
    n = readFileToString(file, &str);
    if (n > 0) {
        //search section
        ret = INI_SEC_NOT_FOUND;
        while (findNextSection(str, end, &start, &end)) {
            if (matchStr(str, start, end, section)) {
                //search key
                ret = INI_KEY_NOT_FOUND;
                while (findNextKey(str, end, &start, &end)) {
                    if (index == count) {
                        copyStr(str, start, end, key_name);
                        ret = INI_OK;
                        break;
                    } else {
                        count++;
                    }
                }
                break;
            }
        }
        free(str);
    }
    return ret;
}

ini_status_t ini_read_value(char* file, char* section, char* key, char* value) {
    ini_status_t ret;
    char *str;
    long n, start, end = 0;
    //read file
    n = readFileToString(file, &str);
    if (n > 0) {
        //search section
        ret = INI_SEC_NOT_FOUND;
        while (findNextSection(str, end, &start, &end)) {
            if (matchStr(str, start, end, section)) {
                //search key
                ret = INI_KEY_NOT_FOUND;
                while (findNextKey(str, end, &start, &end)) {
                    if (matchStr(str, start, end, key)) {
                        //read value
                        ret = INI_ERROR;
                        if (findNextValue(str, end, &start, &end)) {
                            ret = INI_OK;
                            if (value != NULL) {
                                copyStr(str, start, end, value);
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
        free(str);
    } else {
        ret = INI_FILE_NOT_FOUND;
    }
    return ret;
}

ini_status_t ini_write_value(char* file, char* section, char* key, char* value) {
    ini_status_t ret;
    FILE *fp;
    char *str;
    long n, start, end;
    //evaluate
    ret = ini_read_value(file, section, key, NULL);
    n = readFileToString(file, &str);
    //find insertion point
    switch (ret) {
        case INI_FILE_NOT_FOUND:
            start = end = 0;
            break;
        case INI_SEC_NOT_FOUND:
            //end of file
            start = end = n - 1;
            break;
        case INI_KEY_NOT_FOUND:
            //find section
            end = 0;
            do {
                findNextSection(str, end, &start, &end);
            } while (!matchStr(str, start, end, section));
            //find last key
            long last;
            while (findNextKey(str, end, &start, &end)) {
                last = end;
            }
            findNextValue(str, last, &start, &end);
            start = end;
            break;
        case INI_OK:
            //find section
            end = 0;
            do {
                findNextSection(str, end, &start, &end);
            } while (!matchStr(str, start, end, section));
            //find key
            do {
                findNextKey(str, end, &start, &end);
            } while (!matchStr(str, start, end, key));
            //find value
            findNextValue(str, end, &start, &end);
            break;
    }
    //write file
    fp = fopen(file, "wb");
    if (fp == NULL) {
        ret = INI_ERROR;
    } else {
        //previous content
        writeStringToFile(&fp, str, 0, start);
        //new content
        switch (ret) {
            case INI_FILE_NOT_FOUND:
            case INI_SEC_NOT_FOUND:
                if (n > 1) {
                    writeCharToFile(&fp, CHAR_END_LINE);
                    writeCharToFile(&fp, CHAR_END_LINE);
                }
                writeCharToFile(&fp, CHAR_SEC_OPEN);
                writeStringToFile(&fp, section, 0, strlen(section));
                writeCharToFile(&fp, CHAR_SEC_CLOSE);
                writeCharToFile(&fp, CHAR_END_LINE);
                writeStringToFile(&fp, key, 0, strlen(key));
                writeCharToFile(&fp, ' ');
                writeCharToFile(&fp, CHAR_SET_VALUE);
                break;
            case INI_KEY_NOT_FOUND:
                writeCharToFile(&fp, CHAR_END_LINE);
                writeStringToFile(&fp, key, 0, strlen(key));
                writeCharToFile(&fp, ' ');
                writeCharToFile(&fp, CHAR_SET_VALUE);
                break;
            case INI_OK:
                break;
        }
        writeCharToFile(&fp, ' ');
        writeStringToFile(&fp, value, 0, strlen(value));
        //following content
        writeStringToFile(&fp, str, end, n - 1);
        //close file
        fclose(fp);
        ret = INI_OK;
    }
    //free memory
    if (str != NULL) {
        free(str);
    }
    return ret;
}