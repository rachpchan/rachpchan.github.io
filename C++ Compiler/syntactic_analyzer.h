#ifndef syntactic_analyzer_h
#define syntactic_analyzer_h
#include <fstream>
#include "parser.h"
#include "symbol_table.h"

using namespace std;

class SyntaxError
{
public:

	SyntaxError(string message, int numLine);
	~SyntaxError();

	string getMessage() const;

private:
	string message;
	int numLine;
};

class syntacticAnalyzer
{
public:

	syntacticAnalyzer(const vector<Parser::Token>& tokens, ofstream& output, bool print = false);
	~syntacticAnalyzer();

	void Analyze();

	string PrintAll();

private:
	enum ErrorType
	{
		mismatched_type,
		duplicate_symbols,
		undeclared_variable
	};

	void Assign();
	void Body();
	void Compound();
	void Condition();
	void Declaration();
	void DeclarationList();
	void Empty();
	void Expression();
	void ExpressionPrime();
	void Factor();
	void Function();
	void FunctionDefinitions();
	void IDs();
	void Identifier();
	void If();
	void Integer();
	void myFunc();
	void OptDeclarationList();
	void OptFunctionDefinitions();
	void OptParameterList();
	void ParameterList();
	void Parameter();
	void Primary();
	void Print();
	void Qualifier();
	void Return();
	void Scan();
	void StatementList();
	void Statement();
	void While();
	void Real();
	void Redo();
	void Term();
	void TermPrime();

	void error(ErrorType errorType, int numLine, string expected = "");

	void getNextToken();
	void printCurrentToken();

	const vector<Parser::Token>& tokens;
	vector<Parser::Token>::const_iterator it;
	Parser::Token currentToken;
	bool print;
	ofstream& output;
	symbolTable symbolTable;
	string* savedOp;
	string* savedType;
	Parser::Token* save;
	ostringstream err;
	int errorCount;
	bool isDeclaration;
	bool assign;
};

syntacticAnalyzer::syntacticAnalyzer(const vector<Parser::Token>& tokens, ofstream& output, bool print) : tokens(tokens), it(tokens.begin()), currentToken(*(it)), output(output), save(nullptr)
{
	this->print = print;
	this->save = new Parser::Token();
	this->errorCount = 0;
	this->isDeclaration = false;
	this->assign = false;
}

syntacticAnalyzer::~syntacticAnalyzer()
{
}

void syntacticAnalyzer::error(ErrorType errorType, int numLine, string expected)
{
	errorCount++;
	err << "( ERROR on Line " << numLine << " ) ";
	switch (errorType) {

	case duplicate_symbols:
	{
		err << "DUPLICATE SYMBOL DETECTED.";
		if (expected != "")
			err << " \"" << expected << "\"";
		break;
	}

	case mismatched_type:
	{
		err << "TYPE MISMATCH DETECTED";
		if (expected != "")
			err << ". Expected a(n) \"" << expected << "\"";
		break;
	}

	case undeclared_variable:
	{
		err << "UNDECLARED VARIABLE DETECTED.";

		if (expected != "")
			err << " \"" << expected << "\"";

		break;
	}

	}
	err << endl;
}

void syntacticAnalyzer::getNextToken()
{
	++it;

	if (it == this->tokens.end())
	{
		--it;
		throw SyntaxError("ERROR", currentToken.numLine);
	}

	this->currentToken = *(it);

	if (this->currentToken.token == "Invalid")
	{
		throw SyntaxError("Invalid input \'" + this->currentToken.lexeme + "\'", this->currentToken.numLine);
	}
}

void syntacticAnalyzer::myFunc()
{

	OptFunctionDefinitions();

	if (currentToken.lexeme == "$$")
	{
		getNextToken();
		OptDeclarationList();
		StatementList();
	}

	if (currentToken.lexeme != "$$")
	{
		throw SyntaxError("Expected '$$'.", currentToken.numLine);
	}
}

