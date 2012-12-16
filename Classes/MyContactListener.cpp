#include "MyContactListener.h"

MyContactListener::MyContactListener() : _contacts(){
}

MyContactListener::~MyContactListener(){
}

void MyContactListener::BeginContact(b2Contact *contact){
	//b2Contact는 box2d가 재사용하기 때문에 복사를 해놔야 한다.
	MyContact myContact={contact->GetFixtureA(), contact->GetFixtureB()};
	_contacts.push_back(myContact);
}

//BeginContact에서는 방금 충돌한 fixture들의 복사본을 만들고 벡터에 그것들을 저장한다.
//EndContact에서는 contact point가 벡터 내에 있는지 확인하고 존재하면 그것을 삭제한다.

void MyContactListener::EndContact(b2Contact *contact){
	MyContact myContact={contact->GetFixtureA(), contact->GetFixtureB()};
	std::vector<MyContact>::iterator pos;
	pos=std::find(_contacts.begin(), _contacts.end(), myContact);
	if(pos!=_contacts.end()){
		_contacts.erase(pos);
	}
}

void MyContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold){
}

void MyContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse){
}