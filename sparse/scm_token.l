%{
#include "sanya_api.h"
#include "scm_syntax.h"
%}

%%

\( {
    return T_LPAREN;
}

\) {
    return T_RPAREN;
}

"[" {
    return T_LBRACKET;
}

"]" {
    return T_RBRACKET;
}

\. {
    return T_PERIOD;
}

"'" {
    return T_QUOTE;
}

"`" {
    return T_QUASIQUOTE;
}

",@" {
    return T_SPLICING;
}

"," {
    return T_UNQUOTE;
}

"#t" {
    yylval.obj_val = alloc_handle(make_true());
    return T_EXPR;
}

"#f" {
    yylval.obj_val = alloc_handle(make_false());
    return T_EXPR;
}

"#(" {
    return T_SHARPLPAREN;
}


-?[0-9]+ {
    yylval.obj_val = alloc_handle(make_fixnum(yytext));
    return T_EXPR;
}

-?[0-9]*\.[0-9]+ {
    yylval.obj_val = alloc_handle(make_flonum(yytext));
    return T_EXPR;
}

\"(\\\"|[^"\n\r])*\" {
    // Escaping seq is resolved in make_string
    yylval.obj_val = alloc_handle(make_string(yytext + 1, yyleng - 2));
    return T_EXPR;
}

;[^\n]* ;  // Ignore comments

[\.\+\-\*\^\?a-zA-Z!<=>\_~/$%&:][\.\+\-\*\^\?a-zA-Z0-9!<=>\_~/$%&:]* {
    yylval.obj_val = alloc_handle(make_symbol(yytext));
    return T_EXPR;
}

[ \n\t]+ ;  // Ignore whitespaces

%%

int
yywrap(void)
{
    return 1;
}

extern void sparse_raise_syntax_error(const char *why);

void
yyerror(const char *s)
{
    // Well, got a parse error...
    sparse_raise_syntax_error(s);
}

