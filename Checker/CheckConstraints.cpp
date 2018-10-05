#include<vector>
#include<iostream>

#include"CheckConstraints.h"

namespace xxf {
	
	CheckConstraints::CheckConstraints(const xxf::Output &output, const xxf::Input &input) {
		inputins = input;
		obj = 0;
		for (int n = 0; n < inputins.graph().nodes().size(); n++)
		{
			AdjList.push_back(std::vector<xxf::Edge>());    //邻接表添加一维向量
			Degree.push_back(0);                           //初始化每个节点的度为0
			visited.push_back(false);                     //初始化访问标记
		}
		for (auto edgeOnTree = output.edges().begin(); edgeOnTree != output.edges().end(); ++edgeOnTree)   //邻接表初始化,更新每个节点的度
		{
			Edge edge;
			edge.set_id(edgeOnTree->id());
			edge.set_source(edgeOnTree->source());
			edge.set_target(edgeOnTree->target());
			edge.set_length(edgeOnTree->length());
			Degree[edge.source()]++;
			Degree[edge.target()]++;
			AdjList[edge.source()].push_back(edge);
			AdjList[edge.target()].push_back(edge);
			obj += edge.length();
		}
	}

	bool CheckConstraints::isCyclicUtil(int node, int parent)
	{
		visited[node] = true;
		for (int i = 0; i < AdjList[node].size(); i++)
		{
			if (!visited[AdjList[node][i].target()])
			{
				if (isCyclicUtil(AdjList[node][i].target(), node))return true;
			}
			else if (AdjList[node][i].target() == parent)return true;
		}
		return false;
	}

	bool CheckConstraints::isCyclic()
	{
		for (int u = 0; u < inputins.graph().nodes().size(); u++)
			if (!visited[u])
				if (isCyclicUtil(u, -1))return true;
		std::cout << "Exist loop." << std::endl;
		return false;
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