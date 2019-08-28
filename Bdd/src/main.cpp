/*
 * main.cpp
 *
 *  Created on: Aug 24, 2019
 *      Author: rey
 */

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <deque>
#include <algorithm>
#include <cmath>
#include <map>


std::string infija = "(¬U*B*CAB * ¬B) + (D * ¬Y)";

class BddNode
{
public:
	BddNode * hi = nullptr;
	BddNode * lo = nullptr;
	int varId;
	std::string var;
	bool isTerminal = false;
	int id;

	inline static std::deque<BddNode*> queueNodes;
	inline static bool firstExpresion = true;
	inline static int currentId = 1;
	inline static std::vector<BddNode*> labelNodes;
	inline static std::map<std::string, BddNode*> nodesMap;
	inline static std::set<std::string> variables;
	inline static std::vector<bool> truthTable;

	/*~BddNode()
	{
	}*/
	void erase()
	{
		if(hi != nullptr && lo != nullptr)
		{
			hi->erase();
			this->hi = nullptr;
			lo->erase();
			this->lo = nullptr;
		}
		truthTable.clear();
		delete this;
	}

	bool getFuncionBoolean(){
		if(queueNodes.empty())
			firstExpresion = true;
		queueNodes.push_front(this);
		if(isTerminal)
		{
			queueNodes.pop_front();
			return atof(var.c_str());
		}
		if(this->lo->getFuncionBoolean())
		{
			if(firstExpresion)
			{
				firstExpresion = false;
				std::cout << "(";
			}else
				std::cout << "+(";

			std::cout << "¬" << this->var;
			BddNode *currentNode = this;
			BddNode *prevNode;
			for (unsigned int i = 1; i < queueNodes.size(); i++) {
				prevNode = queueNodes[i];
				if (prevNode->lo == currentNode)
					std::cout << "* ¬" << queueNodes[i]->var;
				else
					std::cout << "*" << queueNodes[i]->var;
				currentNode = prevNode;
			}
			std::cout << ")";
		}
		if(this->hi->getFuncionBoolean())
		{
			if(firstExpresion)
			{
				firstExpresion = false;
				std::cout << "(";
			} else
				std::cout << "+(";

			std::cout << this->var;
			BddNode *currentNode = this;
			BddNode *prevNode;
			for (unsigned int i = 1; i < queueNodes.size(); i++) {
				prevNode = queueNodes[i];
				if(prevNode->lo == currentNode)
					std::cout << "*¬" << queueNodes[i]->var;
				else
					std::cout << "*" << queueNodes[i]->var;
				currentNode = prevNode;
			}
			std::cout << ")";
		}
		queueNodes.pop_front();
		if(queueNodes.empty())
		{
			firstExpresion = true;
			std::cout << std::endl;
		}

		return false;
	}

	void label()
	{
		//std::cout << "var:" << var.c_str() << std::endl;
		//std::string parent = var;
		if(this->lo != nullptr)
			lo->label();
		else
		{
			this->id = atof(this->var.c_str());
			if(std::find(labelNodes.begin(), labelNodes.end(), this) == labelNodes.end())
				this->labelNodes.push_back(this);
			return;
		}
		if(this->hi != nullptr)
			hi->label();
		else
		{
			this->id = atof(this->var.c_str());
			if(std::find(labelNodes.begin(), labelNodes.end(), this) == labelNodes.end())
				this->labelNodes.push_back(this);
			return;
		}
		if(this->lo != nullptr && this->hi != nullptr)
		{
			if(this->hi->id == this->lo->id)
			{
				this->id = this->hi->id;
				this->labelNodes.push_back(this);
			}
			else
			{
				std::vector<BddNode*>::iterator it = std::find_if(
						labelNodes.begin(), labelNodes.end(), *this);

				if (it != labelNodes.end() && (*it)->lo->id == this->lo->id
						&& (*it)->hi->id == this->hi->id) {
					this->id = (*it)->id;
				}
				else
				{
					currentId += 1;
					this->id = currentId;
					this->labelNodes.push_back(this);
				}
			}

		}
	}

