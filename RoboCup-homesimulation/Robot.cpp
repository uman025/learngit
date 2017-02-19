#include "Robot.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <boost/concept_check.hpp>

using namespace _home;
using namespace std;

Robot::Robot() : Plug("AHU_Home")		//队名
{
	object_num = 0;
	robot_location = 0;
	robot_plate.Clear();
	robot_hold.Clear();
}

Robot::~Robot()
{
}

const Object & Robot::GetPlate() const				//获得盘子里面的物体id，0表示空
{
	return robot_plate;
}

void Robot::SetPlate(const Object x)				//设置盘子里面的物体
{
	robot_plate = x;
}

const Object & Robot::GetHold() const				//获得手里面的物体id,0表示空
{
	return robot_hold;
}

void Robot::SetHold(const Object x)				//设置手里面的物体
{
	robot_hold = x;
}

const uint & Robot::GetLocation() const			//获得机器人的位置
{
	return robot_location;
}

void Robot::SetLocation(const uint x)			//设置机器人的位置
{
	robot_location = x;
}


void Robot::ReadEnvDes(const string &x)
{
	sexp_buf = const_cast<string &>(x); 		//将接收到的s-expression存到sexp_buf
}

void Robot::ReadTaskDes(const string &x)
{
	sexp_buf = const_cast<string &>(x); 		//讲接受到的s-expression存到sexp_buf
}

const string & Robot::GetSexp() const
{
	return sexp_buf;
}


vector<domain> Robot::AnalyseDomain(const sexp_t *sx, DomainType domain_type) 		//接受传递的是list
{
	/*
	 * (:domain (at 0 0) (sort 1 human) (hold 0))
	 */
	vector<domain> vector_tmp_domain;
	struct domain tmp_domain;
	sexp_t *head, *sx_tmp;
	uint chosen = 1;

	if (sx != NULL) {
		sx = sx->next;
	}

	while (sx != NULL) {
		sx_tmp = sx->list;
		while (sx_tmp != NULL) {
			switch (chosen) { 		//知道chosen的作用了
				case 1:
					tmp_domain.keyword = sx_tmp->val;
					sx_tmp = sx_tmp->next;
					chosen++;
					break;
				case 2:
					tmp_domain.id = sx_tmp->val;
					sx_tmp = sx_tmp->next;
					chosen++;
					break;
				case 3:
					//tmp_domain.id = sx_tmp->val; 	//找到错了。。。
					tmp_domain.other = sx_tmp->val;
					sx_tmp = sx_tmp->next;
					chosen++;
					break;
			}
		}
		chosen = 1;
		vector_tmp_domain.push_back(tmp_domain); 		//加入vector_tmp_domain
		sx = sx->next;
	}

	return vector_tmp_domain;
}

vector<Object> Robot::StoreObject(vector<domain> vector_tmp_domain, DomainType domain_type) 		
{
	/*
	 * 接受一个vector<domain>,然后存到Object
	 */
	vector<Object> vector_tmp_object;
	
	DowithVectorDomain(vector_tmp_object, vector_tmp_domain, domain_type);

	return vector_tmp_object;
}

