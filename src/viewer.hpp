#include <string>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "glm/glm.hpp"

using namespace std;
class Viewer
{
public:
    Viewer(const int width, const int height, const std::string& title);
    void update(glm::uvec3** framebuffer);
    bool shouldQuit();
private:
    const int width, height;
    bool quit = false;
    SDL_Window* window;
    SDL_Surface* windowSurface;
    SDL_Surface* framebuffer;
};