	void redirect()
	{
		if(this->hi != nullptr && this->lo != nullptr)
		{
			this->lo->redirect();
			this->hi->redirect();
		}
	}

	void reduce()
	{
		currentId = 1;
		labelNodes.clear();
		label();
		redirect();
		labelNodes.clear();
	}

	bool generateTruthTable(){

		if(queueNodes.empty())
			firstExpresion = true;
		queueNodes.push_front(this);

		if(firstExpresion)
		{
			size_t tableSize = 1;
			for (unsigned int i=0; i< variables.size(); ++i) tableSize*=2;
			truthTable.clear();
			truthTable = std::vector<bool>(tableSize, false);
			firstExpresion = false;
		}

		if(this->lo == this->hi && this->lo == nullptr)
			return atoi(var.c_str());

		if(lo->generateTruthTable())
		{
			std::vector<std::string> codigos;
			std::string base;
			for(unsigned int i = 0; i < variables.size(); i++)
				base += "*";

			BddNode *currentNode = queueNodes[0];
			BddNode *prevNode = this;
			for (unsigned int i = 1; i <= queueNodes.size() - 1; i++) {
				int indexVar = 0;
				for (std::set<std::string>::iterator it = variables.begin();
						it != variables.end(); it++, indexVar++) {
					if ((*it).compare(prevNode->var) == 0)
						break;
				}

				if (prevNode->lo == currentNode)
					base[indexVar] = '0';
				else
					base[indexVar] = '1';
				if(i < queueNodes.size() - 1)
				{
					prevNode = queueNodes[i + 1];
					currentNode = queueNodes[i];
				}
			}

			codigos.push_back(base);
			bool fill;
			do {
				bool findPattern = false;
				fill = false;
				for (unsigned int i = 0; i < codigos.size() && !findPattern;
						i++)
					for (unsigned int j = 0;
							j < codigos[i].size() && !findPattern; j++) {
						if (codigos[i][j] == '*') {
							std::string newPattern = codigos[i];
							codigos.erase(codigos.begin() + i);
							newPattern[j] = '0';
							codigos.push_back(newPattern);
							newPattern[j] = '1';
							codigos.push_back(newPattern);
							findPattern = true;
							fill = true;
						}
					}
			} while (fill);

			for(unsigned int i = 0; i < codigos.size(); i++)
			{
				int idx = 0;
				for(unsigned int j = 0; j < codigos[i].size(); j++)
				{
					if (codigos[i][j] == '1')
						idx |= (1 << j);
				}
				truthTable[idx] = 1;
			}
		}
		queueNodes.pop_front();
		if(hi->generateTruthTable())
		{
			std::vector<std::string> codigos;
			std::string base;
			for (unsigned int i = 0; i < variables.size(); i++)
				base += "*";

			BddNode *currentNode = queueNodes[0];
			BddNode *prevNode = this;
			for (unsigned int i = 1; i <= queueNodes.size() - 1; i++) {
				int indexVar = 0;
				for (std::set<std::string>::iterator it = variables.begin();
						it != variables.end(); it++, indexVar++) {
					if ((*it).compare(prevNode->var) == 0)
						break;
				}

				if (prevNode->lo == currentNode)
					base[indexVar] = '0';
				else
					base[indexVar] = '1';
				if(i < queueNodes.size() - 1)
				{
					prevNode = queueNodes[i + 1];
					currentNode = queueNodes[i];
				}
			}

			codigos.push_back(base);
			bool fill;
			do {
				bool findPattern = false;
				fill = false;
				for (unsigned int i = 0; i < codigos.size() && !findPattern;
						i++)
					for (unsigned int j = 0;
							j < codigos[i].size() && !findPattern; j++) {
						if (codigos[i][j] == '*') {
							std::string newPattern = codigos[i];
							codigos.erase(codigos.begin() + i);
							newPattern[j] = '0';
							codigos.push_back(newPattern);
							newPattern[j] = '1';
							codigos.push_back(newPattern);
							findPattern = true;
							fill = true;
						}
					}
			} while (fill);

			for (unsigned int i = 0; i < codigos.size(); i++) {
				int idx = 0;
				for (unsigned int j = 0; j < codigos[i].size(); j++) {
					if (codigos[i][j] == '1')
						idx |= (1 << j);
				}
				truthTable[idx] = 1;
			}
		}
		queueNodes.pop_front();
		return false;
	}

