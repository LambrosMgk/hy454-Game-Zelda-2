#pragma once
#include <functional>	/*need this for "using Action = std::function<void(void)>;" to work*/
#include "al_init.h"
#include "GameObjectsClasses.h"
#include "Render.h"
#include "User_Input.h"
#include "ProgressAnimations.h"
#include "Physics.h"


namespace app
{
	class Game;

	class App
	{
		protected:
			Game *game;

		public:
			virtual void	Initialise(void) = 0;
			virtual void	Load(void) = 0;
			virtual void	Run(void);
			virtual void	RunIteration(void);
			Game& GetGame(void);
			const Game& GetGame(void) const;
			virtual void	Clear(void) = 0;
			void Main(void);

			App();

			App(const App&) = default;	//otherwise constructor gets deleted and cannot instantiate Game object in the main function
	};

	class Game : public App
	{ // app::Game namespace, the mother application
		public:
			using Action = std::function<void(void)>;
			using Pred = std::function<bool(void)>;
		private:
			Action render, anim, input, ai, physics, destruct, collisions, user;
			Pred done;
			void Invoke(const Action& f) { if (f) f(); }
		public:
			void SetRender(const Action& f);
			void Render(void);

			void SetProgressAnimations(const Action& f);
			void ProgressAnimations(void);

			void SetInput(const Action& f);
			void Input(void);

			void SetAI(const Action& f);
			void AI(void);

			void SetPhysics(const Action& f);
			void Physics(void);

			void SetCollisionChecking(const Action& f);
			void CollisionChecking(void);

			void SetCommitDestructions(const Action& f);
			void CommitDestructions(void);

			void SetUserCode(const Action& f);
			void UserCode(void);

			void Initialise();
			void Load();
			void Clear();

			bool IsFinished(void) const;
			void MainLoop(void);
			void MainLoopIteration(void);
	};
}