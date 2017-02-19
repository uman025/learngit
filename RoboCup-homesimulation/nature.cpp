#include "nature.hpp"

void VectorSentence(string &str, vector<string> &vsent)
{
      int pos=str.find("Please");
	while(pos!=string::npos){
	  str.replace(pos,8,"");	
	  pos=str.find("Please",pos+1);
	}
	pos=str.find("Then");
	while(pos!=string::npos){
	  str.replace(pos,5,"");
	  pos=str.find("Then",pos+1);
	}
	
	pos=str.find("Finally");
	while(pos!=string::npos){
	  str.replace(pos,9,"");
	  pos=str.find("Finally",pos+1);
	}
	
	pos=str.find("And");
	while(pos!=string::npos){
	  str.replace(pos,4,"");
	  pos=str.find("And",pos+1);
	}
	
	pos=str.find("'s");
	while(pos!=string::npos){
	  str.replace(pos,2,"");
	  pos=str.find("'s",pos+1);
	}
	
	int post=str.find(".",0);
	string temp_s;
	for (int i=0;i<=post;++i)
	{
		temp_s.push_back(str[i]);
	}
	int post_begin=0;

	while (post!=string::npos)
	{
		int start_mast = temp_s.find("and");
		int start=start_mast+3;
		int end=start;
		if(start_mast!=string::npos)
		{
			if(temp_s[start]==' ')
			{
				++start;++end;
			}
			while(start!=temp_s.length() && end!=temp_s.length())
			{
				if(temp_s[start]==' ')
				{  ++start;++end;}
				else if(temp_s[end]==' '||temp_s[end]=='.')
				{
					string temp;
					for(int i=start;i!=end;++i)
					{
						temp.push_back(temp_s[i]);
					}
					    
					if(IsSort(temp)!=0)
					{
					    //cout<<"{"<<end<<"}"<<":"<<temp<<endl;
						break;
					}
					else
					{
						start=end;
					}
				}
				else
				{
					++end;
				}
			}

			string vsent1;
			int temp_start=start_mast;
			if(temp_s[end]=='.')
			  temp_start=start_mast-1;
			for(int i=0;i!=(temp_start);++i)
			{
				vsent1.push_back(temp_s[i]);
			}
			if(temp_s[end]=='.')
			  vsent1.push_back(temp_s[end]);
			for(int i=end+1;i!=temp_s.length();++i)
			{
				vsent1.push_back(temp_s[i]);
			}
			vsent.push_back(vsent1);

			string vsent2="";
			int j=0;
			do
			{
				vsent2.push_back(temp_s[j]);
				++j;
			}while (temp_s[j]!=' ');
			
			int flag=0;
			if(vsent2.compare("pick")==0||vsent2.compare("Pick")==0)
			{
			   flag=1;
			  for(int i=0;i<4;++i)
			  {
			    vsent2.push_back(temp_s[j]);
			    ++j;
			  }
			}
			
			if(vsent2.compare("take")==0||vsent2.compare("Take")==0)
			{
			   flag=1;
			  for(int i=0;i<5;++i)
			  {
			    vsent2.push_back(temp_s[j]);
			    ++j;
			  }
			}

			if(flag==0)
			{
			  vsent2.push_back(temp_s[j]);
			}
			
			for(int i=start_mast+4;i!=temp_s.length();++i)
			{
				vsent2.push_back(temp_s[i]);
			}
			vsent.push_back(vsent2);
		}
		else
		{
			vsent.push_back(temp_s);
		}

		post_begin=post+1;
		post=str.find(".",post+1);
		temp_s.clear();
		if (post!=string::npos)
		{
			while(str[post_begin]==' ')
			{
			    post_begin++;
			}
			
			for (int i=post_begin;i<=post;++i)
			{
			    temp_s.push_back(str[i]); 
			}
		}
		else
			break;
	}
}