	std::string bin(unsigned n, unsigned int bytes = 32)
	{
		std::string binaryStr;
	    unsigned i;
	    for (i = 1 << (bytes - 1); i > 0; i = i / 2)
	        (n & i)? binaryStr += "1": binaryStr += "0";
	    return binaryStr;
	}

	void printTruthTable()
	{
		for(unsigned int i = 0; i < truthTable.size(); i++)
		{
			std::string binary = bin(i, variables.size());
			int idx = 0;
			for (unsigned int j = 0; j < binary.size(); j++) {
				if (binary[j] == '1')
					idx |= (1 << j);
			}
			std::cout << binary << " | " << truthTable[idx]  << std::endl;
		}
	}

	bool operator()(const BddNode *obj2) const
	{
		if (var.compare(obj2->var) == 0)
			return true;
		else
			return false;
	}
};

typedef struct _Operator
{
	_Operator(std::string op)
	{
		this->op = op;
	}

	_Operator(std::string op, int precedencia, int index)
	{
		this->op = op;
		this->precedencia = precedencia;
		this->index = index;
	}

	_Operator(){
	}

	std::string op;
	int precedencia;
	int index;

	bool operator== ( const _Operator &n)
	{
	        return this->op == n.op;
	}
} Operator;

std::vector<std::string> infijaPosfija(std::string infija) {
	std::deque<std::string> queue;
	//std::string operators[] = { "¬", "*", "+", "(", ")" };
	Operator operators[] = {
			Operator("¬", 3, 0),
			Operator("*", 2, 1),
			Operator("+", 1, 2),
			Operator("(", 0, 3),
			Operator(")", 0, 4)
	};

	std::set<std::string> variables;
	std::vector<std::string> posFija;
	unsigned int sizeOperators = sizeof(operators) / sizeof(Operator);
	// std::cout << "sizeOperators: " << sizeOperators << std::endl;
	std::string token;
	std::vector<std::string> tokens;
	bool isOperator = false;
	std::string stringInfija("");
	for (unsigned int i = 0; i < infija.size(); i++) {
		if(infija[i] == -62)
		{
			stringInfija += std::string(1, infija[i]);
			continue;
		}
		stringInfija += std::string(1, infija[i]);
		if (stringInfija.compare(" ") != 0) {
			for (unsigned int j = 0; j < sizeOperators && !isOperator; j++) {
				if (stringInfija.compare(operators[j].op) == 0)
				{
					if(token.compare("") != 0)
						tokens.push_back(token);
					token = "";
					tokens.push_back(stringInfija);
					isOperator = true;
				}
			}
			if(!isOperator)
				token += stringInfija;
			isOperator = false;
			// queue.push();
		}
		stringInfija = "";
	}
	for(unsigned int i = 0; i < tokens.size(); i++)
	{
		// std::cout << "Token: " << tokens[i] << std::endl;
		isOperator = false;
		for(unsigned int j = 0; j < sizeOperators && !isOperator; j++){
			if( tokens[i].compare(operators[j].op) == 0)
				isOperator = true;
		}
		if(!isOperator)
		{
			// std::cout << "Variable: " << tokens[i] << std::endl;
			variables.insert(tokens[i]);
		}
	}
	std::vector<Operator> vecOperators(std::begin(operators),
			std::end(operators));
	while(tokens.size() > 0){
		token = tokens[0];
		if(tokens.size() > 1)
			tokens.erase(tokens.begin());
		else
			tokens = std::vector<std::string>();
		if(variables.find(token) != variables.end())
			posFija.push_back(token);
		else if(token.compare("(") == 0)
			queue.push_front(token);
		else if(token.compare(")") == 0)
		{
			while(!queue.empty() && queue.front().compare("(") != 0)
			{
				posFija.push_back(queue.front());
				queue.pop_front();
			}
			if(queue.front().compare("(") == 0)
				queue.pop_front();
			else
			{
				std::cout << "Error" << std::endl;
				break;
			}
		}
		else{
			std::vector<Operator>::iterator it = std::find(vecOperators.begin(),
					vecOperators.end(), Operator(token));
			if (it != vecOperators.end()) {
				while (!queue.empty()) {
					std::vector<Operator>::iterator itQueueTop = std::find(
							vecOperators.begin(), vecOperators.end(),
							Operator(queue.front()));
					if(queue.front().compare("(") == 0 || queue.front().compare(")") == 0)
						break;
					if(itQueueTop->precedencia >= it->precedencia )
					{
						posFija.push_back(queue.front());
						queue.pop_front();
					}
					else
						break;
				}
				queue.push_front(token);
			}
		}
	}
	while(!queue.empty())
	{
		posFija.push_back(queue.front());
		queue.pop_front();
	}

	std::cout << "PosFija:";
	for(unsigned int i = 0; i < posFija.size(); i++)
		std::cout << " " << posFija[i];
	 std::cout << std::endl;

	return posFija;
}

