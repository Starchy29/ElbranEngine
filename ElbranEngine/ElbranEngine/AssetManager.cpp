#include "AssetManager.h"

AssetManager* AssetManager::instance;

AssetManager* AssetManager::GetInstance() {
    if(instance == nullptr) {
        instance = new AssetManager();
    }
    return instance;
}

AssetManager::AssetManager() {

}

AssetManager::~AssetManager() {

}
