#include "..\\Headers\\App.h"

void app::App::Main(void) {
	Initialise();
	Load();
	Run();
	Clear();
}

app::App::App()
{

}

void app::App::Run(void) { game->MainLoop(); }
void app::App::RunIteration(void) { game->MainLoopIteration(); }
app::Game& app::App::GetGame(void) { return *game; }
const app::Game& app::App::GetGame(void) const { return *game; }

//Class Game

void app::Game::SetRender(const Action& f) { render = f; }
void app::Game::Render(void) { Invoke(render); }

void app::Game::SetProgressAnimations(const Action& f) { anim = f; }
void app::Game::ProgressAnimations(void) { Invoke(anim); }

void app::Game::SetInput(const Action& f) { input = f; }
void app::Game::Input(void) { Invoke(input); }

void app::Game::SetAI(const Action& f) { ai = f; }
void app::Game::AI(void) { Invoke(ai); }

void app::Game::SetPhysics(const Action& f) { physics = f; }
void app::Game::Physics(void) { Invoke(physics); }

void app::Game::SetCollisionChecking(const Action& f) { collisions = f; }
void app::Game::CollisionChecking(void) { Invoke(collisions); }

void app::Game::SetCommitDestructions(const Action& f) { destruct = f; }
void app::Game::CommitDestructions(void) { Invoke(destruct); }

void app::Game::SetUserCode(const Action& f) { user = f; }
void app::Game::UserCode(void) { Invoke(user); }

bool app::Game::IsFinished(void) const { return done(); }

void app::Game::Initialise()
{
	SetRender(&Renderer);
	SetProgressAnimations(&Animator);
	SetInput(&UserInput);
	SetAI(&Calculate_AI);
	SetPhysics(&Calculate_Physics);
	SetCollisionChecking(&CheckCollisions);
	//SetCommitDestructions();
	//SetUserCode();

	allegro_startup();	//from custom header file al_init.h
	Render_init();
	User_Input_init();
	Animator_Init();
	Physics_Init();
	Collisions_init();
	AI_Init();


	Game::done = isDone;
	game = this;
}

void app::Game::Load()
{
	Load_Start_Screen();
	gameObj.Play_Music(START_SCREEN_MUSIC);
}

void app::Game::Clear()
{
	Render_Clear();
	al_rest(1.0);
	al_destroy_display(gameObj.display);
}

void app::Game::MainLoop(void)
{
	while (!IsFinished())	//maybe remove the fps queues from the other files and merge them here under a single fps timer
		MainLoopIteration();
}

void app::Game::MainLoopIteration(void)
{
	Render();
	Input();
	ProgressAnimations();
	AI();
	Physics();
	CollisionChecking();
	//UserCode(); // hook for custom code at end
	//CommitDestructions();
}