void syntacticAnalyzer::Parameter()
{

	IDs();

	if (currentToken.lexeme != ":")
	{
		throw SyntaxError("Expected ':'", currentToken.numLine);
	}

	getNextToken();
	Qualifier();
}

void syntacticAnalyzer::Function()
{

	Identifier();

	getNextToken();

	if (currentToken.lexeme != "(")
		throw SyntaxError("Expected '('", currentToken.numLine);

	getNextToken();

	OptParameterList();

	if (currentToken.lexeme != ")")
		throw SyntaxError("Expected ')'", currentToken.numLine);

	getNextToken();
	OptDeclarationList();
	Body();
}

void syntacticAnalyzer::OptFunctionDefinitions()
{

	if (currentToken.lexeme == "function")
	{
		getNextToken();
		FunctionDefinitions();
	}
	else
	{
		Empty();
	}
}

void syntacticAnalyzer::OptDeclarationList()
{

	if (currentToken.lexeme == "boolean" || currentToken.lexeme == "int" || currentToken.lexeme == "real")
	{
		DeclarationList();
	}
	else
	{
		Empty();
	}
}

void syntacticAnalyzer::DeclarationList()
{

	savedType = new string(currentToken.lexeme);

	this->isDeclaration = true;

	Declaration();

	if (currentToken.lexeme == ";")
	{
		symbolTable.pop_typeStack();

		getNextToken();

		if (currentToken.lexeme == "boolean" || currentToken.lexeme == "int" || currentToken.lexeme == "real")
			DeclarationList();
	}

	this->isDeclaration = false;
}

void syntacticAnalyzer::Declaration()
{

	Qualifier();
	getNextToken();

	if (currentToken.token == "Identifier")
		IDs();
}

void syntacticAnalyzer::Qualifier()
{
	// 
}

void syntacticAnalyzer::IDs()
{
	
	if (isDeclaration)
	{
		if (!symbolTable.searchSymbolTable(currentToken))
		{
			symbolTable.insert(currentToken, *savedType);
		}
		else
		{
			error(duplicate_symbols, currentToken.numLine, currentToken.lexeme);
		}
	}

	Identifier();
	getNextToken();

	if (currentToken.lexeme == ",")
	{
		getNextToken();
		if (currentToken.token == "Identifier")
		{
			IDs();
		}
		else
		{
			throw SyntaxError("Expected identifier", currentToken.numLine);
		}
	}
}

void syntacticAnalyzer::Identifier()
{
	// 
}

void syntacticAnalyzer::StatementList()
{
	
	Statement();

	if (currentToken.lexeme == "get" || currentToken.token == "Identifier" || currentToken.lexeme == "if" ||
		currentToken.lexeme == "put" || currentToken.lexeme == "return" || currentToken.lexeme == "while") {

		StatementList();

	}
}

void syntacticAnalyzer::Statement()
{
	
	if (currentToken.lexeme == "{")
	{
		getNextToken();
		Compound();
	}
	else if (currentToken.lexeme == "get")
	{
		getNextToken();
		Scan();
	}
	else if (currentToken.token == "Identifier")
	{
		Assign();
	}
	else if (currentToken.lexeme == "if")
	{
		getNextToken();
		If();
	}
	else if (currentToken.lexeme == "return")
	{
		getNextToken();
		Return();
	}
	else if (currentToken.lexeme == "put")
	{
		getNextToken();
		Print();
	}
	
	else if (currentToken.lexeme == "while")
	{
		getNextToken();
		While();
	}
	else
	{
		throw SyntaxError("Expected '{', identifier or keyword.\n", currentToken.numLine);
	}
}

void syntacticAnalyzer::Compound()
{
	if (print)
	{
		output << "\t<Compound> -> { <Statement List> }" << endl;
	}

	StatementList();

	if (currentToken.lexeme != "}")
	{
		throw SyntaxError("Expected '}'", currentToken.numLine);
	}

	getNextToken();
}

