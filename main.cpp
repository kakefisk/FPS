#include <iostream>
#include <sstream>
#include <cmath>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

using namespace std;

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;
const int SCREEN_DEPTH = 32;
const int FPS = 60;

SDL_Surface* screen = NULL;
SDL_Surface* txtFPS = NULL;
SDL_Surface* txtX = NULL;
SDL_Surface* txtY = NULL;
SDL_Event event;

class Player
{
    public:
    SDL_Surface* sprite;
    int x;
    int y;
    float speed = 4.0f;
    float xVel = 0;
    float yVel = 0;

    Player(SDL_Surface* _sprite, int _x, int _y)
    {
        sprite = _sprite;
        x = _x;
        y = _y;
    }

    void handle_input(Uint8* keystates)
    {
        if (keystates[SDLK_LEFT])
        {
            xVel = -speed;
        }
        else if (keystates[SDLK_RIGHT])
        {
            xVel = speed;
        }
        else
        {
            xVel = 0;
        }
        if (keystates[SDLK_UP])
        {
            yVel = -speed;
        }
        else if (keystates[SDLK_DOWN])
        {
            yVel = speed;
        }
        else
        {
            yVel = 0;
        }
    }

    void move()
    {
        x += xVel;
        y += yVel;
    }
};

bool SDL_Start()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        return false;
    }
    if (TTF_Init() == -1)
    {
        return false;
    }

    SDL_WM_SetCaption("FPS Test", NULL);

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        return false;
    }
    return true;
}

void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface(source, NULL, destination, &offset);
}

SDL_Surface* load_sprite(string filename)
{
    SDL_Surface* sprite = NULL;
    if (filename != "")
    {
        SDL_Surface* loadedSprite = IMG_Load(filename.c_str());
        if (loadedSprite != NULL)
        {
            sprite = SDL_DisplayFormatAlpha(loadedSprite);
            SDL_FreeSurface(loadedSprite);
        }
        else
        {
        	cout << "Failed to load " << filename << " because " << IMG_GetError() << endl;
        }
    }
    return sprite;
}

void SDL_Exit()
{
    SDL_Quit();
}

string intToString(int i)
{
    ostringstream ss;
    ss << i;
    return ss.str();
}

string floatToString(float f)
{
    ostringstream ss;
    ss << f;
    return ss.str();
}

int main(int argc, char** argv)
{
    if (!SDL_Start())
    {
        return 1;
    }

    Player objPlayer = Player(load_sprite("res/player.png"), 100, 100);
    TTF_Font* font = TTF_OpenFont("res/arial.ttf", 12);

    float previousTime = 0;
    float timeStep = 60/1;

    bool isRunning = true;
    while (isRunning)
    {
        Uint8 *keystates = SDL_GetKeyState(NULL);

        float currentTime = SDL_GetTicks();
        if (previousTime == 0 || previousTime > currentTime)
        previousTime = currentTime;

        while (currentTime - previousTime > timeStep)
        {
            previousTime += timeStep;

            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    isRunning = false;
                }
            }

            objPlayer.handle_input(keystates);
            objPlayer.move();
        }

        SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 100, 100, 100));

        // Trying to draw FPS
        static float lastFrame = SDL_GetTicks();
        float curFrame = SDL_GetTicks();
        static float avgFPS = 0.0f;
        avgFPS = avgFPS * 0.95f + (1.0f / (curFrame - lastFrame)) * 0.05f;
        lastFrame = curFrame;
        txtFPS = TTF_RenderText_Blended(font, floatToString(avgFPS).c_str(), {255, 255, 0});
        apply_surface(8, 8, txtFPS, screen);

        txtX = TTF_RenderText_Blended(font, intToString(objPlayer.x).c_str(), {255, 255, 0});
        apply_surface(8, 24, txtX, screen);
        txtY = TTF_RenderText_Blended(font, intToString(objPlayer.y).c_str(), {255, 255, 0});
        apply_surface(8, 40, txtY, screen);

        apply_surface(objPlayer.x, objPlayer.y, objPlayer.sprite, screen);

        if (SDL_Flip(screen) == -1)
        {
            return 2;
        }
    }

    return 0;
}
