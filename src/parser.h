#ifndef parser
#define parser

/*
 * ParseError definitions
 */

typedef struct ParseError ParseError;
struct ParseError {
    char *text;
    int line_no;
    int char_no;
};

FORWARD_DECLARE_VECTORABLE(ParseError)

ParseError *ParseError_init(char *text, int line_no, int char_no);
void ParseError_print(ParseError *err);
bool ParseError_eq(ParseError *err1, ParseError *err2);
void ParseError_free(ParseError *err);
void ParseErrorVector_print(ParseErrorVector *errs);

/*
 * ParseResult definitions
 */

typedef enum { parseSuccess, parseFail } ParseResultType;

typedef struct ParseResult ParseResult;
struct ParseResult {
    ParseResultType type;
    Prog *prog;
    ParseErrorVector *errors;
};

ParseResult *ParseSuccess_init(Prog *prog);
ParseResult *ParseFail_init(ParseErrorVector *errors);
void ParseResult_free(ParseResult *res);

/*
 * The parser function
 */

ParseResult *parse(LexerResult *lexer_result);

#endif // parser
