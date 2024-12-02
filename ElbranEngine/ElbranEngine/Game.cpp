#include "Game.h"
#include "Application.h"
#include "Enums.h"
#include "TextRenderer.h"
#include "Menu.h"

void Start(Button* clicked) {
	clicked->GetLabel()->GetRenderer<TextRenderer>()->text = "started :)";
}

void Quit(Button* clicked) {
	APP->Quit();
}

GameObject* text;
Game::Game(AssetManager* assets) {
	// load assets
	assets->arial = assets->LoadFont(L"Arial.spritefont");
	assets->testImage = std::make_shared<Sprite>(L"temp sprite.png");

	// set up game objects
	sampleScene = new Scene(10, Color(0.1f, 0.1f, 0.1f));

	testObject = new GameObject(-20, Color(0, 0.8f, 0.5f, 0.7f), true);
	sampleScene->Add(testObject);

	picture = new GameObject(1, assets->testImage, false);
	sampleScene->Add(picture);
	picture->GetTransform()->SetPosition(Vector2(0, -2));

	text = new GameObject(0, "Hello is there text here? what about there?", assets->arial, Color::White);
	text->GetTransform()->SetScale(4, 0.5f);
	text->GetRenderer<TextRenderer>()->horizontalAlignment = Direction::Left;
	GameObject* textBack = new GameObject(1, Color::Cyan);
	textBack->SetParent(text);
	sampleScene->Add(text);


	sampleMenu = new Menu(Color(0.1f, 0.2f, 0.4f));
	Button* startButton = new Button(Start, Color::White, Color::Blue, Color::Black, "Start");
	sampleMenu->AddButton(startButton);

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
	//text->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));
	testObject->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));
	//text->GetTransform()->Rotate(2*deltaTime);
	if(APP->Input()->IsPressed(VK_UP)) {
		text->GetTransform()->Stretch(1.5f * deltaTime, 0.0f);
	}
	else if (APP->Input()->IsPressed(VK_DOWN)) {
		text->GetTransform()->Stretch(-1.5f * deltaTime, 0.0f);
	}

	sampleMenu->Update(deltaTime);
}

void Game::Draw() {
	//sampleScene->Draw();
	sampleMenu->Draw();
}
