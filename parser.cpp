#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

enum TokenType {
    T_INT, T_FLOAT, T_ID, T_NUM, T_STRING, STRING, 
    T_IF, T_ELSE, T_RETURN, T_D_TYPE,
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,  
    T_SEMICOLON, T_GT, T_EOF, 
};


struct Token {
    TokenType type;
    string value;
    int line;
};

class Lexer {

    private:
        string src;
        size_t pos;

    public:
        Lexer(const string &src) {
            this->src = src;  
            this->pos = 0;    
        }

        vector<Token> tokenize() {
            vector<Token> tokens;
            int linum = 1;
            while (pos < src.size()) {
                char current = src[pos];
                if (current == '\n') {
                    linum++;
                    pos++;
                    continue;
                }
                if (isspace(current)) {
                    pos++;
                    continue;
                }
                if (isdigit(current)) {
                    tokens.push_back(Token{T_NUM, consumeNumber(), linum});
                    continue;
                }
                if (isalpha(current)) {
                    string word = consumeWord();
                    if (word == "int") tokens.push_back(Token{T_INT, word, linum});
                    else if (word == "float") tokens.push_back(Token{T_FLOAT, word, linum});
                    else if (word == "string") tokens.push_back(Token{T_STRING, word, linum});
                    else if (word == "if") tokens.push_back(Token{T_IF, word, linum});
                    else if (word == "else") tokens.push_back(Token{T_ELSE, word, linum});
                    else if (word == "return") tokens.push_back(Token{T_RETURN, word, linum});
                    else tokens.push_back(Token{T_ID, word, linum});
                    continue;
                }
                if (current == '\"') {
                    tokens.push_back(Token{STRING, consumeString(), linum});
                    continue;
                }

                if (current == '/') {
                    pos++;
                    if (src[pos] == '/') while (src[pos] != '\n') pos++;
                    else tokens.push_back(Token{T_DIV, "/"});
                    continue;
                } 
                
                switch (current) {
                        case '=': tokens.push_back(Token{T_ASSIGN, "=", linum}); break;
                        case '+': tokens.push_back(Token{T_PLUS, "+", linum}); break;
                        case '-': tokens.push_back(Token{T_MINUS, "-", linum}); break;
                        case '*': tokens.push_back(Token{T_MUL, "*", linum}); break;
                        // case '/': tokens.push_back(Token{T_DIV, "/", linum}); break;
                        case '(': tokens.push_back(Token{T_LPAREN, "(", linum}); break;
                        case ')': tokens.push_back(Token{T_RPAREN, ")", linum}); break;
                        case '{': tokens.push_back(Token{T_LBRACE, "{", linum}); break;  
                        case '}': tokens.push_back(Token{T_RBRACE, "}", linum}); break;  
                        case ';': tokens.push_back(Token{T_SEMICOLON, ";", linum}); break;
                        case '>': tokens.push_back(Token{T_GT, ">", linum}); break;
                        default: cout << "Unexpected character: " << current << endl; exit(1);
                }
                pos++;
            }
            tokens.push_back(Token{T_EOF, "", linum});
            return tokens;
        }

        string consumeNumber() {
            size_t start = pos;
            while (pos < src.size() && isdigit(src[pos])) pos++;
            return src.substr(start, pos - start);
        }

        string consumeWord() {
            size_t start = pos;
            while (pos < src.size() && isalnum(src[pos])) pos++;
            return src.substr(start, pos - start);
        }

        string consumeString() {
            pos++;
            size_t start = pos;
            while (pos < src.size() && src[pos] != '\"') pos++;
            pos++;
            return src.substr(start, pos - start - 1);
        }
};


class Parser {
 

public:
    Parser(const vector<Token> &tokens) {
        this->tokens = tokens;  
        this->pos = 0;          
    }

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;

    void parseStatement() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_STRING) {
            parseDeclaration();
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_IF) {
            parseIfStatement();
        } else if (tokens[pos].type == T_RETURN) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_LBRACE) {  
            parseBraceBlock();
        } else if (tokens[pos].type == T_LPAREN) {
            parseParenBlock();
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value << " at line: " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void parseBraceBlock() {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);  
    }
    void parseParenBlock() {
        expect(T_LPAREN);
        while (tokens[pos].type != T_RPAREN && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RPAREN);  
    }
    void parseDeclaration() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT) {
            pos++;
            expect(T_ID);
            if (tokens[pos].type == T_ASSIGN) {
                pos++;
                parseExpression();
            }
            expect(T_SEMICOLON);
        } else if (tokens[pos].type == T_STRING) {
            pos++;
            expect(T_ID);
            if (tokens[pos].type == T_ASSIGN) {
                pos++;
                expect(STRING);
            }
            expect(T_SEMICOLON);
        } else if (tokens[pos].type == T_LBRACE) {
        } else {
            cout << "Syntax error: expected data type at line: " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void parseParams() {
        expect(T_D_TYPE);
    }
    void parseAssignment() {
        expect(T_ID);
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();  
        if (tokens[pos].type == T_ELSE) {
            expect(T_ELSE);
            parseStatement();  
        }
    }

    void parseReturnStatement() {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseExpression() {
        parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            pos++;
            parseTerm();
        }
        if (tokens[pos].type == T_GT) {
            pos++;
            parseExpression();
        }
    }

    void parseTerm() {
        parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            pos++;
            parseFactor();
        }
    }

    void parseFactor() {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_ID) {
            pos++;
        } else if (tokens[pos].type == T_LPAREN) {
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value << " at line: " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void expect(TokenType type) {
        if (type == T_D_TYPE) {
            
        }
        if (tokens[pos].type == type) {
            pos++;
        } else {
            cout << "Syntax error: expected " << type << " but found " << tokens[pos].value << " at line: " << tokens[pos].line << endl;
            exit(1);
        }
    }
};

int main() {
    string input = R"(
        int a;
        a = 5;
        int b;
        b = a + 10;
        if (b > 10) {
            return b;
        } else {
            return 0;
        }
    )";

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();
    for (auto token: tokens) cout << token.type << "\t" << token.value << endl;
    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}