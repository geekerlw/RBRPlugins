#include "input.h"

namespace Input {

SDLBackend *SDLBackend::m_sSDLBackend = nullptr;
SDLBackend* SDLBackend::GetInstance(void) {
	if (m_sSDLBackend == nullptr) {
		m_sSDLBackend = new SDLBackend();
	}
	return m_sSDLBackend;
}

SDLBackend::SDLBackend(void) {
	m_listeners.clear();
	m_isExit = false;
	m_sdlthread = nullptr;
	m_sdlmutex = SDL_CreateMutex();
}

SDLBackend::~SDLBackend(void) {
	Stop();
	SDL_DestroyMutex(m_sdlmutex);
	m_listeners.clear();
}

static int SDLBackendThread(void* arg) {
	SDLBackend* pthis = static_cast<SDLBackend*>(arg);
	if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
		return -1;
	}

	SDL_JoystickEventState(SDL_ENABLE);

	// Open all connected joysticks
	std::map<int, SDL_Joystick*> joys;
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		joys[i] = SDL_JoystickOpen(i);
	}

	while (!pthis->m_isExit) {
		SDL_LockMutex(pthis->m_sdlmutex);
		std::list<SDLListener*> templist = pthis->m_listeners;
		SDL_UnlockMutex(pthis->m_sdlmutex);

		SDL_Event event;
		while (SDL_WaitEventTimeout(&event, 20))
		{
			std::list<SDLListener*>::iterator iter = templist.begin();
			while (iter != templist.end()) {
				(*iter)->OnEvent(event);
				iter++;
			}
		}

		std::list<SDLListener*>::iterator iter = templist.begin();
		while (iter != templist.end()) {
			(*iter)->OnWork();
			iter++;
		}

	}

	std::map<int, SDL_Joystick*>::iterator iter = joys.begin();
	while (iter != joys.end()) {
		SDL_JoystickClose(iter->second);
		iter++;
	}
	SDL_Quit();
	return 0;
}

void SDLBackend::Start(void) {
	m_sdlthread = SDL_CreateThread(SDLBackendThread, "SDLBackend", this);
	SDL_DetachThread(m_sdlthread);
}

void SDLBackend::Stop() {
	m_isExit = true;
	if (m_sdlthread) {
		m_sdlthread = nullptr;
	}
}

void SDLBackend::regListener(SDLListener* listener) {
	SDL_LockMutex(m_sdlmutex);
	std::list<SDLListener*>::iterator iter = m_listeners.begin();
	while (iter != m_listeners.end()) {
		if (*iter == listener) {
			SDL_UnlockMutex(m_sdlmutex);
			return; // make sure register only once.
		}
		iter++;
	}
	m_listeners.push_back(listener);
	SDL_UnlockMutex(m_sdlmutex);
}

void SDLBackend::unregListener(SDLListener* listener) {
	SDL_LockMutex(m_sdlmutex);
	std::list<SDLListener*>::iterator iter = m_listeners.begin();
	while (iter != m_listeners.end()) {
		if (*iter == listener) {
			iter = m_listeners.erase(iter);
			SDL_UnlockMutex(m_sdlmutex);
			return;
		}
		iter++;
	}
	SDL_UnlockMutex(m_sdlmutex);
}

};// namespace Input