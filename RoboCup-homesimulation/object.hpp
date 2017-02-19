#ifndef OBJECT_HPP_
#define OBJECT_HPP_

#include "global.hpp"

/*
 * Basic data structure for object!
 *
 * ulonglong 
 * 8bytes  64bit 	0~63位
 *
 *--------------------------------------------------------------------------------------------------------------------------------------------------------
 *| id | sort | color | size | type | location | inside | position | door | plate | hold | priority | reserved(保留位) |
 *--------------------------------------------------------------------------------------------------------------------------------------------------------
 *    8      6          4          2          2           6             8               12           2            2          2           3                     7                   |
 *--------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * 使用一个ulonglong变量存储所有的关于物体的信息，每个部分都对应着相应的信息。
 * 以后的相应操作都是对里面的相应位进行操作。这样可以变得更简单，更简洁，也使处理速度更快。
 */

class Object 		//物体类,描述物体
{
	private:
		ulonglong object_all; 		//这个变量含有了一个物体的全部信息，具体的格式见上面

	public:
		Object(); 		//构造函数
		~Object(); 		//析构函数

		const uint 		GetId() const; 				//获得物体的id, 63~56
		void 			SetId(const uint x);  		//设置物体的id

		const uint 		GetSort() const; 			//获得物体的种类, 55~50
		void 			SetSort(const uint x); 			//设置物体的种类

		const uint 		GetColor() const; 			//获得物体的颜色, 49~46
		void 			SetColor(const uint x); 			//设置物体的颜色

		const uint 		GetSize() const; 			//获得物体的大小, 45~44
		void 			SetSize(const uint x); 			//设置物体的大小

		const uint 		GetType() const; 			//获得物体的额外类型, 43~42
		void 			SetType(const uint x); 			//设置物体的额外类型

		const uint 		GetLocation() const; 		//获得物体的位置, 41~36
		void 			SetLocation(const uint x); 		//设置物体的位置

		const uint 		GetInside() const; 				//获得物体是否在别的物体内部, 35~28 如果物体是容器，则此表示容器里的物体
		void 			SetInside(const uint x); 		//设置物体是否在别的物体内部

		const uint 		GetPosition() const; 		     	//获得与其他物体的关系，27~24 on,near; 23~16 id
		void 			SetPosition(const uint x); 		//设置与其他物体的关系

		const uint 		GetDoor() const; 			       //获得门的状态, 15~14
		void 			SetDoor(const uint x); 			//设置门的状态

		const uint 		GetPlate() const; 			//获得物体是否在机器人盘子里, 13~12
		void 			SetPlate(const uint x); 			//设置物体是否在机器人盘子里

		const uint 		GetHold() const; 			//获得物体是否在机器人手中, 11~10
		void 			SetHold(const uint x); 			//设置物体是否在机器人手中
		
		const uint 		GetPriority() const; 			//获得该物体的处理优先级，9~7
		void 			SetPriority(const uint x); 		//设置该物体的处理优先级

		const ulonglong 	GetAll() const; 			//获得物体的全部信息
		void 			SetAll(const ulonglong x);
		void 			Clear(); 					//所有的位都置0
		
		bool 			operator < (const Object &x) const; 	//重载操作符，对要处理的物体按降序排序

		friend std::ostream & operator<< (std::ostream &os, const Object &obj); 	//这个类的友元用于直接输出这个类的对象
};

#endif 		//the end of object.hpp
