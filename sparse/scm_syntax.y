%{
#include "sanya_api.h"
#include "parse_api.h"
#include "scm_token.flex.h"

extern int yylex(void);
extern void yyerror(const char *);
void sparse_clean_syntax_error();
static bool got_error = 0;
HandleZone *parser_zone = NULL;

static Handle prog_expr = Handle(NULL);

%}

%token T_LPAREN T_RPAREN T_PERIOD T_SHARPLPAREN T_LBRACKET T_RBRACKET
%token T_QUOTE T_QUASIQUOTE T_UNQUOTE T_SPLICING
%token <obj_val> T_EXPR
%type <obj_val> sexpr pair list prog vector

%union {
    Handle *obj_val;
};

%%

prog: sexpr { $$ = alloc_handle(make_pair($1->raw(), make_nil()));
              prog_expr = $$->raw(); }
    | sexpr prog { Handle sexpr = *($1);
                   Handle prog = *($2);
                   Handle retval = make_pair(sexpr.raw(), prog.raw());
                   $$ = alloc_handle(retval.raw());
                   prog_expr = $$->raw(); }

sexpr: T_EXPR { $$ = $1; }
     | list { $$ = $1; }
     | vector { $$ = $1; }
     | T_QUOTE sexpr { $$ = alloc_handle(make_quoted($2->raw())); }
     | T_QUASIQUOTE sexpr { $$ = alloc_handle(make_quasiquoted($2->raw())); }
     | T_UNQUOTE sexpr { $$ = alloc_handle(make_unquoted($2->raw())); }
     | T_SPLICING sexpr { $$ = alloc_handle(make_splicing($2->raw())); }

vector: T_SHARPLPAREN pair T_RPAREN { $$ = alloc_handle(make_vector($2->raw())); }

list: T_LPAREN pair T_RPAREN { $$ = $2; }
    | T_LBRACKET pair T_RBRACKET { $$ = $2; }

pair: sexpr T_PERIOD sexpr { $$ = alloc_handle(make_pair($1->raw(),
                                               $3->raw())); }
    | sexpr pair { $$ = alloc_handle(make_pair($1->raw(), $2->raw())); }
    | { $$ = alloc_handle(make_nil()); }

%%

void
sparse_init()
{
    static bool initialized = false;
    if (initialized)
        return;
    else
        initialized = true;
}

RawObject *
sparse_do_string(const char *s)
{
    RawObject *retval;
    YY_BUFFER_STATE buf;
    if (parser_zone)
        delete parser_zone;
    parser_zone = new HandleZone();

    sparse_clean_syntax_error();
    buf = yy_scan_string(s);
    yy_switch_to_buffer(buf);
    yyparse();
    yy_delete_buffer(buf);

    retval = prog_expr.raw();
    prog_expr = NULL;
    return retval;
}

RawObject *
sparse_do_file(FILE *fp)
{
    RawObject *retval;
    YY_BUFFER_STATE buf;
    if (parser_zone)
        delete parser_zone;
    parser_zone = new HandleZone();

    sparse_clean_syntax_error();
    buf = yy_create_buffer(fp, YY_BUF_SIZE);
    yy_switch_to_buffer(buf);
    yyparse();
    yy_delete_buffer(buf);

    retval = prog_expr.raw();
    prog_expr = NULL;
    return retval;
}

void
sparse_raise_syntax_error(const char *why)
{
    got_error = 1;
}

void
sparse_clean_syntax_error()
{
    got_error = 0;
}

