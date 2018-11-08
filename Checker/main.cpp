#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include"CheckConstraints.h"
#include "../Solver/PbReader.h"
#include "../Solver/DegreeConstrainedMST.pb.h"


using namespace std;
using namespace xxf;
using namespace pb;

int main(int argc, char *argv[]) {
	enum CheckerFlag {
		IoError = 0x0,             //十六进制数，可以直接通过判断IoError|FormatError的数值来确定哪个出错
		FormatError = 0x1,
		NodeNotCorveredError = 0x2,
		LoopExistError = 0x4,
		DegreeOverError = 0x8
	};

	string inputPath;
	string outputPath;

	if (argc > 1) {
		inputPath = argv[1];
	}
	else {
		cerr << "input path: " << flush;
		cin >> inputPath;
	}

	if (argc > 2) {
		outputPath = argv[2];
	}
	else {
		cerr << "output path: "<< flush;
		cin >> outputPath;
	}

	xxf::Input input;
	if (!load(inputPath, input)) { return ~CheckerFlag::IoError; }

	xxf::Output output;
	ifstream ifs(outputPath);
	if (!ifs.is_open()) { return ~CheckerFlag::IoError; }
	string submission;
	getline(ifs, submission); // skip the first line.
	ostringstream oss;
	oss << ifs.rdbuf();
	jsonToProtobuf(oss.str(), output);

	// check solution.    
	int error = 0;
	int edgeLengthSumOnTree = 0;
	// check legal
	if (output.edges().size() == 0)
	{
		error |= CheckerFlag::FormatError;
        cerr << "The solution is empty." << endl;
	}  //解为空
	vector<int> edgeFlag(input.graph().edges().size(), 0);    //记录边是否重复标记
	for (auto edgeOnTree = output.edges().begin(); edgeOnTree != output.edges().end(); ++edgeOnTree)
	{
		edgeFlag[edgeOnTree->id()] ++;
		if (edgeOnTree->id() < 0 || (edgeOnTree->id() >= input.graph().edges().size()))
		{
			error |= CheckerFlag::FormatError;
            cerr << "The edgeOnTree " << edgeOnTree->id() << " is not exist." << endl;
		}
		for (auto edge = input.graph().edges().begin(); edge != input.graph().edges().end(); ++edge) {
			if (edgeOnTree->id() == edge->id())                //检查边是否存在
			{
				if (((edgeOnTree->source() != edge->source()) || (edgeOnTree->target() != edge->target()) || (edgeOnTree->length() != edge->length())) || (edgeFlag[edgeOnTree->id()] >= 2))  //检查边是否重复
				{
					error |= CheckerFlag::FormatError;
					cerr << "The edgeOnTree " << edgeOnTree->id() << " is not exist or repeated." << endl;
				}
			}
		}
	}
	// check constrains
	xxf::CheckConstraints checkcons(output, input);
	if (checkcons.isCyclic()) { error |= CheckerFlag::LoopExistError; }
	int errorFlag = checkcons.checkNodeDegree();
	if (errorFlag == 1) { error |= CheckerFlag::DegreeOverError; }
	else if (errorFlag == 2) { error |= CheckerFlag::NodeNotCorveredError; }
	else if (errorFlag == 3) 
	{ 
		error |= CheckerFlag::DegreeOverError; 
		error |= CheckerFlag::NodeNotCorveredError;
	}
	else;
	edgeLengthSumOnTree = checkcons.GetObj();

	int returnCode = (error == 0) ? edgeLengthSumOnTree : ~error;
	cout << returnCode << endl;
	return returnCode;
}
