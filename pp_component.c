/*****************************************************************************/
/* pp_component.c                                                            */
/* preprocessor for component files.                                         */
/* This program reads component (.c) files and produces a .h and .c file that*/
/* can be included/compiled to use the component within the engine.          */
/* by using the scene2c program.                                             */
/*                                                                           */
/* Bryce Wilson                                                              */
/* Created: July 9, 2013                                                     */
/*****************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "glib.h"
#include "util.h"

#define ATTRIBUTE_MAX_NAME_SIZE 64
#define ATTRIBUTE_MAX_TYPE_NAME_SIZE 64
#define ATTRIBUTE_MAX_PROTOTYPE_SIZE 2048
/* who makes a MB function after comment removal!? */
#define ATTRIBUTE_MAX_BODY_SIZE 1048576

#define STATE_IS_PUBLIC 0
#define STATE_SEEK_NAME 1
#define STATE_TYPE_DECLARE 2
#define STATE_CHECK_IF_FUNCTION 3
#define STATE_FUNCTION_PROTOTYPE 4 
#define STATE_FUNCTION_BODY 5
#define STATE_FUNCTION_BEGIN 6
#define STATE_DONE 7

#include "cJSON.h"

#define TYPE_INT        1
#define TYPE_FLOAT      2
#define TYPE_STRING     3
#define TYPE_POINTER    4
#define TYPE_BOOL       5

#define TYPE_VECTOR2    6
#define TYPE_VECTOR3    7
#define TYPE_RECT       8
#define TYPE_AABB       9

typedef struct tagAttribute {
    int public;
    int is_function;
    char* type;
    char* name;

    char* prototype;    //if function
    char* definition;    //if function
}Attribute;


FILE* header_fp;
FILE* c_fp;

//void write_json(cJSON* json, char* type, char* name);
void C_To_Scene(char* in_file, char* header_fp, char* c_fp);
void ReadComponent(cJSON* c);

/* given pointer at start of a block, returns pointer to end of { } block. */
char* get_block(char* text);
/* turns all whitespace into a single space and deletes comments */
void simplify(char* text);

int get_type(char* text);
Attribute* get_attribute(char* text);
Attribute* new_attribute(char* name, char* type, char* prototype, char* body);

int main(int argc, char** argv)
{
    char header[256];
    char c[256];
    if(argc != 3) {
        puts("Usage: ./thisprogram input_file");
        exit(1);
    }
    strcpy(c, argv[1]);
    strcat(c, ".c");
    strcpy(header, argv[1]);
    strcat(header, ".h");
    printf("C file: %s\nHeader file: %s\n", c, header);
    C_To_Scene(argv[1], header, c);
    
    return 0;
}

/**************************************
example:
---------- private (*.h) --------------
typedef struct Component_Animation {
    void DoSomething();
}Component_Animation;
Component* Component_New_Animation();

--------- public (*.c) ----------------
typedef struct Component_Animation {
    void DoSomething();
    private members....
}
Component* Component_New_Animation()
{
    ...
}
function prototypes
    ...
functions
    ...
**************************************/

