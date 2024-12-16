#include "Game.h"
#include "Application.h"
#include "Enums.h"
#include "TextRenderer.h"
#include "Menu.h"
#include "AtlasRenderer.h"
#include "SpriteAnimator.h"
#include "HueSwapRenderer.h"
#include "HSVPostProcess.h"

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

	APP->Graphics()->postProcesses.push_back(new HSVPostProcess(0.3f, -1, 0));

	testObject = new GameObject(-20, Color(0, 0.8f, 0.5f, 0.7f), true);
	sampleScene->Add(testObject);

	/*picture = new GameObject(1, assets->testImage, false);
	sampleScene->Add(picture);
	picture->GetTransform()->SetPosition(Vector2(0, -1));*/

	GameObject* photo = new GameObject(0, std::make_shared<Sprite>(L"nature.jpg"), false);
	sampleScene->Add(photo);
	photo->GetTransform()->GrowWidth(9);

	std::shared_ptr<Sprite> crackleSprite = std::make_shared<Sprite>(L"crackle.png");
	
	/*GameObject* spawned = new GameObject(0, crackleSprite, false);
	sampleScene->Add(spawned);
	spawned->GetTransform()->Translate(Vector2(-1, 1));*/

	/*HueSwapRenderer* hueSwapper = new HueSwapRenderer(crackleSprite);
	spawned = new GameObject(0, RenderMode::Opaque, hueSwapper);
	sampleScene->Add(spawned);
	spawned->GetTransform()->Translate(Vector2(1, 1));
	spawned->GetTransform()->SetScale(0.7f, 1.0f);

	hueSwapper->oldHue = Color(1.f, 0.5f, 0);
	hueSwapper->newHue = Color(0.5, 0, 1);
	hueSwapper->sensitivity = 0.3f;*/

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
	
	HSVPostProcess* pp = (HSVPostProcess*)APP->Graphics()->postProcesses[0];
	if(APP->Input()->IsKeyPressed(VK_UP)) {
		pp->brightness += deltaTime;
	}
	if(APP->Input()->IsKeyPressed(VK_DOWN)) {
		pp->brightness -= deltaTime;
	}

	/*if (pp->saturation < -1) {
		pp->saturation = -1;
	}*/

	if(APP->Input()->KeyJustPressed(VK_SPACE)) {
		pp->brightness = 0.f;
		//pp->contrast = 0.f;
		//pp->saturation = 0.f;
	}

	//sampleMenu->Update(deltaTime);
}

void Game::Draw() {
	sampleScene->Draw();
	//sampleMenu->Draw();
}