Nature * NatureList(const string &str) 	//把句子变成链表
{
	uint i = 0;
	bool first = true; 		//判断是否产生第一个节点
	char tmp[20] = "\0";
	char *p = const_cast<char *>(str.c_str());
	Nature *head, *np;

	while (*p != '.') { 		//'.'结束
		tmp[i++] = *p;
		if ((*(p + 1) == ' ') || (*(p + 1) == '.')) { 	//读完一个单词了
			tmp[i] = '\0';
			if (first) { 	//如果是第一个，赋给head
				head = new Nature;
				head->word = tmp;
				head->next = NULL;
				np = head;
				first = false;
			} else {
				np->next = new Nature;
				np = np->next;
				np->word = tmp;
				np->next = NULL;
			}

			if (*(p + 1) == ' ') { 		//如果后面一个字符是空格,跳过
				p++;
			} else if (*(p + 1) == '.') {
				//nothing 
			}
			i = 0; 	//置0用于存下一个单词
		}
		p++;
	} 		//这句解析完成，生成链表

	// . 也是要保存的。
	np->next = new Nature;
	np = np->next;
	np->word = ".";
	np->next = NULL;

	if ((head->word[0] >= 'A') && (head->word[0] <= 'Z')) {
		head->word[0] = head->word[0] + 32; 	//开头的大写转成小写
	}

	return head;
}


void ShowNatureList(const Nature *head) 		//显示句子链表
{
	Nature *p = const_cast<Nature *>(head);
	while (p != NULL) {
		//cout << p->word;
		//if (p->next != NULL) 	//不是最后一个
		//	cout << " ";
		//else 
		//	cout << "." << endl; //是最后一个
		p = p->next;
	}
}


void DeleteNatureList(Nature *head) 		//释放那些链表的空间
{
	Nature *p = head;
	while (head->next != NULL) {
		head = head->next;
		delete p;
		p = head;
	}
	delete head;
}

Nature * DowithThe(Nature *head, Object &obj)      //跳过out of/from、the、door of(each)、the，跳出which、to；注意是逻辑顺序
{
	string str;
	uint tmp = 0; 	//记录枚举值
	while (1) {

		if ((head->word == nout) && (head->next->word == nof)) { 		//跳过out of 
			head = head->next->next;
		}
		
		if(head->word == nfrom){                                 //跳过from
			head = head->next;
		}

		if ((head->word == nthe) || (head->word == na)) { 	          //可能不是the/a,而直接是物体
			head = head->next; 		//这个时候head指的a/the后面的
		}

		if ((head->word == ndoor) && (head->next->word == nof)) {     //a/the后面可能是door of
			head = head->next->next; 		//head指向of后面那个
			if (head->word == neach) {           //如果door of后面是each, 可能是sort或container
				head = head->next; 		//each之后应该没有a,the了
				if ((tmp = IsType(head->word)) != 0) { 		//是container
					obj.SetType(tmp);
					head = head->next; 		//应该指向 .
					break;
				}
			}
		}

		if ((head->word == nthe) || (head->word == na)) {            //door of 后面可能有the或者a
			head = head->next;
		}

		str = head->word;

		if ((tmp = IsColor(str)) != 0) { 	//是个描述颜色的词
			obj.SetColor(tmp); 	
			head = head->next;

			str = head->word; 			//忘记更新这个值了
		}
		
		if ((tmp = IsSort(str)) != 0) { 	        //是个种类的词
			obj.SetSort(tmp);
			head = head->next; 		//head指向 .
		}
		/*
		/////////////////////////////////////////
		if(head->word == nand){
		  head=head->next;
		  if ((head->word == nthe) || (head->word == na)) {
			head = head->next;
		    
		  }
		  str = head->word;
		
		 if ((tmp = IsColor(str)) != 0) { 	//是个描述颜色的词	
			head = head->next;
			str = head->word; //忘记更新这个值了
		 }
		

		 if ((tmp = IsSort(str)) != 0) { 	//是个种类的词
			
			head = head->next; 		//head指向 .
		 }
		}
		///////////////////////////////////////////
		*/
		if (head->word == nwhich) {        //遇到which跳出跳转到DowithWhich函数
			//cout << __FILE__ << " " << __LINE__ << "---which---" << endl;
			break;
		} else if (head->word == nto) {     //或者to跳出
			//cout << __FILE__ << " " << __LINE__ << head->word << endl;
			head = head->next->next;
			//cout << __FILE__ << " " << __LINE__ << "---to--" << endl;
			break;
		} else {
			break;
		}
	}

	return head;
}

