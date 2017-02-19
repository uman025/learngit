#include "instruction.hpp"

Instruction::Instruction()
{
	keyword = INSKEYWORD_NULL;
	ins_flag = INSFLAG_NULL;
}

Instruction::~Instruction()
{
}

InsKeyword Instruction::GetKeyword() const
{
	return keyword;
}

void Instruction::SetKeyword(const InsKeyword &x)
{
	keyword = x;
}

Object Instruction::GetObjectx() const
{
	return object_x;
}

void Instruction::SetObjectx(const Object &x)
{
	object_x = x;
}

Object Instruction::GetMaskx() const
{
	return mask_x;
}

void Instruction::SetMaskx(const Object &x)
{
	mask_x = x;
}

Object Instruction::GetObjecty() const
{
	return object_y;
}

void Instruction::SetObjecty(const Object &x)
{
	object_y = x;
}

Object Instruction::GetMasky() const
{
	return mask_y;
}

void Instruction::SetMasky(const Object &x)
{
	mask_y = x;
}

Insflag Instruction::GetInsflag() const
{
	return ins_flag;
}

void Instruction::SetInsflag(const Insflag &x) 
{
	ins_flag = x;
}

Object Instruction::GenerateMask(const Object &x)
{
	Object mask;
	mask.Clear();
	ulonglong mask_all = mask.GetAll();

	if (x.GetId())
		mask_all = mask_all | 0xff00000000000000;
	if (x.GetSort())
		mask_all = mask_all | 0x00fc000000000000;
	if (x.GetColor())
		mask_all = mask_all | 0x0003c00000000000;
	if (x.GetSize())
		mask_all = mask_all | 0x0000300000000000;
	if (x.GetType())
		mask_all = mask_all | 0x00000c0000000000;
	if (x.GetLocation())
		mask_all = mask_all | 0x000003f000000000;
	if (x.GetInside())
		mask_all = mask_all | 0x0000000ff0000000;
	if (x.GetPosition())
		mask_all = mask_all | 0x000000000fff0000;
	if (x.GetDoor())
		mask_all = mask_all | 0x000000000000c000;
	if (x.GetPlate())
		mask_all = mask_all | 0x0000000000003000;
	if (x.GetHold())
		mask_all = mask_all | 0x0000000000000c00;

	mask.SetAll(mask_all);

	return mask;
}
	
Priority Instruction::GetPriority() const
{
	return priority;
}

void Instruction::SetPriority(const Priority &x) 
{
	priority = x;
}

bool Instruction::operator<(const Instruction &x) const
{
	return priority > x.priority;
}

void Instruction::Clear() 
{
	keyword = INSKEYWORD_NULL;
	object_x.Clear();
	mask_x.Clear();
	object_y.Clear();
	mask_y.Clear();
	ins_flag = INSFLAG_NULL;
	priority = PRI_LOW;
}


std::ostream & operator<< (std::ostream &os, const Instruction &ins)
{
	Insflag instmp;
	instmp = ins.GetInsflag();
	if (instmp == INFO) {
		os << info << endl;
	} else if (instmp == TASK) {
		os << task << endl;
	} else if (instmp == CONS_NOT_TASK) {
		os << not_task << endl;
	} else if (instmp == CONS_NOT_INFO) {
		os << not_info << endl;
	} else if (instmp == CONS_NOTNOT_INFO) {
		os << notnot_info << endl;
	}
	os << ins.GetObjectx(); 	//十六进制输出
	os << ins.GetObjecty() << endl; 	//十六进制输出

	return os;
}
