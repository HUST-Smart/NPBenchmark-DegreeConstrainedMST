#include<vector>
#include<iostream>

#include "CheckConstraints.h"

namespace xxf {
	
	void CheckConstraints::initSet() {
		for (int n = 0; n < inputins.graph().nodes().size(); n++)
		{
			Parent.push_back(-1); //初始化每个节点的父节点记为-1和每个节点的度为0
			Degree.push_back(0);
		}
	}

	int CheckConstraints::findParent(int node) {
		
		if (Parent[node] == -1)return node;
		return findParent(Parent[node]);
	}

	void CheckConstraints::UnionSet(int source, int target) {
		
		int sourceParent = findParent(source);
		int targetParent = findParent(target);
		Parent[sourceParent] = targetParent;
	
	}

	bool CheckConstraints::checkLoop() {
	
		bool flag = true;   //记录是否有环的标记
		initSet();
		for (auto edgeOnTree = outputsolve.edges().begin(); edgeOnTree != outputsolve.edges().end(); ++edgeOnTree)
		{
			Degree[edgeOnTree->source()]++;  //节点度加1
			Degree[edgeOnTree->target()]++;

			obj += edgeOnTree->length();

			int sp = findParent(edgeOnTree->source());   //判断是否有环
			int tp = findParent(edgeOnTree->target());
			if (sp == tp) {
				std::cout << "Exist loop." << std::endl;
				flag = false;
			}
			UnionSet(edgeOnTree->source(), edgeOnTree->target());
		}
		return flag;
	}

	int CheckConstraints::checkNodeDegree() {
	 
		bool nodecoverFlag = true;   //节点是否全部被覆盖标记
		bool degreeconsFlag = true;   //节点的度约束标记
		for (int i = 0; i < inputins.graph().nodes().size(); i++)
		{
			if (Degree[i] == 0) {
				std::cout << "The node "<<i <<" is not covered."<< std::endl;
				nodecoverFlag = false;
			}
			else if (Degree[i] > inputins.maxdegree())
			{
				std::cout << "The degree of node " << i << " dissatisfies constraint ." << std::endl;
				degreeconsFlag = false;
			}
			else;
		}
		if (nodecoverFlag) {
			if (degreeconsFlag)return 0;   //同时满足两个约束
			else return 1;   //满足节点覆盖约束，不满足度约束
		}
		else {
			if (degreeconsFlag)return 2;   //满足度约束，不满足节点覆盖约束
			else return 3;    //两个约束都不满足
		}
	}

	int CheckConstraints::GetObj() {
		return obj;
	}
}