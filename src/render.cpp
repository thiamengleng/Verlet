
int roundUpToMultipleOfEight(int v) {
    return (v + (8 - 1)) & -8;
}
void RenderCircle(SDL_Renderer *renderer, float cx, float cy, float radius, int segments, SDL_FColor color) { //rewrite this crappy gpt code
    if (segments < 3) return; // Need at least 3 segments to form a circle

    int vertex_count = segments + 1; // Center + perimeter points
    int index_count = segments * 3;  // 3 indices per triangle

    // Allocate memory for positions and colors
    float positions[vertex_count*2];
    int indices[index_count];
    SDL_FColor colors[vertex_count];
    //float *positions = (float*)malloc(sizeof(float) * vertex_count * 2); // x and y for each vertex
    //SDL_FColor *colors = (SDL_FColor*)malloc(sizeof(SDL_FColor) * vertex_count);
    //int *indices = (int*)malloc(sizeof(int) * index_count);


    // Center vertex
    positions[0] = cx;
    positions[1] = cy;
    colors[0] = color;

    // Perimeter vertices
    for (int i = 0; i < segments; ++i) {
        float angle = (float)i / segments * 2.0f * M_PI;
        float x = cx + radius * cosf(angle);
        float y = cy + radius * sinf(angle);
        positions[(i + 1) * 2] = x;
        positions[(i + 1) * 2 + 1] = y;
        colors[i + 1] = color;
    }

    // Indices for triangle fan
    for (int i = 0; i < segments; ++i) {
        indices[i * 3] = 0;               // Center vertex
        indices[i * 3 + 1] = i + 1;       // Current perimeter vertex
        indices[i * 3 + 2] = (i + 2 > segments) ? 1 : i + 2; // Next perimeter vertex, wrap around
    }

    // Render the geometry
    SDL_RenderGeometryRaw(renderer,
                          NULL, // No texture
                          positions, sizeof(float) * 2,
                          colors, sizeof(SDL_FColor),
                          NULL, 0, // No texture coordinates
                          vertex_count,
                          indices, index_count,
                          sizeof(int));
}

void DrawCircle(SDL_Renderer* renderer, SDL_FPoint center, int radius, Uint8 r = 255, Uint8 g = 255, Uint8 b = 0){
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

    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE); //Yellow Color 
    SDL_RenderPoints( renderer, points, drawCount);
}