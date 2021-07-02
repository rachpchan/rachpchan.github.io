#ifndef parser_h
#define parser_h
#include <iomanip>
#include <sstream>
#include <unordered_set>
#include <vector>
using namespace std;

class Parser {

public:

    enum States
    {
        NULL_STATE = 0,
        STATE_1,
        STATE_2,
        STATE_3,
        STATE_4,
        STATE_5,
        STATE_6,
        STATE_7,
        STATE_8,
        STATE_9,
        STATE_10,
        STATE_11,
        STATE_12,
        STATE_13,
        STATE_14,
        TERMINAL,
    };

    enum tokenType
    {
        IDENTIFIER = 0,
        INTEGER,
        REAL,
        CARROT,
        EQUALS,
        GREATER_THAN,
        LESS_THAN,
        ADD,
        SUBTRACT,
        MULT,
        DIV,
        SEPARATOR,
        DOLLAR_SIGN,
        REJECT,
    };

    int stateTable[18][14] = {

        {STATE_1, STATE_4, TERMINAL, STATE_10, STATE_12, STATE_14, STATE_14, STATE_14, STATE_14, STATE_14, STATE_14, STATE_7, STATE_8, TERMINAL},
        {STATE_2, STATE_3, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {STATE_2, STATE_3, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {STATE_2, STATE_3, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, STATE_4, STATE_5, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, STATE_6, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, STATE_6, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, STATE_9, TERMINAL},
        {TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, TERMINAL, TERMINAL, TERMINAL, STATE_11, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, TERMINAL, TERMINAL, TERMINAL, STATE_13, STATE_13, STATE_13, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},
        {TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL, TERMINAL},

    };

    unordered_set<string> keywords = {

        "and", "boolean", "do", "doend", "else", "enddo", "endelse", "endfor", "endif", "false", "float", "for", "function",
        "get", "if", "ifend", "int", "not", "or", "put", "return", "real", "STDinput", "STDoutput", "then", "true", "while", "whileend"

    };

    unordered_set<char> separators = { ',', ':', ';', '(', ')', '{', '}' };

    struct Token
    {

        Token() : token("next"), lexeme("next"), numLine(-1) { };

        Token(string token, string lexeme, int numLine)
        {
            this->token = token;
            this->lexeme = lexeme;
            this->numLine = numLine;
        }

        string token;
        string lexeme;
        int numLine;

    };

    Parser();
    ~Parser();

    vector<Token> lex(stringstream& buffer, int numLine);

private:

    bool isComment;

    int getTransition(char tokenChar) const;

    string stateToString(int state) const;

    bool isValidSeparator(char ch) const;

    bool isKeyword(string token) const;
};

Parser::Parser() : isComment(false) { }
Parser::~Parser() { }

vector<Parser::Token> Parser::lex(stringstream& buffer, int numLine)
{

    char ch;
    int currState = 0;
    int prevState = 0;
    int transitionState;
    string lexeme = "";
    string tokenStr = "";
    vector<Token> tokens;
    Token* token;
    

    while (buffer.get(ch))
    {

        if (isComment == true || (ch == '[' && buffer.peek() == '*'))
        {
            while (ch != '*' || buffer.peek() != ']')
            {
                if (buffer.eof())
                {
                    isComment = true;
                    ch = ' ';
                    break;
                }

                buffer.get(ch);
            }

            if (!buffer.eof() && ch == '*')
            {
                isComment = false;

                buffer.get(ch).get(ch);
            }
        }

        transitionState = getTransition(ch);

        currState = Parser::stateTable[currState][transitionState];

        if (currState == TERMINAL)
        {
            tokenStr = stateToString(prevState);

            if (tokenStr != "INVALID")
            {

                if (tokenStr == "Identifier")
                {
                    if (isKeyword(lexeme))
                        tokenStr = "Keyword";
                }

                token = new Token(tokenStr, lexeme, numLine);
                tokens.push_back(*token);

                currState = NULL_STATE;
                lexeme.clear();
                tokenStr.clear();

                if (!isspace(ch))
                    buffer.putback(ch);

            }
            else {

                if (!lexeme.empty())
                {
                    token = new Token(tokenStr, lexeme, numLine);
                    tokens.push_back(*token);
                }

                currState = NULL_STATE;
                lexeme.clear();
                tokenStr.clear();
            }
        }
        else
        {
            if (!isspace(ch))
                lexeme.push_back(ch);
        }

        prevState = currState;
    }

    tokenStr = stateToString(prevState);

    if (tokenStr != "INVALID")
    {
        if (tokenStr == "Identifier")
        {
            if (isKeyword(lexeme))
                tokenStr = "Keyword";
        }

        token = new Token(tokenStr, lexeme, numLine);
        tokens.push_back(*token);
    }

    return tokens;
}

int Parser::getTransition(char ch) const
{
    int transitionState = REJECT;

    if (ch == '+')
        transitionState = ADD;

    if (ch == '-')
        transitionState = SUBTRACT;

    if (ch == '*')
        transitionState = MULT;

    if (ch == '/')
        transitionState = DIV;

    if (ch == '$')
        transitionState = DOLLAR_SIGN;

    if (ch == '=')
        transitionState = EQUALS;

    if (ch == '>')
        transitionState = GREATER_THAN;

    if (ch == '<')
        transitionState = LESS_THAN;

    if (ch == '^')
        transitionState = CARROT;

    if (ch == '.')
        transitionState = REAL;

    if (isalpha(ch))
        transitionState = IDENTIFIER;

    if (isdigit(ch))
        transitionState = INTEGER;

    if (isValidSeparator(ch))
        transitionState = SEPARATOR;

    return transitionState;
}

string Parser::stateToString(int state) const {

    string stateString = "INVALID";

    switch (state) {

    case STATE_1:

    case STATE_2:

        stateString = "Identifier";
        break;

    case STATE_3:

    case STATE_4:

        stateString = "Integer";
        break;

    case STATE_5:

    case STATE_6:

        stateString = "Real";
        break;

    case STATE_7:

    case STATE_8:

    case STATE_9:

        stateString = "Separator";
        break;

    case STATE_10:

    case STATE_11:

    case STATE_12:

    case STATE_13:

    case STATE_14:

        stateString = "Operator";
        break;

    }

    return stateString;
}

bool Parser::isValidSeparator(char ch) const {

    return separators.count(ch);

}

bool Parser::isKeyword(string token) const {

    return keywords.count(token);

}

#endif