void Robot::DowithVectorDomain(vector<Object> &vector_tmp_object, vector<domain> vector_tmp_domain, DomainType domain_type)
{
	/*
	 * 在STL中自己写循环并不是一个很好的想法。。。不过暂时先用着。。。
	 *
	 */
	Object tmp;
	string keyword;
	uint id;
	string other;
	vector<domain>::iterator pr;

	uint vec_index = 0; 	//跟踪容器id,记录的是上一个的id

	if (domain_type == DOMAINTYPE_ENV) {
		vector_tmp_object.push_back(tmp); 	//机器人默认编号就是0
		vec_index = 0; 
	}
	
	if (domain_type == DOMAINTYPE_INS) {
		vector_tmp_object.push_back(tmp); 		//肯定有一个物体
		for (pr = vector_tmp_domain.begin(); pr != vector_tmp_domain.end(); pr++) {
			//debug
			//cout << "---------------" << endl;
			//cout << pr->keyword << endl;
			//cout << pr->id << endl;
			//cout << pr->other << endl;
			//cout << "---------------" << endl;
			//debug

			if (pr->id == "Y") { 	//表示有两个物体 human?
				vector_tmp_object.push_back(tmp);
				break; 		//跳出，已经确定有两个物体了
			} /*else if (pr->id == "X") { 		//从后往前看，没遇到Y，那么就一个物体
				continue;
			}*/ 	//这句不用了
		}
	}
			
	
	//debug
	//for (pr = vector_tmp_domain.begin(); pr != vector_tmp_domain.end(); pr++)
	//{
	//	cout << pr->keyword << endl;
	//	cout << pr->id << endl;
	//	cout << pr->other << endl;
	//	cout << *pr << endl;
	//}
	//debug


	for (pr = vector_tmp_domain.begin(); pr != vector_tmp_domain.end(); pr++)
	{
		keyword = pr->keyword;
		if (domain_type == DOMAINTYPE_INS) { 	
			if (pr->id == "X") 		//是X用,magic number = 0, id与容器索引相关联
				id = 0;
			else if (pr->id == "Y") 	//Y, magic number = 1 
				id = 1;
		} else if (domain_type == DOMAINTYPE_ENV) {
			//vector_tmp_object.push_back(tmp); 		//vector_tmp_object[0]指带的是机器人
			id = uint(atoi((pr->id).c_str())); 		//domain的id
		}
		other = pr->other;

		//debug
		//cout << pr->keyword << endl;
		//cout << pr->id << endl;
		//cout << pr->other << endl;
		//debug

		if (domain_type == DOMAINTYPE_ENV) {
			if (vec_index < id) {
				for (uint j = 1; j < id - vec_index; j++) { 	//把索引跟id之间的补齐,相差1为正常
					++vec_index;
					tmp.SetId(vec_index);
					vector_tmp_object.push_back(tmp); 	
				}
			}
		}

		if (keyword == sortx) {
			if (domain_type == DOMAINTYPE_ENV) { 	//如果是来自指令的则已经分配过了
				vector_tmp_object.push_back(tmp); 	//如果是物体分配一个空间
				if (domain_type == DOMAINTYPE_ENV) 
					++vec_index;
			}
			if (other == human) {
				vector_tmp_object[id].SetId(id); 	//能知道id和sort，所以存起来
				vector_tmp_object[id].SetSort(HUMAN);
			} else if (other == plant) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(PLANT);
			} else if (other == couch) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(COUCH);
			} else if (other == chair) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(CHAIR);
			} else if (other == sofa) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(SOFA);
			} else if (other == bed) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(BED);
			} else if (other == table) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(TABLE);
			} else if (other == workspace) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(WORKSPACE);
			} else if (other == worktable) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(WORKTABLE);
			} else if (other == teapoy) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(TEAPOY);
			} else if (other == desk) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(DESK);
			} else if (other == television) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(TELEVISION);
			} else if (other == airconditioner) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(AIRCONDITIONER);
			} else if (other == washmachine) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(WASHMACHINE);
			} else if (other == closet) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(CLOSET);
			} else if (other == cupboard) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(CUPBOARD);
			} else if (other == refrigerator) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(REFRIGERATOR);
			} else if (other == microwave) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(MICROWAVE);
			} else if (other == book) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(BOOK);
			} else if (other == can) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(CAN);
			} else if (other == remotecontrol) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(REMOTECONTROL);
			} else if (other == bottle) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(BOTTLE);
			} else if (other == cup) {
				vector_tmp_object[id].SetId(id);
				vector_tmp_object[id].SetSort(CUP);
			}
		} else if (keyword == at && id == 0) { 		//机器人
			//vector_tmp_object.push_back(tmp); 	//直接在开始加一个机器人
			vector_tmp_object[id].SetLocation(uint(atoi(other.c_str())));
			SetLocation(vector_tmp_object[id].GetLocation());
		} else if (keyword == at && id != 0) {
			vector_tmp_object[id].SetLocation(uint(atoi(other.c_str())));
		} else if (keyword == size) { 		//大小
			if (other == big) {
				vector_tmp_object[id].SetSize(BIG);
			} else if (other == small) {
				vector_tmp_object[id].SetSize(SMALL);
			}
		} else if (keyword == color) { 		//颜色
			if (other == white) {
				vector_tmp_object[id].SetColor(WHITE);
			} else if (other == green) {
				vector_tmp_object[id].SetColor(GREEN);
			} else if (other == red) {
				vector_tmp_object[id].SetColor(RED);
			} else if (other == yellow) {
				vector_tmp_object[id].SetColor(YELLOW);
			} else if (other == blue) {
				vector_tmp_object[id].SetColor(BLUE);
			} else if (other == black) {
				vector_tmp_object[id].SetColor(BLACK);
			}
		} else if (keyword == type) { 		//容器类型
			if (other == container) {
				vector_tmp_object[id].SetType(CONTAINER); 		//表示是container
			}
		} else if (keyword == inside) { 		//在哪个物体的里面
			//大物体里面可能有多个小物体
			//不对大物体进行处理，只设置小物体在哪个容器里面
			uint tmpid = uint(atoi(other.c_str()));
			vector_tmp_object[id].SetInside(tmpid); 	//表示在哪个物体里面, A在B内，loc为B的id
			//大物体在前面已经设置好了
			vector_tmp_object[id].SetLocation(vector_tmp_object[tmpid].GetLocation());

		} else if (keyword == on) { 		//相对于其他物体的位置position
			vector_tmp_object[id].SetPosition(ON); 	//确定是on
			vector_tmp_object[id].SetPosition(vector_object[id].GetPosition() | uint(atoi(other.c_str()))); 	//先确定高4位，然后与获得的id或，从而填满position的信息
		} else if (keyword == near) { 		//相对于其他物体的位置position
			vector_tmp_object[id].SetPosition(NEAR); 	//确定是near
			vector_tmp_object[id].SetPosition(vector_object[id].GetPosition() | uint(atoi(other.c_str()))); 	//先确定高4位，然后与获得的id或，从而填满position的信息
		} else if (keyword == opened) { 		//门的状态
			vector_tmp_object[id].SetDoor(OPENED); 	//门是开的
		} else if (keyword == closed) { 		//门的状态
			vector_tmp_object[id].SetDoor(CLOSED); 	//门是关的
		} else if (keyword == plate && id != 0) { 		//机器人盘子里面是什么物体,id=0表示没有物体
			Object obj_tmp;
			obj_tmp.Clear();

			obj_tmp.SetId(id); 	//记录一下物体的id
			SetPlate(obj_tmp); 	//机器人盘子里有这个物体，在最后会根据id重新更新盘子里的物体

		} else if (keyword == hold && id != 0) {
			Object obj_tmp;
			obj_tmp.Clear();

			obj_tmp.SetId(id);
			SetHold(obj_tmp);
		}
			
			/*
			vector_tmp_object[id].SetPlate(PLATE_YES); 		//表示这个物体在机器人盘子里
			vector_tmp_object[id].SetLocation(GetLocation()); 		//将物体的位置设置成机器人的位置
			SetPlate(vector_tmp_object[id]); 			//机器人盘子里有这个物体
			//vector_tmp_object[0].SetPlate(id); 			//机器人是id=0,可以不看
		} else if (keyword == hold && id != 0) { 			//机器人手上是什么物体,id=0表示没有物体
			vector_tmp_object[id].SetHold(HOLD_YES); 		//这个物体在机器人手中
			vector_tmp_object[id].SetLocation(GetLocation()); 		//将物体的位置设置成机器人的位置
			SetHold(vector_tmp_object[id]); 				//机器人手中有这个物体
		}
		*/
	}
	//根据手上或者盘子里的物体id,重新更新手上或者盘子里的物体
	if (domain_type == DOMAINTYPE_ENV) {
		uint rholdid = GetHold().GetId();
		uint rplateid = GetPlate().GetId();
		uint rloc = GetLocation();
		if (rholdid != 0) {
			SetHold(vector_tmp_object[rholdid]);
			vector_tmp_object[rholdid].SetHold(HOLD_YES);
			vector_tmp_object[rholdid].SetLocation(rloc);
		}
		if (rplateid != 0) {
			SetPlate(vector_tmp_object[rplateid]);
			vector_tmp_object[rplateid].SetPlate(PLATE_YES);
			vector_tmp_object[rplateid].SetLocation(rloc);
		}
	}

}