/*
 * 暂时只能处理颜色
 * a/the can which is black
 * a/the can which is black to human
 *
 */
Nature * DowithWhich(Nature *head, Object &obj) 
{
	string str;
	uint tmp = 0; 	//记录枚举值

	head = head->next->next; 	            //直接跳到is后面的单词

	if ((tmp = IsColor(head->word)) != 0) { 		//判断物体的颜色
		obj.SetColor(tmp);
		head = head->next;
	}

	if ((head->word == nto)||(head->word == nfor)) {              //存在can which is black to/for human 的情况
		//cout << __FILE__ << " " <<  __LINE__ << head->word << endl;
		head = head->next->next;
	}

	return head;         //处理完成返回指针
}



/*
 * 暂且注释
 *
Nature * DowithWhich(Nature *head, Object &objx, Object &objy) 	//处理从句
{
	string str;
	//while ((head = head->next) != NULL) {
	while ((head->next) != NULL) {
		head = head->next;
		str = head->word;

		if (str == is) { 	//必须是对先行词的修饰
			head = head->next;
			str = head->word;

			if (str == on) { 		//在某个物体之上
				head = head->next;
				str = head->word;
				if ((str == the) || (str == a)) {
					head = DowithThe(head, objy);
					objx.SetLocation(objy.GetLocation());
					objx.SetPosition((1 << 8) | objy.GetId());
				}
			} else if (str == near) { 	//在某个物体旁边
				head = head->next;
				str = head->word;
				if ((str == the) || (str == a)) {
					head = DowithThe(head, objy);
					objx.SetLocation(objy.GetLocation());
					objx.SetPosition((1 << 9) | objy.GetId());
				}
			} else if (str == in) { 	//在某个物体里面
				head = head->next;
				str = head->word;
				if ((str == the) || (str == a)) {
					head = DowithThe(head, objy);
					objx.SetLocation(objy.GetLocation());
					objx.SetInside(objy.GetId());
				}
			}


			if (str == white) { 	//颜色或大小
				objx.SetColor(WHITE);
			} else if (str == black) {
				objx.SetColor(BLACK);
			} else if (str == red) {
				objx.SetColor(RED);
			} else if (str == green) {
				objx.SetColor(GREEN);
			} else if (str == yellow) {
				objx.SetColor(YELLOW);
			} else if (str == blue) {
				objx.SetColor(BLUE);
			} else if (str == big) {
				objx.SetSize(BIG);
			} else if (str == small) {
				objx.SetSize(SMALL);
			}
			
		}
	}
	
	return head;
}
*/



