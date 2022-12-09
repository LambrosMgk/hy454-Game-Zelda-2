#pragma once
#include <functional>	/*need this for "using Action = std::function<void(void)>;" to work*/
#include "Render.h"
#include "User_Input.h"

namespace app
{
	class App
	{
		protected:
			Game game;

		public:
			virtual void	Initialise(void) = 0;
			virtual void	Load(void) = 0;
			virtual void	Run(void) { game.MainLoop(); }
			virtual void	RunIteration(void)
			{
				game.MainLoopIteration();
			}
			Game& GetGame(void) { return game; }
			const Game& GetGame(void) const { return game; }
			virtual void	Clear(void) = 0;
			void Main(void) {
				Initialise();
				Load();
				Run();
				//Clear();
			}
	};

	class Game : App
	{ // app::Game namespace, the mother application
		public:
			using Action = std::function<void(void)>;
			using Pred = std::function<bool(void)>;
		private:
			Action render, anim, input, ai, physics, destruct, collisions, user;
			Pred done;
			void Invoke(const Action& f) { if (f) f(); }
		public:
			void SetRender(const Action& f) { render = f; } // rest of setters are similary defined
			void Render(void) { Invoke(render); }

			void SetProgressAnimations(const Action& f) { anim = f; }
			void ProgressAnimations(void) { Invoke(anim); }

			void SetInput(const Action& f) { input = f; }
			void Input(void) { Invoke(input); }

			void SetAI(const Action& f) { ai = f; }
			void AI(void) { Invoke(ai); }

			void SetPhysics(const Action& f) { physics = f; }
			void Physics(void) { Invoke(physics); }

			void SetCollisionChecking(const Action& f) { collisions = f; }
			void CollisionChecking(void) { Invoke(collisions); }

			void SetCommitDestructions(const Action& f) { destruct = f; }
			void CommitDestructions(void) { Invoke(destruct); }

			void SetUserCode(const Action& f) { user = f; }
			void UserCode(void) { Invoke(user); }

			void Initialise();
			void Load();
			void Clear();

			bool IsFinished(void) const { return !done(); }
			void MainLoop(void);
			void MainLoopIteration(void);
	};

	void Game::Initialise()
	{
		SetRender(&Renderer);
		Render_init();
		//SetProgressAnimations();		fill these later
		SetInput(&UserInput);
		User_Input_init();
		//SetAI();
		//SetPhysics();
		//SetCollisionChecking();
		//SetCommitDestructions();
		//SetUserCode();

		done = &isDone;
	}

	void Game::Load()
	{

	}

	void Game::Clear()
	{

	}
	
	void Game::MainLoop(void) 
	{
		while (!IsFinished())
			MainLoopIteration();
	}

	void Game::MainLoopIteration(void)
	{
		Render();
		Input();
		//ProgressAnimations();
		//AI();
		//Physics();
		//CollisionChecking();
		//UserCode(); // hook for custom code at end
		//CommitDestructions();
	}


}