#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <random>

#include <cstring>

#include "Simulator.h"
#include "ThreadPool.h"


using namespace std;


namespace szx {

	void Simulator::initDefaultEnvironment() {
		Solver::Environment env;
		env.save(Env::DefaultEnvPath());

		Solver::Configuration cfg;
		cfg.save(Env::DefaultCfgPath());
	}

	void Simulator::run(const Task &task) {
		String instanceName(task.instSet + task.instId + ".json");
		String solutionName(task.instSet + task.instId + ".json");

		char argBuf[Cmd::MaxArgNum][Cmd::MaxArgLen];
		char *argv[Cmd::MaxArgNum];
		for (int i = 0; i < Cmd::MaxArgNum; ++i) { argv[i] = argBuf[i]; }
		strcpy(argv[ArgIndex::ExeName], ProgramName().c_str());

		int argc = ArgIndex::ArgStart;

		strcpy(argv[argc++], Cmd::InstancePathOption().c_str());
		strcpy(argv[argc++], (InstanceDir() + instanceName).c_str());

		System::makeSureDirExist(SolutionDir());
		strcpy(argv[argc++], Cmd::SolutionPathOption().c_str());
		strcpy(argv[argc++], (SolutionDir() + solutionName).c_str());

		if (!task.randSeed.empty()) {
			strcpy(argv[argc++], Cmd::RandSeedOption().c_str());
			strcpy(argv[argc++], task.randSeed.c_str());
		}

		if (!task.timeout.empty()) {
			strcpy(argv[argc++], Cmd::TimeoutOption().c_str());
			strcpy(argv[argc++], task.timeout.c_str());
		}

		if (!task.maxIter.empty()) {
			strcpy(argv[argc++], Cmd::MaxIterOption().c_str());
			strcpy(argv[argc++], task.maxIter.c_str());
		}

		if (!task.jobNum.empty()) {
			strcpy(argv[argc++], Cmd::JobNumOption().c_str());
			strcpy(argv[argc++], task.jobNum.c_str());
		}

		if (!task.runId.empty()) {
			strcpy(argv[argc++], Cmd::RunIdOption().c_str());
			strcpy(argv[argc++], task.runId.c_str());
		}

		if (!task.cfgPath.empty()) {
			strcpy(argv[argc++], Cmd::ConfigPathOption().c_str());
			strcpy(argv[argc++], task.cfgPath.c_str());
		}

		if (!task.logPath.empty()) {
			strcpy(argv[argc++], Cmd::LogPathOption().c_str());
			strcpy(argv[argc++], task.logPath.c_str());
		}

		Cmd::run(argc, argv);
	}

	void Simulator::run(const String &envPath) {
		char argBuf[Cmd::MaxArgNum][Cmd::MaxArgLen];
		char *argv[Cmd::MaxArgNum];
		for (int i = 0; i < Cmd::MaxArgNum; ++i) { argv[i] = argBuf[i]; }
		strcpy(argv[ArgIndex::ExeName], ProgramName().c_str());

		int argc = ArgIndex::ArgStart;

		strcpy(argv[argc++], Cmd::EnvironmentPathOption().c_str());
		strcpy(argv[argc++], envPath.c_str());

		Cmd::run(argc, argv);
	}

	void Simulator::debug() {   //����һ������
		Task task;
		task.instSet = "";
		task.instId = "rand.v50e400dc3";
		task.randSeed = "1500972793";
		//task.randSeed = to_string(RandSeed::generate());
		task.timeout = "180";
		//task.maxIter = "1000000000";
		task.jobNum = "1";
		task.cfgPath = Env::DefaultCfgPath();
		task.logPath = Env::DefaultLogPath();
		task.runId = "0";

		run(task);
	}

	void Simulator::benchmark(int repeat) {   //����������
		Task task;
		task.instSet = "";
		//task.timeout = "180";
		//task.maxIter = "1000000000";
		task.timeout = "3600";
		//task.maxIter = "1000000000";
		task.jobNum = "1";
		task.cfgPath = Env::DefaultCfgPath();
		task.logPath = Env::DefaultLogPath();

		random_device rd;
		mt19937 rgen(rd());
		// EXTEND[szx][5]: read it from InstanceList.txt.
		vector<String> instList({ "rand.v50e400dc3", "rand.v200e5000dc2" ,"rand.v1000e20000dc4"});
		for (int i = 0; i < repeat; ++i) {
			//shuffle(instList.begin(), instList.end(), rgen);
			for (auto inst = instList.begin(); inst != instList.end(); ++inst) {
				task.instId = *inst;
				task.randSeed = to_string(Random::generateSeed());
				task.runId = to_string(i);
				run(task);
			}
		}
	}

