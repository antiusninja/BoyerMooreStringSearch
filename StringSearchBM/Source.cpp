#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <list>
#include <assert.h>
using namespace std;

vector<unsigned> goodSuffixTable;



class BoyerMooreStringSearch {
public:
	struct SubPattern {
		SubPattern(int rIndex, int length) :rIndex{ rIndex }, length{ length } {}
		int rIndex;//index from the right;
		int length;//number of elements in matching subpattern;
	};


	BoyerMooreStringSearch(){}
	BoyerMooreStringSearch(const string& pattern) :pattern{ pattern } {
		InitWithPattern(pattern);
	}
	void InitWithPattern(const string& in_pattern) {
		pattern = in_pattern;
		BadCharacterPreprocess();
		GoodSuffixPreprocess();
		preprocess_completed = true;
		cout << "\nBoyerMoore preprocess complete\n";
		PrintPattern();
		PrintBadCharacterTable();
		PrintGoodSuffixTable();
	}

	void BadCharacterPreprocess() {
		patternLength = pattern.length();
		badCharacterTable.resize(0x100);
		std::fill(badCharacterTable.begin(), badCharacterTable.end(), patternLength);
		for (int i = 0; i < patternLength - 1; ++i)
		{
			char current = pattern[i];
			badCharacterTable[current] = patternLength - 1 - i;
		}
	}
	void GoodSuffixPreprocess(){
	//initiate linked list of sub-pattern elemnents
	list<SubPattern> subpatterns;
	typedef list<SubPattern>::iterator SubPatternIterator;
	queue<SubPatternIterator> que;
	queue<SubPatternIterator> helperQue;


	char lastChar = pattern.back();
	for (int i = pattern.length()-2 ; i !=-1 ; --i)
	{
		while (!que.empty()) 
		{
			auto subpatternIter = que.front();
			que.pop();
			if (pattern[subpatternIter->rIndex - subpatternIter->length] == pattern[pattern.length()-1-subpatternIter->length]){
				++subpatternIter->length;
				helperQue.push(subpatternIter);
			}
		}
		while (!helperQue.empty()) {
			auto subpatternIter = helperQue.front();
			helperQue.pop();
			que.push(subpatternIter);
		}
		if (pattern[i] == lastChar)
		{
			SubPattern newSub{i,1};
			subpatterns.push_back(newSub);
			que.push(--subpatterns.end());
		}
	}
	goodSuffixTable.resize(pattern.length());

	for (int i = 1; i < (int)pattern.length(); ++i)
	{
		int min = 0x7fffffff;
		auto firstRightOccurenceIt = subpatterns.end();
		for (auto& it = subpatterns.begin(); it != subpatterns.end();++it) 
		{
			if (it->length >= i){
				firstRightOccurenceIt = it;
				break;
			}
			else if (i - it->length < min) 
			{
				firstRightOccurenceIt = it;
				min = i - it->length;
			}
		}
		if (firstRightOccurenceIt == subpatterns.end()) {
			goodSuffixTable[i] = 0;
		}
		else {
			goodSuffixTable[i] = pattern.length() - firstRightOccurenceIt->rIndex-1;
		}
	}
}

	int BadCharacterDecision(int numberOfMatches, char unmatchedChar) {
		int offset = badCharacterTable[unmatchedChar];
		if (offset == patternLength) {
			return patternLength;
		}
		else if (numberOfMatches >= offset) {
			return 1;
		}
		return offset - numberOfMatches;
	}

	void PrintPattern() {
		cout << pattern << endl;
	}
	void PrintBadCharacterTable() {
		cout << "Bad character table:\n";
		for (int i = 0; i < 0x100; ++i) {
			if (badCharacterTable[i] != patternLength) {
				cout << (char)i << ": " << badCharacterTable[i] << "  ";
			}
		}
		cout << endl;
	}
	void PrintGoodSuffixTable() {
		cout << "GoodSuffixTable:\n";
		for (int i = 0; i < goodSuffixTable.size(); ++i) {
			cout << goodSuffixTable[i] << " ";
		}
		cout << endl;
	}
	void SetSearchBuffer(const string& in_searchBuffer){
		searchBuffer = in_searchBuffer;
		searchBufferLength = searchBuffer.length();
	}
	int MatchDecision(){
		int gs = goodSuffixTable[patternLength - 1];
		return gs > patternLength ? gs : patternLength;
	}
	void DebugSearchStep(int i) {
		string bufferPrefix = searchBuffer.substr(0, i-patternLength+1);
		string bufferMatch = searchBuffer.substr(i - (patternLength - 1), patternLength);
		string bufferSuffix = searchBuffer.substr(i+1);

		cout << bufferPrefix << " " << bufferMatch << " " << bufferSuffix << endl;

		for (int j = 0; j < i - patternLength + 1; ++j)
			cout << "-";
		cout << " " << pattern << endl << endl;
	}
	int GoodSuffixDecision(int j){
		return goodSuffixTable[j];
	}
	
	void Search(){
		cout << "\nStarting search:\n";
		for (int i = patternLength-1; i < searchBufferLength;) {
			DebugSearchStep(i);
			int j = 0;
			for (; j < patternLength; ++j) {
				
				char currentBufferChar = searchBuffer[i - j];
				char currentPatternChar = pattern[patternLength - 1 - j];
				if (currentBufferChar != currentPatternChar)
				{
					int bc = BadCharacterDecision(j, currentBufferChar);
					int gs = GoodSuffixDecision(j);
					int shift = bc < gs ? gs : bc;
					i += shift;
					cout << "\tbad character:"<<bc<<"\n\tgood suffix:"<<gs<<"\n\tshifting by " << shift << endl;
					break;
				}
			}
			if (j == patternLength) {
				cout << "\tMatch at :" << i << endl;
				searchResults.push_back(i-patternLength+1);
				i += MatchDecision();
			}

		}
		cout << "End of search\n-------------------------\n\n";
	}
private:
	bool preprocess_completed{ false };
	string pattern;
	string searchBuffer;
	int patternLength{ 0 };
	int searchBufferLength{ 0 };
	vector<int> badCharacterTable;
	vector<int> goodSuffixTable;
	list<int> searchResults;
};

int main() {
	string buffer{ "valsvalval" };
	string pattern;
	BoyerMooreStringSearch bm;
	bm.SetSearchBuffer(buffer);

	bm.InitWithPattern("lval");
	bm.Search();


	

	bm.InitWithPattern("CDATDCDAABCACDACDA");
	bm.Search();

	int x;
	cin >> x;
	return 0;
}


/*
AAAAAABADDBC
------BCDDBC

AAAAAABADDBC
----+----DBCDDBC


*/