void Robot::AnalyseSexpEnv(const string &sexp) 		//分析接受到的环境s-expression,然后调用AnalyseDomain() -> StoreObject() -> DowithVectorDomain()
{
	/* 
	 * src/sfsexp/sexp.h sexp_t  利用这个结构体，要弄清楚这个结构体的结构
	 * val, list, next 主要是这三个元素
	 * sexp_t * parse_sexp(char *s, size_t len); 	用这个函数去解析s-expression
	 * void destroy_sexp(sexp_t *s); 		用这个函数释放已经分配的空间
	 *
	 * 可以用gdb写个简单的程序去调试看看sexp_t里面的基本结构
	 */
	sexp_t *head, *tmp;
	head = parse_sexp(const_cast<char *>(sexp.c_str()), sexp.length());
	tmp = head;

	if (tmp->list != NULL) {
		tmp = tmp->list;
	}

	vector_object = StoreObject(AnalyseDomain(tmp, DOMAINTYPE_ENV), DOMAINTYPE_ENV);
	
	destroy_sexp(head); 		//别忘了释放内存!
}


void Robot::AnalyseSexpIns(const string &sexp) 		//分析接受到的指令的s-expression,然后调用AnalyseDomain() -> StoreObject() -> DowithVectorDomain()
{
	vector<Object> vector_tmp_object;
	Instruction tmp_ins;
	sexp_t *head, *tmp, *tmp_one, *tmp_one_list, *tmp_two, *tmp_two_list, *tmp_three, *tmp_end;
	string tmp_one_val, tmp_two_val;
	Object tmp_mask; 	//mask_x, mask_y
	
	head = parse_sexp(const_cast<char *>(sexp.c_str()), sexp.length());
	tmp = head;

	if (tmp->list != NULL) {
		tmp_one = tmp->list->next; 		//tmp->list->val is (:ins
	}

	while (tmp_one != NULL) {
		tmp_one_list = tmp_one->list;
		tmp_one_val = tmp_one->list->val; 		//(:info, (:task, (:cons_not, (:cons_notnot
		
		if (tmp_one_val == _task) { 		//设置指令的Insflag
			//tmp_ins.SetInsflag(TASK);
			//mp_ins.SetPriority(PRI_LOW); 	//设置指令的优先级
			 tmp_two = tmp_one->list->next;
		} else if (tmp_one_val == _info) {
			//tmp_ins.SetInsflag(INFO);
			//tmp_ins.SetPriority(PRI_HIGH);
			tmp_two = tmp_one->list->next;  
		} else if (tmp_one_val == _cons_not) {
			if (tmp_one_list->next->list->val == _task) {
				//tmp_ins.SetInsflag(CONS_NOT_TASK);
				//tmp_ins.SetPriority(PRI_MEDIUM);
				tmp_two = tmp_one->list->next->list->next; 
			} else if (tmp_one_list->next->list->val == _info) {
				//tmp_ins.SetInsflag(CONS_NOT_INFO);
				//tmp_ins.SetPriority(PRI_MEDIUM);
				tmp_two = tmp_one->list->next->list->next; 
			}
		} else if (tmp_one_val == _cons_notnot) {
			//tmp_ins.SetInsflag(CONS_NOTNOT_INFO);
			//tmp_ins.SetPriority(PRI_MEDIUM);
			tmp_two = tmp_one->list->next->list->next; 
		}
		
		tmp_two_list = tmp_two->list;
			tmp_two_val = tmp_two->list->val;  //on, give......
		
		if (tmp_one_val == _task ) { 		//设置指令的Insflag
			tmp_ins.SetInsflag(TASK);
			if(tmp_two_val==takeout)
			  tmp_ins.SetPriority(PRI_TAKEOUT);
			else 
			  tmp_ins.SetPriority(PRI_LOW); 
		}
		
		else if (tmp_one_val == _info) {
			tmp_ins.SetInsflag(INFO);
			tmp_ins.SetPriority(PRI_HIGH);
			
		}
		
		else if (tmp_one_val == _cons_not) {
			if (tmp_one_list->next->list->val == _task) {
				tmp_ins.SetInsflag(CONS_NOT_TASK);
				tmp_ins.SetPriority(PRI_MEDIUM);
				
			} else if (tmp_one_list->next->list->val == _info) {
				tmp_ins.SetInsflag(CONS_NOT_INFO);
				tmp_ins.SetPriority(PRI_MEDIUM);
				
			}
		} 
		
		else if (tmp_one_val == _cons_notnot) {
			tmp_ins.SetInsflag(CONS_NOTNOT_INFO);
			tmp_ins.SetPriority(PRI_MEDIUM);
			
		}


		/*
		 * 传给这个循环的应该都是on, give...
		 */
		while (tmp_two != NULL) {
			tmp_two_list = tmp_two->list;
			tmp_two_val = tmp_two->list->val;  //on, give......
			if (tmp_two_val == give) {
				tmp_ins.SetKeyword(GIVE_INS);

			} else if (tmp_two_val == puton) {
				tmp_ins.SetKeyword(PUTON_INS);

			} else if (tmp_two_val == gotox) {
				tmp_ins.SetKeyword(GOTO_INS);

			} else if (tmp_two_val == putdown) {
				tmp_ins.SetKeyword(PUTDOWN_INS);

			} else if (tmp_two_val == pickup) {
				tmp_ins.SetKeyword(PICKUP_INS);

			} else if (tmp_two_val == open) {
				tmp_ins.SetKeyword(OPEN_INS);

			} else if (tmp_two_val == closex) { 		//是closex
				tmp_ins.SetKeyword(CLOSE_INS);

			} else if (tmp_two_val == putin) {
				tmp_ins.SetKeyword(PUTIN_INS);

			} else if (tmp_two_val == takeout) {
				tmp_ins.SetKeyword(TAKEOUT_INS);
			
			} else if (tmp_two_val == at) {
				tmp_ins.SetKeyword(AT_INS);

			} else if (tmp_two_val == on) {
				tmp_ins.SetKeyword(ON_INS);

			} else if (tmp_two_val == near) {
				tmp_ins.SetKeyword(NEAR_INS);

			} else if (tmp_two_val == plate) {
				tmp_ins.SetKeyword(PLATE_INS);

			} else if (tmp_two_val == inside) {
				tmp_ins.SetKeyword(INSIDE_INS);

			} else if (tmp_two_val == opened) {
				tmp_ins.SetKeyword(OPENED_INS);

			} else if (tmp_two_val == closed) {
				tmp_ins.SetKeyword(CLOSED_INS);
			
			}
			tmp_three = tmp_two->next; 		
			tmp_end = tmp_three->list; 		//(:cond

			vector_tmp_object = StoreObject(AnalyseDomain(tmp_end, DOMAINTYPE_INS), DOMAINTYPE_INS);

			if (vector_tmp_object.size() == 2) { 		//如果是两个物体
				tmp_ins.SetObjectx(vector_tmp_object[0]);
				tmp_ins.GetObjectx().SetId(0); 		//id为0,不表示任何的具体物体

				tmp_mask = tmp_ins.GenerateMask(vector_tmp_object[0]); 		//生成对应的mask
				if(vector_tmp_object[0].GetColor() != 0){
					tmp_ins.SetPriority(PRI_COLOR);
				}
				tmp_ins.SetMaskx(tmp_mask);

				tmp_ins.SetObjecty(vector_tmp_object[1]);
				tmp_ins.GetObjecty().SetId(0); 		//id为0,不表示任何的具体物体

				tmp_mask = tmp_ins.GenerateMask(vector_tmp_object[1]);
				tmp_ins.SetMasky(tmp_mask);

			} else if (vector_tmp_object.size() == 1) { 	//如果是一个物体
				tmp_ins.SetObjectx(vector_tmp_object[0]);
				tmp_ins.GetObjectx().SetId(0); 		//id为0,不表示任何的具体物体

				tmp_mask = tmp_ins.GenerateMask(vector_tmp_object[0]); 		//生成对应的mask
				if(vector_tmp_object[0].GetColor() != 0){
					tmp_ins.SetPriority(PRI_COLOR);
				}
				tmp_ins.SetMaskx(tmp_mask);
				//debug
				//cout << "vector_tmp_object[0]: " << vector_tmp_object[0] << endl;
				//cout << "tmp_mask: " << tmp_mask << endl;
				//debug
				Object tmp_obj;
				tmp_ins.SetObjecty(tmp_obj); 	//用0去填充object_y
				tmp_mask.Clear();
				tmp_ins.SetMasky(tmp_mask);
			}
			tmp_two = tmp_three->next; 		//结束这个循环
		}

		tmp_one = tmp_one->next;
		vector_ins.push_back(tmp_ins); 		//加到存指令的容器里面
	}
	destroy_sexp(head); 		//别忘了释放内存!
}


