#include "Solver.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>

#include <cmath>


using namespace std;


namespace xxf {

#pragma region Solver::Cli
	int Solver::Cli::run(int argc, char * argv[]) {
		Log(LogSwitch::Xxf::Cli) << "parse command line arguments." << endl;
		Set<String> switchSet;
		Map<String, char*> optionMap({ // use string as key to compare string contents instead of pointers.
			{ InstancePathOption(), nullptr },
			{ SolutionPathOption(), nullptr },
			{ RandSeedOption(), nullptr },
			{ TimeoutOption(), nullptr },
			{ MaxIterOption(), nullptr },
			{ JobNumOption(), nullptr },
			{ RunIdOption(), nullptr },
			{ EnvironmentPathOption(), nullptr },
			{ ConfigPathOption(), nullptr },
			{ LogPathOption(), nullptr }
			});

		for (int i = 1; i < argc; ++i) { // skip executable name.
			auto mapIter = optionMap.find(argv[i]);
			if (mapIter != optionMap.end()) { // option argument.
				mapIter->second = argv[++i];
			}
			else { // switch argument.
				switchSet.insert(argv[i]);
			}
		}

		Log(LogSwitch::Xxf::Cli) << "execute commands." << endl;
		if (switchSet.find(HelpSwitch()) != switchSet.end()) {
			cout << HelpInfo() << endl;
		}

		if (switchSet.find(AuthorNameSwitch()) != switchSet.end()) {
			cout << AuthorName() << endl;
		}

		Solver::Environment env;
		env.load(optionMap);
		if (env.instPath.empty() || env.slnPath.empty()) { return -1; }

		Solver::Configuration cfg;
		cfg.load(env.cfgPath);

		Log(LogSwitch::Xxf::Input) << "load instance " << env.instPath << " (seed=" << env.randSeed << ")." << endl;
		Problem::Input input;
		if (!input.load(env.instPath)) { return -1; }

		Solver solver(input, env, cfg);
		solver.solve();

		xxf::Submission submission;
		submission.set_thread(to_string(env.jobNum));
		submission.set_instance(env.friendlyInstName());
		submission.set_duration(to_string(solver.timer.elapsedSeconds()) + "s");

		solver.output.save(env.slnPath, submission);
#if SZX_DEBUG
		solver.output.save(env.solutionPathWithTime(), submission);
		solver.record();
#endif // SZX_DEBUG

		return 0;
	}
#pragma endregion Solver::Cli

#pragma region Solver::Environment
	void Solver::Environment::load(const Map<String, char*> &optionMap) {
		char *str;

		str = optionMap.at(Cli::EnvironmentPathOption());
		if (str != nullptr) { loadWithoutCalibrate(str); }

		str = optionMap.at(Cli::InstancePathOption());
		if (str != nullptr) { instPath = str; }

		str = optionMap.at(Cli::SolutionPathOption());
		if (str != nullptr) { slnPath = str; }

		str = optionMap.at(Cli::RandSeedOption());
		if (str != nullptr) { randSeed = atoi(str); }

		str = optionMap.at(Cli::TimeoutOption());
		if (str != nullptr) { msTimeout = static_cast<Duration>(atof(str) * Timer::MillisecondsPerSecond); }

		str = optionMap.at(Cli::MaxIterOption());
		if (str != nullptr) { maxIter = atoi(str); }

		str = optionMap.at(Cli::JobNumOption());
		if (str != nullptr) { jobNum = atoi(str); }

		str = optionMap.at(Cli::RunIdOption());
		if (str != nullptr) { rid = str; }

		str = optionMap.at(Cli::ConfigPathOption());
		if (str != nullptr) { cfgPath = str; }

		str = optionMap.at(Cli::LogPathOption());
		if (str != nullptr) { logPath = str; }

		calibrate();
	}

	void Solver::Environment::load(const String &filePath) {
		loadWithoutCalibrate(filePath);
		calibrate();
	}

	void Solver::Environment::loadWithoutCalibrate(const String &filePath) {
		// EXTEND[szx][8]: load environment from file.
		// EXTEND[szx][8]: check file existence first.
	}

	void Solver::Environment::save(const String &filePath) const {
		// EXTEND[szx][8]: save environment to file.
	}
	void Solver::Environment::calibrate() {
		// adjust thread number.
		int threadNum = thread::hardware_concurrency();
		if ((jobNum <= 0) || (jobNum > threadNum)) { jobNum = threadNum; }

		// adjust timeout.
		msTimeout -= Environment::SaveSolutionTimeInMillisecond;
	}
#pragma endregion Solver::Environment

#pragma region Solver::Configuration
	void Solver::Configuration::load(const String &filePath) {
		// EXTEND[szx][5]: load configuration from file.
		// EXTEND[szx][8]: check file existence first.
	}

