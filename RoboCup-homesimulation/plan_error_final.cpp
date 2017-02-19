 #include "Robot.hpp"
#include <boost/concept_check.hpp>
using namespace _home;
using namespace std;
const uint SENSE = 2;   //当要处理的物体数量小于1,就没有必要再sense了
const uint MAXWRONGTIME = 2;

// 雷政 这部分是机器人执行任务的规划：
// 1.先获取所有已知信息，包括环境信息和任务表中提供的有用信息
// 2.根据任务表，确定好要处理物体的最终状态，将其放入容器B中
// 3.按照容器B执行任务，采用已知信息优先原则

void Robot::Plan() 		//规划,, 
{
	ClearAll();
	ReadEnvDes (GetEnvDes()); 		        	//读取环境表
	AnalyseSexpEnv	(GetSexp()); 			//处理环境表
	ReadTaskDes	(GetTaskDes()); 			//读取任务表

	if(	sexp_buf[0] == '(') { 		//指令语言
		AnalyseSexpIns(GetSexp());//处理任务表
	}
	else {
		vector<Instruction> vec_ins;
		string zzz=sexp_buf;
		DowithSentence(zzz, vec_ins);//自然语言处理
		vector_ins = vec_ins;   
	}
	sort(vector_ins.begin(), vector_ins.end()); 	//对任务按优先级进行排序

	InsKeyword ins_key;
	Insflag ins_flag;

	Object objzero;
	objzero.Clear();
	Object objx, objy;
	Object maskx, masky;
	
	Object tObj;
	Object pObj; 

	vector<Instruction>::iterator pins;
	vector<Object>::iterator pobj;
	
	vector < Object > notnot_object;    	//存放不能动的物体
	vector<Object> askWrong;      //存储获取到的错误信息
	vector<Object>::iterator ak_it;
	
	vector <Object> bobj_Vector;            	//存放物体的最终状态
	vector <Object>::iterator bobj_it;
	vector <Object>::iterator aobj_it;
	vector <Object>::iterator bobj_it2;
	vector <Object>::iterator bobj_it3;
	
	vector <Object> PickUp_Container;  	//有些容器已经把里面要处理的东西全部拿出来了,只需要PickUp就可以了
	vector <Object>::iterator pc_it;
	
	vector<uint> Close_V;
	vector<uint>::iterator c_it;
	
	vector <uint> Sensed_Loc;                 	//已经Sense过的位置,每个位置应该只要sense一次
	vector<uint> Sensed_again;
	Sensed_again.clear();
	
	vector <uint> PickUp_objid;   		//存储要执行PickUp操作的物体id
	
	uint finalLoc = 100;	//机器人的最终位置,100表示可以是任意位置
	uint deelId;
	uint objectId;
	uint notPutDownId = 0;      //不能放下来的物体ID
	bool ope_all;			//判断是不是对所有容器做相同的操作
	bool continueFlag;
	bool senseFlag;

	//要处理对象不明确的任务就不做了
	for(pins = vector_ins.begin(); pins != vector_ins.end();pins ++){
		if(pins->GetPriority() < PRI_COLOR && pins->GetKeyword() != GOTO_INS && pins->GetKeyword() != OPEN_INS && pins->GetKeyword() != CLOSE_INS){
			pins = vector_ins.erase(pins);
			-- pins;
		} 
	}
	
       //雷政 下面的for循环是为了找x,y物体,并确定x,y物体的初始状态和最终状态
	for (pins = vector_ins.begin(); pins != vector_ins.end(); pins++) {
		ins_key = pins->GetKeyword();
		ins_flag = pins->GetInsflag();
		objx = pins->GetObjectx();
		objy = pins->GetObjecty();
		maskx = pins->GetMaskx();
		masky = pins->GetMasky();

		//都要id置0，不然会有影响
		objx.SetId(0);
		maskx.SetId(0);
		objy.SetId(0);
		masky.SetId(0);

		//task是open,close可能是对所有容器的要求，所以此处不能确定是某一个具体的容器
		if ((ins_flag == TASK)&& ((ins_key == OPEN_INS) || (ins_key == CLOSE_INS))) {
			if ((objx.GetAll() & 0xff03f3ffffffffff) == 0) { //是对全部这个种类的容器的操作 sort, type
				ope_all = true;
			}
		} 
		else {  //不是对所有的容器进行相同的操作
			ope_all = false;
		}
		//先找到环境表中具体对应的物体,如果只有一个X物体，应该基本可以确定具体是哪一个物体
		//如果有X，Y两个物体，应该基本可以确定Y具体是哪一个物体，而无法确定X，必须要在Y的基础上确定X是哪一个物体
		vector<Object>::iterator nn_obj;
		if ((objx.GetAll() != 0) && (objy.GetAll() == 0) && !ope_all) {       //只有X
			for (pobj = vector_object.begin(); pobj != vector_object.end();pobj++) {
				for(nn_obj = notnot_object.begin();nn_obj != notnot_object.end(); nn_obj++){
					if(pobj->GetId() == nn_obj->GetId())
						break;
				}
				if(nn_obj == notnot_object.end()){
					if((pobj->GetAll() & maskx.GetAll()) == objx.GetAll()){
						objx = *pobj;
						break;
					}
				}
			}
		} 
		else if ((objx.GetAll() != 0) && (objy.GetAll() != 0) && !ope_all) {                    //有X,Y,只能确定Y
			for (pobj = vector_object.begin(); pobj != vector_object.end();pobj++) { 	//先找到X
				for(nn_obj = notnot_object.begin();nn_obj != notnot_object.end();nn_obj++){
					if(pobj->GetId() == nn_obj->GetId())
						break;
				}
				if(nn_obj == notnot_object.end()){
					if ((pobj->GetAll() & maskx.GetAll()) == objx.GetAll()){
						objx = *pobj;
					}else{
						//do nothing
					}
				}
			}

			for (pobj = vector_object.begin(); pobj != vector_object.end();pobj++) {
				if ((pobj->GetAll() & masky.GetAll()) == objy.GetAll()) {
					objy = *pobj;
				}
			}
		} 
		else if (!ope_all) {  //未知情况
			//do nothing
		}
		
		
		//不同的ins对应不同的优先级,INFO优先级较高,所以应该先处理这个,获取所有有用的信息,
		//要注意的是,第二阶段,这些信息有可能时错误的,暂时不管他,先获取到,后面在处理
		if (ins_flag == INFO) {
			if (ins_key == NEAR_INS) {
				//near(objx, objy)
				uint objxid = objx.GetId();
				uint objyid = objy.GetId();
				uint objxloc = objx.GetLocation();
				uint objyloc = objy.GetLocation();

				if (objxloc == 0) { 			//objx位置不知道
					if (objyloc == 0) { 		//objy位置不知道
					} else if (objyloc != 0) { //objy位置知道
						vector_object[objxid].SetLocation(objyloc);
					}
				} else if (objxloc != 0) { 		//objx位置知道
					if (objyloc == 0) { 		//objy位置不知道
						vector_object[objyid].SetLocation(objxloc);
					} else if (objyloc != 0) { 		//objy位置知道
						if (objxloc == objyloc) { 		//两个位置一样正好
						} else if (objxloc != objyloc) { 	//两个位置不样
						}
					}
				}
			}
			
			else if (ins_key == PLATE_INS) {
				//onplate(objx)
				if (objx.GetSize() != SMALL) { 	//是否是小物体
				}
				//应该可以设置状态成功
				Object obj_tmp;
				obj_tmp.Clear();

				obj_tmp.SetId(objx.GetId()); 	//记录一下物体的id
				SetPlate(obj_tmp); 
				SetPlate(vector_object[objx.GetId()]);
				vector_object[objx.GetId()].SetPlate(PLATE_YES);
			}

			else if (ins_key == INSIDE_INS) {
				//inside(objx, objy)
				uint objxid = objx.GetId();
				uint objyid = objy.GetId();
				uint objxloc = objx.GetLocation();
				uint objyloc = objy.GetLocation();

				if (objxloc == 0) { 			//objx位置不知道
					if (objyloc == 0) { 		//objy位置不知道
					} else if (objyloc != 0) { //objy位置知道
						if (objx.GetInside() == 0) { 	//inside信息
							vector_object[objxid].SetInside(objyid);
						}
						vector_object[objxid].SetLocation(objyloc);
					}
				} else if (objxloc != 0) { 		//objx位置知道
					if (objyloc == 0) { 		//objy位置不知道
						if (objy.GetType() != CONTAINER) { 		//是否是容器
						}
						vector_object[objyid].SetLocation(objxloc);
						if (objx.GetInside() == 0) { 		//inside信息
							vector_object[objxid].SetInside(objyid);
						}
					} else if (objyloc != 0) { 		//objy位置知道
						if (objxloc == objyloc) { 		//两个位置一样正好
						} else if (objxloc != objyloc) { 	//两个位置不样
						}
					}
				}
			}

			else if (ins_key == OPENED_INS) {
				if (objx.GetType() != CONTAINER) { 		//是否是容器
				}
				else
				 vector_object[objx.GetId()].SetDoor(OPENED);
			}

			else if (ins_key == CLOSED_INS) {
				if (objx.GetType() != CONTAINER) { 		//是否是容器
				}
				else
				   vector_object[objx.GetId()].SetDoor(CLOSED);
			}
		}

		//这里其实没有实际意义,只不过在前面的确定x,y物体的时候用到了,目的是处理相同物体不同属性的选择
		else if (ins_flag == CONS_NOTNOT_INFO) {
			notnot_object.push_back(objx);			
		}
		else if (ins_flag == CONS_NOT_TASK) {
			if(ins_key == PUTDOWN_INS){
				if(notPutDownId == 0 &&(objx.GetPlate() == PLATE_YES || objx.GetHold() == HOLD_YES)){
					notPutDownId = objx.GetId();
				}
				else{
					if(objx.GetPlate() == PLATE_YES){
						notPutDownId == objx.GetId();
					}
				}
			}
			notnot_object.push_back(objx);			
		}
		//上面的操作完成后,所有能够获取的物体信息已经全部知保存了,下面的操作根据要处理的动作,确定物体的最终位置
		//物体状态可能改变的信息只有三个:1.门 2.位置 3.是不是在容器里面
		else if (ins_flag == TASK){
			//如果是PickUp ,就把该物体的id保存到PickUp_objid容器中,最后再处理
			if (ins_key == OPEN_INS) {
				if ((objx.GetId() == 0) && ope_all) { 	//操作所有同类的物体
					Object objx2 = objx;
					for (pobj = vector_object.begin();pobj != vector_object.end(); pobj++) {
						if ((pobj->GetAll() & maskx.GetAll())== objx2.GetAll()) {
							objx = *pobj;
						}
					}
				}
				objx.SetDoor(OPENED);
				objx.SetInside(0);
				objx.SetLocation(objx.GetLocation());
				bobj_Vector.push_back(objx);
				notnot_object.push_back(objx);
			}
			else if (ins_key == CLOSE_INS) {
				if ((objx.GetId() == 0) && ope_all) { 	//操作所有同类的物体
					Object objx2 = objx;
					for (pobj = vector_object.begin();pobj != vector_object.end(); pobj++) {
						if ((pobj->GetAll() & maskx.GetAll())== objx2.GetAll()) {
							objx = *pobj;
						}
					}
				}
				objx.SetDoor(CLOSED);
				objx.SetInside(0);
				objx.SetLocation(objx.GetLocation());
				Close_V.push_back(objx.GetId());
				//bobj_Vector.push_back(objx);
				notnot_object.push_back(objx);
			}
			
			if(objx.GetId() == 0){   //没有i找到该物体
				continue;
			}
			
			if(ins_key == PICKUP_INS){
				if(vector_object[objx.GetId()].GetSize() == SMALL){
					PickUp_objid.push_back(objx.GetId());
					notnot_object.push_back(objx);
				}
			}			
			//如果友Goto操作,及保存一下物体的最终位置,最后再处理他,否则之为100,任何位置都可以
			else if(ins_key == GOTO_INS){
				finalLoc = objx.GetLocation();
			}
			
			else if (ins_key == GIVE_INS) {
				for(pobj = vector_object.begin();pobj != vector_object.end();pobj++){
					if(pobj->GetSort() == HUMAN){
						objy = *pobj;
					}
				}
				for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it ++){   //已经处理过了,按照优先级,将最终状态覆盖就可以了
					if(bobj_it->GetId() == objx.GetId()){
						bobj_it->SetDoor(DOOR_NULL);
						bobj_it->SetInside(0);
						bobj_it->SetLocation(objy.GetLocation());
						notnot_object.push_back(objx);
						break;
					} 
				}
				if(bobj_it == bobj_Vector.end()&&vector_object[objx.GetId()].GetSize() == SMALL){     //没有处理过,push到bobj_Vector中
					objx.SetDoor(DOOR_NULL);
					objx.SetInside(0);
					objx.SetLocation(objy.GetLocation());
					notnot_object.push_back(objx);  		//应该是物体的最终状态了
					bobj_Vector.push_back(objx);
				}
			}
			else if (ins_key == PUTON_INS) {
				for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it ++){   //已经处理过了,按照优先级,将最终状态覆盖就可以了
					if(bobj_it->GetId() == objx.GetId()){
						bobj_it->SetDoor(DOOR_NULL);
						bobj_it->SetInside(0);
						bobj_it->SetLocation(objy.GetLocation());
						notnot_object.push_back(objx);
						break;
					} 
				}
				if(bobj_it == bobj_Vector.end()&&vector_object[objx.GetId()].GetSize() == SMALL){     //没有处理过,push到bobj_Vector中
					objx.SetDoor(DOOR_NULL);
					objx.SetInside(0);
					objx.SetLocation(objy.GetLocation());
					notnot_object.push_back(objx);  		//应该是物体的最终状态了
					bobj_Vector.push_back(objx);
				}
			}
			else if (ins_key == PUTDOWN_INS){
				if(GetHold().GetId() == objx.GetId()){
					deelId = objx.GetId();
					PutDown(deelId);
					SetHold(objzero);
					vector_object[deelId].SetHold(HOLD_NO);
					vector_object[deelId].SetLocation(GetLocation());
				}
				if(GetPlate().GetId() == objx.GetId()){
					uint p = objx.GetId();
					if(GetHold().GetId() != 0){
						deelId = GetHold().GetId();
						PutDown(deelId);
						FromPlate(p);
						PutDown(p);
						if(GetLocation() == 0){
							//cout<<"pinkup1"<<endl;
							PickUp(deelId);
							SetPlate(objzero);
							vector_object[p].SetPlate(PLATE_NO);
							vector_object[p].SetLocation(GetLocation());
						}else{
							SetPlate(objzero);
							SetHold(objzero);
							vector_object[p].SetPlate(PLATE_NO);
							vector_object[p].SetLocation(GetLocation());
							vector_object[deelId].SetHold(HOLD_NO);
							vector_object[deelId].SetLocation(GetLocation());
						}
					}else{
						FromPlate(p);
						PutDown(p);
						SetPlate(objzero);
						vector_object[p].SetHold(HOLD_NO);
						vector_object[p].SetLocation(GetLocation());
					}
				}
				notnot_object.push_back(objx);
			}
			else if (ins_key == PUTIN_INS) {
				for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it ++){   //已经处理过了,按照优先级,将最终状态覆盖就可以了
					if(bobj_it->GetId() == objx.GetId()){
						bobj_it->SetDoor(DOOR_NULL);
						bobj_it->SetInside(objy.GetId());
						bobj_it->SetLocation(objy.GetLocation());
						notnot_object.push_back(objx);
						break;
					} 
				}
				if(bobj_it == bobj_Vector.end()&&vector_object[objx.GetId()].GetSize() == SMALL){     //没有处理过,push到bobj_Vector中
					objx.SetDoor(DOOR_NULL);
					objx.SetInside(objy.GetId());
					objx.SetLocation(objy.GetLocation());
					notnot_object.push_back(objx);  		//应该是物体的最终状态了
					bobj_Vector.push_back(objx);
				}
			}
			else if (ins_key == TAKEOUT_INS){
				deelId = objx.GetId();
				vector_object[deelId].SetInside(objy.GetId());
				vector_object[deelId].SetLocation(objy.GetLocation());
				objx.SetDoor(0);
				objx.SetInside(0);
				objx.SetLocation(objy.GetLocation());
				objx.SetPriority(CONFIRM);
				bobj_Vector.push_back(objx);
			}
			else{//未知情况
				//do nothing
			}
		}
	}
	
	//显示要处理物体的初始信息和最终信息
	/* cout<<"a::"<<endl;
	 for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it++){
		 if(vector_object[bobj_it->GetId()].GetId() == bobj_it->GetId()){
			  cout<<"ID:"<<vector_object[bobj_it->GetId()].GetId()<<"\tLOC:"<<vector_object[bobj_it->GetId()].GetLocation()<<"\tIN:";
			  cout<<vector_object[bobj_it->GetId()].GetInside()<<"\tDOR:"<<vector_object[bobj_it->GetId()].GetDoor()<<"\tPRI:";
			  cout<<vector_object[bobj_it->GetId()].GetPriority()<<endl;
		}
	}
	cout<<"b::"<<endl;
	for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it++){
		 cout<<"ID:"<<bobj_it->GetId()<<"\tLOC:"<<bobj_it->GetLocation()<<"\tIN:"<<bobj_it->GetInside();
		 cout<<"\tDOR:"<<bobj_it->GetDoor()<<"\tPRI:"<<bobj_it->GetPriority()<<endl;
	}
	*/
	//雷政 先看手和盘子里的物体要不要处理
	if(PickUp_Container.size() == 2){  //后面有两个物体要捡起来，所以只要手上和盘子里的物体是不需要处理的都可以放下来
		if(GetHold().GetId() != 0){
			for(aobj_it = bobj_Vector.begin();aobj_it != bobj_Vector.end();aobj_it++){
				if(aobj_it->GetId() == GetHold().GetId())
					break;
			}
			if(aobj_it == bobj_Vector.end()){   //手上的物体是不需要处理的，可以直接放下
				PutDown(GetHold().GetId());
				vector_object[GetHold().GetId()].SetLocation(GetLocation());
				vector_object[GetHold().GetId()].SetHold(HOLD_NO);
				SetHold(objzero);
			}
		}
		if(GetPlate().GetId() != 0){
			for(aobj_it = bobj_Vector.end();aobj_it != bobj_Vector.end();aobj_it ++){
				if(aobj_it ->GetId() == GetPlate().GetId())
					break;
			}
			if(aobj_it == bobj_Vector.end()){   //盘子里的物体是不需要处理的，可以直接放下
				FromPlate(GetPlate().GetId());
				PutDown(GetPlate().GetId());
				vector_object[GetPlate().GetId()].SetLocation(GetLocation());
				vector_object[GetPlate().GetId()].SetPlate(PLATE_NO);
				SetPlate(objzero);
			}
		}
	}
	else{    //有一个或者没有物体要捡起来则盘子里可以留下一个物体      
		if(notPutDownId != 0){   //存在不能放下的物体，则盘子里应该留下不能放下的物体
			if(GetPlate().GetId() == notPutDownId){   //直接看手上的物体能不能放下
				if(GetHold().GetId() != 0){
					for(aobj_it = bobj_Vector.begin();aobj_it != bobj_Vector.end();aobj_it ++){
						if(aobj_it ->GetId() == GetHold().GetId())
							break;
					}
					if(aobj_it == bobj_Vector.end()){  //手上的物体能放下
						PutDown(GetHold().GetId());
						vector_object[GetHold().GetId()].SetLocation(GetLocation());
						vector_object[GetHold().GetId()].SetHold(HOLD_NO);
						SetHold(objzero);
					}
				}
			}
			else if(GetHold().GetId() == notPutDownId){
				if(GetPlate().GetId() != 0){  //盘子里有物体
					PutDown(notPutDownId);
					SetHold(objzero);
					SetPlate(vector_object[notPutDownId]);
					vector_object[notPutDownId].SetHold(HOLD_NO);
					vector_object[notPutDownId].SetLocation(GetLocation());
				}
				else{  //盘子里没有物体
					ToPlate(notPutDownId);
					SetHold(objzero);
					SetPlate(vector_object[notPutDownId]);
					vector_object[notPutDownId].SetHold(HOLD_NO);
					vector_object[notPutDownId].SetPlate(PLATE_YES);
				}
			}
		}
		else{    //随便留下一个
			if(GetHold().GetId() != 0){
				PutDown(GetHold().GetId());
				vector_object[GetHold().GetId()].SetLocation(GetLocation());
				vector_object[GetHold().GetId()].SetHold(HOLD_NO);
				SetHold(objzero);
			}
		}
	}
	
	
	
	uint DO = 0;
	//雷政 下面的while是对单个物体进行处理,将初始状态变成最终状态,这里面可以修改策略,例如合肥师范的A*算法,我们这里使用的最近原则
	while(bobj_Vector.size()>0 && DO < 40){ //以bobj_Vector为空作为循环结束的条件，因为b中物体数量可能比a中物体的数量少，而且erase方便
		//先处理手中的物体,该物体来源可能有两个,本来就在手中或者为了完成任务特意捡起来的
		//以下处理为了完成任务特意捡起来的情况,处理后捡起来尽可能多的要处理的物体:
		
		/*如果手中有物体
		 * 1.该物体是要处理的
		 *     1.1该物体的目标位置就在当前位置 
		 * 	1.2该物体的目标位置不在当前位置
		 * 2.该物体是不需要处理的,放下
  		*/
		
		//处理一些明显错误的信息
		for(aobj_it = bobj_Vector.begin();aobj_it != bobj_Vector.end();aobj_it ++){
			if(vector_object[aobj_it->GetId()].GetInside() != 0 && vector_object[vector_object[aobj_it->GetId()].GetInside()].GetType() != CONTAINER){  //位置信息明显时错误的
				vector_object[aobj_it->GetId()].SetLocation(0);
				vector_object[aobj_it->GetId()].SetInside(0);
			}
			if(aobj_it->GetSize() == BIG && aobj_it->GetLocation() != vector_object[aobj_it->GetId()].GetLocation()){		//任务不和逻辑,大物体的位置变了
				aobj_it = bobj_Vector.erase(aobj_it);
				aobj_it--;
			}
		}
		
		
		//对物体处理优先级进行排序
		ObjectSort(bobj_Vector);
		
		continueFlag = false;   //标识位,协助后面程序执行判断是不是要continue
  		if(GetHold().GetId() != 0){  //手中有物体
			uint holdId = GetHold().GetId();
			uint plateId = GetPlate().GetId();
			uint robotLoc = GetLocation();
			uint deelObjId = 0;      //全局变量,存储操作对象的id
			
			//手上物体是需要处理的,机器人此时位置就是目标位置
			for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it ++){
				if(holdId == bobj_it->GetId() && robotLoc == bobj_it->GetLocation()&&robotLoc != 0){  //b容器中找到了该物体,且该物体的最终位置就是当前位置
					vector<Object>::iterator bigObj;
					for(bigObj = vector_object.begin();bigObj !=vector_object.end();bigObj++){
						if(bigObj->GetSize() == BIG && bigObj->GetLocation() == GetLocation())
							break;   //找到了大物体
					}		
					if(bobj_it->GetInside() == 0){      //不是PutIn操作
					/*	if(bigObj->GetType() != CONTAINER&&bobj_Vector.size()>SENSE){   //不是容器,可以Sense一下             //这里sense不知道有没有意义，先注释掉看一看
							cout<<"sense1"<<endl;
							senseFlag = DeelSense1(Sensed_Loc,bobj_Vector,bigObj->GetId(),0);
						}else if(bigObj->GetType() == CONTAINER && bigObj->GetDoor() == OPENED&&bobj_Vector.size()>SENSE){
							cout<<"sense2"<<endl;
							senseFlag = DeelSense1(Sensed_Loc,bobj_Vector, bigObj->GetId(),0);
						}	*/
						PutDown(holdId);
						DO++;
						SetHold(objzero);
						bobj_it = bobj_Vector.erase(bobj_it);   //处理完成
						continueFlag = true;
						//break;
					}
					else if(bobj_it->GetInside() != 0){  	                //是PutIn操作	
						if(bigObj->GetDoor() != OPENED){    	//容器是关着的,或者不知道是开是关,都执行一下open
							if(bigObj->GetDoor() == CLOSED){
								PutDown(holdId);
								DO++;
							/*	if(bobj_Vector.size()>SENSE){            //这里sense不知道有没有意义，先注释掉看一看
									cout<<"sense3"<<endl;
									senseFlag = DeelSense2(Sensed_Loc,Sensed_again,bobj_Vector,bigObj->GetId(),0);
								}  */
								Open(bobj_it->GetInside());       //robotLoc == GetLocation == objy.GetLocation == objy.GetId;
								vector_object[bobj_it->GetInside()].SetDoor(OPENED);
								DO++;
							/*	if(bobj_Vector.size()>SENSE){
									cout<<"sense4"<<endl;
									senseFlag = DeelSense2(Sensed_Loc,Sensed_again,bobj_Vector,bigObj->GetId(),0);
								} */
								//cout<<"pickup2"<<endl;
								PickUp(holdId);
								DO++;
							}else{
								PutDown(holdId);
								DO++;
								Open(bobj_it->GetInside());       //robotLoc == GetLocation == objy.GetLocation == objy.GetId;
								DO ++;
								vector_object[bobj_it->GetInside()].SetDoor(OPENED);
								//cout<<"pickup3"<<endl;
								PickUp(holdId);
								DO++;
							}
						}
						DO++;
						if(PutIn(holdId,bobj_it->GetInside())){
							SetHold(objzero);
							bobj_it = bobj_Vector.erase(bobj_it);  //处理完成
							continueFlag = true;	
						}
						// break;
					}
					
					//手上的物体操作完毕，如果盘子里有物体应当顺便处理盘子里的物体
					if(plateId != 0){
						for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it ++){
							if(bobj_it->GetId() == plateId && bobj_it->GetLocation() == robotLoc){//盘子里的物体是要处理的物体而且它的最终位置就是当前位置
								FromPlate(plateId);
								DO++;
								SetPlate(objzero);
								if(bobj_it->GetInside() == 0){     			 //不是PutIn操作
									PutDown(plateId);
									DO++;
									bobj_it = bobj_Vector.erase(bobj_it);   //处理完成
									break;
								}
								else if(bobj_it->GetInside() != 0){  	                //是PutIn操作	
									PutIn(holdId,bobj_it->GetInside());
									DO++;
									bobj_it = bobj_Vector.erase(bobj_it);  //处理完成
									break;
								}
							}
						}
					}
				
					//如果当前位置的物体是容器,且该容器最终是关着的,就把里面所有还需要处理的东西先拿出来再关上
					/*for(bobj_it3 = bobj_Vector.begin();bobj_it3 != bobj_Vector.end();bobj_it3++){
						if(bobj_it3->GetLocation() == robotLoc&& bobj_it3->GetType() == CONTAINER&&bobj_it3->GetDoor() == CLOSED){   //找到了这个容器
							for(aobj_it = bobj_Vector.begin();aobj_it != bobj_Vector.end();aobj_it ++){
								if(aobj_it->GetInside() == bobj_it3->GetId()){
									break;
								}
							}
							if(bobj_it3->GetDoor() == CLOSED && aobj_it == bobj_Vector.end()&&vector_object[bobj_it3->GetId()].GetDoor() == OPENED){    //最后应该是关着的,而且之后没有物体要放进去
								for(bobj_it2 = bobj_Vector.begin();bobj_it2 != bobj_Vector.end();bobj_it2++){
									deelId = bobj_it2->GetId();
									if(vector_object[deelId].GetLocation() == robotLoc && vector_object[deelId].GetInside() == robotLoc && vector_object[deelId].GetSize() == SMALL){   //有要处理的物体在容器里面
										DO++;
										if(TakeOut(deelId,vector_object[deelId].GetInside())){
											PutDown(deelId);
											DO++;
											vector_object[deelId].SetInside(0);
										}
										else{  //拿不出来，虽然可能在容器外面，但是直接当做不在这里
											askWrong.push_back(vector_object[deelId]);
											vector_object[deelId].SetLocation(0);
											vector_object[deelId].SetInside(0);
										}
									}
								}
								Close(bobj_it3->GetId());
								DO++;
								vector_object[bobj_it3->GetId()].SetDoor(CLOSED);
								bobj_it3 = bobj_Vector.erase(bobj_it3);  
								break;;
							}
							else{//该容器是不用进行操作的
								bobj_it3 = bobj_Vector.erase(bobj_it3);
								bobj_it3--;
							}
							break;
						}
					}*/
					//如果当前位置有物体是需要处理的,就捡起来,因为执行到这里,至少机器人手是空的,如果有物体要处理,最多可以带走两个物体
					uint objFlag = 0;
					for(bobj_it3 = bobj_Vector.begin();bobj_it3 != bobj_Vector.end();bobj_it3++){
						if(vector_object[bobj_it3->GetId()].GetLocation() == robotLoc && bobj_it3->GetSize() == SMALL){
							deelObjId = bobj_it3->GetId();
							if(GetHold().GetId() == 0){   //手是空的
								if(vector_object[deelObjId].GetInside() == bigObj->GetId()){  //在容器里，直接takeout
									
									//保证门是开着的
									if(vector_object[vector_object[deelObjId].GetInside()].GetDoor() != OPENED){    //只要不是打开的,就open一下,如果说是opened,应该不会错
										if(vector_object[vector_object[deelObjId].GetInside()].GetDoor() == CLOSED){
											if(bobj_Vector.size()>SENSE){
												senseFlag = false;
												//cout<<"sense4"<<endl;
												senseFlag =DeelSense2 (Sensed_Loc,Sensed_again,bobj_Vector,vector_object[deelObjId].GetInside(),deelObjId);
											}
											Open(vector_object[deelObjId].GetInside());
											vector_object[vector_object[deelObjId].GetInside()].SetDoor(OPENED);
											DO++;
											if(bobj_Vector.size()>SENSE){
												senseFlag = false;
												//cout<<"sense5"<<endl;
												senseFlag = DeelSense2(Sensed_Loc,Sensed_again,bobj_Vector,vector_object[deelObjId].GetInside(),deelObjId);
												if(senseFlag == false){
													continueFlag = true;
													break;
												}
											}
										}
										else{
											Open(vector_object[deelObjId].GetInside()); DO++;
											vector_object[bobj_Vector[deelObjId].GetInside()].SetDoor(OPENED);
										}
									}
									
									if(objFlag == 1 && GetPlate().GetId() == 0){    //手上拿过一个物体且盘子里是空的
										ToPlate(deelObjId);
										DO++;
										SetHold(objzero);
										SetPlate(vector_object[deelObjId]);
										vector_object[deelObjId].SetHold(HOLD_NO);
										vector_object[deelObjId].SetPlate(PLATE_YES);
									}else if (objFlag == 1 && GetPlate().GetId() != 0){   //手上拿过一个物体且盘子里有东西，不能在拿了
										break;
									}
									DO++;
								//	cout<<"takeout1"<<endl;
									if(TakeOut(deelObjId,bigObj->GetId())){ 
										objFlag++;
										continueFlag ==true;    		//手上有物体了,应该循环一次了       
										SetHold(vector_object[deelObjId]);
										vector_object[deelObjId].SetHold(HOLD_YES);
										vector_object[deelObjId].SetInside(0);
									}else{    //物体信息不知道了
										if(bobj_it3->GetPriority() == CONFIRM){     //物体的位置是确定的，但是还是做错了，就不处理该物体了
											bobj_it3 = bobj_Vector.erase(bobj_it3);
											bobj_it3--;
											break;
										}
										else if(bobj_it3->GetPriority() == LOCCONFIRM){      //位置是确定的，那么肯定不在容器里
											vector_object[deelObjId].SetInside(0);
											bobj_it3--;          //退回去，试一试PickUp
										}
										else if(bobj_it3->GetPriority() < LOCCONFIRM){
											askWrong.push_back(vector_object[deelObjId]);
											vector_object[deelObjId].SetLocation(0);
											vector_object[deelObjId].SetInside(0);
										}
									}
								}
								else if(vector_object[deelObjId].GetInside() == 0){   //不在容器里，直接pickup
									senseFlag = false;
									senseFlag = DeelSense1(Sensed_Loc,bobj_Vector,bigObj->GetId(),deelObjId);
									if(senseFlag == false){   //没有找到该物体
										continue;
									}
									if(objFlag == 1 && GetPlate().GetId() == 0){    //手上拿过一个物体且盘子里是空的
										ToPlate(deelObjId);
										DO++;
										SetHold(objzero);
										SetPlate(vector_object[deelObjId]);
										vector_object[deelObjId].SetHold(HOLD_NO);
										vector_object[deelObjId].SetPlate(PLATE_YES);
									}
									else if (objFlag == 1 && GetPlate().GetId() != 0){   //手上拿过一个物体且盘子里有东西，不能在拿了
										break;
									}
									DO++;
									//cout<<"pickup4"<<endl;
									if(PickUp(deelObjId)){     
										objFlag ++;
										continueFlag ==true;    //手上有物体了,应该循环一次了       
										SetHold(vector_object[deelObjId]);
										vector_object[deelObjId].SetHold(HOLD_YES);
									}else{    //物体信息不知道了
										if(bobj_it3->GetPriority() == CONFIRM){     //物体的位置是确定的，但是还是做错了，就不处理该物体了
											bobj_it3 = bobj_Vector.erase(bobj_it3);
											bobj_it3--;
											break;
										}
										else if(bobj_it3->GetPriority() == LOCCONFIRM){      //位置是确定的，那么肯定不在容器里
											vector_object[deelObjId].SetInside(0);
											bobj_it3--;          //退回去，试一试PickUp
										}else if(bobj_it3->GetPriority() < LOCCONFIRM){
											askWrong.push_back(vector_object[deelObjId]);
											vector_object[deelObjId].SetLocation(0);
											vector_object[deelObjId].SetInside(0);
										}
									}
								}
							}
						}
					}
					break;  //找到了一个物体就应该结束循环
				}
			}
			if(continueFlag == true){
				continue;
			}
			
			//更新一下机器人信息
			holdId = GetHold().GetId();
			plateId = GetPlate().GetId();
			robotLoc = GetLocation();
			deelObjId = 0;      //全局变量,存储操作对象的id
			//手上物体是需要处理的,但是机器人此时位置不在目标位置
			for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it++){
				 if(holdId == bobj_it->GetId() && bobj_it->GetLocation() != robotLoc){
					uint goalLoc = bobj_it->GetLocation(); //物体的目标位置
					if(goalLoc != 0){
						Move(goalLoc);      //移动到目标位置
						DO++;
						SetLocation(goalLoc);
						//更新一下机器人身上的物体信息
						holdId = GetHold().GetId();
						plateId = GetPlate().GetId();
						robotLoc = GetLocation();
						if (holdId) {
							vector_object[holdId].SetLocation(robotLoc);
							SetHold (vector_object[holdId]);
						}
						if (plateId) {
							vector_object[plateId].SetLocation(robotLoc);
							SetPlate (vector_object[plateId]);
						}
						continueFlag = true;
					}
					else{
						bobj_it = bobj_Vector.erase(bobj_it);
						if(bobj_it != bobj_Vector.begin()){
							bobj_it--;
						}
						continueFlag = true;
					}
				}
			}
			if(continueFlag == true)
				continue;                   //跳转到循环初始程序执行
			

			holdId = GetHold().GetId();
			plateId = GetPlate().GetId();
			robotLoc = GetLocation();
			deelObjId = 0;      //全局变量,存储操作对象的id
			//手上物体是不需要处理的,就把他放下来,这里为防止出错,重写一个迭代器遍历
			for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it++){
				if(holdId == bobj_it->GetId())
					break;
			}
			if(bobj_it == bobj_Vector.end()){ //没有找到该物体
				PutDown(holdId); 
				DO++;
				SetHold(objzero);
				vector_object[holdId].SetLocation(GetLocation());
				vector_object[holdId].SetHold(HOLD_NO);
			}
		}
	
		/*如果盘子中有物体,执行到这里,手中应该是没有物体了
		 * 1.该物体是要处理的
		 *     FromPlate,continue
		 * 2.该物体是不需要处理的,放下
  		*/
		if(GetPlate().GetId() != 0){  //盘子里有物体
			uint plateId = GetPlate().GetId();
			for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it++){
				if(plateId == bobj_it->GetId()){  //该物体是要处理的
					FromPlate(plateId);
					DO++;
					SetHold(vector_object[plateId]);
					SetPlate(objzero);
					vector_object[plateId].SetHold(HOLD_YES);
					vector_object[plateId].SetPlate(PLATE_NO);
					continueFlag = true;
				}
			}
			if(continueFlag == true)
				continue;
			
			//之前已经处理过了，所以这部分程序不需要了
			/*if(bobj_it == bobj_Vector.end()){   //bobj_Vector中没有找到该物体,说明该物体是不需要进行处理的
				FromPlate(plateId);
				DO++;
				PutDown(plateId);
				DO++;
				SetPlate(objzero);
				vector_object[plateId].SetLocation(GetLocation());
				vector_object[plateId].SetPlate(PLATE_NO);
			}*/
		}

		//此时手上和盘子里应该都没有物体了，如果当前位置是容器，处理该容器
		for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it++){
			if(bobj_it->GetLocation() ==GetLocation()&&bobj_it->GetType() == CONTAINER){   //当前位置的容器是要处理的
				uint objId = bobj_it->GetId();
				uint rloc = GetLocation();
				if(bobj_it->GetDoor() == OPENED){
					if(vector_object[objId].GetDoor() == CLOSED){   //当前状态是关着的，应该把他打开
						if(bobj_Vector.size()>SENSE){
							senseFlag = false;
							//cout<<"sense6"<<endl;
							senseFlag = DeelSense2(Sensed_Loc,Sensed_again,bobj_Vector,objId,1);
						}
						Open(objId);
						vector_object[objId].SetDoor(OPENED);
						DO++;
						if(bobj_Vector.size()>SENSE){
							senseFlag = false;
					//		cout<<"2"<<endl;
							senseFlag = DeelSense2(Sensed_Loc,Sensed_again,bobj_Vector,objId,1);
						}
						bobj_it = bobj_Vector.erase(bobj_it);           //删除该物体
						continueFlag = true;
						break;
					}
					else if(vector_object[objId].GetDoor() == OPENED){
						vector_object[objId].SetDoor(OPENED);
						bobj_it = bobj_Vector.erase(bobj_it);           //删除该物体
						continueFlag = true;
						break;
					}
					else{
						Open(objId);
						DO++;
						vector_object[objId].SetDoor(OPENED);
						bobj_it = bobj_Vector.erase(bobj_it);           //删除该物体
						continueFlag = true;
						break;
					}
				}
				else if (bobj_it->GetDoor() == CLOSED){
					for(aobj_it = bobj_Vector.begin();aobj_it != bobj_Vector.end();aobj_it ++){
						if(aobj_it->GetInside() == bobj_it->GetId()){
							break;
						}
					}
					if(vector_object[objId].GetDoor() == OPENED&&aobj_it == bobj_Vector.end()){
						if(bobj_Vector.size()>SENSE){
							senseFlag = false;
							//cout<<"sense7"<<endl;
							senseFlag = DeelSense1(Sensed_Loc,bobj_Vector,bobj_it->GetId(),0);
						}
						for(bobj_it2 = bobj_Vector.begin();bobj_it2 != bobj_Vector.end();bobj_it2++){
							deelId = bobj_it2->GetId();
							if(vector_object[deelId].GetLocation() == rloc&& vector_object[deelId].GetInside() != 0 && vector_object[deelId].GetSize() == SMALL){   //有要处理的物体在容器里面
								DO++;
					//			cout<<"takeout2"<<endl;
								if(TakeOut(deelId,vector_object[deelId].GetInside())){  //可以取出来
									PutDown(deelId);
									DO++;
									vector_object[deelId].SetInside(0);//现在该物体已经不在容器里面了
								}else{
									if(bobj_it2->GetPriority() == CONFIRM){     //物体的位置是确定的，但是还是做错了，就不处理该物体了
										bobj_it2 = bobj_Vector.erase(bobj_it2);
										bobj_it2--;
										break;
									}
									else if(bobj_it2->GetPriority() == LOCCONFIRM){      //位置是确定的，那么肯定不在容器里
										vector_object[deelId].SetInside(0);
									}
									else if(bobj_it2->GetPriority() < LOCCONFIRM){
										askWrong.push_back(vector_object[deelId]);
										vector_object[deelId].SetLocation(0);
										vector_object[deelId].SetInside(0);
									}
								}
							}
						}
						Close(objId);
						DO++;
						vector_object[objId].SetDoor(CLOSED);
						bobj_it = bobj_Vector.erase(bobj_it);  
						continueFlag = true;
						break;
					}
					else{
						vector_object[objId].SetDoor(CLOSED);
						bobj_it = bobj_Vector.erase(bobj_it);  
						continueFlag = true;
						break;
					}
				}
			break;	
			}
		}
		if(continueFlag == true)
			continue;
		
		//到这一步,盘子和手上应该都没有物体了,处理最近的物体 
		//如果找到一个物体的初始位置等于当前位置,处理该物体
		uint bigObj1 = 100;
		for(bobj_it2 =vector_object.begin();bobj_it2 != vector_object.end();++bobj_it2 ){
			if(bobj_it2->GetLocation() == GetLocation() && bobj_it2 ->GetSize() == BIG){  //找到了所在位置的大物体的id
				bigObj1 =bobj_it2->GetId();
			}
		}
		
		for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it++){
			objectId = bobj_it->GetId();
			if(vector_object[objectId].GetLocation() == GetLocation() && GetLocation() != 0&& vector_object[objectId].GetSize() == SMALL){    //是小物体
				uint rloc = GetLocation();
				if(vector_object[objectId].GetInside() != 0){   //在容器里面
					if(vector_object[vector_object[objectId].GetInside()].GetDoor() != OPENED){    //只要不是打开的,就open一下,如果说是opened,应该不会错
						if(vector_object[vector_object[objectId].GetInside()].GetDoor() == CLOSED){
							if(bobj_Vector.size()>SENSE){
								senseFlag = false;
								//cout<<"sense8"<<endl;
								senseFlag =DeelSense2 (Sensed_Loc,Sensed_again,bobj_Vector,vector_object[objectId].GetInside(),objectId);
							}
							Open(vector_object[objectId].GetInside());
							vector_object[vector_object[objectId].GetInside()].SetDoor(OPENED);
							DO++;
							if(bobj_Vector.size()>SENSE){
								senseFlag = false;
								//cout<<"sense9"<<endl;
								senseFlag = DeelSense2(Sensed_Loc,Sensed_again,bobj_Vector,vector_object[objectId].GetInside(),objectId);
								if(senseFlag == false){
									continueFlag = true;
									break;
								}
							}
						}
						else{
							Open(vector_object[objectId].GetInside()); DO++;
							vector_object[bobj_Vector[objectId].GetInside()].SetDoor(OPENED);
						}
					}
					DO++;
				//	cout<<"takeout3"<<endl;
					if(TakeOut(objectId,vector_object[objectId].GetInside())){   		//取出成功
						for(bobj_it3 = bobj_Vector.begin();bobj_it3 != bobj_Vector.end();bobj_it3++){
							if(bobj_it3->GetId() == objectId)
								bobj_it3->SetPriority(VIP);
						}
						continueFlag = true;
						SetHold(vector_object[objectId]);
						vector_object[objectId].SetHold(HOLD_YES);
						uint goalLoc = bobj_it->GetLocation();
						if(goalLoc != 0){
							if(goalLoc != GetLocation()){    //如果要处理的位置不是当前位置,就需要移动
								Move(goalLoc);  //移动到目标位置
								DO++;
								SetLocation(goalLoc);
								//更新一下机器人身上的物体信息
								uint holdId = GetHold().GetId();
								uint plateId = GetPlate().GetId();
								if (holdId) {
									vector_object[holdId].SetLocation(goalLoc);
								}
								if (plateId) {
									vector_object[plateId].SetLocation(goalLoc);
								}
							}
						}
						else{    //该物体的目标位置不能确定，该题目是有问题的，不做处理
							bobj_it = bobj_Vector.erase(bobj_it);
							bobj_it --;
						}
					}
					else{                                                //拿不出来,认为是物体应该不在容器里面
				//		cout<<" id: " << bobj_it->GetId() << "     " << "Priority:"<<bobj_it->GetPriority()<<endl;
						if(bobj_it->GetPriority() == CONFIRM){     //物体的位置是确定的，但是还是做错了，就不处理该物体了
							bobj_it = bobj_Vector.erase(bobj_it);
				//			cout << "CONFIRM" << endl;
							bobj_it--;
							break;
						}
						else if(bobj_it->GetPriority() == LOCCONFIRM){      //位置是确定的，那么肯定不在容器里s
							vector_object[objectId].SetInside(0);
							//cout<<"pickup5"<<endl;
							if(PickUp(objectId)){   //捡起来成功了
								for(bobj_it3 = bobj_Vector.begin();bobj_it3 != bobj_Vector.end();bobj_it3++){
									if(bobj_it3->GetId() == objectId)
										bobj_it3->SetPriority(VIP);
								}
								continueFlag = true;
								SetHold(vector_object[objectId]);
								vector_object[objectId].SetHold(HOLD_YES);
								uint goalLoc = bobj_it->GetLocation();
								if(goalLoc != 0){
									if(goalLoc != GetLocation()){    //如果要处理的位置不是当前位置,就需要移动
										Move(goalLoc);  //移动到目标位置
										DO++;
										SetLocation(goalLoc);
										//更新一下机器人身上的物体信息
										uint holdId = GetHold().GetId();
										uint plateId = GetPlate().GetId();
										if (holdId) {
											vector_object[holdId].SetLocation(goalLoc);
										}
										if (plateId) {
											vector_object[plateId].SetLocation(goalLoc);
										}
									}
								}
								else{
									bobj_it = bobj_Vector.erase(bobj_it);
									bobj_it --;
								}
							}
						}
						else if(bobj_it->GetPriority() < LOCCONFIRM){   //物体的位置应该不知道了
				//			cout << "not know" << endl;
							askWrong.push_back(vector_object[objectId]);
							vector_object[objectId].SetLocation(0);
							vector_object[objectId].SetInside(0);
						}
					}
				}
				else if(vector_object[objectId].GetInside() == 0){  //不在容器里面;
					bool senseFlag;
					//cout<<"sense10"<<endl;
					senseFlag = DeelSense1(Sensed_Loc,bobj_Vector,bigObj1,objectId);
					vector <Object>::iterator ot1;

					if(senseFlag == false){   //没有找到该物体
						if(bobj_it->GetPriority() >= LOCCONFIRM){
							bobj_it = bobj_Vector.erase(bobj_it);
							bobj_it --;
						}
						else{
							vector_object[objectId].SetLocation(0);
						}
						break;
					}
					 DO++;
					 //cout<<"pickup6"<<endl;
					 if(PickUp(objectId)){   //取出成功
						 for(bobj_it3 = bobj_Vector.begin();bobj_it3 != bobj_Vector.end();bobj_it3++){
							if(bobj_it3->GetId() == objectId)
								bobj_it3->SetPriority(VIP);
						}
						continueFlag = true;
						SetHold(vector_object[objectId]);
						vector_object[objectId].SetHold(HOLD_YES);
						uint goalLoc = bobj_it->GetLocation();
						if(goalLoc != 0){
							if(goalLoc != GetLocation()){    //如果要处理的位置不是当前位置,就需要移动
								Move(goalLoc);  //移动到目标位置
								DO++;
								SetLocation(goalLoc);
								//更新一下机器人身上的物体信息
								uint holdId = GetHold().GetId();
								uint plateId = GetPlate().GetId();
								if (holdId) {
									vector_object[holdId].SetLocation(goalLoc);
								}
								if (plateId) {
									vector_object[plateId].SetLocation(goalLoc);
								}
							}
						}
						else{
							bobj_it = bobj_Vector.erase(bobj_it);
							bobj_it --;
						}
					}
					else if(bobj_it->GetPriority() != CONFIRM && vector_object[objectId].GetInside() != 0){   //试一试TakeOut操作
				//		cout<<"takeout4"<<endl;
						if(TakeOut(objectId,vector_object[objectId].GetInside())){   		//取出成功
							for(bobj_it3 = bobj_Vector.begin();bobj_it3 != bobj_Vector.end();bobj_it3++){
								if(bobj_it3->GetId() == objectId)
									bobj_it3->SetPriority(VIP);
							}
							continueFlag = true;
							SetHold(vector_object[objectId]);
							vector_object[objectId].SetHold(HOLD_YES);
							uint goalLoc = bobj_it->GetLocation();
							if(goalLoc != 0){
								if(goalLoc != GetLocation()){    //如果要处理的位置不是当前位置,就需要移动
									Move(goalLoc);  //移动到目标位置
									DO++;
									SetLocation(goalLoc);
									//更新一下机器人身上的物体信息
									uint holdId = GetHold().GetId();
									uint plateId = GetPlate().GetId();
									if (holdId) {
										vector_object[holdId].SetLocation(goalLoc);
									}
									if (plateId) {
										vector_object[plateId].SetLocation(goalLoc);
									}
								}
							}
							else{    //该物体的目标位置不能确定，该题目是有问题的，不做处理
								bobj_it = bobj_Vector.erase(bobj_it);
								bobj_it --;
							}
						}
					}
					else{                                                //拿不出来,认为是物体的位置信息错误
						continueFlag = true;
						askWrong.push_back(vector_object[objectId]);
						vector_object[objectId].SetLocation(0);
						vector_object[objectId].SetInside(0);
					}
				}
				break;    //找到了一个物体就结束循环
			}
		}
		if(continueFlag == true)
			continue;
		
		
		//如果能找到一个物体的初始位置已知,处理该物体
		for(bobj_it = bobj_Vector.begin();bobj_it != bobj_Vector.end();bobj_it++){
			objectId = bobj_it->GetId();
			if(vector_object[objectId].GetLocation() != 0 ){  //找到一个物体的初始位置已知,处理该物体
				uint gLoc = vector_object[objectId].GetLocation();
				Move(gLoc);
				DO++;
				SetLocation(gLoc);
				//更新一下机器人身上的物体信息
				uint holdId = GetHold().GetId();
				uint plateId = GetPlate().GetId();
				if (holdId) {
					vector_object[holdId].SetLocation(gLoc);
				}
				if (plateId) {
					vector_object[plateId].SetLocation(gLoc);
				}
				continueFlag = true;
				break;
			}
		}	
		if(continueFlag == true)
			continue;

		//到这一步,手中的,盘子里的,当前位置可见的物体应该都处理完了,需要调用AskLoc
		uint wcount= 0; 
