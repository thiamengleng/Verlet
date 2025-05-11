
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include <memory>
#include <cmath>
#include <vector>
#include <chrono>
#include <random>
#include "Vec2Math.h"

#define MAX_SLICES 24
#define M_PI 3.14159265358979323846

struct VerletObject {
    float radius;
    SDL_FPoint position;
    SDL_FPoint previousPosition;
    SDL_FPoint acceleration;
    void SimulateGravity(float dt);
    void CalculatePosition(float dt);
    void ApplyConstraint(SDL_FPoint center, float radius);
    
    VerletObject(float x, float y, float r) : radius(r) {
        position.x = x;
        position.y = y;
        previousPosition.x = x;
        previousPosition.y = y;
    };
} typedef VerletObject;

struct VerletObjects {
    std::vector<std::shared_ptr<VerletObject>> objects;
    void SimulateObjects(float dt);
    void SolveCollisions();
    void DrawObjects(SDL_Renderer* renderer);
} typedef VerletObjects;

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static VerletObjects* Objs = new VerletObjects();
bool pause = false;
bool step = false;



void DrawCircle(SDL_Renderer* renderer, SDL_FPoint, int radius);
void AddObjects(float speed);
void chatgpt();
int roundUpToMultipleOfEight(int v);

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("VerletIntegration", "1.0", "com.example.renderer-clear");
    static std::shared_ptr<VerletObject> aa = std::make_shared<VerletObject>(1000, 400, static_cast<float>(20));
    Objs->objects.push_back(aa);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("VerletIntegration", 1920, 1080, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{   
    static std::mt19937 rng;
    static uint64_t last = 0;
    static std::uniform_int_distribution<int> ran(15, 20032);
    static std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * M_PI);
    static std::uniform_real_distribution<float> distRadius(0.0f, 1.0f);
    if (event->type == SDL_EVENT_KEY_DOWN) {
        SDL_Keycode key = event->key.key;
        switch(key) {
            case SDLK_W:
                if (SDL_GetTicks() - last > 2) {
                    float radius = 400.0f;

                    float angle = distAngle(rng);
                    float r = radius * std::sqrt(distRadius(rng));
                    std::shared_ptr<VerletObject> obj = std::make_shared<VerletObject>(1200, 180, SDL_clamp(ran(rng), 10, 25));
                    obj->acceleration = {(float)SDL_clamp(ran(rng), 0, 250)-4200,(float)SDL_clamp(ran(rng), 1220, 1500)};
                    obj->CalculatePosition(0.02);
                    Objs->objects.push_back(obj);

                    last = SDL_GetTicks();
                    break;
                }
            case SDLK_F:
                if (SDL_GetTicks()-last > 50) {
                    AddObjects(1.0f);
                }
            case SDLK_SPACE:
                pause = !pause;
                break;
            case SDLK_S:
                step = true;    
                break;
        }
    }

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}
/* This function runs once per frame, and is the heart of the program. */

SDL_AppResult SDL_AppIterate(void *appstate)
{
    static uint64_t currentTicks, frameTime;
    //static std::shared_ptr<VerletObjects> Objs = std::make_shared<VerletObjects>();
    static std::shared_ptr<VerletObject> aa = Objs->objects[0];
	SDL_FRect rect;
	float red = 0.2, green = 0.3, blue=0.6;
	rect.y = 0;
    rect.x = 0;
	rect.h = 1080;
	rect.w = 1920;
    
    if (!pause)
        Objs->SimulateObjects(frameTime/1000.0f);
    if (pause && step) {
        Objs->SimulateObjects(frameTime/1000.0f);
        step = false;
    }
    //aa->Simulate(frameTime/1000.0f);

	SDL_SetRenderDrawColor(renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);  /* dark gray, full alpha */
    SDL_RenderClear(renderer);
	
    SDL_SetRenderDrawColorFloat(renderer, 0.2, 0.2, blue, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */
	SDL_RenderFillRect(renderer, &rect);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);  /* yellow, full alpha */
    //SDL_RenderDebugText(renderer, 200, 300, std::to_string(frameTime).c_str());
    //SDL_RenderDebugText(renderer, 200, 400, std::to_string(Objs->objects.size()).c_str());
    Objs->DrawObjects(renderer);
    //DrawCircle(renderer, aa->position, aa->radius);
    DrawCircle(renderer, {1000,500}, 500);

    /* put the newly-cleared rendering on the screen. */
    frameTime = SDL_GetTicks() - currentTicks;
    currentTicks = SDL_GetTicks();
    SDL_GetPerformanceCounter();
    
    SDL_RenderPresent(renderer);
    SDL_Delay(2);
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}

void AddObjects(float speed) {
    static float angle = 0;
    static bool direction = false;

    std::shared_ptr<VerletObject> obj = std::make_shared<VerletObject>(1000, 250, 15); //Middle Top of the constaint
    obj->acceleration.x = std::cos(angle) * 2000;
    obj->acceleration.y = std::sin(angle) * 2000;
    //std::string debug = std::to_string(obj->acceleration.x) + " " + std::to_string(obj->acceleration.y) + " " + std::to_string(angle / M_PI * 180);
    //SDL_Log(debug.c_str());
    
    obj->CalculatePosition(0.02);
    Objs->objects.push_back(obj);

    if (!direction)
        angle += 4.0f * (M_PI/180.0f);
    else
        angle -= 4.0f * (M_PI/180.0f);
    if (angle > (M_PI))
        direction = true; 
    else if (angle < 0) 
        direction = false;
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
    acceleration.y += 500.0f;
}

void VerletObject::ApplyConstraint(SDL_FPoint center, float cRadius) {
    SDL_FPoint delta = {position.x - center.x, position.y - center.y};
    float distance = magnitude(delta);
    if (distance > cRadius - radius) {
        SDL_FPoint normal = {delta.x / distance, delta.y / distance};
        position.x = center.x + normal.x * (cRadius - radius);
        position.y = center.y + normal.y * (cRadius - radius);
    }
}

void VerletObjects::SolveCollisions() {
    for (int i = 0; i < objects.size(); i++) {
        for (int j = i+1; j < objects.size(); j++) {
            std::shared_ptr<VerletObject> first = objects[i], second = objects[j];
            SDL_FPoint delta = minus(first->position, second->position);
            float distance = magnitude(delta);
            if (distance < 0.01f) {
                delta = {1.0f, 1.0f};
                distance = 0.01f;
            }
            if (distance < first->radius + second->radius) {
                SDL_FPoint normal = {delta.x / distance, delta.y / distance};
                
                float unresolved = first->radius + second->radius - distance;
                SDL_FPoint addus = {static_cast<float>(0.5*unresolved*normal.x), static_cast<float>(0.5*unresolved*normal.y)};
                first->position = add(first->position, addus);
                second->position = minus(second->position, addus);
            }
        }
    }
    return;
}

void VerletObjects::SimulateObjects(float dt) {
    float subdt = dt / 8;
    for (int i = 0; i < 8; i++) {
        for (auto& obj: objects) {
            obj->SimulateGravity(subdt);
            obj->ApplyConstraint({1000,500},500);
            obj->CalculatePosition(subdt);
        }
        SolveCollisions();
    }
}

void VerletObjects::DrawObjects(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE); //Yellow Color 
    for (auto& obj: objects) {
        DrawCircle(renderer, obj->position, static_cast<int>(obj->radius));
    }
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