void C_To_Scene(char* in_file, char* out_header, char* out_c)
{
    char* line;
    char* text;
    FILE* in_fp;
    FILE* header_fp;
    FILE* c_fp;
    cJSON* root;
    cJSON* json_attributes;
    cJSON* tmp;
    char* pch;
    char* out;
    GSList* attributes;
    GSList* it;

    char* type;
    char* name;

    int i;


    /* open input file */
    in_fp = fopen(in_file, "r");
    if(in_fp == NULL) {
        fprintf(stderr, "Error: couldn't open file %s for reading\n", in_file);
        exit(-1);
    }
    /* open output files (*.c and *.h) */
    header_fp = fopen(out_header, "w");
    if(header_fp == NULL) {
        fprintf(stderr, "Error: couldn't open file %s for writing\n", header_fp);
        exit(-2);
    }
    c_fp = fopen(out_c, "w");
    if(c_fp == NULL) {
        fprintf(stderr, "Error: couldn't open file %s for writing\n", c_fp);
        exit(-3);
    }

    /*
    root = cJSON_CreateObject();
    json_attributes=cJSON_CreateObject();
    cJSON_AddItemToObject(root, component_name, json_attributes);
    */

    /* read the input file */
    UtilReadFile(in_file, &text);
    /* simplify the input */
    simplify(text);

    /* find Component:: definition */
    pch = strstr(text, "Component ");
    if(pch == NULL) {
        fprintf(stderr, "Error: no Component definition found\n");
        exit(-4);
    }
    
    /* copy everything before the component definition to the C file */
    for(; text != pch; ++text) {
        fputc(*text, c_fp);
    }

    /* get the name of the component */
    pch = strtok(pch, " ");
    name = pch = strtok(NULL, " ");

    /* move pointer to the meat of the component */
    pch += strlen(name) + 1;
    if(*pch == '{') {
        ++pch;
    }
    else if(*pch+1 == '{') {
        pch += 2;
    }
    else {
        fprintf(stderr, "Error: expected '{' after \"Component %s\"\n", name);
        exit(-5);
    }
    text = pch;

    attributes = NULL;
    /* read attributes */
    while(*text) {
        char* pname;
        char* ptype;
        Attribute* a;
        a = get_attribute(text);
        if(a != NULL) {
            /* TODO: CAUSES CRASH....WHY!????? */
            attributes = g_slist_append(attributes, (gpointer)a);
        }
        puts("RLY THO?");
        return;
    }

    /* write header */
    fprintf(header_fp, "#ifndef COMPONENT_%s\n#define COMPONENT_%s", 
            name, name);
    fprintf(header_fp, "typedef struct Component_%s {\n", name);
    fprintf(header_fp, "    Component base;\n", name);
    for(it = attributes; it != NULL; it = g_slist_next(it)) {
        if(((Attribute*)(it->data))->public) {
            fprintf(header_fp, "    %s %s;\n", 
                    ((Attribute*)it->data)->type,
                    ((Attribute*)it->data)->name);
        }
    }
    fprintf(header_fp, "}Component_%s;\n", name);
    fprintf(header_fp, "Component* Component_%s_New();\n", name);
    fprintf(header_fp, "#endif\n");

    /* write C file */
    fprintf(c_fp, "#include \"component.h\"\n");
    fprintf(c_fp, "typedef struct Component_%s {\n", name);
    fprintf(header_fp, "    Component base;\n", name);
    for(it = attributes; it != NULL; it = g_slist_next(it)) {
        fprintf(c_fp, "    %s %s;\n",
                ((Attribute*)it->data)->type,
                ((Attribute*)it->data)->name);
    }
    fprintf(c_fp, "}Component_%s;\n", name);
    for(it = attributes; it != NULL; it = g_slist_next(it)) {
        if(((Attribute*)it->data)->is_function) {
            fprintf(c_fp, "static %s %s %s;\n", 
                    ((Attribute*)it->data)->type,
                    ((Attribute*)it->data)->name,
                    ((Attribute*)it->data)->prototype);
        }
    }
    fprintf(c_fp, "Component* Component_%s_New()\n{\n", name);
    fprintf(c_fp, "    Component_%s* self = "
           "(Component_%s*)malloc(sizeof(Component_%s));\n", name, name, name);
    fprintf(c_fp, "    self->base.start = Start;\n");
    fprintf(c_fp, "    self->base.update = Update;\n");
    fprintf(c_fp, "    self->base.collide = Collide;\n");
    fprintf(c_fp, "    self->base.id = CID_%s;\n", name);
    for(it = attributes; it != NULL; it = g_slist_next(it)) {
        if(((Attribute*)it->data)->is_function) {
            fprintf(c_fp, "    self->%s = %s;\n", 
                    (((Attribute*)it->data))->name,
                    (((Attribute*)it->data))->name);
        }
    }
    fprintf(c_fp, "}\n");
    for(it = attributes; it != NULL; it = g_slist_next(it)) {
        if(((Attribute*)it->data)->is_function) {
            fprintf(c_fp, "%s\n", 
                    (((Attribute*)it->data))->definition);
        }
    }
    /* write JSON skeleton TODO: */

    /* cleanup */
    out = cJSON_Print(root);
    cJSON_Delete(root);
    printf("%s\n", out);
    free(out);
}

