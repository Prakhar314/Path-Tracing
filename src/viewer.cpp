#include "viewer.hpp"

Viewer::Viewer(const int width, const int height, const std::string& title): width(width), height(height){
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    throw "SDL could not initialize!";
  }
  else
  {
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
      throw "Window could not be created!";
    }
    else
    {
      windowSurface = SDL_GetWindowSurface(window);
      framebuffer = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    }
  }
}

void Viewer::update(glm::uvec3 **renderedOutput){
  SDL_PixelFormat *format = framebuffer->format;
  Uint32* pixels = (Uint32*)framebuffer->pixels;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      glm::uvec3 pixel = renderedOutput[i][j];
      pixels[i + (height - 1 - j) * width] = SDL_MapRGB(format, pixel.r, pixel.g, pixel.b);
    } 
  }
  SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
  SDL_UpdateWindowSurface(window);
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0)
  {
    if (e.type == SDL_QUIT)
    {
      quit = true;
    }
  }
}

bool Viewer::shouldQuit(){
  return quit;
}
