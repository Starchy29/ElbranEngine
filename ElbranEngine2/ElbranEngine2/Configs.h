#define GAME_TITLE "Elbran Engine"
#define MAX_FPS 60.f
#define ASPECT_RATIO 16.f / 9.f

#if defined(DEBUG) | defined(_DEBUG)
#define SHOW_FPS true
#endif

#ifdef WINDOWS
#define START_WINDOW_WIDTH 960
#define START_WINDOW_HEIGHT 600
#endif
