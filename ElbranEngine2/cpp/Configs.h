#define GAME_TITLE L"Elbran Engine"
#define MAX_FPS 240.f
#define ASPECT_RATIO 16.f / 9.f
#define MAX_PLAYERS 1

#if defined(DEBUG) | defined(_DEBUG)
#define SHOW_FPS false
#endif

#ifdef WINDOWS
#define START_WINDOW_WIDTH 960
#define START_WINDOW_HEIGHT 540
#endif
