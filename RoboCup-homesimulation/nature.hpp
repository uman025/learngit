#ifndef NATURE_HPP_
#define NATURE_HPP_

#include "global.hpp"
#include "object.hpp"
#include "instruction.hpp"

/*
 * 当从服务器接受到s-expression表达式的时候，提供了一个sfsexp库去解析。
 * 那么现在接受到的是自然语言，所以写这个库去解析自然语言中的关键部分。
 * 
 * 为了让这个库与整个底层高度独立，能在这里定义声明的变量，都不会在global.h
 * 里面实现。
 *
 * 思路:
 * 先将一句话里面所有单词转换成链表，然后前向遍历链表一个一个处理。
 *
 * 当然还有很多地方不完善。 :-)
 *
 */

//关键字
//变量以n开头表示这个变量实在nature.hpp 中定义的
const string na = "a";
const string nthe = "the";
const string nand = "and";
const string nwhich = "which";
const string nis = "is";
const string nin = "in";
const string ninto = "into";
const string ninside = "inside";
const string non = "on";
const string nfrom = "from";
const string nto = "to";
const string nfor = "for";
const string nup = "up";
const string nof = "of";
const string nnear = "near";
const string nnext = "next";
const string ndown = "down";

const string nout = "out";

const string ncannot = "cannot";

const string neach = "each";
const string nthere = "there";
const string nnot = "not";
const string ndo = "do";

const string ndoor = "door";
const string nplate = "plate";

const string nmust = "must";

const string ngive = "give";
const string nput = "put";
const string ngo = "go";
const string npick = "pick";
const string npickup="pickup";
const string ntake = "take";
const string nopen = "open";
const string nclose = "close";

const string nopened = "opened";
const string nclosed = "closed";

const string nme = "me";


const string nremote = "remote";
const string nwash = "wash";

const string ndot = ".";

const string nhuman = "human";

enum HeadKeyword
{
	H_KEYWORD_NULL,
	//task
	H_GIVE,
	H_PUT,
	H_GO,
	H_PICK,
	H_TAKE,
	H_OPEN,
	H_CLOSE,

	//info
	H_THE,
	H_A,
	H_THERE,

	H_INFOCONS,

	H_INFO_ON,
	H_INFO_NEAR,
	H_INFO_ONPLATE,
	H_INFO_INSIDE,
	H_INFO_OPENED,
	H_INFO_CLOSED,

	//cons
	H_CONS_NOT_TASK,
	H_CONS_NOT_INFO,
	H_CONS_NOTNOT_INFO
	
};

enum Hasdomust
{
	HAS_NULL,
	HAS_DO,
	HAS_NOT, 	//补充信息
	HAS_MUST,
	HAS_DO_NOT,
	HAS_MUST_NOT
};

/*
 * 句子的类型。
 *
 * task
 *
 * info:
 * 	info on
 * 	info near
 * 	info onplate
 * 	info inside
 * 	info opened
 * 	info closed
 *
 * cons:
 * 	not task
 * 	not info
 * 	not not info
enum SenType
{
	S_NULL;
	S_TASK, 
	S_INFO_ON,
	S_INFO_NEAR,
	S_INFO_ONPLATE,
	S_INFO_INSIDE,
	S_INFO_OPENED,
	S_INFO_CLOSED,
	S_CONS_NOT_TASK,
	S_CONS_NOT_INFO
	S_CONS_NOTNOT_INFO
};
有问题
*/


struct nature
{
	string word;
	HeadKeyword hkey; 	//用于标识句首单词
	struct nature *next;
};

typedef struct nature Nature;

void VectorSentence(string &str, vector<string> &vsent); 		//讲接受的每个句子都存到vsent
Nature * NatureList(const string &str); 	//接受字符串产生单词链表,会把第一个字母大写变小写
void ShowNatureList(const Nature *head); 		//显示链表，直观的看到是否出错
void DeleteNatureList(Nature *head); 		//删除单词链表

void DowithSentence(string &str, vector<Instruction> &veci); 		//plan.cpp中只调用这个API
void ParseSentence(Nature *head, Instruction &ins); 			//解析句子。根据一个句子链表设置一条指令。

/* 
 * a/the can
 * a/the black can
 *
 */
Nature * DowithThe(Nature *head, Object &obj); 		//处理关键字the,返回指向当前的指针

/*
 * 暂时都是颜色
 * a/the can which is red
 */
Nature * DowithWhich(Nature *head, Object &obj); 	//就是对先行词的修饰。(简单版本)

uint IsKeyword(const string &str); 		//检测是否是关键字
uint IsSort(const string &str); 		//检测是否种类
uint IsColor(const string &str); 		//检测是否是颜色
uint IsType(const string &str); 		//检测类型，（container)

uint HasDoMust(const Nature *head); 		//接受一个句子链表，然后检测是否含有must,即可知道是约束


#endif 	//the end of nature.hpp
