////////////////////////////////
/// usage : 1.	data that identifies a guillotine cut problem and its solution.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SMART_XXF_DEGREECONSTRAINED_PROBLEM_H
#define SMART_XXF_DEGREECONSTRAINED_PROBLEM_H


#include "Config.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "Common.h"
#include "PbReader.h"
#include "DegreeConstrainedMST.pb.h"


namespace xxf {

	class Problem {
#pragma region Type
	public:
		struct Input : public xxf::Input {
			bool load(const String &path) { return pb::load(path, *this); }
		};

		struct Output : public xxf::Output {
			bool save(const String &path, xxf::Submission &submission) const {
				std::ofstream ofs(path);
				if (!ofs.is_open()) { return false; }

				// TODO[0]: fill the submission information.
				submission.set_author("xxf");
				submission.set_algorithm("rand");
				submission.set_cpu("AMD Athlon X4 640 Processor 3.00GHz");
				submission.set_ram("4G 1333MHz");
				submission.set_language("C++");
				submission.set_compiler("VS2017");
				submission.set_os("Windows 10");
				submission.set_problem("PipelinePlanning");

				ofs << pb::protobufToJson(submission, false) << std::endl << pb::protobufToJson(*this);
				return true;
			}

			Length edgeLengthSumOnTree = 0;
		};
#pragma endregion Type

#pragma region Constant
	public:
		enum {
			//图的边的最大值，算例的度约束最大值
			MaxEdgeLength = 50,
			MaxDegreeCons = 7,
		};
#pragma endregion Constant

#pragma region Constructor
	public:
#pragma endregion Constructor

#pragma region Method
	public:
#pragma endregion Method

#pragma region Field
	public:
#pragma endregion Field
	}; // Problem

}


#endif // SMART_SZX_GATE_ASSIGNMENT_PROBLEM_H