#if 0
void write_json(cJSON* json, char* type, char* name)
{
    /* determine what type we're dealing with and create the 
     * appropriate JSON for that type. */
    switch(get_type(type)) {
        case TYPE_INT:
            cJSON_AddNumberToObject(attributes, name, 0);
            break;
        case TYPE_FLOAT:
            cJSON_AddNumberToObject(attributes, name, 0);
            break;
        case TYPE_STRING:
            cJSON_AddStringToObject(attributes, name, "");
            break;
        case TYPE_BOOL:
            cJSON_AddFalseToObject(attributes, name);
            break;
        case TYPE_VECTOR2:
            tmp = cJSON_CreateObject();
            cJSON_AddNumberToObject(tmp, "x", 0);
            cJSON_AddNumberToObject(tmp, "y", 0);
            cJSON_AddItemToObject(attributes, name, tmp);
            break;
        case TYPE_VECTOR3:
            tmp = cJSON_CreateObject();
            cJSON_AddNumberToObject(tmp, "x", 0);
            cJSON_AddNumberToObject(tmp, "y", 0);
            cJSON_AddNumberToObject(tmp, "z", 0);
            cJSON_AddItemToObject(attributes, name, tmp);
            break;
        case TYPE_RECT:
            tmp = cJSON_CreateObject();
            cJSON_AddNumberToObject(tmp, "x", 0);
            cJSON_AddNumberToObject(tmp, "y", 0);
            cJSON_AddNumberToObject(tmp, "w", 0);
            cJSON_AddNumberToObject(tmp, "h", 0);
            cJSON_AddItemToObject(attributes, name, tmp);
            break;
        case TYPE_AABB:
            cJSON_AddNumberToObject(tmp, "w", 0);
            cJSON_AddNumberToObject(tmp, "h", 0);
            cJSON_AddNumberToObject(tmp, "d", 0);
            cJSON_AddItemToObject(attributes, name, tmp);
            break;
        default:
            break;
    }
}
#endif

void simplify(char* text)
{
    char* new = text;
    for(; *text; ++text) {
        /* delete any comments */
        if(*text == '/') {
            /* block comment */
            if(*(text+1) == '*') {
                while((*text != '*') || (*(text+1) != '/')) {
                    ++text;
                }
                /* get past comment */
                ++text;
            }
            /* line comment */
            else if(*(text+1) == '/') {
                while(*text != '\n') {
                    ++text;
                }
                ++text;
            }
        }
        /* turn all whitespace into a single space */
        else if(isspace(*text)) {
            while(isspace(*(++text))) *(text);
            --text;
            *new++ = *text;
        }
        else {
            *new++ = *text;
        }
    }
    *new = '\0';
}

char* get_block(char* text)
{
    char* block = text;
    int brace_cnt = 0;
    for(; *text; ++text) {
        if(*text == '{') {
            ++brace_cnt;
        }
        else if(*text == '}') {
            --brace_cnt;
            if(brace_cnt == 0) {
                /* find next whitespace */
                while(!isspace(*text)) {
                    ++text;
                }
                /* replace whitespace with terminating NULL */
                text = '\0';
                /* point text to 1 character after block */
                ++text;
                return block;
            }
        }
    }
    return NULL;
}

int get_type(char* text)
{
    if(strncmp("int", text, 3) == 0) {
        return TYPE_INT;
    }
    if(strncmp("float", text, 5) == 0) {
        return TYPE_FLOAT;
    }
    if(strncmp("char*", text, 5) == 0) {
        return TYPE_STRING;
    }
    if(strncmp("bool", text, 4) == 0) {
        return TYPE_BOOL;
    }
    if(strncmp("Vector2", text, 6) == 0) {
        return TYPE_VECTOR2;
    }
    if(strncmp("Vector3", text, 6) == 0) {
        return TYPE_VECTOR3;
    }
    if(strncmp("Rect", text, 4) == 0) {
        return TYPE_RECT;
    }
    if(strncmp("AABB", text, 4) == 0) {
        return TYPE_AABB;
    }
    return 0;
}

Attribute* new_attribute(char* name, char* type, char* prototype, char* body)
{
    Attribute* a = (Attribute*)malloc(sizeof(Attribute));
    a->name = (char*)malloc(strlen(name)*sizeof(char));
    strcpy(a->name, name);

    a->type = (char*)malloc(strlen(type)*sizeof(char));
    strcpy(a->type, type);

    if(strlen(prototype) > 0) {
        a->prototype = (char*)malloc(strlen(prototype)*sizeof(char));
        strcpy(a->prototype, prototype);
    }
    if(strlen(body) > 0) {
        a->definition = (char*)malloc(strlen(body)*sizeof(char));
        strcpy(a->definition, body);
    }

    return a;
}

