#pragma once

#include <list>
#include <map>
#include "SDL.h"

namespace Input {
	class SDLListener {
	protected:
		SDL_Event m_lastEvent;
	public:
		SDLListener() { memset(&m_lastEvent, 0x0, sizeof(SDL_Event)); };
		virtual ~SDLListener() {};

		virtual void OnWork(void) {};

		virtual void OnEvent(SDL_Event& event) { m_lastEvent = event; };

		void ClearLastEvent(void) { memset(&m_lastEvent, 0x0, sizeof(SDL_Event)); };

		const SDL_Event GetLastEvent(void) { return m_lastEvent; };

		virtual bool IsKeyDown(void) { return false; };

		virtual bool IsKeyUp(void) { return false; };
	};

	class SDLBackend {
	private:
		static SDLBackend* m_sSDLBackend;
		SDL_Thread *m_sdlthread;
	public:
		SDL_mutex* m_sdlmutex;
		std::list<SDLListener*> m_listeners;
		bool m_isExit;
	private:
		SDLBackend();
		virtual ~SDLBackend();

	public:
		static SDLBackend* GetInstance(void);
		void Start();
		void Stop();
		void regListener(SDLListener* listener);
		void unregListener(SDLListener* listener);
	};
}