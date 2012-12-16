#include "Box2D.h"
#include <vector>
#include <algorithm>

struct MyContact{
	b2Fixture *fixtureA;
	b2Fixture *fixtureB;
	//find() 메소드를 사용하기 위해 vector에서 서로 일치하는 객체들을 찾는 메소드를 선언함.
	bool operator==(const MyContact& other) const{
		return (fixtureA == other.fixtureA) && (fixtureB == other.fixtureB);
	}
};

//contact listener는 우리가 box2d에 제공하는 C++ 객체이다. 두 개의 객체들이 터치되기 시작하거나
//터치되는 것이 멈췄을 떄를 알게 해주는 메소드이다.
//callback 내에서 물리엔진을 변경할 수 없기 때문에 contact listener가 필요하다. 
//이러하기 떄문에 우리가 원하는 두 개의 겍차가 충돌했을 때 객체가 사라지는 동작을 구현하려면
//그 부분을 나중에 처리할 수 있도록 reference를 보관 해놔야 한다.
//그리고 listener에 보내지는 contact point의 reference를 저장할 수 없다. 왜냐하면 box2d에 의해 재사용되기 때문이다.
//그래서 그 reference의 복사본을 저장해야만 한다.

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
	