void DowithSentence(string &str, vector<Instruction> &veci) 		//plan.cpp中只调用这个API
{
	Nature *head, *orig;
	Instruction ins_tmp;
	vector<string> vec_str;
	vector<string>::iterator str_ite; 		//迭代器
	VectorSentence(str, vec_str); 		//把句子分解分别独立放到vec_str中
	/*for(vector<string>::iterator s1=vec_str.begin();s1!=vec_str.end();++s1)
	{
	  cout<<"["<<*s1<<"]"<<endl;
	}*/
	for (str_ite = vec_str.begin(); str_ite != vec_str.end(); str_ite++) {
		head = NatureList(*str_ite); 	//把句子转成链表
		orig = head; 					//保存头地址
		ParseSentence(head, ins_tmp); 	//解析句子。信息将被存到ins_tmp
		veci.push_back(ins_tmp); 		//加入指令

		ins_tmp.Clear(); 			//用完清空信息
		DeleteNatureList(orig); 		//释放分配的内存空间
	}

}



	    
void ParseSentence(Nature *head, Instruction &ins) 		//解析句子。根据链表设置指令对象。
{
	string str = "";
	uint tmp = 0; 	//记录枚举值
	uint stype = 0; 	//记录Hasdomust枚举值

	Object objx, objy;

	stype = HasDoMust(head);

	if ((Hasdomust)stype == HAS_DO) { 		//cons task
		head = head->next->next;
		ins.SetInsflag(CONS_NOT_TASK);
		ins.SetPriority(PRI_MEDIUM);
	} else if ((Hasdomust)stype == HAS_NULL) { 		//task or info
		ins.SetInsflag(TASK); 				//这两行只有为task时才有效
		ins.SetPriority(PRI_LOW);
	}

	if ((tmp = IsKeyword(head->word)) != 0) { 

		switch((HeadKeyword)tmp) { 			//根据句首关键字枚举值
			case H_GIVE:
				{
					/*
					 * Give me a can.
					 * Give human a can.
					 * Give me a black can.
					 * Give me a can which is black.
					 *
					 * Give a can which is black to human.
					 * Give a can which is black for human.
					 */
					head = head->next; 		//指到下一个单词了	
					ins.SetKeyword(GIVE_INS);
					ins.SetPriority(PRI_LOW);


					if ((head->word == nme) || (head->word == nhuman)) { 	//默认就是人了,跳到me后面
						head = head->next; 
						objx.SetSort(HUMAN); 		//objx是人
					}
					

					head = DowithThe(head, objy); 		//处理完之后，head指向which或者to/for或者.
					if (head->word == nwhich) {
						head = DowithWhich(head, objy); 	//处理完之后，head指向.
					}
					
					if ((head->word == nme) || (head->word == nhuman)) { 	//默认就是人了,跳到me后面
						head = head->next; 
						objx.SetSort(HUMAN); 		//objx是人
					}
					if(objy.GetColor() != 0){
						ins.SetPriority(PRI_COLOR);
					}
					ins.SetObjecty(objx);
					ins.SetMasky(ins.GenerateMask(objx));
					ins.SetObjectx(objy);
					ins.SetMaskx(ins.GenerateMask(objy));

					if (head->next == NULL) {
						//cout << __FILE__ << " " << __LINE__ << " " << "give语句处理完毕。" << endl;
					}

					break;
				}

			case H_PUT:
				{
					/* 
					 * Put the can which is black on/near/next to/down to the desk.
					 * Put the can which is black in the closet.
					 * Put the can which is black down.
					 * Put down the can which is black.
					 * Put a can on the bed.
					 */

					//cout << "---put" << endl;

					head = head->next; 		//指到下一个单词了	

					if (head->word == ndown) { 		//put down A
						ins.SetKeyword(PUTDOWN_INS);
						ins.SetPriority(PRI_LOW);

						head = head->next; 		//head指向the
						head = DowithThe(head, objx);
						if (head->word == nwhich) {
							head = DowithWhich(head, objx); 	//处理完之后，head指向.
						}
						ins.SetObjectx(objx);
						ins.SetMaskx(ins.GenerateMask(objx));
					} //else if ((head->word == nthe) || (head->word == na)) {
					else{
						//涉及A，B
						//先处理A
						head = DowithThe(head, objx);            //处理小物体
						if (head->word == nwhich) {
							head = DowithWhich(head, objx); 	//处理完之后，head指向.
						}
						ins.SetObjectx(objx);
						ins.SetMaskx(ins.GenerateMask(objx));

						//开始处理B
						if ((head->word == ndown) && (head->word != nto)) { 		//put A down
							ins.SetKeyword(PUTDOWN_INS);
							ins.SetPriority(PRI_LOW);
						} else if ((head->word == nin) || (head->word == ninto)) { 	//put A in/into B
							ins.SetKeyword(PUTIN_INS);
							ins.SetPriority(PRI_LOW);

							head = head->next; 		//head指向the
							head = DowithThe(head, objy);
							if (head->word == nwhich) {
								head = DowithWhich(head, objy); 	//处理完之后，head指向.
							}
							ins.SetObjecty(objy);
							ins.SetMasky(ins.GenerateMask(objy));
						} else { 		                                        //put on/near/next to/down to B
							ins.SetKeyword(PUTON_INS);
							ins.SetPriority(PRI_LOW);

							head = head->next;
							if ((head->word == nnext) || (head->word == ndown)||(head->word == non)||(head->word == nnear)) {
								head = head->next; 		//跳过to
							}
							head = DowithThe(head, objy);
							if (head->word == nwhich) {
								head = DowithWhich(head, objy); 	//处理完之后，head指向.
							}
							ins.SetObjecty(objy);
							ins.SetMasky(ins.GenerateMask(objy));
						}
					}
					if(objx.GetColor() != 0){
						ins.SetPriority(PRI_COLOR);
					}
					if (head->next == NULL) {
						//cout << __FILE__ << " " << __LINE__ << " " << "put 语句处理完毕。" << endl;
					}

					break;

				}

			case H_GO:
				{
					/*
					 * Go to the can.
					 * Go to the can which is black.
					 *
					 */
					head = head->next; 		//指到下一个单词了	
					ins.SetKeyword(GOTO_INS);
					ins.SetPriority(PRI_LOW);

					if (head->word == nto) {
						//cout << __FILE__ << " " << __LINE__ << "确实是to!" << endl;
						head = head->next;
						head = DowithThe(head, objx);
						if (head->word == nwhich) {
							head = DowithWhich(head, objx); 	//处理完之后，head指向.
						}

						ins.SetObjectx(objx);
						ins.SetMaskx(ins.GenerateMask(objx));
					
					} else {
						//cout << __FILE__ << " " << __LINE__ << "不是go to!" << endl;
					}

					if (head->next == NULL) {
						//cout << __FILE__ << " " << __LINE__ << " " << "go语句处理完毕。" << endl;
					}

					break;

				}

			case H_PICK:
				{
					/*
					 * Pick up the can.
					 * Pick up the can which is black.
					 * Pick the can which is black up.
					 */

					head = head->next; 		//指到下一个单词了	
					ins.SetKeyword(PICKUP_INS);
					ins.SetPriority(PRI_LOW);

					if (head->word == nup) {
						//cout << __FILE__ << " " << __LINE__ << "确实是up!" << endl;
						head = head->next;
						head = DowithThe(head, objx);
						if (head->word == nwhich) {
							head = DowithWhich(head, objx); 	//处理完之后，head指向.
						}

						ins.SetObjectx(objx);
						ins.SetMaskx(ins.GenerateMask(objx));
					
					} else {
						//cout << __FILE__ <<"不是pick up!" << endl;
						head = DowithThe(head,objx);
						if(head->word==nwhich){
							head = DowithWhich(head,objx);
						}
						if(head->word== nup){
							ins.SetObjectx(objx);
							ins.SetMaskx(ins.GenerateMask(objx));
						}
					}
					
					if(objx.GetColor() != 0){
						ins.SetPriority(PRI_COLOR);
					}
					
					if (head->next == NULL) {
					//	cout << __FILE__ << " " << __LINE__ << " " << "pickup语句处理完毕。" << endl;
					}

					break;
				}

			case H_TAKE:
				{
					/*
					 * Take the can from the closet.
					 * Take the can which is black from the closet.
					 * Take the can out of the closet.
					 *
					 * Take out a can from microwave.
					 */

					head = head->next; 		//指到下一个单词了	
					ins.SetKeyword(TAKEOUT_INS);
					ins.SetPriority(PRI_TAKEOUT);

					if (head->word == nout) {
						head = head->next; 		//take out 跳过out
					}

					//if (head->word == nthe)  		//不知道这个地方会不会出现 a
					//干脆直接跳过去，不再判断定冠词
					head = DowithThe(head, objx);
					if (head->word == nout) {
						head = head->next; 		//take out 跳过out
					}
					if (head->word == nwhich) { 		//可能为which
						head = DowithWhich(head, objx); 		//head应该指向from
						head = head->next; 		//head应该指向the

					}  
					head = DowithThe(head, objy); 		//应该是物体objy了, 此时head应该指向the
					if (head->word == nwhich) {
						head = DowithWhich(head, objy); //head应该指向 .
					}

					ins.SetObjectx(objx);
					ins.SetMaskx(ins.GenerateMask(objx));
					ins.SetObjecty(objy);
					ins.SetMasky(ins.GenerateMask(objy));
					if(objx.GetColor() != 0){
						ins.SetPriority(PRI_COLOR);
					}
					if (head->next == NULL) {
					//	cout << __FILE__ << " " << __LINE__ << " " << "takeout语句处理完毕。" << endl;
					}

					break;

				}

			case H_OPEN:
				{
					/*
					 * Open closet.
					 *
					 * Open the closet.
					 * Open the closet which is black.
					 * Open the door of closet.
					 * Open the door of closet which is black.
					 * Open the door of each container.
					 * Open the door of each closet.
					 *
					 */

					head = head->next; 		//指到下一个单词了	
					ins.SetKeyword(OPEN_INS);
					ins.SetPriority(PRI_LOW);

					//有可能有the 也有可能直接就是物体。
					head = DowithThe(head, objx);
					if (head->word == nwhich) {
						head = DowithWhich(head, objx); 		//处理完之后，head指向
					}

					ins.SetObjectx(objx);
					ins.SetMaskx(ins.GenerateMask(objx));

					if (head->next == NULL) {
					//	cout << __FILE__ << " " << __LINE__ << " " << "open语句处理完毕。" << endl;
					}

					break;

				}
			case H_CLOSE:
				{
					/*
					 * Close closet.
					 *
					 * Close the closet.
					 * Close the closet which is black.
					 * Close the door of closet.
					 * Close the door of the closet.
					 * Close the door of closet which is black.
					 * Close the door of each container.
					 * Close the door of each closet.
					 *
					 */

					head = head->next; 		//指到下一个单词了	
					ins.SetKeyword(CLOSE_INS);
					ins.SetPriority(PRI_LOW);

					//有可能有the 也有可能直接就是物体。
					head = DowithThe(head, objx);
					if (head->word == nwhich) {
						head = DowithWhich(head, objx); 		//处理完之后，head指向
					}

					ins.SetObjectx(objx);
					ins.SetMaskx(ins.GenerateMask(objx));

					if (head->next == NULL) {
					//	cout << __FILE__ << " " << __LINE__ << " " << "close语句处理完毕。" << endl;
					}

					break;
				}

			case H_INFOCONS: 	//包括info和cons
				{
					/*
					 * info:
					 * The black can is on the table. 			=== 	on
					 * The can which is black on the table. 
					 *
					 * The black can is on the plate. 			=== 	onplate
					 *
					 * The black can is near the table. 		=== 	near
					 *
					 * The black can is in the closet. 			=== 	inside
					 *
					 * The black can is inside the closet. 		=== 	inside
					 *
					 * The black closet is opened. 				=== 	opened
					 * The black closet is not closed. 			
					 *
					 * The black closet is closed. 				=== 	closed
					 * The black closet is not opened.
					 *
					 * cons:
					 *
					 *
					 */


					/*
					 * There is a yellow can on the chair.
					 * There must (not) be a yellow can on the chair.
					 * The yellow can on the chair.
					 * The yellow can must (not) be on the chair.
					 *
					 * the是there后续的一部分。
					 */

					/*
					 * 暂时只处理cons_notnot
					 *
					 * 还有info,到了这儿的句子里，如果没有do,not,must,便是info
					 *
					 */

					//默认是关键字，如遇到关键字则重新设置命令的信息
					ins.SetInsflag(INFO);
					ins.SetPriority(PRI_HIGH);

					if (head->word == nthere) {
						if ((Hasdomust)stype == HAS_MUST) { 		//cons_not info
							head = head->next->next->next;
							ins.SetInsflag(CONS_NOTNOT_INFO);
							ins.SetPriority(PRI_MEDIUM);
						} else if ((Hasdomust)stype == HAS_MUST_NOT) { 		//cons_notnot info
							head = head->next->next->next->next;
							ins.SetInsflag(CONS_NOT_INFO);
							ins.SetPriority(PRI_MEDIUM);
						} else { 			//info
							head = head->next->next;
							ins.SetInsflag(INFO);
							ins.SetPriority(PRI_HIGH);
						}
					}
					head = DowithThe(head, objx);
					if (head->word == nwhich) {
						head = DowithWhich(head, objx); 		//处理完之后，head指向
					} else if (head->word == nis) { 	//is
						head = head->next;
					}

					if (head->word == nmust) {
						head = head->next; 		//not or be
						if (head->word == nnot) {
							head = head->next; 			//be
							ins.SetInsflag(CONS_NOT_INFO);
							ins.SetPriority(PRI_MEDIUM);
						} else {
							ins.SetInsflag(CONS_NOTNOT_INFO);
							ins.SetPriority(PRI_MEDIUM);
						}
						head = head->next; 		//指向关键词
					}
						//cout << "---" << head->word << endl;
					//只处理cons_notnot
					if (head->word == ncannot) {
						break;
					}


					if (head->word == non) 		{ 	//on, onplate
						if (head->next->next->word == nplate) { 		//on the plate
							ins.SetKeyword(PLATE_INS);
							head = head->next->next; 		//指向plate
						}
						ins.SetKeyword(ON_INS);

					} else if (head->word == nnear) { 	//near 	还有next to 未考虑。
						ins.SetKeyword(NEAR_INS);
					} else if ((head->word == nin) || (head->word == ninside)) { 		//inside
						//cout << "---" << head->word << endl;
						ins.SetKeyword(INSIDE_INS);
					} else if (head->word == nopened) {
						ins.SetKeyword(OPENED_INS);
					} else if (head->word == nclosed) {
						ins.SetKeyword(CLOSED_INS);
					} else if ((head->word == nnot) && (head->next->word == nclosed)) { 		//opened or not closed
						ins.SetKeyword(OPENED_INS);
						head = head->next;
					} else if ((head->word == nnot) && (head->next->word == nopened)) { 		//closed or not opened
						ins.SetKeyword(CLOSED_INS);
						head = head->next;
					}
						

					head = head->next; 		//关键词之后的单词 物体或者 .

					if (head->word != ndot) { 	//是物体Y
						head = DowithThe(head, objy); 		//应该是物体objy了, 此时head应该指向the
						if (head->word == nwhich) {
							head = DowithWhich(head, objy); 	//head应该指向 .
						}
						ins.SetObjecty(objy);
						ins.SetMasky(ins.GenerateMask(objy));
					}

					ins.SetObjectx(objx);
					ins.SetMaskx(ins.GenerateMask(objx));

					break;
				}
			
			default:
				//cout << __FILE__ << __LINE__ << "未知选项！" << endl;
				break;


		}

	} else {
		//cout << __FILE__ << " " << __LINE__ << "句首单词未知！" << endl;
	}
}




