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
//#include <sstream>

//std::string infija = "(X0 | X1) & ! X2";
std::string infija = "!(X0 & X1 | !X2) & ! X0";

class BddNode
{
public:
	BddNode * hi = nullptr;
	BddNode * lo = nullptr;
	int varId;
	std::string var;
	bool isTerminal = false;
	int id;

	~BddNode()
	{
	}

	void erase(std::set<BddNode*> *toDelete)
	{
		if(this->lo != nullptr)
			this->lo->erase(toDelete);
		if(this->hi != nullptr)
			this->hi->erase(toDelete);
		//delete this;
		toDelete->insert(this);
	}

	bool operator()(const BddNode *obj2) const
	{
		if (var.compare(obj2->var) == 0)
			return true;
		else
			return false;
	}

protected:
	bool getBooleanFunction(std::deque<BddNode*> *queueNodes, bool *firstExpresion){
		if(queueNodes->empty())
			*firstExpresion = true;
		queueNodes->push_front(this);
		if(isTerminal)
		{
			queueNodes->pop_front();
			return atof(var.c_str());
		}
		if(this->lo->getBooleanFunction(queueNodes, firstExpresion))
		{
			if(*firstExpresion)
			{
				*firstExpresion = false;
				std::cout << "(";
			}else
				std::cout << "|(";

			std::cout << "!" << this->var;
			BddNode *currentNode = this;
			BddNode *prevNode;
			for (unsigned int i = 1; i < queueNodes->size(); i++) {
				prevNode = queueNodes->at(i);
				if (prevNode->lo == currentNode)
					std::cout << "&!" << queueNodes->at(i)->var;
				else
					std::cout << "&" << queueNodes->at(i)->var;
				currentNode = prevNode;
			}
			std::cout << ")";
		}
		if(this->hi->getBooleanFunction(queueNodes, firstExpresion))
		{
			if(*firstExpresion)
			{
				*firstExpresion = false;
				std::cout << "(";
			} else
				std::cout << "|(";

			std::cout << this->var;
			BddNode *currentNode = this;
			BddNode *prevNode;
			for (unsigned int i = 1; i < queueNodes->size(); i++) {
				prevNode = queueNodes->at(i);
				if(prevNode->lo == currentNode)
					std::cout << "&!" << queueNodes->at(i)->var;
				else
					std::cout << "&" << queueNodes->at(i)->var;
				currentNode = prevNode;
			}
			std::cout << ")";
		}
		queueNodes->pop_front();
		if(queueNodes->empty()){
			*firstExpresion = true;
			std::cout << std::endl;
		}

		return false;
	}

	void label(std::vector<BddNode*> *labelNodes, int *currentId)
	{
		//std::cout << "var:" << var.c_str() << std::endl;
		//std::string parent = var;
		if(this->lo != nullptr)
			lo->label(labelNodes, currentId);
		else
		{
			this->id = atof(this->var.c_str());
			if(std::find(labelNodes->begin(), labelNodes->end(), this) == labelNodes->end())
				labelNodes->push_back(this);
			return;
		}
		if(this->hi != nullptr)
			hi->label(labelNodes, currentId);
		else
		{
			this->id = atof(this->var.c_str());
			if(std::find(labelNodes->begin(), labelNodes->end(), this) == labelNodes->end())
				labelNodes->push_back(this);
			return;
		}
		if(this->lo != nullptr && this->hi != nullptr)
		{
			if(this->hi->id == this->lo->id)
			{
				this->id = this->hi->id;
				labelNodes->push_back(this);
			}
			else
			{
				std::vector<BddNode*>::iterator it = std::find_if(
						labelNodes->begin(), labelNodes->end(), *this);

				if (it != labelNodes->end() && (*it)->lo->id == this->lo->id
						&& (*it)->hi->id == this->hi->id) {
					this->id = (*it)->id;
				}
				else
				{
					*currentId += 1;
					this->id = *currentId;
					labelNodes->push_back(this);
				}
			}
		}
	}

