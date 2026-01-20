/* lexer.c - (Simplified) Lexer implementation */
#include "lexer.h"
#include <string.h>
#include <stdio.h>

// --- Globals (state of the lexer) ---
static const char* start;
static const char* current;
static int line;

// --- Helper Functions ---
static bool isAtEnd() {
    return *current == '\0';
}

static char advance() {
    current++;
    return current[-1];
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = start;
    token.length = (int)(current - start);
    token.line = line;
    return token;
}

static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = line;
    return token;
}

static void skipWhitespace() {
    for (;;) {
        char c = *current;
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                advance();
                break;
            case '/':
                if (current[1] == '/') { // Comments
                    while (*current != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static Token number() {
    while (isDigit(*current)) advance();
    if (*current == '.' && isDigit(current[1])) {
        advance(); // Consume '.'
        while (isDigit(*current)) advance();
    }
    return makeToken(TOKEN_NUMBER);
}

// --- Public API ---
void initLexer(const char* source) {
    start = source;
    current = source;
    line = 1;
}

Token scanToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();
    if (isDigit(c)) return number();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        
        // Simplified: no '!', '!=', '=', etc.
        case 'n': if (strncmp(start, "nil", 3) == 0) return makeToken(TOKEN_NIL); break;
        case 't': if (strncmp(start, "true", 4) == 0) return makeToken(TOKEN_TRUE); break;
        case 'f': if (strncmp(start, "false", 5) == 0) return makeToken(TOKEN_FALSE); break;
    }

    return errorToken("Unexpected character.");
}
