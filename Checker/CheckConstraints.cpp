#include<vector>
#include<iostream>

#include"CheckConstraints.h"

namespace xxf {
	
	CheckConstraints::CheckConstraints(const xxf::Output &output, const xxf::Input &input) {
		inputins = input;
		obj = 0;
		for (int n = 0; n < inputins.graph().nodes().size(); n++)
		{
			AdjList.push_back(std::vector<xxf::Edge>());    //�ڽӱ����һά����
			Degree.push_back(0);                           //��ʼ��ÿ���ڵ�Ķ�Ϊ0
			visited.push_back(false);                     //��ʼ�����ʱ��
		}
		for (auto edgeOnTree = output.edges().begin(); edgeOnTree != output.edges().end(); ++edgeOnTree)   //�ڽӱ��ʼ��,����ÿ���ڵ�Ķ�
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
	 
		bool nodecoverFlag = true;   //�ڵ��Ƿ�ȫ�������Ǳ��
		bool degreeconsFlag = true;   //�ڵ�Ķ�Լ�����
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
			if (degreeconsFlag)return 0;   //ͬʱ��������Լ��
			else return 1;   //����ڵ㸲��Լ�����������Լ��
		}
		else {
			if (degreeconsFlag)return 2;   //�����Լ����������ڵ㸲��Լ��
			else return 3;    //����Լ����������
		}
	}

	int CheckConstraints::GetObj() {
		return obj;
	}
}