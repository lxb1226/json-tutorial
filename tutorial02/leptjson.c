#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_literal(lept_context* c, lept_value* v, char* type, int len){
    int i;
    for(i = 0; i < len; ++i){
        if(c->json[i] != type[i]){
            return LEPT_PARSE_INVALID_VALUE;
        }
    }
    
    if(c->json[0] == 'n'){
        v->type = LEPT_NULL;
    }else if(c->json[0] == 't'){
        v->type = LEPT_TRUE;
    }else if(c->json[0] == 'f'){
        v->type = LEPT_FALSE;
    }
    c->json += len;
    return LEPT_PARSE_OK;
}

/* 检验字符串是不是合法的数字 */
static int validate_number(char* json){
    /* 判断第一个是不是 符号位 '-'*/
    int flag = 0;
    char* s = json;
    if(!ISDIGIT(s[0])){
        if(s[0] == '-'){
            flag = 1;
        }else{
            return 0;
        }
    }

    

    
    return 1;
}

static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
    /* \TODO validate number */
    v->n = strtod(c->json, &end);
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_literal(c, v, "true", 4);
        case 'f':  return lept_parse_literal(c, v, "false", 5);
        case 'n':  return lept_parse_literal(c, v, "null", 4);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