Attribute* get_attribute(char* text)
{
    int i;
    char* name_buff;
    char* type_buff;
    char* proto_buff;
    char* body_buff;
    int done = 0;
    int public = 0;
    int is_function = 0;
    int paren_cnt = 0;

    int state = STATE_IS_PUBLIC;

    name_buff = (char*)malloc(sizeof(char) * ATTRIBUTE_MAX_NAME_SIZE);
    type_buff = (char*)malloc(sizeof(char) * ATTRIBUTE_MAX_TYPE_NAME_SIZE);
    proto_buff = (char*)malloc(sizeof(char) * ATTRIBUTE_MAX_PROTOTYPE_SIZE);
    body_buff = (char*)malloc(sizeof(char) * ATTRIBUTE_MAX_BODY_SIZE);

    proto_buff[0] = '\0';
    body_buff[0] = '\0';

    puts(text);
    /* I hate state machines */
    for(; !done; ++text) {
        switch(state) {
            /* test if public or not */
            case STATE_IS_PUBLIC:
                if(isspace(*text)) {
                    continue;
                }
                if(strncmp(text, "public", 6) == 0) {
                    if(isspace(*(text+6))) {
                        public = 1;
                        text += 7;
                    }
                }
                if(!isspace(*text)) {
                    type_buff[0] = *text;
                    state = STATE_TYPE_DECLARE;
                    i = 1;
                }
                break;

            /* get the type of the attribute declaration */
            case STATE_TYPE_DECLARE:
                if(!isspace(*text)) {
                    type_buff[i] = *text;
                    ++i;
                }
                else {
                    type_buff[i] = '\0';
                    state = STATE_SEEK_NAME;
                    i = 0;
                }
                break;

            /* get the name of the attribute & check if function or not */
            case STATE_SEEK_NAME:
                if(isspace(*text)) {
                    name_buff[i] = '\0';
                    state = STATE_CHECK_IF_FUNCTION;
                    i = 0;
                }
                else if(*text == ';') {
                    name_buff[i] = '\0';
                    state = STATE_DONE;
                    i = 0;
                }
                else if(*text == '(') {
                    name_buff[i] = '\0';
                    paren_cnt = 1;
                    state = STATE_FUNCTION_PROTOTYPE;
                    i = 0;
                }
                else {
                    name_buff[i] = *text;
                    ++i;
                }
                break;

            /* check if the attribute is a function or variable */
            case STATE_CHECK_IF_FUNCTION:
                if(*text == '(') {
                    paren_cnt = 1;
                    state = STATE_FUNCTION_PROTOTYPE;
                }
                if(*text == ';') {
                    state = STATE_DONE;
                }
                else if(!isspace(*text)) {
                    fprintf(stderr, "Error: expected ';' at end of declaration"
                            " of attribute %s\n", name_buff);
                    return NULL;
                }
                break;

            /* get everything in the parentheses for the function prototype */
            case STATE_FUNCTION_PROTOTYPE:
                if(*text == '(') {
                    ++paren_cnt;
                }
                else if(*text == ')') {
                    --paren_cnt;
                    if(paren_cnt == 0) {
                        proto_buff[i] = '\0';
                        state = STATE_FUNCTION_BEGIN;
                        i = 0;
                        break;
                    }
                }
                proto_buff[i] = *text;
                ++i;
                break;

            /* find the start of the function body - the opening brace '{' */
            case STATE_FUNCTION_BEGIN:
                if(*text == '{') {
                    state = STATE_FUNCTION_BODY;
                    paren_cnt = 1;
                }
                break; 

            /* get the body of the function */
            case STATE_FUNCTION_BODY:
                if(*text == '{') {
                    ++paren_cnt;
                }
                else if(*text == '}') {
                    --paren_cnt;
                    if(paren_cnt == 0) {
                        body_buff[i] = '\0';
                        state = STATE_DONE;
                        break;
                    }
                }
                body_buff[i] = *text;
                ++i;
                break;

            /* we're done */
            case STATE_DONE:
                done = 1;
                break;

            /* shouldn't reach */
            default:
                fprintf(stderr, "Uhhhh, something went wrong.\n");
                return NULL;
        }
    }
    puts(type_buff);
    puts(name_buff);
    puts(proto_buff);
    puts(body_buff);
    Attribute* a = new_attribute(name_buff, type_buff, proto_buff, body_buff);
    a->is_function = is_function;
    a->public = public;

    printf("public: %d\nfunction: %d\n", a->public, a->is_function);

    free(name_buff);
    free(type_buff);
    free(proto_buff);
    free(body_buff);
    return a;
}
 
