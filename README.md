# PLProject3OnNetbeans
  * a compiler with simple C
  * 每次進 {} 就要 push 一個 scope 到 stack
  * 出 {} pop

```
/*
 *    Our C - the grammar (May 5th, 2016)
 *
 *    缺陷說明： 'a++b'會被視為error，因為'++'會被視為'PP'
 *               要'a+ +b'才不會被視為error。
 *               (原則上，測試數據不會測這個。)
 */

// the lexical part (僅供參考)

%token  Identifier
%token  Constant   // e.g., 35, 35.67, 'a', "Hi, there", true, false
                   //       .35, 35., 0023
%token  INT     // int
%token  FLOAT   // float
%token  CHAR    // char
%token  BOOL    // bool
%token  STRING  // string <----------- 注意全小寫！
%token  VOID    // void
%token  IF      // if
%token  ELSE    // else
%token  WHILE   // while
%token  DO      // do
%token  RETURN  // return
%token  '('
%token  ')'
%token  '['
%token  ']'
%token  '{'
%token  '}'
%token  '+'
%token  '-'
%token  '*'
%token  '/'
%token  '%'
%token  '^'
%token  '>'
%token  '<'
%token  GE      // >=
%token  LE      // <=
%token  EQ      // ==
%token  NEQ     // !=
%token  '&'
%token  '|'
%token  '='
%token  '!'
%token  AND     // &&
%token  OR      // ||
%token  PE      // +=
%token  ME      // -=
%token  TE      // *=
%token  DE      // /=
%token  RE      // %=
%token  EE      // ^=
%token  PP      // ++
%token  MM      // --
%token  RS      // >>
%token  LS      // <<
%token  ';'
%token  ','
%token  '?'
%token  ':'

/*
 * precedence (lower ones are given higher precedence)
 * and associativity
 */

%left   ','
%right  '=' PE ME TE DE RE
%right  '?'+':'
%left   OR
%left   AND
%left   '|'
%left   '^'
%left   '&'
%left   EQ NEQ
%left   '<' '>' GE LE
%left   '+' '-' '!'
%left   '*' '/' '%'
%right  PP MM

%%  // the syntactical part in EBNF

user_input
    : ( definition | statement ) { definition | statement }

definition
    :           VOID Identifier function_definition_without_ID
    | type_specifier Identifier function_definition_or_declarators

type_specifier
    : INT | CHAR | FLOAT | STRING | BOOL

function_definition_or_declarators
    : function_definition_without_ID
    | rest_of_declarators

rest_of_declarators
    : [ '[' Constant ']' ]
      { ',' Identifier [ '[' Constant ']' ] } ';'

function_definition_without_ID
    : '(' [ VOID | formal_parameter_list ] ')' compound_statement

formal_parameter_list
    : type_specifier [ '&' ] Identifier [ '[' Constant ']' ]
      { ',' type_specifier [ '&' ] Identifier [ '[' Constant ']' ] }

compound_statement
    : // '{' { declaration } { statement } '}'  // was
      '{' { declaration | statement } '}'       // now

declaration
    : type_specifier Identifier rest_of_declarators

statement
    : ';'     /* the null statement */
    | expression ';'  /* expression here should not be empty */
    | RETURN [ expression ] ';'
    | compound_statement
    | IF '(' expression ')' statement [ ELSE statement ]
    | WHILE '(' expression ')' statement
    | DO statement WHILE '(' expression ')' ';'

expression
    : basic_expression [ rest_of_expression ] { ',' basic_expression }

rest_of_expression
    : ',' basic_expression [ rest_of_expression ]
    | '?' expression ':' basic_expression [ rest_of_expression ]

basic_expression
    : Identifier rest_of_Identifier_started_basic_exp
    | ( PP | MM ) Identifier rest_of_PPMM_Identifier_started_basic_exp
    | sign { sign } signed_basic_expression signed_unary_exp romce_and_romloe
    | ( Constant | '(' expression ')' ) romce_and_romloe

rest_of_Identifier_started_basic_exp
    : [ '[' expression ']' ]
      ( assignment_operator basic_expression
        |
        [ PP | MM ] romce_and_rmloe
      )
    | '(' [ actual_parameter_list ] ')' romce_and_rmloe

rest_of_PPMM_Identifier_started_basic_exp
    : [ '[' expression ']' ]
      rest_of_maybe_logical_OR_exp

signed_basic_expression
    : Identifier rest_of_Identifier_started_signed_basic_exp
    | ( Constant | '(' expression ')' ) rest_of_maybe_logical_OR_exp

rest_of_Identifier_started_signed_basic_exp
    : [ '[' expression ']' ]
      [ PP | MM ]
      rest_of_maybe_logical_OR_exp
    | '(' [ actual_parameter_list ] ')' rest_of_maybe_logical_OR_exp

sign
    : '+' | '-' | '!'

actual_parameter_list
    : non_comma_expression { ',' non_comma_expression }
      basic_expression { ',' basic_expression }

non_comma_expression
    : basic_expression [ rest_of_non_comma_expression ]

rest_of_non_comma_expression
    : '?' expression ':' basic_expression [ rest_of_non_comma_expression ]

assignment_operator
    : '=' | TE | DE | RE | PE | ME

rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp // 即romce_and_romloe
    : rest_of_maybe_logical_OR_exp [ '?' basic_expression ':' basic_expression ]

rest_of_maybe_logical_OR_exp
    : rest_of_maybe_logical_AND_exp { OR maybe_logical_AND_exp }

maybe_logical_AND_exp
    : maybe_bit_OR_exp { AND maybe_bit_OR_exp }
rest_of_maybe_logical_AND_exp
    : rest_of_maybe_bit_OR_exp { AND maybe_bit_OR_exp }

maybe_bit_OR_exp
    : maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }
rest_of_maybe_bit_OR_exp
    : rest_of_maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }

maybe_bit_ex_OR_exp
    : maybe_bit_AND_exp { '^' maybe_bit_AND_exp }
rest_of_maybe_bit_ex_OR_exp
    : rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }

maybe_bit_AND_exp
    : maybe_equality_exp { '&' maybe_equality_exp }
rest_of_maybe_bit_AND_exp
    : rest_of_maybe_equality_exp { '&' maybe_equality_exp }

maybe_equality_exp
    : maybe_relational_exp
      { ( EQ | NEQ ) maybe_relational_exp}
rest_of_maybe_equality_exp
    : rest_of_maybe_relational_exp
      { ( EQ | NEQ ) maybe_relational_exp }

maybe_relational_exp
    : maybe_shift_exp
      { ( '<' | '>' | LE | GE ) maybe_shift_exp }
rest_of_maybe_relational_exp
    : rest_of_maybe_shift_exp
      { ( '<' | '>' | LE | GE ) maybe_shift_exp }

maybe_shift_exp
    : maybe_additive_exp { ( LS | RS ) maybe_additive_exp }
rest_of_maybe_shift_exp
    : rest_of_maybe_additive_exp { ( LS | RS ) maybe_additive_exp }

maybe_additive_exp
    : maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }
rest_of_maybe_additive_exp
    : rest_of_maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }

maybe_mult_exp
    : unary_exp rest_of_maybe_mult_exp
rest_of_maybe_mult_exp
    : { ( '*' | '/' | '%' ) unary_exp }  /* could be empty ! */

unary_exp
    : sign { sign } signed_unary_exp
    | unsigned_unary_exp
    | ( PP | MM ) Identifier [ '[' expression ']' ]

signed_unary_exp
    : Identifier [ '(' [ actual_parameter_list ] ')'
                   |
                   '[' expression ']' [ ( PP | MM ) ]
                   |
                   ( PP | MM )
                 ]
    | Constant
    | '(' expression ')'

unsigned_unary_exp
    : Identifier [ '(' [ actual_parameter_list ] ')'
                   |
                   [ '[' expression ']' ] [ ( PP | MM ) ]
                 ]
    | Constant
    | '(' expression ')'
```
