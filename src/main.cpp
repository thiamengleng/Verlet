
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include <memory>
#include <cmath>
#include <vector>
#include <chrono>
#include <random>
#include "Vmath.cpp"
#include "render.cpp"



#define MAX_SLICES 24
#define M_PI 3.14159265358979323846

struct VerletObject {
    float radius;
    SDL_FPoint position;
    SDL_FPoint previousPosition;
    SDL_FPoint acceleration;
    float r, g, b;

    void SimulateGravity(float dt);
    void CalculatePosition(float dt);
    void ApplyConstraint(SDL_FPoint center, float radius);
    int GetGrid();

    VerletObject(float x, float y, float r) : radius(r) {
        position.x = x;
        position.y = y;
        previousPosition.x = x;
        previousPosition.y = y;
    };
} typedef VerletObject;

struct VerletObjects {
    std::vector<std::vector<std::vector<VerletObject*>>> grid;
    std::vector<VerletObject*> objects;

    void SimulateObjects(float dt);
    void SolveCollisions();
    void UpdateGrid();
    void ExploreAndSolve(VerletObject* obj);
    void SolveCollision(VerletObject* first, VerletObject* second);
    void DrawObjects(SDL_Renderer* renderer);
    VerletObjects() {
        grid.resize(10);
        for (auto& row : grid) {
            row.resize(10);
        }
    }
    ~VerletObjects() {
        for (VerletObject*& obj: objects) {
            delete(obj);
        }
    }
} typedef VerletObjects;


static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static VerletObjects* Objs = new VerletObjects();

std::mt19937 rng;
std::uniform_int_distribution<int> ran(0, 20000);
std::uniform_real_distribution<float> color(0.6, 1.0);



bool pause = false;
bool step = false;

uint64_t frameTime;


void AddObjects(float speed);

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("VerletIntegration", "1.0", "com.example.renderer-clear");
    VerletObject* aa = new VerletObject(1000, 400, 20.0f);
    aa->r = 1.0f; aa->g = 1.0f; aa->g = 1.0f;
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
    
    static uint64_t last = 0;
    
    static std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * M_PI);
    static std::uniform_real_distribution<float> distRadius(0.0f, 1.0f);
    
    if (event->type == SDL_EVENT_KEY_DOWN) {
        SDL_Keycode key = event->key.key;
        switch(key) {
            case SDLK_F:
                if (SDL_GetTicks()-last > 2) {
                    AddObjects(1.8f);
                    last = SDL_GetTicks();
                }
                break;
            case SDLK_SPACE:
                pause = !pause;
                break;
            case SDLK_S:
                step = true;    
                break;
            case SDLK_C:
                SDL_Log("Object Count: %d\t Frame Time: %dms", static_cast<int>(Objs->objects.size()), static_cast<int>(frameTime));
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
    static uint64_t currentTicks;
    
    if (!pause)
        Objs->SimulateObjects(frameTime/1000.0f);
    if (pause && step) {
        Objs->SimulateObjects(frameTime/1000.0f);
        step = false;
    }

	SDL_SetRenderDrawColor(renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);  /* dark gray, full alpha */
    SDL_RenderClear(renderer);
    SDL_FColor whit = {1.0, 1.0, 1.0, 1.0};

    Objs->DrawObjects(renderer);
    DrawCircle(renderer, {1000,500}, 500, 255, 255, 255);

    /* put the newly-cleared rendering on the screen. */
    frameTime = SDL_GetTicks() - currentTicks;
    currentTicks = SDL_GetTicks();
    
    SDL_RenderPresent(renderer);
    if (frameTime < 2) 
        SDL_Delay(2);
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}

void AddObjects(float speed) {
    static float angle = 0.3;
    static bool direction = false;

    VerletObject* obj = new VerletObject(1000,250,SDL_clamp(ran(rng)%12, 4, 12));
    obj->acceleration.x = std::cos(angle) * 2000;
    obj->acceleration.y = std::sin(angle) * 2000;
    
    obj->CalculatePosition(0.03);
    Objs->objects.push_back(obj);
    obj->r = color(rng); obj->g = color(rng); obj->b = color(rng);

    if (!direction)
        angle += 4.0f * (M_PI/180.0f);
    else
        angle -= 4.0f * (M_PI/180.0f);
    if (angle > M_PI - 0.3)
        direction = true; 
    else if (angle < 0.3) 
        direction = false;
}


void VerletObject::CalculatePosition(float dt) {
    SDL_FPoint newCurrent;
    newCurrent.x = 2 * position.x - previousPosition.x + acceleration.x * dt * dt;
    newCurrent.y = 2 * position.y - previousPosition.y + acceleration.y * dt * dt;
    
    previousPosition.y = position.y;
    previousPosition.x = position.x;
    //Update Positions
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

int VerletObject::GetGrid() {
    int row = SDL_clamp(position.y / 108 + 1, 1, 10), column = SDL_clamp(position.x / 192, 0, 10);
    
    return row*10 + column;
}

void VerletObjects::ExploreAndSolve(VerletObject* obj) {
    int row = obj->GetGrid() / 10 - 1, column = obj->GetGrid() % 10;

    for (int i = SDL_max(0, row - 1); i <= SDL_min(9,row + 1); i++) {
        for (int j = SDL_max(0, column - 1); j <= SDL_min(9,column + 1); j++) {
            for (auto& other : grid[i][j]) {
                SolveCollision(obj, other); 
            }
        }
    }
}

void VerletObjects::SolveCollision(VerletObject* first, VerletObject* second) {
    SDL_FPoint delta = minus(first->position, second->position);
    float distance = magnitude(delta);
        if (distance < 0.01f) {
            delta = {1.0f, 1.0f};
            distance = 0.01f;
        }
        if (distance < first->radius + second->radius) {
            float unresolved = first->radius + second->radius - distance;
            SDL_FPoint addus = {static_cast<float>(0.5*unresolved*(delta.x/distance)), static_cast<float>(0.5*unresolved*(delta.y/distance))};
            first->position = add(first->position, addus);
            second->position = minus(second->position, addus);
        }
}

void VerletObjects::SolveCollisions() {
    for (int i = 0; i < objects.size(); i++) {
        ExploreAndSolve(objects[i]);
    }
    return;
}

void VerletObjects::UpdateGrid() {
    for (auto& row : grid) {
        for (auto& cell : row) {
            //SDL_Log("woorking");
            cell.clear(); 
        }
    }
    for (auto& obj : objects) {
        int row = obj->GetGrid() / 10 - 1, column = obj->GetGrid() % 10;
        if (row < 0 || row >= 10 || column < 0 || column >= 10) {
            SDL_Log("Invalid grid access: row=%d, col=%d, pos=(%.2f, %.2f)", row, column, obj->position.x, obj->position.y);
        }
        grid[row][column].push_back(obj);
    }
}

void VerletObjects::SimulateObjects(float dt) {
    float subdt = dt / 6;
    for (int i = 0; i < 6; i++) {
        for (auto& obj: objects) {
            obj->SimulateGravity(subdt);
            obj->ApplyConstraint({1000,500},500);
            obj->CalculatePosition(subdt);
        }
        UpdateGrid();
        SolveCollisions();
    }
}

void VerletObjects::DrawObjects(SDL_Renderer* renderer) {
    
    for (auto& obj: objects) {
        RenderCircle(renderer, obj->position.x, obj->position.y, obj->radius, 24, {obj->r, obj->g, obj->b, 1.0});
    }
}
