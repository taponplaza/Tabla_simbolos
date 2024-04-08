using namespace std;
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

bool verbose = true;
stringstream outputStream;

string getOutputStream()
{
    string temp = outputStream.str();
    outputStream.str(std::string());
    return temp;
}

/**
 * Calculates the sdbm hash value for a given string.
 *
 * @param str The input string to calculate the hash value for.
 * @return The sdbm hash value of the input string.
 */
uint32_t sdbm(string str)
{
    uint32_t hash = 0;

    for (char c : str)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

/**
 * @brief A class for storing a string to be printed.
 */
class ForPrinting
{
    string to_print;

public:
    ForPrinting() {}

    void setToPrint(string s)
    {
        this->to_print = s;
    }

    string getToPrint()
    {
        return this->to_print;
    }

    ForPrinting(const ForPrinting &old_obj)
    {
        to_print = old_obj.to_print;
    };
};

/**
 * @class Symbol
 * @brief Represents a symbol in a symbol table.
 *
 * The Symbol class stores information about a symbol, such as its name, type, function parameters,
 * function return type, variable type, array size, and whether the function is defined or not.
 * It provides methods to access and modify these properties.
 */
class Symbol
{
    string name;
    string type;
    Symbol *next;
    vector<string> func_params;
    string func_return_type;
    string var_type;
    string to_print;
    int arrSize = 0;
    bool funcDefined = false;

public:
    Symbol(const Symbol &old_obj)
    {
        name = old_obj.name;
        type = old_obj.type;
        func_params = old_obj.func_params;
        func_return_type = old_obj.func_return_type;
        var_type = old_obj.var_type;
        to_print = old_obj.to_print;
        arrSize = old_obj.arrSize;
        next = nullptr;
    };

    Symbol(string name, string type)
    {
        this->name = name;
        this->type = type;
        this->next = nullptr;
        this->var_type = "undeclared type";
    }

    bool getFuncDefined()
    {
        return this->funcDefined;
    }

    void setFuncDefined(bool b)
    {
        this->funcDefined = b;
    }

    ~Symbol()
    {
        if (next != nullptr)
            delete next;
    }

    void setToPrint(string s)
    {
        this->to_print = s;
    }

    string getToPrint()
    {
        return this->to_print;
    }
    void setArrSize(int i)
    {
        arrSize = i;
    }

    void setName(string s)
    {
        this->name = s;
    }

    int getArrSize()
    {
        return arrSize;
    }

    string getType()
    {
        return type;
    }
    void setType(string Type)
    {
        this->type = Type;
    }
    void setFuncParameters(vector<string> parameters)
    {
        func_params = parameters;
    }

    vector<string> getFuncParameters()
    {
        return func_params;
    }

    void setVarType(string type)
    {
        var_type = type;
    }

    string getVarType()
    {
        if (arrSize != 0)
            return var_type + " array";

        if (var_type.empty())
            return "Undefined type";
        return var_type;
    }

    string getElementType()
    {
        if (arrSize == 0)
            return "Error: Not an array";
        return var_type;
    }
    string getVarType(bool debug)
    {
        if (arrSize != 0)
            return var_type + " array";

        return var_type;
    }

    void setFuncReturnType(string type)
    {
        func_return_type = type;
    }

    string getFuncReturnType()
    {
        return func_return_type;
    }

    void setNext(Symbol *next)
    {
        this->next = next;
    }

    Symbol *getNext()
    {
        return this->next;
    }

    string getName()
    {
        return name;
    }

    void printSymbol()
    {
        outputStream << "< " << name << " : " << type << " >";
    }
};

/**
 * @class ScopeTable
 * @brief Represents a scope table for symbol table management.
 *
 * The ScopeTable class is responsible for managing a scope table, which is used for symbol table operations.
 * It provides methods for inserting, deleting, and looking up symbols in the table.
 * Each scope table has a parent scope, a unique identifier, and a vector of symbols.
 */
class ScopeTable
{
    ScopeTable *parentScope;
    int size;
    int numChildren;
    string id;
    vector<Symbol *> symbols;

public:
    string getId()
    {
        return this->id;
    }

    int getNumChildren()
    {
        return this->numChildren;
    }

    ScopeTable(int size, string parentId, int order)
    {
        this->numChildren = 0;
        this->size = size;
        this->id = parentId + to_string(order);
        this->symbols.resize(size, nullptr);
    }

    void incrementNumChildren()
    {
        this->numChildren++;
    }

    ~ScopeTable()
    {
        for (Symbol *symbol : symbols)
        {
            while (symbol != nullptr)
            {
                Symbol *next = symbol->getNext();
                delete symbol;
                symbol = next;
            }
        }
    }

    void setParentScope(ScopeTable *parentScope)
    {
        this->parentScope = parentScope;
    }

    ScopeTable *getParentScope()
    {
        return this->parentScope;
    }

    bool insertSymbol(const string &name, const string &type)
    {
        int index = sdbm(name) % this->size;

        if (this->symbols[index] == nullptr)
        {
            this->symbols[index] = new Symbol(name, type);
        }
        else
        {
            Symbol *current = this->symbols[index];

            while (current != nullptr)
            {
                if (current->getName().compare(name) == 0)
                {
                    if (verbose)
                    {
                        current->printSymbol();
                        outputStream << " already exists in current ScopeTable" << endl;
                    }
                    return false;
                }

                if (current->getNext() == nullptr)
                {
                    current->setNext(new Symbol(name, type));
                    break;
                }
                else
                {
                    current = current->getNext();
                }
            }
        }

        string toPrint = "Inserted  <" + name + " , " + type + "> in ScopeTable# " + this->getId() + " at position " + to_string(index);
        if (verbose)
            outputStream << toPrint << endl;
        return true;
    }

    bool deleteSymbol(string name)
    {
        int index = sdbm(name) % this->size;

        if (this->symbols[index] == nullptr)
        {
            if (verbose)
                outputStream << name << " not found" << endl;
            return false;
        }

        Symbol *current = this->symbols[index];
        Symbol *before = nullptr;

        while (current != nullptr && current->getName().compare(name) != 0)
        {
            before = current;
            current = current->getNext();
        }

        if (current == nullptr)
        {
            if (verbose)
                outputStream << name << " not found" << endl;
            return false;
        }

        if (before == nullptr)
        {
            this->symbols[index] = current->getNext();
        }
        else
        {
            before->setNext(current->getNext());
        }

        delete current;

        if (verbose)
            outputStream << "Deleted Entry " << index << " from current ScopeTable" << endl;

        return true;
    }

    Symbol *lookupSymbol(string name)
    {
        int index = sdbm(name) % this->size;

        Symbol *current = this->symbols[index];

        while (current != nullptr && current->getName().compare(name) != 0)
        {
            current = current->getNext();
        }

        if (current != nullptr)
        {
            if (verbose)
                outputStream << "Found in ScopeTable# " + this->getId() + " at position " + to_string(index) << endl;
        }
        else
        {
            if (verbose)
                outputStream << name << " not found in Current Scope Table# "+ this->getId() << endl;
        }

        return current;
    }

    void printOneRow(int index)
    {
        if (this->symbols[index] == nullptr)
            return;

        Symbol *current = this->symbols[index];

        while (current != nullptr)
        {
            current->printSymbol();
            outputStream << " ";
            current = current->getNext();
        }
    }

    void printScopeTable()
    {
        outputStream << "ScopeTable# " << this->getId() << endl;

        for (int i = 0; i < size; i++)
        {
            if (this->symbols[i] == nullptr)
                continue;

            outputStream << i << "--> ";
            printOneRow(i);
            outputStream << endl;
        }

        outputStream << endl;
    }
};

/**
 * @class SymbolTable
 * @brief Represents a symbol table for storing and managing symbols.
 *
 * The SymbolTable class provides functionality to enter and exit scopes, insert and remove symbols,
 * lookup symbols, and print the scope tables.
 */
class SymbolTable
{
    ScopeTable *current;
    int size;
    int globalScopeCount;

public:
    SymbolTable(int size)
    {
        this->size = size;
        this->globalScopeCount = 1;
        current = new ScopeTable(size, "", globalScopeCount);
        current->setParentScope(nullptr);
    }

    void enterScope()
    {
        string parentId;
        int order;

        if (current != nullptr)
        {
            parentId = current->getId() + ".";
            current->incrementNumChildren();
            order = current->getNumChildren();
        }
        else
        {
            parentId = "";
            this->globalScopeCount++;
            order = this->globalScopeCount;
        }

        ScopeTable *temp = current;
        current = new ScopeTable(size, parentId, order);
        current->setParentScope(temp);

        if (verbose)
            outputStream << "New ScopeTable with id " + current->getId() + " created" << endl;
    }

    void printCurrentScopeId()
    {
        if (current == nullptr)
            return;

        outputStream << current->getId() << endl;
    }

    void exitScope()
    {
        if (current == nullptr)
        {
            outputStream << "No current Scope. Cannot exit." << endl;
            return;
        }

        ScopeTable *temp = current->getParentScope();
        if (verbose)
            outputStream << "ScopeTable with id " + current->getId() + " removed" << endl;
        delete current;
        current = temp;
    }

    bool insert(string name, string type)
    {
        if (current == nullptr)
        {
            outputStream << "No current Scope. Cannot Insert." << endl;
            return false;
        }

        return current->insertSymbol(name, type);
    }

    bool remove(string name)
    {
        if (current == nullptr)
        {
            outputStream << "No current Scope. Cannot Remove." << endl;
            return false;
        }

        bool flag = current->deleteSymbol(name);
        return flag;
    }

    Symbol *lookup(string name)
    {
        if (current == nullptr)
        {
            outputStream << "No current Scope. Cannot Lookup." << endl;
            return nullptr;
        }

        ScopeTable *toSearch = current;

        while (toSearch != nullptr)
        {
            Symbol *returnVal = toSearch->lookupSymbol(name);
            if (returnVal != nullptr)
                return returnVal;
            toSearch = toSearch->getParentScope();
        }

        outputStream << "Not found in any Table" << endl;
        return nullptr;
    }

    void printCurrentScopeTable()
    {
        outputStream.str(string());
        if (current == nullptr)
        {
            outputStream << "No current Scope. Cannot Print." << endl;
            return;
        }
        current->printScopeTable();
    }

    void recurseTablesForPrinting(ScopeTable *x)
    {
        if (x == nullptr)
            return;

        x->printScopeTable();
        recurseTablesForPrinting(x->getParentScope());
    };

    void printAllScopeTable()
    {
        outputStream.str(string());

        if (current == nullptr)
        {
            outputStream << "No current Scope. Cannot Print." << endl;
            return;
        }

        ScopeTable *x = current;
        recurseTablesForPrinting(x);
    }
};