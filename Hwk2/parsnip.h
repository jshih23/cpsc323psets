// parsnip.h                                    Stan Eisenstat (09/11/18)
//
// Header file for command line parser Parse
//
// Bash version based on arrays of tokens

#ifndef PARSE_INCLUDED
#define PARSE_INCLUDED          // parse.h has been #include-d

// A token is
//
// (1) a maximal, contiguous, nonempty sequence of nonwhitespace characters
//     other than the metacharacters <, >, ;, &, |, (, and ) [a SIMPLE token];
//
// (2) a redirection symbol (<, <<, >, >>, 2>&1, or 2>&-);
//
// (3) a pipeline symbol (|);
//
// (4) a command operator (&& or ||);
//
// (5) a command terminator (; or &);
//
// (6) a left or right parenthesis (used to group commands).


// A token is a (TYPE,TEXT) pair.  The TYPE is specified by the symbolic
// constants defined below.  If TEXT is non-NULL, Storage for TEXT allocated
// by malloc()/realloc().

typedef struct token {          // Struct for each token in array of tokens
    int type;                   //   Corresponding type
    char *text;                 //   String containing token or NULL
} token;


// Token types used by tokenize() and parse()

enum {

   // Token types used by tokenize() and parse()

      SIMPLE,           // Maximal contiguous sequence ... (as above)

      RED_IN,           // <    Redirect stdin to file
      RED_IN_HERE,      // <<   Redirect stdin to HERE document

      RED_OUT,          // >    Redirect stdout to file
      RED_OUT_APP,      // >>   Append stdout to file

      RED_ERR_OUT,      // 2>&1 Redirect stderr to stdout
      RED_ERR_CLS,      // 2>&- Close stderr

      PIPE,             // |

      SEP_AND,          // &&
      SEP_OR,           // ||

      SEP_END,          // ;
      SEP_BG,           // &

      PAR_LEFT,         // (
      PAR_RIGHT,        // )

   // Types used only by parse()

      NONE,             // End of token list
      ERROR,            // Error
      SUBCMD,           // Subcommand
      LOCAL,            // Local variable
      ARG,              // Argument
};


// String containing all metacharacters that terminate SIMPLE tokens
#define METACHAR "<>;&|()"


// String containing all characters that may appear in variable names
#define VARCHR "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789"


// Macro that checks whether a token is a redirection symbol
#define RED_OP(type) \
    (type == RED_IN      || type == RED_IN_HERE || \
     type == RED_OUT     || type == RED_OUT_APP || \
     type == RED_ERR_OUT || type == RED_ERR_CLS)


// Break string LINE into a NONE-terminated array of typed tokens (or NULL
// if none were found or an error was detected).
token *tokenize (char *line);


// Free NONE-terminated array of tokens TOK and return NULL.
token *freeList (token *tok);


// Print NONE-terminated array LIST of parsed tokens.
void dumpList (token *list);


/////////////////////////////////////////////////////////////////////////////

// The syntax for a bash command is
//
//   <argument> = SIMPLE
//   <filename> = SIMPLE
//   <local>    = VARIABLE=VALUE
//   <red_op>   = < / << / > / >>
//   <red_sym>  = 2>&1 / 2>&-
//   <redirect> = <red_op> <filename> / <red_sym>
//   <prefix>   = [ <local> / <redirect> ]*
//   <suffix>   = [ <argument> / <redirect> ]*
//   <red_list> = [ <redirect> ]*
//   <simple>   = <prefix> <argument> <suffix>
//   <subcmd>   = <prefix> (<command>) <red_list>
//   <stage>    = <simple> / <subcmd>
//   <pipeline> = <stage> [ | <stage> ]*
//   <and_or>   = <pipeline> [ && <pipeline> / || <pipeline> ]*
//   <list>     = <and_or> [; <and_or> / & <and_or> ]*
//   <command>  = <list> / <list> ; / <list> &
//
// where / denotes alternatives; [...]* denotes a series of zero or more
// occurrences; and VARIABLE=VALUE is a SIMPLE of that form.  For example,
// <pipeline> and <pipeline && <pipeline> and <pipeline || <pipeline> and
// <pipeline> && <pipeline> && <pipeline and ... are all instances of an
// <and_or>.