void syntacticAnalyzer::Assign() 
{
	
	Identifier();

	*save = currentToken;
	string type = symbolTable.getTokenType(*save);
	if (type == "")
	{
		error(undeclared_variable, currentToken.numLine, currentToken.lexeme);
		this->assign = true;
	}
	else
	{
		symbolTable.push_typeStack(symbolTable.getTokenType(*save));
	}

	getNextToken();

	if (currentToken.lexeme != "=")
	{
		throw SyntaxError("Expected '='", currentToken.numLine);
	}

	getNextToken();
	Expression();

	symbolTable.generateInstruction("POPM", symbolTable.getAddress(*save));

	if (currentToken.lexeme != ";")
	{
		throw SyntaxError("Expected ';'", currentToken.numLine);
	}

	symbolTable.pop_typeStack();

	getNextToken();
}

void syntacticAnalyzer::Expression()
{
	
	Term();
	ExpressionPrime();
}

void syntacticAnalyzer::ExpressionPrime()
{
	
	if (currentToken.lexeme == "+" || currentToken.lexeme == "-")
	{
		string opString = currentToken.lexeme;
		getNextToken();

		Term();

		if (opString == "+")
		{
			symbolTable.generateInstruction("ADD", nextInstruct);
		}
		else
		{
			symbolTable.generateInstruction("SUB", nextInstruct);
		}
		ExpressionPrime();
	}
	else
	{
		Empty();
	}
}

void syntacticAnalyzer::Term()
{
	Factor();
	TermPrime();
}

void syntacticAnalyzer::Factor()
{
	if (currentToken.lexeme == "-")
	{
		getNextToken();
	}

	Primary();
}

void syntacticAnalyzer::Primary()
{
	if (!symbolTable.typeStack_empty() && symbolTable.top_typeStack() == "")
	{
		symbolTable.pop_typeStack();
	}

	if (currentToken.token == "Identifier")
	{
		if (symbolTable.getTokenType(currentToken) == "")
		{
			if (!this->assign)
			{
				error(undeclared_variable, currentToken.numLine, currentToken.lexeme);
			}
			this->assign = false;
		}

		if (symbolTable.typeStack_empty())
		{
			if (symbolTable.getTokenType(currentToken) != "")
				symbolTable.push_typeStack(*savedType);
		}
		else if ((symbolTable.getTokenType(currentToken) != symbolTable.top_typeStack()) && symbolTable.top_typeStack() != "")
		{
			error(mismatched_type, currentToken.numLine, symbolTable.top_typeStack());
		}

		Identifier();
		symbolTable.generateInstruction("PUSHM", symbolTable.searchSymbolTable(currentToken));

		getNextToken();
		if (currentToken.lexeme == "(")
		{
			getNextToken();
			IDs();

			if (currentToken.lexeme != ")")
			{
				throw SyntaxError("Expected ')'", currentToken.numLine);
			}

			getNextToken();
		}
	}
	else if (currentToken.token == "Integer")
	{
		if (!symbolTable.typeStack_empty() && symbolTable.top_typeStack() != "int")
		{
			error(mismatched_type, currentToken.numLine, symbolTable.top_typeStack());
		}

		Integer();
		symbolTable.generateInstruction("PUSHI", stoi(currentToken.lexeme));
		getNextToken();
	}
	else if (currentToken.lexeme == "(")
	{
		getNextToken();

		Expression();

		if (currentToken.lexeme != ")")
		{
			throw SyntaxError("Expected ')'", currentToken.numLine);
		}
		getNextToken();
	}
	else if (currentToken.token == "Real")
	{
		if (!symbolTable.typeStack_empty() && symbolTable.top_typeStack() != "real")
		{
			error(mismatched_type, currentToken.numLine, symbolTable.top_typeStack());
		}

		Real();
		getNextToken();
	}
	else if (currentToken.lexeme == "true")
	{

		if (!symbolTable.typeStack_empty() && symbolTable.top_typeStack() != "boolean")
		{
			error(mismatched_type, currentToken.numLine, symbolTable.top_typeStack());
		}

		if (print)
		{
			output << "\ttrue" << endl;
		}

		symbolTable.generateInstruction("PUSHI", 1);
		getNextToken();
	}
	else if (currentToken.lexeme == "false")
	{

		if (!symbolTable.typeStack_empty() && symbolTable.top_typeStack() != "boolean")
		{
			error(mismatched_type, currentToken.numLine, symbolTable.top_typeStack());
		}

		if (print)
		{
			output << "\tfalse" << endl;
		}
		symbolTable.generateInstruction("PUSHI", 0);
		getNextToken();
	}
}