bool Robot::AnalyseAskLoc(const string &x,vector<uint>Sense_loc)
{
	/*
	 * 获得位置信息有两种。
	 * (at num loc) (inside num num')
	 * 仍然可以利用struct domain
	 *
	 * 1.将x处理成struct domain。
	 * 2.从struct domain里提取信息。直接修改对应物体的位置信息。
	 */
	struct domain tmp_domain;
	char str[10];
	int i = 0;
	int count = 0;
	char *s = const_cast<char *>(x.c_str());
	char *p;
	p = s;
	//inside(*,*)
	while (*p != '(') {
		str[i] = *p;
		i++;
		p++;
	}
	str[i] = '\0';
	tmp_domain.keyword = str;

	i = 0;
	p++;
	while (*p != ',') {
		str[i] = *p;
		i++;
		p++;
	}
	str[i] = '\0';
	tmp_domain.id = str;

	i = 0;
	p++;
	while (*p != ')') {
		str[i] = *p;
		i++;
		p++;
	}
	str[i] = '\0';
	tmp_domain.other = str;
	uint id = uint(atoi((tmp_domain.id).c_str()));
	uint otherid = uint(atoi((tmp_domain.other).c_str()));
	vector<uint>::iterator it;
	for(it = Sense_loc.begin();it != Sense_loc.end();++it){
		if(*it == otherid){
			break;
		}
	}
	if(it == Sense_loc.end()){
		if (tmp_domain.keyword == at) {
			vector_object[id].SetLocation(otherid);
			vector_object[id].SetInside(0);
		} else if (tmp_domain.keyword == inside) {
			vector_object[id].SetInside(otherid);
			vector_object[id].SetLocation(vector_object[otherid].GetLocation()); 	//在物体里面，应该在同一个地方
		}
		return true;
	}
	else{
		return false;
	}
}


