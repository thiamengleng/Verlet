
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include <memory>
#include <vector>
#include <chrono>

#define MAX_SLICES 24
/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

struct VerletObject {
    float radius;
    SDL_FPoint position;
    SDL_FPoint previousPosition;
    SDL_FPoint acceleration;
    void SimulateGravity(float dt);
    void CalculatePosition(float dt);
    void Simulate(float dt);
    VerletObject(float x, float y, float r) : radius(r) {
        position.x = x;
        position.y = y;
        previousPosition.x = x;
        previousPosition.y = y;
    };
} typedef VerletObject;

struct VerletObjects {
    std::vector<std::shared_ptr<VerletObject>> objects;

} typedef VerletObjects;

void DrawCircle(SDL_Renderer* renderer, SDL_FPoint, int radius);
int roundUpToMultipleOfEight(int v);

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", 640, 480, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}
/* This function runs once per frame, and is the heart of the program. */

SDL_AppResult SDL_AppIterate(void *appstate)
{
    static uint64_t currentTicks, frameTime;
    static std::shared_ptr<VerletObject> aa = std::make_shared<VerletObject>(300, 100, static_cast<float>(150));
	SDL_FRect rect;
	float red = 0.2, green = 0.3, blue=0.6;
	rect.x = rect.y = 100;
	rect.h = 400;
	rect.w = 400;
    
    aa->Simulate(frameTime/1000.0f);

	SDL_SetRenderDrawColor(renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);  /* dark gray, full alpha */
    SDL_RenderClear(renderer);
	
    SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */
	SDL_RenderFillRect(renderer, &rect);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);  /* yellow, full alpha */
    SDL_RenderDebugText(renderer, 200, 200, std::to_string(aa->position.y).c_str());
    SDL_RenderDebugText(renderer, 200, 300, std::to_string(frameTime).c_str());
    DrawCircle(renderer, aa->position, aa->radius);
    /* clear the window to the draw color. */
    

    /* put the newly-cleared rendering on the screen. */
    frameTime = SDL_GetTicks() - currentTicks;
    currentTicks = SDL_GetTicks();
    
    SDL_RenderPresent(renderer);
    SDL_Delay(2);
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}
int roundUpToMultipleOfEight(int v) {
    return (v + (8 - 1)) & -8;
}

void VerletObject::CalculatePosition(float dt) {
    SDL_FPoint newCurrent;
    newCurrent.x = 2 * position.x - previousPosition.x + acceleration.x * dt * dt;
    newCurrent.y = 2 * position.y - previousPosition.y + acceleration.y * dt * dt;
    
    previousPosition.y = position.y;
    previousPosition.x = position.x;
    position.x = newCurrent.x;
    position.y = newCurrent.y;
    acceleration = (SDL_FPoint){0, 0};
    return;
}

void VerletObject::SimulateGravity(float dt) {
    acceleration.x += 0;
    acceleration.y += 400.0f * dt;
}

void VerletObject::Simulate(float dt) {
    SimulateGravity(dt);
    CalculatePosition(dt);
}

void DrawCircle(SDL_Renderer* renderer, SDL_FPoint center, int radius){
    // 35 / 49 is a slightly biased approximation of 1/sqrt(2)
    const int arrSize = roundUpToMultipleOfEight( radius * 8 * 35 / 49 );
    SDL_FPoint points[arrSize];
    int       drawCount = 0;

    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);
    
    while( x >= y )
    {
        // Each of the following renders an octant of the circle
        points[drawCount+0] = { center.x + x, center.y - y };
        points[drawCount+1] = { center.x + x, center.y + y };
        points[drawCount+2] = { center.x - x, center.y - y };
        points[drawCount+3] = { center.x - x, center.y + y };
        points[drawCount+4] = { center.x + y, center.y - x };
        points[drawCount+5] = { center.x + y, center.y + x };
        points[drawCount+6] = { center.x - y, center.y - x };
        points[drawCount+7] = { center.x - y, center.y + x };

        drawCount += 8;

        if( error <= 0 )
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if( error > 0 )
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
    SDL_RenderPoints( renderer, points, drawCount );
}

