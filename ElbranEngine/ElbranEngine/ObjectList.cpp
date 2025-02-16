#include "ObjectList.h"
#include "GameObject.h"

ObjectList::ObjectList() {
	first = nullptr;
}

ObjectList::~ObjectList() {
	delete first;
}

ObjectNode* ObjectList::GetFirst() {
	return first;
}

void ObjectList::AddToFront(GameObject* object) {
	ObjectNode* newNode = new ObjectNode{ object, first };
	first = newNode;
}

// places into the list based on ascending z coordinate order
void ObjectList::SortInto(GameObject* newObject, bool ascending) {
	float newZ = newObject->GetTransform()->GetGlobalZ();
	if(first == nullptr || (ascending ? newZ <= first->object->GetTransform()->GetGlobalZ() : newZ >= first->object->GetTransform()->GetGlobalZ())) {
		AddToFront(newObject);
		return;
	}

	
	ObjectNode* insertSpot = first;
	while(insertSpot->next != nullptr && (ascending ? newZ > insertSpot->next->object->GetTransform()->GetGlobalZ() : newZ < insertSpot->next->object->GetTransform()->GetGlobalZ())) {
		insertSpot = insertSpot->next;
	}

	ObjectNode* newNode = new ObjectNode{ newObject, insertSpot->next };
	insertSpot->next = newNode;
}

void ObjectList::Remove(GameObject* element) {
	ObjectNode* removal;
	if(element == first->object) {
		removal = first;
		first = first->next;
		removal->next = nullptr; // don't recursively delete the rest of the elements
		delete removal;
		return;
	}

	ObjectNode* node = first;
	while(node->next->object != element) {
		node = node->next;
	}

	removal = node->next;
	node->next = removal->next;
	removal->next = nullptr; // don't recursively delete the rest of the elements
	delete removal;
}