void Robot::ClearAll()
{
	//把上一个测试文件的信息清空
	robot_plate.Clear();
	robot_hold.Clear();
	robot_location = 0;
	object_num = 0;
	sexp_buf = "";
	vector_object.clear();
	vector_ins.clear();
}


void Robot::DreamWingN()
{
    cout << "#(Robot): DreamWingN" << endl;
}


//接受一个物体id
std::string Robot::DeelAskLoc(uint objid) 
{
	std::string str;
	while(1){
		str = AskLoc(objid);
		if(str == "not_known"){
			continue;
		}
		else{
			return str;
		}
	}
}

bool Robot::DeelSense2(vector< uint > &Sense_loc,vector<uint> &Sense_1,vector<Object>&bobj_V, uint bigObjID,uint deelId)
{
	bool isFind = true;
	vector<uint>Sense_2;
	vector<Object>::iterator pobj;
	vector<Object>::iterator it;
	vector<Object>::iterator ot;
	vector<uint>::iterator it1;
	vector<uint>::iterator it2;
	//只处理先关后开的情况，对于先开后关的情况不需要处理，因为不确实到底什么时候才会关上
	if(Sense_1.size() != 0){   //说明之前有sense过,需再sense一下
		//清理一些错误的inside信息
		for(it1 = Sense_1.begin();it1 != Sense_1.end();it1++){
			if(vector_object[*it1].GetInside() != 0){   //第一次看到的物体不可能在物体里面
				vector_object[*it1].SetInside(0);
			}
		}
		
		Sense(Sense_2);
		
		for(it1 = Sense_2.begin();it1 != Sense_2.end();++ it1){
			vector_object[*it1].SetPriority(LOCCONFIRM);
		}

		//先设置location
		for(it = vector_object.begin();it != vector_object.end();it++){
			if(it->GetLocation() ==GetLocation()){
				for(it2 = Sense_2.begin();it2 != Sense_2.end();it2++){
					if(it->GetId() == *it2){
						break;
					}
				}
				if(it2 == Sense_2.end()){  //*ot根本不在这里	
					it->SetLocation(0);
					it->SetInside(0);
				}
			}
		}
		
		for(it2 = Sense_2.begin();it2 != Sense_2.end();it2++){
			for(it = vector_object.begin(); it != vector_object.end(); it ++){
				if(it->GetId() == *it2){
					for(ot = bobj_V.begin();ot != bobj_V.end();ot++){
						if(ot->GetId() == *it2){
							ot->SetPriority(LOCCONFIRM);
							//cout << "2w: " << ot->GetId() << "----- " << " P : " << ot->GetPriority() << endl;
						}
					}
					it->SetLocation(GetLocation());
				}
			}
			for(it1 = Sense_1.begin();it1 != Sense_1.end();it1++){
				if(*it2== *it1){
					it2 = Sense_2.erase(it2);
					it2--;
				}
			}
		}
		
		for(it1 = Sense_2.begin();it1 != Sense_2.end();++ it1){
			vector_object[*it1].SetPriority(CONFIRM);
		}
		//再设置inside
		if(Sense_2.size() != 0){  //剩下的物体肯定是在容器里面的了
			for(pobj = vector_object.begin();pobj != vector_object.end();pobj++){
				if(pobj->GetInside() == GetLocation()){
					for(it2 = Sense_2.begin();it2 != Sense_2.end();it2++){
						if(pobj->GetId() == *it2)
							break;
					}
					if(it2 == Sense_2.end()){  //说明*pobj物体实际上不在这个容器里
						pobj->SetInside(0);
					}
				}
			}
			for(it2 = Sense_2.begin();it2 != Sense_2.end();it2++){
				if(vector_object[*it2].GetSize() == SMALL ){
					vector_object[*it2].SetInside(bigObjID);
				}
				for(it = bobj_V.begin();it != bobj_V.end();it++){
					if(it->GetId() == *it2){
						it->SetPriority(CONFIRM);
					}
				}
			}
		}
		
		for(it2 = Sense_2.begin();it2 != Sense_2.end();it2++){
			if(*it2 == deelId)
				break;
		}
		if(it2 == Sense_2.end())
			isFind = false;
		
		Sense_1.clear();   //清空容器
	}
	else{
		for(it1 = Sense_loc.begin();it1 != Sense_loc.end();it1++){
			if (*it1 == GetLocation())
				return true;
		}
		if(it1 == Sense_loc.end()){  //没有sense过
			  Sense_loc.push_back(GetLocation());
		}
		Sense(Sense_1);
	}
	return isFind;
}

