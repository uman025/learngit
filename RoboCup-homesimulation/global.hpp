//这个文件使用来保存一些全局的东西
#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_

#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>
#include <set>

#include "cserver/plug.hpp"

using namespace std;
using namespace _home;

/*
 * 基本的按位操作技术,后面会经常用到。
 *
 * 1. 打开位:
 * 	object = object | bits
 *
 * 2. 切换位:
 *  object = object ^ bits;
 *
 * 3. 关闭位:
 * 	object = object & ~bits;
 *
 * 4. 测试位:
 *  if (object & bits == bits)
 *
 */

typedef unsigned long long int ulonglong; 		//要求是64位的类型，防止不同机器出现不同情况
typedef unsigned int uint;

//雷政 增加物体的优先级，对于一些物体所在位置还有其他的物体，将这些物体的优先级设置高一点
enum OPRIORITY                  //枚举类型，用来说明物体的处理优先级
{
	OPRI_NULL,
	ONEO,                             //所在位置只有一个要处理的物体
	TWOO,                            //所在位置有两个要处理的物体
	THREEO,                         //所在位置有三个或者更多要处理的物体
	LOCCONFIRM,		//要处理的物体的位置是确定的，inside信息可能不知道
	CONFIRM,                      //要处理的物体的位置和inside都是是确定的，比如sense到的对象和takeout操作的对象
	VIP                                  //特别要求需要先处理的
};

enum Sort			//枚举类型，用来说明物体的种类
{
	SORT_NULL,		//没有种类
	HUMAN,
	PLANT,
	COUCH,
	CHAIR,
	SOFA,
	BED,
	TABLE,
	WORKSPACE,
	WORKTABLE,
	TEAPOY,
	DESK,
	TELEVISION,
	AIRCONDITIONER,
	WASHMACHINE,
	CLOSET,
	CUPBOARD,
	REFRIGERATOR,
	MICROWAVE,
	BOOK,
	CAN,
	REMOTECONTROL,
	BOTTLE,
	CUP
};

enum Color			//枚举颜色，用来说明物体的颜色
{
	COLOR_NULL,		//没有颜色
	WHITE,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	BLACK
};

enum Size			//枚举大小,用来说明物体的大小
{
	SIZE_NULL,		//无大小
	SMALL,
	BIG
};

enum Type			//枚举type-contianer
{
	TYPE_NULL,		//无这个额外说明的类型
	CONTAINER
};

enum Position 		//相对于其他物体的位置
{
	POSITION_NULL, 	//无相对位置
	ON,
	NEAR
};

enum Door 			//容器门的状态
{
	DOOR_NULL, 		//无门
	OPENED,
	CLOSED
};

enum Plate 		//物体在盘子里
{
	PLATE_NO, 	//默认不在盘子里
	PLATE_YES  //在盘子里
};

enum Hold 		//物体在机器人手上
{
	HOLD_NO, 	//默认不在手上
	HOLD_YES 	//在手上
};

enum DomainType 			//环境表和任务表中都含有domain结构体，所以会有不同的处理
{
	DOMAINTYPE_NULL, 		
	DOMAINTYPE_ENV, 		//环境表中读取的
	DOMAINTYPE_INS 			//任务表中
};

enum Insflag 		//指令类型
{
	INSFLAG_NULL,
	INFO,
	TASK,
	CONS_NOT_TASK,
	CONS_NOT_INFO,
	CONS_NOTNOT_INFO
};

//putdown事先处理好了,任务优先级没有实际意义
//takeout之后可能还有其他操作,所以相应的应该先分析
//其他的应该先分析操作对象有颜色的,以后不能再动了
enum Priority 	//指令的优先级
{
	PRI_LOW,
	PRI_COLOR,
	PRI_TAKEOUT,
	PRI_MEDIUM,
	PRI_HIGH
};

enum InsKeyword  			//对应关键字的枚举,为防止与其他枚举冲突加INS后缀
{
	INSKEYWORD_NULL,
	GIVE_INS, 					//give 	
	PUTON_INS, 					//puton
	GOTO_INS, 					//goto
	PUTDOWN_INS, 				//putdown
	PICKUP_INS, 				//pickup
	OPEN_INS, 					//open
	CLOSE_INS, 					//close
	PUTIN_INS, 					//putin
	TAKEOUT_INS, 				//takeout

	ON_INS, 					//on
	NEAR_INS, 					//near
	AT_INS,
	PLATE_INS, 					//plate
	INSIDE_INS, 				//inside
	OPENED_INS, 				//opened
	CLOSED_INS, 				//closed
 
	//NOT_TASK, 				//not task
	//NOT_INFO, 				//not info
	//NOT_NOT_INFO 			//not not info
};



//这样做是为了方便后面进行关键字比较
//与库中的名称有冲突的都在后面加x, 暂未使用命名空间
//
const string sortx = "sort"; 	//因为与库sort函数冲突，改名sortx	
const string human = "human";
const string plant = "plant";
const string couch = "couch";
const string chair = "chair";
const string sofa = "sofa";
const string bed = "bed";
const string table = "table";
const string workspace = "workspace";
const string worktable = "worktable";
const string teapoy = "teapoy";
const string desk = "desk";
const string television = "television";
const string airconditioner = "airconditioner";
const string washmachine = "washmachine";
const string closet = "closet";
const string cupboard = "cupboard";
const string refrigerator = "refrigerator";
const string microwave = "microwave";
const string book = "book";
const string can = "can";
const string remote = "remote";
const string remotecontrol = "remotecontrol";
const string bottle = "bottle";
const string cup = "cup";


const string size = "size";
const string big = "big";
const string small = "small";

const string color = "color";
const string white = "white";
const string red = "red";
const string green = "green";
const string yellow = "yellow";
const string blue = "blue";
const string black = "black";

const string at = "at";
const string type = "type";
const string container = "container";

const string inside = "inside";
const string opened = "opened";
const string closed = "closed";

const string near = "near";
const string on = "on";

const string plate = "plate";
const string hold = "hold";

//ins
//task 	9个
const string _task = ":task";

const string give = "give";
const string puton = "puton";
const string gotox = "goto";
const string putdown = "putdown";
const string pickup = "pickup";
const string open = "open";
const string closex = "close";
const string putin = "putin";
const string takeout = "takeout";

//info 
const string _info = ":info";


//cons 3个
const string _cons_not = ":cons_not";
const string _cons_notnot = ":cons_notnot";

const string info = "info";
const string task = "task";
const string not_task = "not task";
const string not_info = "not info";
const string notnot_info = "not not info";


//环境表结构体
struct domain { 		//从服务器获得的环境表元素基本结构
	string keyword; 	//关键字
	string id; 			//物体id
	string other; 		//其他信息，没有则在处理的时候直接不去处理
	
	friend std::ostream & operator<< (std::ostream &os, const domain &dom); 		//定义友元只为输出这个结构体
};





#endif		//the end of global.hpp