void syntacticAnalyzer::Integer()
{
	//
}

void syntacticAnalyzer::Real()
{
	//
}

void syntacticAnalyzer::Return()
{
	
	if (currentToken.lexeme != ";")
	{
		Expression();
	}
	getNextToken();
}

void syntacticAnalyzer::If()
{
	
	if (currentToken.lexeme != "(")
	{
		throw SyntaxError("Expected '('", currentToken.numLine);
	}

	getNextToken();

	Condition();

	if (currentToken.lexeme != ")")
	{
		throw SyntaxError("Expected ')'", currentToken.numLine);
	}

	getNextToken();

	Statement();

	if (currentToken.lexeme == "else")
	{
		Statement();
	}

	if (currentToken.lexeme != "ifend")
	{
		throw SyntaxError("Expected 'ifend' keyword", currentToken.numLine);
	}

	symbolTable.back_patch(symbolTable.getAddress());
	getNextToken();
}

void syntacticAnalyzer::Condition()
{
	savedType = new string(symbolTable.getTokenType(currentToken));

	if (*savedType == "")
	{
		error(undeclared_variable, currentToken.numLine, currentToken.lexeme);
	}

	Expression();

	Redo();

	getNextToken();
	Expression();

	if (*savedOp == "<")
	{
		symbolTable.generateInstruction("LES", nextInstruct);
	}
	else if (*savedOp == ">")
	{
		symbolTable.generateInstruction("GRT", nextInstruct);
	}
	else if (*savedOp == "==")
	{
		symbolTable.generateInstruction("EQU", nextInstruct);
	}
	else if (*savedOp == "^=")
	{
		symbolTable.generateInstruction("NEQ", nextInstruct);
	}
	else if (*savedOp == "=>")
	{
		symbolTable.generateInstruction("GEQ", nextInstruct);
	}
	else if (*savedOp == "=<")
	{
		symbolTable.generateInstruction("LEQ", nextInstruct);
	}

	symbolTable.push_jumpStack(symbolTable.getAddress());
	symbolTable.generateInstruction("JUMPZ", nextInstruct);
}

void syntacticAnalyzer::Redo()
{
	if (currentToken.lexeme != "==" && currentToken.lexeme != "^=" && currentToken.lexeme != ">" && currentToken.lexeme != "<" && currentToken.lexeme != "=>" && currentToken.lexeme != "=<")
	{
		throw SyntaxError("Expected relational operator", currentToken.numLine);
	}

	this->savedOp = new string(currentToken.lexeme);

}

void syntacticAnalyzer::Empty()
{
	//
}

void syntacticAnalyzer::Body()
{
	
	if (currentToken.lexeme != "{")
	{
		throw SyntaxError("Expected '{'", currentToken.numLine);
	}

	getNextToken();

	StatementList();

	if (currentToken.lexeme != "}")
	{
		throw SyntaxError("Expected '}'", currentToken.numLine);
	}

	getNextToken();
}

void syntacticAnalyzer::FunctionDefinitions()
{
	
	Function();

	if (currentToken.lexeme == "function")
	{
		getNextToken();
		FunctionDefinitions();
	}
}

