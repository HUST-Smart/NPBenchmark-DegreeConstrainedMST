#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//#include "Visualizer.h"

#include "../Solver/PbReader.h"
#include "../Solver/DegreeConstrainedMST.pb.h"
//#include "../Solver/GateAssignment.pb.h"


using namespace std;
using namespace xxf;
using namespace pb;

int main(int argc, char *argv[]) {
	enum CheckerFlag {
		IoError = 0x0,             //ʮ��������������ֱ��ͨ���ж�IoError|FormatError����ֵ��ȷ���ĸ�����
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
		cout << "input path: " << flush;
		cin >> inputPath;
	}

	if (argc > 2) {
		outputPath = argv[2];
	}
	else {
		cout << "output path: " << flush;
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
		cout << "The solution is empty." << endl;
	}  //��Ϊ��
	vector<int> edgeFlag(input.graph().edges().size(), 0);    //��¼���Ƿ��ظ����
	for (auto edgeOnTree = output.edges().begin(); edgeOnTree != output.edges().end(); ++edgeOnTree)
	{
		edgeFlag[edgeOnTree->id()] ++;
		for (auto edge = input.graph().edges().begin(); edge != input.graph().edges().end(); ++edge) {
			if (edgeOnTree->id() == edge->id())                //�����Ƿ����
			{
				if (((edgeOnTree->source() != edge->source()) || (edgeOnTree->target() != edge->target()) || (edgeOnTree->length() != edge->length())) || (edgeFlag[edgeOnTree->id()] >= 2))  //�����Ƿ��ظ�
				{
					error |= CheckerFlag::FormatError;
					cout << "The edgeOnTree " << edgeOnTree->id() << " is not exist or repeated." << endl;
				}
			}
		}
	}
	// check constrains



	int error = 0;
	int flightNumOnBridge = 0;
	if (output.assignments().size() != input.flights().size()) { error |= CheckerFlag::FormatError; }  //��λ������ֵΪerror
	int f = 0;
	for (auto gate = output.assignments().begin(); gate != output.assignments().end(); ++gate, ++f) {
		// check constraints.
		if ((*gate < 0) || (*gate >= input.airport().gates().size())) { error |= CheckerFlag::FlightNotAssignedError; }
		for (auto ig = input.flights(f).incompatiblegates().begin(); ig != input.flights(f).incompatiblegates().end(); ++ig) {
			if (*gate == *ig) { error |= CheckerFlag::IncompatibleAssignmentError; }
		}
		const auto &flight(input.flights(f));
		for (auto flight1 = input.flights().begin(); flight1 != input.flights().end(); ++flight1) {
			if (*gate != output.assignments(flight1->id())) { continue; }
			int gap = max(flight.turnaround().begin() - flight1->turnaround().end(),
				flight1->turnaround().begin() - flight.turnaround().begin());
			if (gap < input.airport().gates(*gate).mingap()) { error |= CheckerFlag::FlightOverlapError; }
		}

		// check objective.
		if (*gate < input.airport().bridgenum()) { ++flightNumOnBridge; }
	}

	// visualize solution.
	//double pixelPerMinute = 1;
	//double pixelPerGate = 30;
	//int horizonLen = 0;
	//for (auto flight = input.flights().begin(); flight != input.flights().end(); ++flight) {
	//    horizonLen = max(horizonLen, flight->turnaround().end());
	//}

	//auto pos = outputPath.find_last_of('/');
	//string outputName = (pos == string::npos) ? outputPath : outputPath.substr(pos + 1);
	//Drawer draw;
	//draw.begin("Visualization/" + outputName + ".html", horizonLen * pixelPerMinute, input.airport().gates().size() * pixelPerGate, 1, 0);
	//f = 0;
	//for (auto gate = output.assignments().begin(); gate != output.assignments().end(); ++gate, ++f) {
	//    // check constraints.
	//    if ((*gate < 0) || (*gate >= input.airport().gates().size())) { continue; }
	//    bool incompat = false;
	//    for (auto ig = input.flights(f).incompatiblegates().begin(); ig != input.flights(f).incompatiblegates().end(); ++ig) {
	//        if (*gate == *ig) { incompat = true; break; }
	//    }
	//    const auto &flight(input.flights(f));
	//    draw.rect(flight.turnaround().begin() * pixelPerMinute, *gate * pixelPerGate, 
	//        (flight.turnaround().end() - flight.turnaround().begin()) * pixelPerMinute, pixelPerGate,
	//        false, to_string(f), "000000", incompat ? "00c00080" : "4080ff80");
	//}
	//draw.end();

	return (error == 0) ? flightNumOnBridge : ~error;
}
