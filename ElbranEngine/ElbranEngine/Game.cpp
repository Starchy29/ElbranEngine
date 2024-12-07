#include "Game.h"
#include "Application.h"
#include "Enums.h"
#include "TextRenderer.h"
#include "Menu.h"
#include "AtlasRenderer.h"

void Start(Button* clicked) {
	clicked->GetLabel()->GetRenderer<TextRenderer>()->text = "started :)";
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

	testObject = new GameObject(-20, Color(0, 0.8f, 0.5f, 0.7f), true);
	sampleScene->Add(testObject);

	picture = new GameObject(1, assets->testImage, false);
	sampleScene->Add(picture);
	picture->GetTransform()->SetPosition(Vector2(0, -2));

	std::shared_ptr<SpriteAtlas> grid = std::make_shared<SpriteAtlas>(L"testAtlas.png", 2, 2);
	GameObject* atlas = new GameObject(0, RenderMode::Opaque, new AtlasRenderer(grid));
	sampleScene->Add(atlas);
	//atlas->GetRenderer<AtlasRenderer>()->row = 1;
	atlas->GetRenderer<AtlasRenderer>()->col = 1;
	atlas->GetRenderer<AtlasRenderer>()->flipX = true;
	atlas->GetRenderer<AtlasRenderer>()->flipY = true;
	atlas->GetRenderer<AtlasRenderer>()->tint = Color(0.3f, 0.4f, 0.5f);

	sampleMenu = new Menu(Color(0.1f, 0.2f, 0.4f));
	Button* startButton = new Button(Start, Color::White, Color::Blue, Color::Black, "Start");
	sampleMenu->Add(startButton);

	Button* quitButton = (Button*)startButton->Clone();
	quitButton->GetLabel()->GetRenderer<TextRenderer>()->text = "Quit";
	quitButton->GetTransform()->Translate(Vector2(0, -10));
	quitButton->clickFunc = Quit;
}

Game::~Game() {
	delete sampleScene;
	delete sampleMenu;
}

void Game::Update(float deltaTime) {
	testObject->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));

	//sampleMenu->Update(deltaTime);
}

void Game::Draw() {
	sampleScene->Draw();
	//sampleMenu->Draw();
}