void syntacticAnalyzer::Print()
{
	
	if (currentToken.lexeme != "(")
	{
		throw SyntaxError("Expected '('", currentToken.numLine);
	}

	getNextToken();
	Expression();

	if (currentToken.lexeme != ")")
	{
		throw SyntaxError("Expected ')'", currentToken.numLine);
	}
	getNextToken();

	if (currentToken.lexeme != ";")
	{
		throw SyntaxError("Expected ';'", currentToken.numLine);
	}

	symbolTable.generateInstruction("STDOUT", nextInstruct);

	getNextToken();
}

void syntacticAnalyzer::Scan()
{
	
	if (currentToken.lexeme != "(")
	{
		throw SyntaxError("Expected '('", currentToken.numLine);
	}

	getNextToken();

	symbolTable.generateInstruction("STDIN", nextInstruct);
	int address = symbolTable.getAddress(currentToken);
	symbolTable.generateInstruction("POPM", address);

	IDs();

	if (currentToken.lexeme != ")")
	{
		throw SyntaxError("Expected ')'", currentToken.numLine);
	}

	getNextToken();
	if (currentToken.lexeme != ";")
	{
		throw SyntaxError("Expected ';'", currentToken.numLine);
	}

	getNextToken();
}

void syntacticAnalyzer::TermPrime()
{
	
	if (currentToken.lexeme == "*" || currentToken.lexeme == "/")
	{
		string opString = currentToken.lexeme;

		getNextToken();

		Factor();

		if (opString == "*")
		{
			symbolTable.generateInstruction("MUL", nextInstruct);
		}
		else
		{
			symbolTable.generateInstruction("DIV", nextInstruct);
		}

		TermPrime();
	}
}

void syntacticAnalyzer::Analyze()
{
	myFunc();
}

void syntacticAnalyzer::OptParameterList()
{
	
	if (currentToken.lexeme == ")")
	{
		Empty();
	}
	else if (currentToken.token == "Identifier")
	{
		ParameterList();
	}
	else
	{
		throw SyntaxError("Expected ')' or identifier", currentToken.numLine);
	}
}

void syntacticAnalyzer::ParameterList()
{
	
	Parameter();

	getNextToken();

	if (currentToken.lexeme == ",")
	{
		getNextToken();
		ParameterList();
	}

}

void syntacticAnalyzer::While()
{
	
	int address = symbolTable.getAddress();
	symbolTable.generateInstruction("LABEL", nextInstruct);

	if (currentToken.lexeme != "(")
	{
		throw SyntaxError("Expected '('", currentToken.numLine);
	}
	getNextToken();

	Condition();

	if (currentToken.lexeme != ")")
	{
		throw SyntaxError("Expected ')'", currentToken.numLine);
	}
	getNextToken();
	Statement();

	if (currentToken.lexeme != "whileend")
	{
		throw SyntaxError("Expected 'whileend' keyword", currentToken.numLine);
	}
	symbolTable.generateInstruction("JUMP", address);
	symbolTable.back_patch(symbolTable.getAddress());

	getNextToken();
}

void syntacticAnalyzer::printCurrentToken()
{
	output << left << endl << setw(10) << "Token:" << setw(20) << currentToken.token << setw(10) << "Lexeme:" << currentToken.lexeme << endl << endl;
}

SyntaxError::SyntaxError(string message, int numLine)
{
	this->message = message;
	this->numLine = numLine;
}

SyntaxError::~SyntaxError() {}

string SyntaxError::getMessage() const
{
	return (this->message + " Line: " + to_string(this->numLine));
}

string syntacticAnalyzer::PrintAll()
{
	ostringstream out;

	out << this->symbolTable.list();
	out << endl;
	out << this->symbolTable.list_instructions();

	if (this->errorCount > 0)
	{
		out << endl << endl;
		out << errorCount << " TOTAL ERROR(S) FOUND" << endl;
		out << "---------------------------------------------------------------------\n\n";
		out << err.str();
	}

	out << endl;

	return out.str();
}

#endif