BddNode * createBDDummy(std::vector<std::string> expresion)
{
	BddNode * root = new BddNode();
	root->var = "z";
	root->varId = 0;
	root->variables.insert("z");
	root->variables.insert("y");
	root->variables.insert("x");

	BddNode * child1 = new BddNode();
	child1->var = "x";
	child1->varId = 2;

	BddNode * child2 = new BddNode();
	child2->var = "x";
	child2->varId = 2;

	BddNode * child3 = new BddNode();
	child3->var = "y";
	child3->varId = 1;

	BddNode * child4 = new BddNode();
	child4->var = "y";
	child4->varId = 1;

	BddNode * child5 = new BddNode();
	child5->var = "y";
	child5->varId = 1;

	BddNode * child6 = new BddNode();
	child6->var = "y";
	child6->varId = 1;

	BddNode * child7 = new BddNode();
	child7->var = "0";
	child7->varId = 0;
	child7->isTerminal = true;

	BddNode * child8 = new BddNode();
	child8->var = "1";
	child8->varId = 1;
	child8->isTerminal = true;

	root->lo = child1;
	root->hi = child2;
	child1->lo = child3;
	child1->hi = child4;
	child3->lo = child7;
	child3->hi = child8;
	child4->lo = child7;
	child4->hi = child8;
	child2->lo = child5;
	child2->hi = child6;
	child5->lo = child7;
	child5->hi = child8;
	child6->lo = child8;
	child6->hi = child7;

	return root;
}

BddNode * createBDDummy2(std::vector<std::string> expresion)
{
	BddNode * root = new BddNode();
	root->var = "z";
	root->varId = 2;
	root->variables.insert("z");
	root->variables.insert("y");
	root->variables.insert("x");

	BddNode * child1 = new BddNode();
	child1->var = "y";
	child1->varId = 1;

	BddNode * child2 = new BddNode();
	child2->var = "y";
	child2->varId = 1;

	BddNode * child3 = new BddNode();
	child3->var = "x";
	child3->varId = 2;

	BddNode * child4 = new BddNode();
	child4->var = "x";
	child4->varId = 2;

	BddNode * child5 = new BddNode();
	child5->var = "0";
	child5->varId = 0;
	child5->isTerminal = true;

	BddNode * child6 = new BddNode();
	child6->var = "1";
	child6->varId = 1;
	child6->isTerminal = true;

	root->lo = child1;
	root->hi = child2;
	child1->lo = child5;
	child1->hi = child3;
	child3->lo = child5;
	child3->hi = child6;
	child2->lo = child3;
	child2->hi = child4;
	child4->lo = child5;
	child4->hi = child6;

	return root;
}

int main(int argc, char ** argv) {
	//infijaPosfija(infija);

	BddNode * root = createBDDummy2(std::vector<std::string>());
	root->getFuncionBoolean();
	root->generateTruthTable();
	root->printTruthTable();
	root->reduce();
	root->erase();

	return 1;
}
