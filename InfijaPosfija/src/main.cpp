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


std::string infija = "(¬U*B*CAB * ¬B) + (D * ¬Y)";

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

int main(int argc, char ** argv) {
	infijaPosfija(infija);
	return 1;
}