// Each sequence of tokens is stored in an array of token structs.  Each token
// struct is a (TYPE,TEXT) pair, where the value of TYPE is specified by one of
// the symbolic constants defined above; and TEXT is either NULL or a pointer
// to an associated string whose storage is allocated by malloc() or realloc().
// The end of the array is signaled by a token struct whose type field is NONE
// and whose text field is NULL.
//
// tokenize() takes a command line and returns a pointer to such a
// NONE-terminated array of token structs.
//
// parse() reads this array; reports any syntax errors; and, if none are
// found, returns a pointer to a new NONE-terminated array of token structs
// that is derived as follows:
//
// 1. For each SIMPLE token that represents a <local> (i.e., one of the form
//    VARIABLE=VALUE), there is a token of type LOCAL whose text field is a
//    pointer to a copy of the string associated with the SIMPLE.
//
// 2. For each SIMPLE token that represents an <argument> there is a token of
//    type ARG whose text field is a pointer to a copy of the string associated
//    with the SIMPLE.
//
// 3. For each pair of tokens that consist of a <red_op> (i.e., a <, <<, >, or
//    >>)and a FILENAME (i.e., a SIMPLE) there is a token whose type is that of
//    the redirection symbol and whose text field is a pointer to a copy of the
//    the string associated with the SIMPLE.  If the redirection symbol is <<,
//    then the text field is instead a pointer to a string containing the
//    expanded HERE document.
//
// 4. For each token that contains a <red_sym> (i.e., a 2>&1 or 2>&-) there is
//    a token with the same type and a NULL text field.
//
// 5. For every other token there is a token of the same type with a NULL text
//    field.
//
// 6. These tokens appear in the array in the same order as the tokens from
//    which they were derived, except that within a subsequence of tokens
//    corresponding to a <stage>, all ARG tokens appear first, followed by all
//    LOCAL tokens, followed by all tokens containing a redirection symbol,
//    followed by all tokens associated with a subcommand (including the
//    surrounding PAR_LEFT and PAR_RIGHT tokens).  Since a <stage> may not
//    include both a <simple> and a <subcmd>, there will never be both ARG
//    tokens and tokens associated with a subcommand.
//
// 7. A token with type ; is inserted at the end of any subsequence of tokens
//    that constitutes a <list> but is not followed by a token of type ; or &.
//
// All storage for the derived array (including any associated strings) must be
// separate from that for the array input.
//
// tokenize() and parse() return NULL if errors are detected.

// Examples (where A, B, C, ... are SIMPLE tokens):
//
//   Command:     < A B=C D E > F
//   Token Array: ARG:D ARG:E LOCAL:B=C <A >F
//
//   Command:     A=B C D > E < F
//   Token Array: ARG:C ARG:D LOCAL:A=B >E <F
//
//   Command:     < A B | C | D | E > F
//   Token Array: ARG:B <A | ARG:C | ARG:D | ARG:E >F
//
//   Command:     A && B || C && D
//   Token Array: ARG:A && ARG:B || ARG:C && ARG:D
//
//   Command:     A ; B & C ; D || E ;
//   Token Array: ARG:A ; ARG:B & ARG:C ; ARG:D || ARG:E ;
//
//   Command:     (A ; B &) | (C || D) && E
//   Token Array: ( ARG:A ; ARG:B & ) | ( C || D ; ) && E ;


// Parse NONE-terminated array of tokens TOK into NONE-terminated parsed token
// array and return a pointer to that array (NULL if errors found).

token *parse (token *tok);

#endif