	void redirect(std::vector<BddNode*> *nodesInsert, BddNode* parent)
	{
		if(this->hi != nullptr && this->lo != nullptr)
		{
			// parent = this;
			this->lo->redirect(nodesInsert, this);
			this->hi->redirect(nodesInsert, this);
			/*std::vector<BddNode*>::iterator it = nodesInsert->begin();
			for (it = nodesInsert->begin(); it != nodesInsert->end(); it++) {
				BddNode* node = *it;
				if (node->id == this->id && node != this)
					break;
			}
			if (it != nodesInsert->end()) {
				if (parent->lo == this)
					parent->lo = *it;
				else
					parent->hi = *it;
				delete this;
			} else if(*it != this)
				nodesInsert->push_back(this);*/
		}
		std::vector<BddNode*>::iterator it;
		for (it = nodesInsert->begin(); it != nodesInsert->end(); it++) {
			BddNode* node = *it;
			if (node->id == this->id && node != this)
				break;
		}
		if (it != nodesInsert->end()) {
			if (parent->lo == this)
				parent->lo = *it;
			else
				parent->hi = *it;
			delete this;
		} else if (*it != this)
			nodesInsert->push_back(this);
		else if (nodesInsert->size() == 0 || *it != this)
			nodesInsert->push_back(this);
	}