	void Simulator::parallelBenchmark(int repeat) {   //���ж���������
		Task task;
		task.instSet = "";
		//task.timeout = "180";
		//task.maxIter = "1000000000";
		task.timeout = "3600";
		//task.maxIter = "1000000000";
		task.jobNum = "1";
		task.cfgPath = Env::DefaultCfgPath();
		task.logPath = Env::DefaultLogPath();

		ThreadPool<> tp(4);

		random_device rd;
		mt19937 rgen(rd());
		// EXTEND[szx][5]: read it from InstanceList.txt.
		vector<String> instList({ "rand.v50e400dc3", "rand.v200e5000dc2"  ,"rand.v1000e20000dc4" });
		for (int i = 0; i < repeat; ++i) {
			//shuffle(instList.begin(), instList.end(), rgen);
			for (auto inst = instList.begin(); inst != instList.end(); ++inst) {
				task.instId = *inst;
				task.randSeed = to_string(Random::generateSeed());
				task.runId = to_string(i);
				tp.push([=]() { run(task); });
				this_thread::sleep_for(1s);
			}
		}
	}

	void Simulator::generateInstance(const InstanceTrait &trait) {
		Random rand;

		//��������������ͼ�Ľڵ�����������ͼ�Ľڵ�����
		int nodeNum = 1000;
		int edgeNum = 20000;
		int degreeMax = 60;
		int degreeCons = rand.pick(trait.degreeCons.begin, trait.degreeCons.end);

		vector<int> nodeDegree(nodeNum, 0);     //nodeDegree�����¼�ڵ�Ķ�

		Problem::Input input;
		input.set_maxdegree(degreeCons);          //���������Ľڵ�Լ��ֵ
		for (int n = 0; n < nodeNum; ++n) {       //��������ͼ�Ľڵ��б�
			input.mutable_graph()->add_nodes(n);
		}

		vector<vector<int>>repeatEdgeFlag(nodeNum, vector<int>(nodeNum, 0));    //�ڽӾ������Ƿ����ظ���

		int edgeNumDyna = 0;                //��¼�ߵ�����
		for (int e = 0; e < nodeNum - 1; ++e) {   //ȷ������������ͼ����ͨͼ
			auto &edge(*input.mutable_graph()->add_edges());
			edge.set_id(edgeNumDyna++);
			edge.set_source(e);
			edge.set_target(e + 1);
			repeatEdgeFlag[e][e + 1] = 1;     //���ñߵĴ��ڱ��
			repeatEdgeFlag[e + 1][e] = 1;
			edge.set_length(rand.pick(trait.edgeLength.begin, trait.edgeLength.end));
			nodeDegree[e]++;						//���½ڵ�Ķ�
			nodeDegree[e + 1]++;
		}

		int resEdges = edgeNum - edgeNumDyna;  //����ͼ�бߵ����������Թ�����ͨͼ��������ֹ
		if (resEdges < 0) {
			assert(false);
		}

		vector<int> nodeFlag(nodeNum, -1);     //��¼�����ȸ���ֵ�Ľڵ��Ƿ��ۼӹ�
		int degreeMaxNodeNum = 0;              ////��¼�ڵ�Ķȴﵽ����ֵ�ĸ���
		for (int e = 0; e < resEdges; ++e) {
			if (degreeMaxNodeNum == nodeNum)break;
			auto &edge(*input.mutable_graph()->add_edges());
			edge.set_id(edgeNumDyna++);

			while (true)
			{
				int source = rand.pick(0, nodeNum);
				if (nodeDegree[source] == degreeMax)   //�˽ڵ�Ķȴﵽ�����ֵ
				{
					if (nodeFlag[source] == -1)
					{
						degreeMaxNodeNum++;
						nodeFlag[source] = 0;
					}
				}
				else
				{
					edge.set_source(source);
					break;
				}
			}

			while (true)
			{
				int target = rand.pick(0, nodeNum);
				if ((target == edge.source()) || (repeatEdgeFlag[edge.source()][target] == 1));   //��ֹ�γ��Ի���ֹ�����ظ���
				else
				{
					if (nodeDegree[target] == degreeMax)   //�˽ڵ�Ķȴﵽ�����ֵ
					{
						if (nodeFlag[target] == -1)
						{
							degreeMaxNodeNum++;
							nodeFlag[target] = 0;
						}
					}
					else
					{
						edge.set_target(target);
						break;
					}
				}

			}

			edge.set_length(rand.pick(trait.edgeLength.begin, trait.edgeLength.end));

			repeatEdgeFlag[edge.source()][edge.target()] = 1;
			repeatEdgeFlag[edge.target()][edge.source()] = 1;

			nodeDegree[edge.source()]++;						//���½ڵ�Ķ�
			nodeDegree[edge.target()]++;
		}

		for (int e = 0; e < nodeDegree.size(); ++e) {
			cout << nodeDegree[e] << endl;
		}

		ostringstream path;
		path << InstanceDir() << "rand.v" << input.graph().nodes().size()
			<< "e" << input.graph().edges().size()
			<< "dc" << input.maxdegree() << ".json";
		pb::save(path.str(), input);
	}

}
