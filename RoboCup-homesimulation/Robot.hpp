#ifndef ROBOT_HPP_
#define ROBOT_HPP_

#include "../src/sfsexp/sexp.h"
#include "cserver/plug.hpp"
#include "global.hpp"
#include "object.hpp"
#include "instruction.hpp"
#include "nature.hpp" 		//自然语言处理模块

namespace _home
{
	class Robot : public Plug
	{
	private:
		Object robot_plate, robot_hold;			//机器人盘子里的物体（为0表示盘子空的），手（0表示空的）
		uint robot_location; 				//机器人的位置
		uint object_num; 				//物体数量
		string sexp_buf; 				//存储从服务器接收的s-expression形式的环境表
		vector<Object> vector_object; 			//存物体, 0为机器人 
		vector<Instruction> vector_ins; 		//存指令

	public:
		Robot();
		~Robot();

		const Object & GetPlate() const;				//获得盘子里面的物体id，0表示空
		void SetPlate(const Object x);					//设置盘子里面的物体
		
		const Object & GetHold() const;					//获得手里面的物体id,0表示空
		void SetHold(const Object x);					//设置手里面的物体
		
		const uint & GetLocation() const;				//获得机器人的位置
		void SetLocation(const uint x);					//设置机器人的位置

		void ReadEnvDes(const string &x);				//读取环境表string，并将信息存储到sexp_buf中
		void ReadTaskDes(const string &x); 				//读取任务表string, 并将信息存储到sexp_buf中
		const string & GetSexp() const; 				//返回获得的s-expression


		vector<domain> AnalyseDomain(const sexp_t *sx, DomainType domain_type); 		//接受sexp_t类型的链表信息, (:domain; (:cond;后面的
		vector<Object> StoreObject(vector<domain> vector_tmp_domain, DomainType domain_type); 		//接受并处理一个vector<domain>，然后存到Object,id号正好作为它们的下标,注意0
		void DowithVectorDomain(vector<Object> &vector_tmp_object, vector<domain> vector_tmp_domain, DomainType domain_type); 		//处理vector_domain里面的每一项,用迭代器作为参数

		void AnalyseSexpEnv(const string &sexp); 		//分析接收到的环境s-expression,然后调用AnalyseDomain()以获得vector<domain>,并对其进行处理

		void AnalyseSexpIns(const string &sexp); 		//分析接受到的指令s-expression,

		bool AnalyseAskLoc(const string &x,vector<uint> Sense_loc); 			//处理AskLoc()询问的获得的信息，at(num, loc), inside(num, num'),和分析askloc结果的有效性，有效返回true

		void ClearAll(); 					//自始自终只会创建一个Robot对象，所以要清空前一个文件的内容
		
		std::string DeelAskLoc(uint objid);
		bool DeelSense2(vector<uint> &Sense_loc,vector<uint> &Sense_1, vector<Object> &bobj_V,uint bigObjId,uint deelId);
		bool DeelSense1(vector<uint> &Sense_loc, vector<Object> &bobj_V, uint bigObjId,uint deelId);
		
		void ObjectSort(vector<Object> &bobj_V);			//对物体进行处理优先级的排序

	protected:
		void Plan();

        void DreamWingN();


	};	//Plug
}//_home


#endif //the end of robot.hpp