	void Solver::Configuration::save(const String &filePath) const {
		// EXTEND[szx][5]: save configuration to file.
	}
#pragma endregion Solver::Configuration

#pragma region Solver
	bool Solver::solve() {
		//init();

		int workerNum = (max)(1, env.jobNum / cfg.threadNumPerWorker);
		cfg.threadNumPerWorker = env.jobNum / workerNum;
		List<Solution> solutions(workerNum, Solution(this));
		List<bool> success(workerNum);

		Log(LogSwitch::Xxf::Framework) << "launch " << workerNum << " workers." << endl;
		List<thread> threadList;
		threadList.reserve(workerNum);
		for (int i = 0; i < workerNum; ++i) {
			// TODO[szx][2]: as *this is captured by ref, the solver should support concurrency itself, i.e., data members should be read-only or independent for each worker.
			// OPTIMIZE[szx][3]: add a list to specify a series of algorithm to be used by each threads in sequence.
			threadList.emplace_back([&, i]() { success[i] = optimize(solutions[i], i); });
		}
		for (int i = 0; i < workerNum; ++i) { threadList.at(i).join(); }

		Log(LogSwitch::Xxf::Framework) << "collect best result among all workers." << endl;
		int bestIndex = -1;
		Length bestValue = 1000000;
		for (int i = 0; i < workerNum; ++i) {    //一个工作者一个算例多种求解方法的比较
			if (!success[i]) { continue; }
			Log(LogSwitch::Xxf::Framework) << "worker " << i << " got " << solutions[i].edgeLengthSumOnTree << endl;
			if (solutions[i].edgeLengthSumOnTree >= bestValue) { continue; }
			bestIndex = i;
			bestValue = solutions[i].edgeLengthSumOnTree;
		}

		env.rid = to_string(bestIndex);
		if (bestIndex < 0) { return false; }
		output = solutions[bestIndex];
		return true;
	}

	void Solver::record() const {
#if SZX_DEBUG
		int generation = 0;

		ostringstream log;

		System::MemoryUsage mu = System::peakMemoryUsage();

		Length obj = output.edgeLengthSumOnTree;
		Length checkerObj = -1;
		bool feasible = check(checkerObj);

		// record basic information.
		log << env.friendlyLocalTime() << ","
			<< env.rid << ","
			<< env.instPath << ","
			<< feasible << "," << (obj - checkerObj) << ","
			<< output.edgeLengthSumOnTree << ","
			<< timer.elapsedSeconds() << ","
			<< mu.physicalMemory << "," << mu.virtualMemory << ","
			<< env.randSeed << ","
			<< cfg.toBriefStr() << ","
			<< generation << "," << iteration;

		// record solution vector.
		// EXTEND[szx][2]: save solution in log.
		log << endl;

		// append all text atomically.
		static mutex logFileMutex;
		lock_guard<mutex> logFileGuard(logFileMutex);

		ofstream logFile(env.logPath, ios::app);
		logFile.seekp(0, ios::end);
		if (logFile.tellp() <= 0) {
			logFile << "Time,ID,Instance,Feasible,ObjMatch,Width,Duration,PhysMem,VirtMem,RandSeed,Config,Generation,Iteration,Solution" << endl;
		}
		logFile << log.str();
		logFile.close();
#endif // SZX_DEBUG
	}

	bool Solver::check(Length &checkerObj) const {
#if SZX_DEBUG
		enum CheckerFlag {
			IoError = 0x0,             //十六进制数，可以直接通过判断IoError|FormatError的数值来确定哪个出错
			FormatError = 0x1,
			NodeNotCorveredError = 0x2,
			LoopExistError = 0x4,
			DegreeOverError = 0x8
		};

		checkerObj = System::exec("Checker.exe " + env.instPath + " " + env.solutionPathWithTime());
		if (checkerObj > 0) { return true; }
		checkerObj = ~checkerObj;
		if (checkerObj == CheckerFlag::IoError) { Log(LogSwitch::Checker) << "IoError." << endl; }
		if (checkerObj & CheckerFlag::FormatError) { Log(LogSwitch::Checker) << "FormatError." << endl; }
		if (checkerObj & CheckerFlag::NodeNotCorveredError) { Log(LogSwitch::Checker) << "NodeNotCorveredError." << endl; }
		if (checkerObj & CheckerFlag::LoopExistError) { Log(LogSwitch::Checker) << "LoopExistError." << endl; }
		if (checkerObj & CheckerFlag::DegreeOverError) { Log(LogSwitch::Checker) << "DegreeOverError." << endl; }
		return false;
#else
		checkerObj = 0;
		return true;
#endif // SZX_DEBUG
	}

	void Solver::init() {
	}

	bool Solver::optimize(Solution &sln, ID workerId) {

		Log(LogSwitch::Xxf::Framework) << "worker " << workerId << " starts." << endl;

		ID nodeNum = input.graph().nodes().size();
		ID edgeNum = input.graph().edges().size();

		// reset solution state.
		bool status = true;
		sln.edgeLengthSumOnTree = 0;

		// TODO[0]: replace the following random assignment with your own algorithm.
		for (int e = 0; !timer.isTimeOut() && (e < nodeNum - 1); ++e) {
			int index = rand.pick(0, edgeNum);
			auto &edge(*sln.add_edges());
			edge.set_id(input.graph().edges(index).id());
			edge.set_source(input.graph().edges(index).source());
			edge.set_target(input.graph().edges(index).target());
			edge.set_length(input.graph().edges(index).length());
			sln.edgeLengthSumOnTree += input.graph().edges(index).length();   //record obj
		}


		Log(LogSwitch::Xxf::Framework) << "worker " << workerId << " ends." << endl;
		return status;
	}
#pragma endregion Solver

}

