#pragma once
class GameObject;

struct ObjectNode {
	GameObject* object;
	ObjectNode* next;

	~ObjectNode() {
		delete next;
	}
};

class ObjectList {
public:
	ObjectList();
	~ObjectList();

	ObjectNode* GetFirst();
	void AddToFront(GameObject* object);
	void SortInto(GameObject* newObject, bool ascending);
	void Remove(GameObject* element);

private:
	ObjectNode* first;
};