bool Robot::DeelSense1(vector<uint> &Sense_loc,vector<Object> &bobj_V,uint bigObjID,uint deelId){
	vector<uint>::iterator it;
	vector <Object>::iterator ot;
	
	vector<uint> Sense_Obj;  //存放是sense到的物体
	vector<uint>::iterator Sense_Obj_it;
	
	for(it = Sense_loc.begin();it != Sense_loc.end();it++){
		if (*it ==GetLocation())
		return true;
	}
	
	if(it == Sense_loc.end() && vector_object[bigObjID].GetDoor() != CLOSED ){
	    Sense_loc.push_back(GetLocation());
	}
	
	Sense(Sense_Obj);
	for(it = Sense_Obj.begin();it != Sense_Obj.end();++it){
		vector_object[*it].SetPriority(LOCCONFIRM);
	}
	for(ot = vector_object.begin();ot != vector_object.end();ot++){
		if(ot->GetLocation() == GetLocation()){
			for(Sense_Obj_it = Sense_Obj.begin();Sense_Obj_it != Sense_Obj.end();++Sense_Obj_it){
				if(ot->GetId() == *Sense_Obj_it){
					break;
				}
			}
			if(Sense_Obj_it == Sense_Obj.end()){  //*ot根本不在这里	
				ot->SetLocation(0);
				ot->SetInside(0);
			}
		}
	}
	
	//sense到的物体的位置肯定是确定的，所以设置优先级为confirm 
	for(Sense_Obj_it = Sense_Obj.begin();  Sense_Obj_it != Sense_Obj.end();Sense_Obj_it++){
		vector_object[*Sense_Obj_it].SetLocation(GetLocation());	                  //设置物体位置信息
		if(vector_object[bigObjID].GetType() == CONTAINER && vector_object[bigObjID].GetDoor() == OPENED){       //如果是容器而且容器是开着的话，默认在容器里面
			if(vector_object[*Sense_Obj_it].GetType() != CONTAINER){
				vector_object[*Sense_Obj_it].SetInside(bigObjID);
			}
		}
		
		for(ot = bobj_V.begin();ot != bobj_V.end();++ot){
			if(ot->GetId() == *Sense_Obj_it ){   //如果是容器的话，物体可能在容易里面，所以不能算的上是确定的位置
				if( vector_object[bigObjID].GetType() != CONTAINER){
				//	cout<<ot->GetId()<<"1111\t"<<endl;
					ot->SetPriority(CONFIRM);
				//	cout<<"#:" << ot->GetId() << ": "<< ot->GetPriority() << endl;;
				}
				else{
				//	cout<<ot->GetId()<<"\t"<<endl;
					ot->SetPriority(LOCCONFIRM);
				//	cout<<"%:" << ot->GetId() << ": "<< ot->GetPriority() << endl;
				}
			}
		}
	}
	
	
	for(Sense_Obj_it = Sense_Obj.begin();Sense_Obj_it != Sense_Obj.end();Sense_Obj_it ++){
		if(*Sense_Obj_it == deelId)
			break;
	}
	if(Sense_Obj_it == Sense_Obj.end())   //没有找到该物体
		return false;
	else
		return true;
}

void Robot::ObjectSort(vector<Object> &bobj_V){
	vector<Object>::iterator it1,it2;
	for(it1 = bobj_V.begin(); it1 != bobj_V.end(); ++it1){
		if(it1->GetPriority()<LOCCONFIRM){
			uint pri = 0;
			for(it2 = bobj_V.begin(); it2 != bobj_V.end(); ++it2){
				if(vector_object[it1->GetId()].GetLocation() == vector_object[it2->GetId()].GetLocation()){
					pri ++;
				}
			}
			switch(pri){
			case 0:
				it1->SetPriority(OPRI_NULL);
				break;
			case 1:
				it1->SetPriority(ONEO);
				break;
			case 2:
				it1->SetPriority(TWOO);
				break;
			default:
				it1->SetPriority(THREEO);
				break;
			}
		}
	}
	sort(bobj_V.begin(),bobj_V.end());
}

//不是Robot类的友元，是结构体domain的友元
std::ostream & operator<< (std::ostream &os, const domain &dom)
{
	os << dom.keyword << endl;
	os << dom.id << endl;
	os << dom.other << endl;

	return os;
}





