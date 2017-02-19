#include "object.hpp"

Object::Object()
{
	object_all = 0; 		//所有位都置0, it's so easy!
}

Object::~Object()
{
	//nothing
}

const uint Object::GetId() const
{
	/*
	 * id: 63~56 8bit
	 *
	 * 64 - 8 = 56
	 */
	ulonglong tmp = (object_all & 0xff00000000000000) >> 56;

	return uint(tmp); 		//强制类型转换: ulonglong -> uint
}

void Object::SetId(const uint x)
{
	/*
	 * 先将对应的值置0，然后再设置。
	 *
	 * 例如:
	 * 0001 0011 -> 0100 0011:
	 *
	 * 0001 0011 -> 0000 0011 -> 0100 0011
	 *
	 */
	object_all = object_all & 0x00ffffffffffffff;
	object_all = object_all | (ulonglong(x) << 56); 		//打开id对应的位
}

const uint Object::GetSort() const
{
	/*
	 * sort: 55~50 6bit
	 *
	 * 56 - 6 = 50
	 */
	ulonglong tmp = (object_all & 0x00fc000000000000) >> 50;

	return uint(tmp);
}

void Object::SetSort(const uint x) 
{
	object_all = object_all & 0xff03ffffffffffff;
	object_all = object_all | (ulonglong(x) << 50); 		//打开sort对应的位
}

const uint Object::GetColor() const
{
	/*
	 * color: 49~46 4bits
	 *
	 * 50 - 4 = 46
	 */
	ulonglong tmp = (object_all & 0x0003c00000000000) >> 46;

	return uint(tmp);
}

void Object::SetColor(const uint x) 
{
	object_all = object_all & 0xfffc3fffffffffff;
	object_all = object_all | (ulonglong(x) << 46); 		//打开color对应的位
}

const uint Object::GetSize() const
{
	/*
	 * size: 45~44 2bits
	 *
	 * 46 - 2 = 44
	 */
	ulonglong tmp = (object_all & 0x0000300000000000) >> 44;

	return uint(tmp);
}

void Object::SetSize(const uint x)
{
	object_all = object_all & 0xffffcfffffffffff;
	object_all = object_all | (ulonglong(x) << 44); 		//打开size对应的位
}

const uint Object::GetType() const
{
	/*
	 * type: 43~42 2bits
	 *
	 * 44 - 2 = 42
	 */
	ulonglong tmp = (object_all & 0x00000c0000000000) >> 42;

	return uint(tmp);
}

void Object::SetType(const uint x)
{
	object_all = object_all & 0xfffff3ffffffffff;
	object_all = object_all | (ulonglong(x) << 42); 		//打开type对应的位
}

const uint Object::GetLocation() const
{
	/*
	 * location: 41~36 6bits
	 *
	 * 42 - 6 = 36
	 */
	ulonglong tmp = (object_all & 0x000003f000000000) >> 36;

	return uint(tmp);
}

void Object::SetLocation(const uint x)
{
	object_all = object_all & 0xfffffc0fffffffff;
	object_all = object_all | (ulonglong(x) << 36); 		//打开location对应的位
}

const uint Object::GetInside() const
{
	/*
	 * inside: 35~28 8bits
	 *
	 * 36 - 8 = 28
	 */
	ulonglong tmp = (object_all & 0x0000000ff0000000) >> 28;

	return uint(tmp);
}

void Object::SetInside(const uint x)
{
	object_all = object_all & 0xfffffff00fffffff;
	object_all = object_all | (ulonglong(x) << 28); 		//打开inside位
}

const uint Object::GetPosition() const
{
	/*
	 * position: 27~16;  on,near 27~24; 23~16 id
	 *
	 * 28 - 12 = 16
	 */
	//ulonglong tmp = (object_all & 0x000000000ffc0000) >> 16;
	ulonglong tmp = (object_all & 0x000000000fff0000) >> 16;      

	return uint(tmp);
}

void Object::SetPosition(const uint x)
{
	object_all = object_all & 0xfffffffff000ffff;
	object_all = object_all | (ulonglong(x) << 16); 		//打开position位
}

const uint Object::GetDoor() const
{
	/*
	 * door: 15~14
	 *
	 * 16 - 2 = 14
	 */
	ulonglong tmp = (object_all & 0x000000000000c000) >> 14;

	return uint(tmp);
}

void Object::SetDoor(const uint x)
{
	object_all = object_all & 0xffffffffffff3fff;
	object_all = object_all | (ulonglong(x) << 14); 		//打开door位
}

const uint Object::GetPlate() const
{
	/*
	 * plate: 13~12
	 *
	 * 14 - 2 = 12
	 */
	ulonglong tmp = (object_all & 0x0000000000003000) >> 12;

	return uint(tmp);
}

void Object::SetPlate(const uint x)
{
	object_all = object_all & 0xffffffffffffcfff;
	object_all = object_all | (ulonglong(x) << 12); 		//打开plate位
}

const uint Object::GetHold() const
{
	/*
	 * hold: 11~10
	 *
	 * 12 - 2 = 10
	 */
	ulonglong tmp = (object_all & 0x0000000000000c00) >> 10;    //1100 0000 0000

	return uint(tmp);
}

void Object::SetHold(const uint x)
{
	object_all = object_all & 0xfffffffffffff3ff;                     //0011 1111 1111
	object_all = object_all | (ulonglong(x) << 10); 		//打开hold位
}


//雷政
const uint Object::	GetPriority() const
{
	/*
	 * hold: 9~7
	 *
	 * 10 - 3  = 7
	 */
	ulonglong tmp = (object_all & 0x0000000000000380) >> 7;   //0011 1000 0000

	return uint(tmp);
}

//雷政
void Object::SetPriority(const uint x)
{
	object_all = object_all & 0xfffffffffffffc7f;                         //1100 0111 1111
	object_all = object_all | (ulonglong(x)<<7);
}


const ulonglong Object::GetAll() const 		//获得全部的信息
{
	return object_all;
}

void Object::SetAll(const ulonglong x)
{
	object_all = x;
}

void Object::Clear()
{
	object_all = 0; 			// 所有位都置0
}

//雷政
bool Object::operator<(const Object &x) const               //按照物体处理优先级递减排列
{
	return GetPriority() > x.GetPriority();
}

std::ostream & operator<< (std::ostream &os, const Object &obj)
{
	os << "id:" << obj.GetId() << "\t";
	os << "sort:" << obj.GetSort() << "\t"; 		
	os << "color:" << obj.GetColor() << "  ";
	os << "size:" << obj.GetSize() << "  ";
	os << "type:" << obj.GetType() << "  ";
	os << "location:" << obj.GetLocation() << "  ";
	os << "inside:" << obj.GetInside() << "  ";
	os << "position:" << obj.GetPosition() << "  ";
	os << "door:" << obj.GetDoor() << "  ";
	os << "plate:" << obj.GetPlate() << "  ";
	os << "hold:" << obj.GetHold() << endl;
	return os;
}

