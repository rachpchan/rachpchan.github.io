#include <iostream>
#include <fstream>
#include <iomanip>
#include "syntactic_analyzer.h"
#define COL_SIZE 25;
using namespace std;

int main()
{
	
	ifstream finish;
	string inFile;
	string line;
	stringstream* buffer;
	vector<Parser::Token> lineofTokens;
	vector<Parser::Token> tokens;

	vector<string> files = { "test1.txt", "test2.txt", "test3.txt" };
	ofstream out;
	out.open("output.txt");

	for (string file : files)
	{
		int numLine = 1;

		finish.open(file.c_str());

		if (!finish)
		{
			cout << "ERROR: File was not found." << endl;
			continue;
		}

		out << endl
			<< "Opening: \"" << file << "\"" << endl << endl;

		Parser* lexer = new Parser();

		while (getline(finish, line))
		{
			buffer = new stringstream(line);
			lineofTokens = lexer->lex(*buffer, numLine);

			tokens.insert(tokens.end(), lineofTokens.begin(), lineofTokens.end());

			numLine++;
		}

		finish.close();

		syntacticAnalyzer* syntaxAnalyzer = new syntacticAnalyzer(tokens, out, false);

		try
		{
			syntaxAnalyzer->Analyze();
		}
		catch (const SyntaxError& error)
		{
			out << endl
				<< "ERROR: " << error.getMessage();
		}

		tokens.clear();
		out << syntaxAnalyzer->PrintAll();

		delete syntaxAnalyzer;
	}

	out.close();
	cout << "File has been analyzed. \nPlease see 'output.txt' for results.\nPress any key to exit." << endl;
	cin.get();

	return 0;
}