#include "Box2D.h"
#include <vector>
#include <algorithm>

struct MyContact{
	b2Fixture *fixtureA;
	b2Fixture *fixtureB;
	//find() �޼ҵ带 ����ϱ� ���� vector���� ���� ��ġ�ϴ� ��ü���� ã�� �޼ҵ带 ������.
	bool operator==(const MyContact& other) const{
		return (fixtureA == other.fixtureA) && (fixtureB == other.fixtureB);
	}
};

//contact listener�� �츮�� box2d�� �����ϴ� C++ ��ü�̴�. �� ���� ��ü���� ��ġ�Ǳ� �����ϰų�
//��ġ�Ǵ� ���� ������ ���� �˰� ���ִ� �޼ҵ��̴�.
//callback ������ ���������� ������ �� ���� ������ contact listener�� �ʿ��ϴ�. 
//�̷��ϱ� ������ �츮�� ���ϴ� �� ���� ������ �浹���� �� ��ü�� ������� ������ �����Ϸ���
//�� �κ��� ���߿� ó���� �� �ֵ��� reference�� ���� �س��� �Ѵ�.
//�׸��� listener�� �������� contact point�� reference�� ������ �� ����. �ֳ��ϸ� box2d�� ���� ����Ǳ� �����̴�.
//�׷��� �� reference�� ���纻�� �����ؾ߸� �Ѵ�.

class MyContactListener:public b2ContactListener{
public:
	std::vector<MyContact> _contacts;

	MyContactListener();
	~MyContactListener();

	virtual void BeginContact(b2Contact *contact);
	virtual void EndContact(b2Contact *contact);
	virtual void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
	virtual void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);
}
	