askagain:uint askobj = bobj_Vector.begin()->GetId();
		for(ak_it = askWrong.begin();ak_it != askWrong.end();ak_it ++){
			if(ak_it->GetId() == askobj)
				wcount ++;
		}
		if(wcount>MAXWRONGTIME){  //收集的错误信息已经三条，不做了
			bobj_Vector.erase(bobj_Vector.begin());
			continue;
		}
		string askLoc = DeelAskLoc(askobj);
		if(askLoc == ""){
			bobj_Vector.erase(bobj_Vector.begin());
			continue;
		}
		
		if(!AnalyseAskLoc(askLoc,Sensed_Loc)){
			goto askagain;
		}
		
		for(ak_it = askWrong.begin();ak_it != askWrong.end();ak_it ++){
			if(ak_it->GetId() == vector_object[askobj].GetId() && ak_it->GetLocation() == vector_object[askobj].GetLocation() && ak_it->GetInside() == vector_object[askobj].GetInside()){
				goto askagain;
			}
		}
		continue;
	}
	
	//最后核对一下close和处理 和Goto
	vector<uint>Close_V2;
	while(Close_V.size()){
		c_it = Close_V.begin();
		uint Loc = *c_it;
		if(vector_object[Loc].GetDoor() != CLOSED){
			if(GetLocation() != Loc){
				Move(vector_object[Loc].GetLocation());
				SetLocation(vector_object[Loc].GetLocation());
			}
			if(GetHold().GetId() != 0){
				uint i = GetHold().GetId();
				PutDown(i);
				Close(Loc);
				PickUp(i);
			}else{
				Close(Loc);
			}
			vector_object[Loc].SetDoor(CLOSED);
		}
		Close_V2.push_back(*c_it);
		c_it = Close_V.erase(c_it);
	}
	
	bool Sp = false;  //记录是不是第二次进行pick操作
	bool openf = false;
	bool isCP;  //记录是不是需要关上的门打开了
	uint goalc;
	uint pickSize;
	vector<uint>::iterator it;
	
	while(PickUp_objid.size()>2){  
		PickUp_objid.pop_back(); //超过两个物体要捡起来,不可能完成,有些任务就不做了
	}
	
	for(it = PickUp_objid.begin();it != PickUp_objid.end();++it){
		bobj_Vector.push_back(vector_object[*it]);
	}
	pickSize = PickUp_objid.size();
	while(PickUp_objid.size()){
		for(it = PickUp_objid.begin();it!= PickUp_objid.end();++it){
			if(GetHold().GetId() == *it || GetPlate().GetId() == *it){
				it = PickUp_objid.erase(it);
				it--;
			}
		}
		if(PickUp_objid.begin() == PickUp_objid.end()){
			break;
		}
		
		vector <Object> Ask_V;
		vector <Object>::iterator ask_it;
		uint wflg = 0;
		it = PickUp_objid.begin();
		uint a = *it;
		it = PickUp_objid.erase(it);
		it --;
		
		if(vector_object[a].GetLocation() == 0){    //要捡起来的物体初始位置不知道
	ask:		if(wflg>2)
				continue;
			else
				wflg ++;
	                string askLoc = DeelAskLoc(a);
			if(askLoc == ""){
				continue;
			}
			else{
				AnalyseAskLoc(askLoc,Sensed_Loc);
			}
		}
		
		for(ask_it = Ask_V.begin();ask_it != Ask_V.end();ask_it++){
			if(vector_object[a].GetLocation() == ask_it->GetLocation() && vector_object[a].GetInside() == ask_it->GetInside()){
				goto ask;
			}
		}
		
		if(ask_it == Ask_V.end()){
			Ask_V.push_back(vector_object[a]);
		}
		
		uint goalc = vector_object[a].GetLocation();
		
		if(goalc == 0)
			continue;
		
		if(goalc != GetLocation()){
			Move(goalc);
			SetLocation(goalc);
		}
		
		if(vector_object[a].GetInside() != 0){   //在容器里
			for(it = Close_V2.begin();it != Close_V2.end();++it){
				if(*it == vector_object[a].GetInside()){
					break;
				}
			}
			isCP == false;
			isCP = !(it == Close_V2.end());
			
			if(vector_object[vector_object[a].GetInside()].GetDoor() == CLOSED){
				senseFlag =DeelSense2 (Sensed_Loc,Sensed_again,bobj_Vector,vector_object[a].GetInside(),a);
				Open(vector_object[a].GetInside());
				vector_object[vector_object[a].GetInside()].SetDoor(OPENED);
				openf = true;
				senseFlag= false;
				senseFlag =DeelSense2 (Sensed_Loc,Sensed_again,bobj_Vector,vector_object[a].GetInside(),a);
				
				if(senseFlag == false){  //没有找到该物体
					if(openf == true && isCP == true){
						Close(vector_object[a].GetInside());
						vector_object[bobj_Vector[a].GetInside()].SetDoor(CLOSED);
					}
					continue;
				}
			}
		//	cout<<"takeout5"<<endl;
			if(TakeOut(a,vector_object[a].GetInside())){
				if(Sp == false && pickSize == 2){
					ToPlate(a);
				}else{
					Sp = true;
				}
				if(openf == true && Sp == true){
					PutDown(a);
					Close(vector_object[a].GetInside());
					PickUp(a);
					vector_object[vector_object[a].GetInside()].SetDoor(CLOSED);
				}
			}
			else{
				if(vector_object[a].GetPriority() == CONFIRM){
					if(openf == true)
						Close(vector_object[a].GetInside());
					vector_object[bobj_Vector[a].GetInside()].SetDoor(CLOSED);
					continue;
				}
				else if(vector_object[a].GetPriority() == LOCCONFIRM){
					if(PickUp(a)){
						if(Sp == false && pickSize == 2){
							ToPlate(a);
						}else{
							Sp = true;
						}
						if(openf == true && Sp == true){
							PutDown(a);
							Close(vector_object[a].GetInside());
							PickUp(a);
							vector_object[vector_object[a].GetInside()].SetDoor(CLOSED);
						}
					}else{
						if(openf == true)
							Close(vector_object[a].GetInside());
						vector_object[bobj_Vector[a].GetInside()].SetDoor(CLOSED);
						continue;
					}
				}
				else{
					if(openf == true)
						Close(vector_object[a].GetInside());
					vector_object[bobj_Vector[a].GetInside()].SetDoor(CLOSED);
					goto ask;
				}
			}
		}
		else{   //不在容器里
			uint BIGO = 100;
			for(aobj_it = vector_object.begin();aobj_it != vector_object.end();++aobj_it){
				if(aobj_it ->GetLocation() == GetLocation() && aobj_it ->GetSize() == BIG){
					BIGO = aobj_it->GetId();     //找到了改位置的大物体
				}
			}
			
			if(BIGO == 100){
				goto ask;
			}
			
			senseFlag == false;
			senseFlag = DeelSense1(Sensed_Loc,bobj_Vector,BIGO,a);
			
			if(senseFlag == false){
				goto ask;    //没有找到物体a;
			}
			
			if(PickUp(a)){
				if(Sp == false && pickSize == 2){
					ToPlate(a);
				}else{
					Sp = true;
				}
			}else if(TakeOut(a,BIGO)){
				if(Sp == false && pickSize == 2){
					ToPlate(a);
				}else{
					Sp = true;
				}
			}else{
				continue;
			}
		}
	}
	
	if(finalLoc != 100 && finalLoc != GetLocation()){
		Move(finalLoc);
	}
}