uint IsKeyword(const string &str) 		//判断句首单词
{
	if (str == ngive) {
		return H_GIVE;
	} else if (str == nput) {
		return H_PUT;
	} else if (str == ngo) {
		return H_GO;
	} else if (str == npick) {
		return H_PICK;
	} else if(str == npickup){
		return H_PICK;
	}else if (str == ntake) {
		return H_TAKE;
	} else if (str == nclose) {
		return H_CLOSE;
	} else if (str == nopen) {
		return H_OPEN;
	} else if ((str == nthe) || (str == nthere)) { 		//以the或there开头的
		return H_INFOCONS;
	} else {
		//未完待续。暂时只处理task
		//cout<<"这个任务没有处理"<<endl;
		return H_KEYWORD_NULL;
	}
}

uint IsSort(const string &str) 		//判断传入的物体种类
{
	if (str == human) { 	
		return HUMAN;
	} else if (str == plant) {
		return PLANT;
	} else if (str == couch) {
		return COUCH;
	} else if (str == chair) {
		return CHAIR;
	} else if (str == sofa) {
		return SOFA;
	} else if (str == bed) {
		return BED;
	} else if (str == table) {
		return TABLE;
	} else if (str == workspace) {
		return WORKSPACE;
	} else if (str == worktable) {
		return WORKTABLE;
	} else if (str == teapoy) {
		return TEAPOY;
	} else if (str == desk) {
		return DESK;
	} else if (str == television) {
		return TELEVISION;
	} else if (str == airconditioner) {
		return AIRCONDITIONER;
	} else if (str == washmachine) {
		return WASHMACHINE;
	} else if (str == closet) {
		return CLOSET;
	} else if (str == cupboard) {
		return CUPBOARD;
	} else if (str == refrigerator) {
		return REFRIGERATOR;
	} else if (str == microwave) {
		return MICROWAVE;
	} else if (str == book) {
		return BOOK;
	} else if (str == can) {
		return CAN;
	} else if (str == remotecontrol) {
		return REMOTECONTROL;
	} else if (str == bottle) {
		return BOTTLE;
	} else if (str == cup) {
		return CUP;
	} else {
		return 0; 		//不是物体
	}

}

