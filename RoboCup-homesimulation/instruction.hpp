#ifndef INSTRUCTION_HPP_
#define INSTRUCTION_HPP_

#include "global.hpp"
#include "object.hpp"

/*
 * Basic data structure for instruction.
 *
 *
 * 示例:
 * (:ins (:info (on X Y) (:cond (sort X bottle) (color X white)
 * 				(sort Y table)))
 * 		 (:task (give human X) (:cond (sort X bottle) (color X red)))
 * 		 (:cons not not (:info (on X Y) (:cond (sort X bottle)
 * 		 					   (sort Y table)))))
 * 	info, task, cons 都可对应为机器人接受到的指令，所以创建一个指令类。
 *
 * 	-----------------
 * 	| keyword  | ---> 例如on,give等这样的关键字
 * 	-----------------
 * 	| object_x | ---> 第一个物体X，这个物体无论哪个动作都有的
 * 	-----------------
 * 	| mask_x   | ---> 保护一个物体某个部分的信息。保护列表中的物体与object_x中对应的部分。便于检测是否object_x与列表中的物体相等。
 * 	-----------------
 * 	| object_y | ---> 第二个物体Y，有的动作可能没有
 * 	-----------------
 * 	| mask_y   | ---> 同mask_x
 * 	-----------------
 * 	| ins_flag | ---> 指令的标志，info, task, cons
 * 	-----------------
 * 	| priority | ---> 设置任务的优先级。 info: PRI_HIGH; cons类: PRI_MEDIUM; task: PRI_LOW
 * 	-----------------
 *
 * 	X,Y用Object表示。这里的X,Y不一定指某一个具体的物体，
 * 	而是可能指某一类物体。
 *
 */

class Instruction 
{
	private:
		InsKeyword keyword; 	//使用枚举类型
		Object object_x;
		Object mask_x;
		Object object_y;
		Object mask_y;
		Insflag ins_flag; 		
		Priority priority;

	public:
		Instruction();
		~Instruction();

		InsKeyword  GetKeyword() const; 			//获得这个关键字
		void 		SetKeyword(const InsKeyword &x); 	//设置这个关键字

		Object 		GetObjectx() const; 			//获得物体x
		void 		SetObjectx(const Object &x); 	//设置物体x

		Object 		GetMaskx() const; 				//获得mask_x
		void 		SetMaskx(const Object &x); 		//设置mask_x

		Object 		GetObjecty() const; 			//获得物体y,不是每个指令都有
		void 		SetObjecty(const Object &x); 	//设置物体y

		Object 		GetMasky() const; 				//获得mask_y,不是每个指令都有
		void 		SetMasky(const Object &x); 		//设置mask_y

		Insflag 	        GetInsflag() const; 			//获得指令类型
		void 		SetInsflag(const Insflag &x); 		//设置指令类型

		Object 		GenerateMask(const Object &x); 		//接受一个物体返回一个mask。 物体： 0101 100 0000 	mask：1111 111 0000

		Priority 	        GetPriority() const; 			//获得指令的优先级
		void 		SetPriority(const Priority &x);  	//设置指令的优先级

		bool 		operator < (const Instruction &x) const; 	//重载操作符，对指令按降序排序
		void 		Clear(); 		//清空信息

		friend std::ostream & operator<< (std::ostream &os, const Instruction &ins); 	//输出对象
};

#endif 	//the end of instruction.hpp
