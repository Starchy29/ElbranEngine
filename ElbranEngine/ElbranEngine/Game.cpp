#include "Game.h"
#include "Application.h"
#include "Enums.h"

#include "TextRenderer.h"
#include "Menu.h"
#include "AtlasRenderer.h"
#include "SpriteAnimator.h"
#include "HueSwapRenderer.h"
#include "BlurPostProcess.h"
#include "HSVPostProcess.h"
#include "BloomPostProcess.h"

void Start(Button* clicked) {
	clicked->GetLabel()->GetRenderer<TextRenderer>()->text = "started :/";
}

void Quit(Button* clicked) {
	APP->Quit();
}

Game::Game(AssetManager* assets) {
	// load assets
	assets->arial = assets->LoadFont(L"Arial.spritefont");
	assets->testImage = std::make_shared<Sprite>(L"temp sprite.png");

	// set up scenes
	sampleScene = new Scene(10, Color(0.1f, 0.1f, 0.1f));

	
	//APP->Graphics()->postProcesses.push_back(new HSVPostProcess(0.3, -1, -0.2));
	APP->Graphics()->postProcesses.push_back(new BloomPostProcess(0.7f, 50));
	//APP->Graphics()->postProcesses.push_back(new BlurPostProcess(10));

	testObject = new GameObject(-20, Color(0.3f, 1.f, 0.5f, 1.f), true);
	sampleScene->Add(testObject);

	/*picture = new GameObject(1, assets->testImage, false);
	sampleScene->Add(picture);
	picture->GetTransform()->SetPosition(Vector2(0, -1));*/

	GameObject* photo = new GameObject(0, std::make_shared<Sprite>(L"nature.jpg"), false);
	sampleScene->Add(photo);
	photo->GetTransform()->GrowWidth(9);

	// menu
	sampleMenu = new Menu(Color(0.1f, 0.2f, 0.4f));

	Button* startButton = new Button(Start, Color::White, Color::Blue, Color::Black, "Start");
	sampleMenu->Add(startButton);

	Button* quitButton = (Button*)startButton->Clone();
	quitButton->GetLabel()->GetRenderer<TextRenderer>()->text = "Quit";
	quitButton->GetTransform()->Translate(Vector2(0, -10));
	quitButton->clickFunc = Quit;

	GameObject* testTitle = new GameObject(0, "Game Title", assets->arial, Color::White);
	sampleMenu->Add(testTitle);
	testTitle->GetTransform()->Translate(Vector2(0, 10.0f));
	testTitle->GetTransform()->SetScale(20, 4);
	GameObject* titleBack = new GameObject(1, Color(0.3f, 0.3f, 0.3f));
	titleBack->SetParent(testTitle);
}

Game::~Game() {
	delete sampleScene;
	delete sampleMenu;
}

void Game::Update(float deltaTime) {
	testObject->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));
	sampleScene->Update(deltaTime);

	//sampleMenu->Update(deltaTime);
}

void Game::Draw() {
	sampleScene->Draw();
	//sampleMenu->Draw();
}
