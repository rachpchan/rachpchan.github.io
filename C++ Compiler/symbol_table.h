#ifndef symbol_table_h
#define symbol_table_h
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stack>
#include "parser.h"
using namespace std;

static const int nextInstruct = -1000;
static int instructionAddress = 1;

class symbolTable {

    public:

        struct Instructions
        {
            int address;
            string opString;
            int operand;

            Instructions(string opString, int operand) : address(instructionAddress++), opString(opString), operand(operand) {}
        };

        struct Symbol
        {
            Parser::Token token;
            int address;
            string type;

            Symbol(Parser::Token tkn, int address, string type) : token(tkn), address(address), type(type) {}
        };

        symbolTable();
        ~symbolTable();

        bool insert(Parser::Token tkn, string type);
        bool pop_typeStack();
        bool remove(Parser::Token tkn);
        bool typeStack_empty() const;
        int searchSymbolTable(Parser::Token tkn);
        int getAddress() const;
        int getAddress(Parser::Token token);
        int getMem();
        string getTokenType(Parser::Token token) const;
        string list();
        string list_instructions();
        string top_typeStack() const;
        void generateInstruction(string opString, int operand);
        void push_jumpStack(int address);
        void back_patch(int jumpAddress);
        void push_typeStack(string type);

    private:

        int memAddress;
        ostringstream error;
        stack <string> typeStack;
        vector<Symbol> table;
        vector<Instructions> instructions;
        vector<int> jumpStack;
        void incrementMem();
};

symbolTable::symbolTable() : memAddress(5000) {}

symbolTable::~symbolTable()
{
    instructionAddress = 1;
}

void symbolTable::incrementMem()
{
    ++this->memAddress;
}

bool symbolTable::insert(Parser::Token tkn, string type)
{
    bool isValid = false;

    if (!searchSymbolTable(tkn))
    {
        Symbol* s = new Symbol(tkn, this->memAddress, type);
        this->table.push_back(*s);
        incrementMem();
        isValid = true;
    }

    return isValid;
}

int symbolTable::searchSymbolTable(Parser::Token tkn)
{
    vector<Symbol>::iterator iter = this->table.begin();
    bool symbolFound = false;

    while (!symbolFound && iter != this->table.end())
    {
        if (iter->token.lexeme == tkn.lexeme && iter->token.token == tkn.token)
        {
            symbolFound = true;
        }
        else
        {
            ++iter;
        }
    }

    return symbolFound ? iter->address : 0;
}

bool symbolTable::remove(Parser::Token tkn)
{
    bool isValid = false;
    int position = 0;

    if (searchSymbolTable(tkn))
    {
        vector<Symbol>::const_iterator iter = this->table.begin();
        while (!isValid && iter != this->table.end())
        {
            if (iter->token.lexeme == tkn.lexeme)
            {
                this->table.erase(this->table.begin() + position);
                isValid = true;
            }
            else
            {
                ++position;
                ++iter;
            }
        }
    }

    return isValid;
}

string symbolTable::list()
{
    const int COL_WIDTH = 20;
    ostringstream printTable;

    printTable << left << setw(COL_WIDTH) << "Identifier" << setw(COL_WIDTH) << "Type"
        << "Memory Address" << endl;
    printTable << setfill('*') << setw(COL_WIDTH * 2 + 14) << '*' << setfill(' ') << endl;

    for (vector<Symbol>::const_iterator iter = this->table.begin(); iter != this->table.end(); ++iter)
        printTable << setw(COL_WIDTH) << iter->token.lexeme << setw(COL_WIDTH) << iter->type << iter->address << endl;

    return printTable.str();
}

string symbolTable::list_instructions()
{
    const int COL_WIDTH = 20;
    ostringstream printTable;

    printTable << left << setw(COL_WIDTH) << "Memory Address" << setw(COL_WIDTH) << "Opcode"
        << "Operand" << endl;
    printTable << setfill('*') << setw(COL_WIDTH * 2 + 7) << '*' << setfill(' ') << endl;

    for (vector<Instructions>::const_iterator iter = this->instructions.begin(); iter != this->instructions.end(); ++iter)
    {
        printTable << setw(COL_WIDTH) << iter->address << setw(COL_WIDTH) << iter->opString;

        if (iter->operand != nextInstruct)
        {
            printTable << iter->operand;
        }

        printTable << endl;
    }

    return printTable.str();
}

void symbolTable::generateInstruction(string opString, int operand)
{
    Instructions* instr = new Instructions(opString, operand);

    this->instructions.push_back(*instr);
}

void symbolTable::push_jumpStack(int address)
{
    this->jumpStack.push_back(address);
}

void symbolTable::back_patch(int jumpAddress)
{
    const int address = jumpStack.back();
    jumpStack.pop_back();

    if (this->instructions.size() >= address)
        this->instructions.at(address - 1).operand = jumpAddress;

}

int symbolTable::getAddress(Parser::Token token)
{
    return searchSymbolTable(token);
}

int symbolTable::getMem()
{
    return this->memAddress;
}

int symbolTable::getAddress() const
{
    return instructionAddress;
}

void symbolTable::push_typeStack(string type)
{
    this->typeStack.push(type);
}

bool symbolTable::pop_typeStack()
{
    bool isValid = false;

    if (!this->typeStack.empty())
    {
        isValid = true;
        this->typeStack.pop();
    }

    return isValid;
}

string symbolTable::top_typeStack() const
{
    return this->typeStack.top();
}

string symbolTable::getTokenType(Parser::Token token) const
{
    string type = "";

    for (Symbol s : this->table)
    {
        if (s.token.lexeme == token.lexeme)
        {
            type = s.type;
            break;
        }
    }

    if (token.token == "Integer")
    {
        type = "int";
    }

    return type;
}

bool symbolTable::typeStack_empty() const
{
    return this->typeStack.empty();
}

#endif