
inline double magnitude(SDL_FPoint calc) {
    return std::sqrt(calc.x*calc.x + calc.y*calc.y);
}

inline SDL_FPoint add(SDL_FPoint lhs, SDL_FPoint rhs) {
    SDL_FPoint a = {lhs.x+rhs.x, lhs.y+rhs.y};
    return a;
}

inline SDL_FPoint minus(SDL_FPoint lhs, SDL_FPoint rhs) {
    SDL_FPoint a = {lhs.x-rhs.x, lhs.y-rhs.y};
    return a;
}

