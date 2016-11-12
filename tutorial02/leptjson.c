#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <string.h>
#include <math.h>
#ifndef TRUE
#define TRUE 1
#endif // !TRUE
#ifndef FALSE
#define FALSE 0
#endif // !FALSE

#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)


typedef struct {
	const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
	const char *p = c->json;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		p++;
	c->json = p;
}

//static int lept_parse_true(lept_context* c, lept_value* v) {
//	EXPECT(c, 't');
//	if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
//		return LEPT_PARSE_INVALID_VALUE;
//	c->json += 3;
//	v->type = LEPT_TRUE;
//	return LEPT_PARSE_OK;
//}
//
//static int lept_parse_false(lept_context* c, lept_value* v) {
//	EXPECT(c, 'f');
//	if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
//		return LEPT_PARSE_INVALID_VALUE;
//	c->json += 4;
//	v->type = LEPT_FALSE;
//	return LEPT_PARSE_OK;
//}
//
//static int lept_parse_null(lept_context* c, lept_value* v) {
//	EXPECT(c, 'n');
//	if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
//		return LEPT_PARSE_INVALID_VALUE;
//	c->json += 3;
//	v->type = LEPT_NULL;
//	return LEPT_PARSE_OK;
//}

static int lept_parse_literal(lept_context* c, lept_value* v, \
	const char* json_value, \
	const int lept_parse_value_type) {
	EXPECT(c, json_value[0]);
	int len = strlen(json_value)-1;  //the len of 'ull','rue','alse'
	for (int i = 0; i < len; ++i) {
		if (c->json[i] != json_value[i+1])
			return LEPT_PARSE_INVALID_VALUE;
	}
	c->json += len;
	v->type = lept_parse_value_type;
	return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v) {
	char* end;
	
	if (c->json[0] == '+' || c->json[0] == '.' \
		||(c->json[0]!='-'&&!ISDIGIT(c->json[0]))) {
		return LEPT_PARSE_INVALID_VALUE;
	}
	if ((c->json[0] == '0'&&ISDIGIT1TO9(c->json[1])) \
		|| (c->json[0] == '0' && c->json[1] != '.'&&c->json[1] != '\0') \
		|| (c->json[0] == '0'&&c->json[1] == '.' && ISDIGIT(c->json[2]))) {
		return LEPT_PARSE_ROOT_NOT_SINGULAR;
	}
	int index = 0;
	for (index = 0; ISDIGIT(c->json[index]); ++index) {};
	if (c->json[index] == '.' && c->json[index + 1] == '\0') {
		return LEPT_PARSE_INVALID_VALUE;
	}

	v->n = strtod(c->json, &end);
	/*if (c->json == end)
		return LEPT_PARSE_INVALID_VALUE;*/	
	if (!isfinite(v->n)) {
		return LEPT_PARSE_NUMBER_TOO_BIG;
	}
	c->json = end;
	v->type = LEPT_NUMBER;
	return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
	switch (*c->json) {
	case 't':  return lept_parse_literal(c, v, "true", LEPT_TRUE);
	case 'f':  return lept_parse_literal(c, v, "false", LEPT_FALSE);
	case 'n':  return lept_parse_literal(c, v, "null", LEPT_NULL);
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




#ifdef TRUE
#undef TRUE
#endif // TEUE
#ifdef FALSE
#undef FALSE
#endif // FALSE
