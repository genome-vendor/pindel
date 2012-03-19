#ifndef SEARCHSHORTINSERTIONS_H
#define SEARCHSHORTINSERTIONS_H

class SearchShortInsertions : public SearchVariant {
public:
	SearchShortInsertions();
	virtual ~SearchShortInsertions();

protected:
	int Count_Var;
	int Count_Var_Plus;
	int Count_Var_Minus;

	std::string typeOfVariant;

	bool decisionBranch1(ControlState& currentState, unsigned ReadIndex, unsigned int CloseIndex, int FarIndex);

	bool decisionBranch2(ControlState& currentState, unsigned ReadIndex, unsigned int CloseIndex, int FarIndex);

	unsigned int calculateIndelSize(ControlState& currentState, unsigned ReadIndex);

	std::string getInsertedStr1(ControlState& currentState, unsigned ReadIndex);

	std::string getInsertedStr2(ControlState& currentState, unsigned ReadIndex);

	void outputResults(ControlState& currentState, std::vector<unsigned> Vars[], const unsigned NumBoxes);

private:
	SearchShortInsertions(const SearchShortInsertions&);

};

#endif // SEARCHSHORTINSERTIONS_H