	bool generateTruthTable(std::deque<BddNode*> *queueNodes,
			std::vector<std::string> *variables, std::vector<bool> *truthTable) {

		queueNodes->push_front(this);
		if (this->lo == this->hi && this->lo == nullptr)
			return atoi(var.c_str());

		if (lo->generateTruthTable(queueNodes, variables, truthTable)) {
			std::vector<std::string> codigos;
			std::string base;
			for (unsigned int i = 0; i < variables->size(); i++)
				base += "&";

			BddNode *currentNode = queueNodes->at(0);
			BddNode *prevNode = this;
			for (unsigned int i = 1; i <= queueNodes->size() - 1; i++) {
				int indexVar = 0;
				for (std::vector<std::string>::iterator it = variables->begin();
						it != variables->end(); it++, indexVar++) {
					if ((*it).compare(prevNode->var) == 0)
						break;
				}

				if (prevNode->lo == currentNode)
					base[indexVar] = '0';
				else
					base[indexVar] = '1';
				if (i < queueNodes->size() - 1) {
					prevNode = queueNodes->at(i + 1);
					currentNode = queueNodes->at(i);
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
						if (codigos[i][j] == '&') {
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
				truthTable->at(idx) = 1;
			}
		}
		queueNodes->pop_front();
		if (hi->generateTruthTable(queueNodes, variables, truthTable)) {
			std::vector<std::string> codigos;
			std::string base;
			for (unsigned int i = 0; i < variables->size(); i++)
				base += "&";

			BddNode *currentNode = queueNodes->at(0);
			BddNode *prevNode = this;
			for (unsigned int i = 1; i <= queueNodes->size() - 1; i++) {
				int indexVar = 0;
				for (std::vector<std::string>::iterator it = variables->begin();
						it != variables->end(); it++, indexVar++) {
					if ((*it).compare(prevNode->var) == 0)
						break;
				}

				if (prevNode->lo == currentNode)
					base[indexVar] = '0';
				else
					base[indexVar] = '1';
				if (i < queueNodes->size() - 1) {
					prevNode = queueNodes->at(i + 1);
					currentNode = queueNodes->at(i);
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
						if (codigos[i][j] == '&') {
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
				truthTable->at(idx) = 1;
			}
		}
		queueNodes->pop_front();
		return false;
	}

	BddNode *apply(std::string op, BddNode *node)
	{
		BddNode * newNode = new BddNode();
		// Todo varID
		if (this->lo == nullptr && this->hi == nullptr
				&& ((node != nullptr && node->lo == nullptr
						&& node->hi == nullptr) || node == nullptr))
		{
			newNode->isTerminal = true;
			newNode->lo = nullptr;
			newNode->hi = nullptr;
			int vf = atoi(this->var.c_str());
			int vg;
			if(node != nullptr)
				vg = atoi(node->var.c_str());
			int vr;
			if (op.compare("|") == 0)
			{
				vr = vf | vg;
			}
			else if (op.compare("&") == 0)
			{
				vr = vf & vg;
			}
			else if(op.compare("!") == 0)
			{
				vr = !vf;
			}
			if(vr)
				newNode->var = '1';
			else
				newNode->var = '0';
			newNode->varId = vr;
		}
		else if(node != nullptr)
		{
			newNode->isTerminal = false;
			if (this->var.compare(node->var) == 0)
			{
				newNode->var = this->var;
				newNode->varId = this->varId;
				newNode->lo = this->lo->apply(op, node->lo);
				newNode->hi = this->hi->apply(op, node->hi);
			} else {
				if (this->lo != nullptr && this->hi != nullptr
						&& ((node->lo == nullptr && node->hi == nullptr)
								|| node->varId > this->varId))
				{
					newNode->var = this->var;
					newNode->varId = this->varId;
					newNode->lo = this->lo->apply(op, node);
					newNode->hi = this->hi->apply(op, node);
				} else {
					if (node->lo != nullptr && node->hi != nullptr
							&& ((this->lo == nullptr && this->hi == nullptr)
									|| this->varId > node->varId))
					{
						newNode->var = node->var;
						newNode->varId = node->varId;
						newNode->lo = node->lo->apply(op, this);
						newNode->hi = node->hi->apply(op, this);
					}
				}
			}
		}
		else if(node == nullptr)
		{
			newNode->var = this->var;
			newNode->varId = this->varId;
			newNode->lo = this->lo->apply(op, node);
			newNode->hi = this->hi->apply(op, node);
		}
		return newNode;
	}

	void setVarId(std::vector<std::string> *variables)
	{
		if(this->lo != nullptr && this->hi !=nullptr)
		{
			for(unsigned int i = 0; i < variables->size(); i++)
			{
				if(this->var.compare(variables->at(i)) == 0)
				{
					this->varId = i;
					break;
				}
			}
			this->lo->setVarId(variables);
			this->hi->setVarId(variables);
		}
	}
};

class BddTree: public BddNode
{
public:
	typedef BddNode super;
	std::deque<BddNode*> queueNodes;
	bool firstExpresion;
	int currentId = 1;
	std::vector<BddNode*> labelNodes;
	std::vector<std::string> variables;
	std::vector<bool> *truthTable;

	BddTree()
	{
		truthTable = nullptr;
		variables = std::vector<std::string>();
		labelNodes = std::vector<BddNode*>();
		queueNodes = std::deque<BddNode*>();
		firstExpresion = true;
	}

	~BddTree()
	{
		if(truthTable != nullptr)
		{
			truthTable->clear();
			delete truthTable;
		}
	}

	void erase()
	{
		std::set<BddNode*> toDelete;
		if(this->lo != nullptr)
			this->lo->erase(&toDelete);
		if(this->hi != nullptr)
			this->hi->erase(&toDelete);
		for (std::set<BddNode*>::iterator it = toDelete.begin();
				it != toDelete.end(); it++)
		{
			BddNode* bddDelete = *it;
			delete (bddDelete);
		}
		delete this;
	}

	void getBooleanFunction()
	{
		queueNodes.clear();
		super::getBooleanFunction(&queueNodes, &firstExpresion);
		queueNodes.clear();
	}

	void reduce()
	{
		currentId = 1;
		labelNodes.clear();
		super::label(&labelNodes, &currentId);
		labelNodes.clear();
		redirect(&labelNodes, this);
		labelNodes.clear();
	}

	std::vector<std::string> generateVariables(BddTree * g)
	{
		std::vector<std::string> newVariables;
		std::vector<std::string>::iterator itCurr1 = this->variables.begin();
		std::vector<std::string>::iterator itCurr2 = g->variables.begin();
		bool found = false;
		for (std::vector<std::string>::iterator it1 = itCurr1;
				it1 != this->variables.end(); )
		{
			for (std::vector<std::string>::iterator it2 = itCurr2;
					it2 != g->variables.end(); it2++)
			{
				if ((*it1).compare((*it2)) == 0)
				{
					std::vector<std::string>::iterator it3 = itCurr2;
					do
					{
						newVariables.push_back(*it3);
					} while(it2 != it3++);

					it1++;
					itCurr2 = ++it2;
					found = true;
					break;
				}
			}
			if(!found)
			{
				newVariables.push_back(*it1);
				it1++;
			}
			found = false;
		}
		while (g->variables.end() != itCurr2)
		{
			newVariables.push_back(*itCurr2);
			itCurr2++;
		};
		return newVariables;
	}

	BddTree * apply(std::string op, BddTree * g = nullptr)
	{
		BddTree * tree = new BddTree();
		if(g != nullptr)
			tree->variables = this->generateVariables(g);
		else
			tree->variables = this->variables;
		super::setVarId(&tree->variables);
		if(g != nullptr)
			g->super::setVarId(&tree->variables);
		BddNode * root = super::apply(op, g);
		tree->isTerminal = root->isTerminal;
		tree->hi = root->hi;
		tree->lo = root->lo;
		tree->var = root->var;
		tree->varId = root->varId;
		delete root;
		return tree;
	}

	void setVarId()
	{
		super::setVarId(&variables);
	}

	void generateTruthTable()
	{
		queueNodes.clear();
		size_t tableSize = 1;
		for (unsigned int i = 0; i < variables.size(); ++i)
			tableSize *= 2;
		truthTable = new std::vector<bool>(tableSize, false);
		super::generateTruthTable(&queueNodes, &variables, truthTable);
		queueNodes.clear();
	}

	void printTruthTable()
	{
		for (unsigned int i = 0; i < variables.size(); i++)
			std::cout << variables[i];
		std::cout << std::endl;
		for (unsigned int i = 0; i < truthTable->size(); i++) {
			std::string binary = bin(i, variables.size());
			int idx = 0;
			for (unsigned int j = 0; j < binary.size(); j++) {
				if (binary[j] == '1')
					idx |= (1 << j);
			}
			std::cout << binary << " | " << truthTable->at(idx) << std::endl;
		}
	}
private:
	std::string bin(unsigned n, unsigned int bytes = 32)
	{
		std::string binaryStr;
		unsigned i;
		for (i = 1 << (bytes - 1); i > 0; i = i / 2)
			(n & i) ? binaryStr += "1" : binaryStr += "0";
		return binaryStr;
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

Operator operators[] = {
		Operator("!", 3, 0),
		Operator("&", 2, 1),
		Operator("|", 1, 2),
		Operator("(", 0, 3),
		Operator(")", 0, 4)
};
unsigned int sizeOperators = sizeof(operators) / sizeof(Operator);

std::vector<std::string> infijaPosfija(std::string infija) {
	std::deque<std::string> queue;
	//std::string operators[] = { "¬", "*", "+", "(", ")" };

	std::set<std::string> variables;
	std::vector<std::string> posFija;
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
	if(token.compare("") != 0)
		tokens.push_back(token);
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

BddTree * createBDDummy(std::vector<std::string> expresion)
{
	BddTree * tree = new BddTree();
	tree->var = "z";
	//tree->varId = 0;
	tree->variables.push_back("z");
	tree->variables.push_back("y");
	tree->variables.push_back("x");

	BddNode * child1 = new BddNode();
	child1->var = "x";
	//child1->varId = 2;

	BddNode * child2 = new BddNode();
	child2->var = "x";
	//child2->varId = 2;

	BddNode * child3 = new BddNode();
	child3->var = "y";
	//child3->varId = 1;

	BddNode * child4 = new BddNode();
	child4->var = "y";
	//child4->varId = 1;

	BddNode * child5 = new BddNode();
	child5->var = "y";
	//child5->varId = 1;

	BddNode * child6 = new BddNode();
	child6->var = "y";
	//child6->varId = 1;

	BddNode * child7 = new BddNode();
	child7->var = "0";
	//child7->varId = 0;
	child7->isTerminal = true;

	BddNode * child8 = new BddNode();
	child8->var = "1";
	//child8->varId = 1;
	child8->isTerminal = true;

	tree->lo = child1;
	tree->hi = child2;
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

	tree->setVarId();

	return tree;
}

BddTree * createBDDummy2(std::vector<std::string> expresion)
{
	BddTree * tree = new BddTree();
	tree->var = "z";
	//tree->varId = 0;
	tree->variables.push_back("z");
	tree->variables.push_back("y");
	tree->variables.push_back("x");

	BddNode * child1 = new BddNode();
	child1->var = "y";
	//child1->varId = 1;

	BddNode * child2 = new BddNode();
	child2->var = "y";
	//child2->varId = 1;

	BddNode * child3 = new BddNode();
	child3->var = "x";
	//child3->varId = 0;

	BddNode * child4 = new BddNode();
	child4->var = "x";
	//child4->varId = 0;

	BddNode * child5 = new BddNode();
	child5->var = "0";
	//child5->varId = 0;
	child5->isTerminal = true;

	BddNode * child6 = new BddNode();
	child6->var = "1";
	//child6->varId = 1;
	child6->isTerminal = true;

	tree->lo = child1;
	tree->hi = child2;
	child1->lo = child5;
	child1->hi = child3;
	child3->lo = child5;
	child3->hi = child6;
	child2->lo = child3;
	child2->hi = child4;
	child4->lo = child5;
	child4->hi = child6;

	tree->setVarId();

	return tree;
}

BddTree * createBDDummyF(std::vector<std::string> expresion)
{
	BddTree * tree = new BddTree();
	tree->var = "z";
	//tree->varId = 0;
	tree->variables.push_back("z");
	tree->variables.push_back("y");
	tree->variables.push_back("x");
	tree->variables.push_back("w");

	BddNode * child1 = new BddNode();
	child1->var = "y";
	//child1->varId = 1;

	BddNode * child2 = new BddNode();
	child2->var = "x";
	//child2->varId = 2;

	BddNode * child3 = new BddNode();
	child3->var = "w";
	//child3->varId = 3;

	BddNode * child5 = new BddNode();
	child5->var = "0";
	//child5->varId = 0;
	child5->isTerminal = true;

	BddNode * child6 = new BddNode();
	child6->var = "1";
	//child6->varId = 1;
	child6->isTerminal = true;

	tree->lo = child1;
	tree->hi = child2;
	child1->lo = child3;
	child1->hi = child2;
	child2->lo = child3;
	child2->hi = child6;
	child3->lo = child5;
	child3->hi = child6;

	tree->setVarId();

	return tree;
}

BddTree * createBDDummyG(std::vector<std::string> expresion)
{
	BddTree * tree = new BddTree();
	tree->var = "z";
	//tree->varId = 0;
	tree->variables.push_back("z");
	tree->variables.push_back("x");
	tree->variables.push_back("w");


	BddNode * child2 = new BddNode();
	child2->var = "x";
	//child2->varId = 2;

	BddNode * child3 = new BddNode();
	child3->var = "w";
	//child3->varId = 3;

	BddNode * child5 = new BddNode();
	child5->var = "0";
	//child5->varId = 0;
	child5->isTerminal = true;

	BddNode * child6 = new BddNode();
	child6->var = "1";
	//child6->varId = 1;
	child6->isTerminal = true;

	tree->lo = child3;
	tree->hi = child2;
	child2->lo = child3;
	child2->hi = child6;
	child3->lo = child5;
	child3->hi = child6;
	tree->setVarId();

	return tree;
}

BddTree * createBDDDummyE()
{
	BddTree * tree = new BddTree();
	tree->var = "x";
	tree->variables.push_back("x");

	BddNode * child5 = new BddNode();
	child5->var = "0";
	child5->isTerminal = true;

	BddNode * child6 = new BddNode();
	child6->var = "1";
	child6->isTerminal = true;

	tree->lo = child5;
	tree->hi = child6;

	return tree;
}

BddTree * createBDDDummyH()
{
	BddTree * tree = new BddTree();
	tree->var = "y";
	tree->variables.push_back("y");

	BddNode * child5 = new BddNode();
	child5->var = "0";
	child5->isTerminal = true;

	BddNode * child6 = new BddNode();
	child6->var = "1";
	child6->isTerminal = true;

	tree->lo = child5;
	tree->hi = child6;

	return tree;
}

BddTree * createBDD(std::vector<std::string> posfija)
{
	std::deque<BddTree*> queue;
	BddTree * bddCreate = nullptr;
	bool isOperator;
	for(unsigned int i = 0; i < posfija.size(); i++)
	{
		isOperator = false;
		std::string stringPosfija = posfija[i];
		for (unsigned int j = 0; j < sizeOperators && !isOperator; j++) {
			if (stringPosfija.compare(operators[j].op) == 0)
				isOperator = true;
		}
		if(!isOperator)
		{
			BddTree * tree = new BddTree();
			tree->var = stringPosfija;
			tree->isTerminal = false;
			BddNode * loChild = new BddNode();
			loChild->var = "0";
			loChild->varId = 0;
			loChild->isTerminal = true;
			BddNode * hiChild = new BddNode();
			hiChild->var = "1";
			hiChild->varId = 1;
			hiChild->isTerminal = true;
			tree->lo = loChild;
			tree->hi = hiChild;
			tree->variables.push_back(stringPosfija);
			queue.push_back(tree);
		}
		else
		{
			if(stringPosfija.compare("!") == 0)
			{
				BddTree * tree1 = queue.back();
				queue.pop_back();
				BddTree * treeResul = tree1->apply("!");
				treeResul->reduce();
				queue.push_back(treeResul);
				tree1->erase();
			}
			else
			{
				/*if(queue.size() > 0)
				{
					BddTree * tree2 = queue.back();
					queue.pop_back();
					tree2->erase();
				}
				if (queue.size() > 0) {
					BddTree * tree1 = queue.back();
					queue.pop_back();
					tree1->erase();
				}*/
				BddTree * tree2 = queue.back();
				queue.pop_back();
				BddTree * tree1 = queue.back();
				queue.pop_back();
				BddTree * treeResul = tree1->apply(stringPosfija, tree2);
				treeResul->reduce();
				queue.push_back(treeResul);
				tree1->erase();
				tree2->erase();
			}
		}
	}
	if(queue.size() > 0)
	{
		BddTree * tree1 = queue.back();
		queue.pop_back();
		bddCreate = tree1;
	}
	return bddCreate;
}

int main(int argc, char ** argv) {
	std::vector<std::string> posfija = infijaPosfija(infija);

	BddTree * tree = createBDD(posfija);
	tree->getBooleanFunction();
	tree->generateTruthTable();
	tree->printTruthTable();
	tree->erase();

	/*BddTree * tree = createBDDummy(std::vector<std::string>());
	tree->getBooleanFunction();
	tree->generateTruthTable();
	tree->printTruthTable();
	tree->reduce();
	tree->getBooleanFunction();
	tree->generateTruthTable();
	tree->printTruthTable();
	tree->erase();*/

	/*BddTree * treeF = createBDDummyF(std::vector<std::string>());
	treeF->getBooleanFunction();
	treeF->generateTruthTable();
	treeF->printTruthTable();
	treeF->reduce();
	treeF->getBooleanFunction();
	treeF->generateTruthTable();
	treeF->printTruthTable();

	BddTree * treeG = createBDDummyG(std::vector<std::string>());
	treeG->getBooleanFunction();
	treeG->generateTruthTable();
	treeG->printTruthTable();
	treeG->reduce();
	treeG->getBooleanFunction();
	treeG->generateTruthTable();
	treeG->printTruthTable();

	BddTree * treeForG = treeG->apply("+", treeF);
	treeForG->getBooleanFunction();
	treeForG->generateTruthTable();
	treeForG->printTruthTable();
	treeForG->reduce();
	treeForG->getBooleanFunction();
	treeForG->generateTruthTable();
	treeForG->printTruthTable();

	treeF->erase();
	treeG->erase();
	treeForG->erase();*/

	/*BddTree * treeH = createBDDDummyH();
	treeH->getBooleanFunction();
	treeH->generateTruthTable();
	treeH->printTruthTable();
	treeH->reduce();
	treeH->getBooleanFunction();
	treeH->generateTruthTable();
	treeH->printTruthTable();

	BddTree * treeE = createBDDDummyE();
	treeE->getBooleanFunction();
	treeE->generateTruthTable();
	treeE->printTruthTable();
	treeE->reduce();
	treeE->getBooleanFunction();
	treeE->generateTruthTable();
	treeE->printTruthTable();

	BddTree * treeEorH = treeE->apply("+", treeH);
	treeEorH->getBooleanFunction();
	treeEorH->generateTruthTable();
	treeEorH->printTruthTable();
	treeEorH->reduce();
	treeEorH->getBooleanFunction();
	treeEorH->generateTruthTable();
	treeEorH->printTruthTable();

	treeE->erase();
	treeH->erase();
	treeEorH->erase();*/

	/*BddTree tree1;
	tree1.variables.push_back("z");
	tree1.variables.push_back("y");
	tree1.variables.push_back("x");
	tree1.variables.push_back("w");
	tree1.variables.push_back("u");
	BddTree tree2;
	tree2.variables.push_back("y");
	tree2.variables.push_back("v");
	tree2.variables.push_back("u");
	tree2.generateVariables(&tree1);*/

	/*BddTree * treeH = createBDDDummyH();
	BddTree * treeE = createBDDDummyE();
	BddTree * nTreeH = treeH->apply("¬");
	BddTree * gr = nTreeH->apply("+", treeE);
	gr->getBooleanFunction();
	gr->reduce();
	gr->getBooleanFunction();
	gr->generateTruthTable();
	gr->printTruthTable();
	treeH->erase();*/

	return 1;
}