uint IsColor(const string &str) 		//判断传入的颜色
{
	if (str == white) {
		return WHITE;
	} else if (str == red) {
		return RED;
	} else if (str == green) {
		return GREEN;
	} else if (str == yellow) {
		return YELLOW;
	} else if (str == blue) {
		return BLUE;
	} else if (str == black) {
		return BLACK;
	} else {
		return 0; 		//不是颜色
	}

}

uint IsType(const string &str) 		//判断传入的类型 container
{
	if (str == container) {
		return CONTAINER;
	} else {
		return 0;
	}
}


/*
 * 这个思路有问题

uint SentenceType(const Nature *head) 		//判断传入的句子类型
{
	string key, prep,
	Nature *tmp = head;

	if (IsSort(tmp->word)) {
		return S_TASK; 		//是task
	}

	//到了这儿意味着不是task
	if (tmp->word == ndo) {
		return S_CONS_NOT_TASK; 		//not task
	}

	while (tmp != NULL) {
		tmp = tmp->next;
		if (tmp->word == nmust) { 		//根据是否存在must来判断
			tmp = tmp->next;
			if (tmp->word == nnot) {
				return S_CONS_NOT_INFO; 	//not info
			} else {
				return S_CONS_NOTNOT_INFO;  	//notnot info
			}

		}

		//开始判断info的类型
		if (tmp->word == non) { 
			tmp = tmp->next;
			if (tmp->word != nplate) {
				return S_INFO_ON;
			} else {
				return S_INFO_ONPLATE;
			}
		} else if (tmp->word == nnear) {
			return S_INFO_NEAR;
		} else if (tmp->word == nin) {
			return S_INFO_INSIDE;
		} else if (tmp->word == nopened) {
			return S_INFO_OPENED;
		} else if (tmp->word == nclosed) {
			return S_INFO_CLOSED;
		}

	}
	cout << __FILE__ << __LINE__ << "运行到这儿就出现错误了！" << endl;

}

*/

uint HasDoMust(const Nature *head) 		//判断传入句子是否有do或must
{
	Nature *tmp = const_cast<Nature *>(head);

	if (tmp->word == ndo) {
		return HAS_DO;

	}

	while (tmp != NULL) {
		if (tmp->word == nmust) {
			tmp = tmp->next;
			if (tmp->word == nnot) {
				return HAS_MUST_NOT;
			} else {
				return HAS_MUST;
			}
		}
		tmp = tmp->next;
	}

	return HAS_NULL;
}


			

