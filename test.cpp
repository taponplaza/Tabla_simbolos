#include "SymTable.h"

/**
 * Splits a string into a vector of substrings based on a delimiter.
 *
 * @param str The string to be split.
 * @return A vector of substrings obtained by splitting the input string.
 */
vector<string> split_words(string str)
{
    stringstream ss(str);
    string temp;
    vector<string> words;

    while (ss)
    {
        ss >> temp;
        words.push_back(temp);
    }

    return words;
}

int main()
{

    string str;
    int n;

    ifstream inputFile("input.txt");
    if (!inputFile)
    {
        cerr << "Failed to open input.txt" << endl;
        return 1;
    }

    inputFile >> n;
    SymbolTable table(n);

    while (getline(inputFile, str))
    {

        vector<string> words = split_words(str);

        if (words[0].compare("I") == 0)
        {
            string name = words[1];
            string value = words[2];
            table.insert(name, value);
            cout << getOutputStream();
        }
        else if (words[0].compare("L") == 0)
        {
            string temp = words[1];
            table.lookup(temp);
            cout << getOutputStream();
        }
        else if (words[0].compare("D") == 0)
        {
            string temp = words[1];
            table.remove(temp);
            cout << getOutputStream();
        }
        else if (words[0].compare("P") == 0)
        {
            if (words[1].compare("A") == 0){
                table.printAllScopeTable();
                cout << getOutputStream();
            }
            else if (words[1].compare("C") == 0){
                table.printCurrentScopeTable();
                cout << getOutputStream();
            }
        }
        else if (words[0].compare("S") == 0)
        {
            table.enterScope();
            cout << getOutputStream();
        }
        else if (words[0].compare("E") == 0)
        {
            table.exitScope();
            cout << getOutputStream();
        }
    }
    inputFile.close();
}