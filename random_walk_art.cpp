#include <SDL2/SDL.h>
#include <random>
#include <cstdint>
#include <complex>
#include <cmath>


// simple single qubit representation
struct Qubit {
    std::complex<double> alpha; // amplitude for |0>
    std::complex<double> beta;  // amplitude for |1>
};

// normalize the qubit 
void normalize(Qubit& q) {
    double norm = std::sqrt(std::norm(q.alpha) + std::norm(q.beta));
    q.alpha /= norm;
    q.beta /= norm;
}

// hadamard gate (creates superposition)
void applyHadamard(Qubit& q) {
    std::complex<double> newAlpha = (q.alpha + q.beta) / std::sqrt(2.0);
    std::complex<double> newBeta  = (q.alpha - q.beta) / std::sqrt(2.0);
    q.alpha = newAlpha;
    q.beta = newBeta;
}

// measure probabilities
double prob0(const Qubit& q) {
    return std::norm(q.alpha);
}

double prob1(const Qubit& q) {
    return std::norm(q.beta);
}


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    const int GRID_SIZE = 50;
    const int SCALE = 20;

    if (SDL_CreateWindowAndRenderer(GRID_SIZE * SCALE, GRID_SIZE * SCALE, 0, &window, &renderer) != 0) {
        SDL_Quit();
        return 1;
    }

    SDL_RenderSetScale(renderer, SCALE, SCALE);

    int x = GRID_SIZE / 2;
    int y = GRID_SIZE / 2;

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dirDist(0, 3);

    // initialize qubit in |0⟩ state
    Qubit q{{1.0, 0.0}, {0.0, 0.0}};

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        int dir = dirDist(rng);

        // random walk movement
        switch (dir) {
            case 0: x -= 1; y -= 1; break;
            case 1: x += 1; y -= 1; break;
            case 2: x += 1; y += 1; break;
            case 3: x -= 1; y += 1; break;
        }

        if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) {
            x = GRID_SIZE / 2;
            y = GRID_SIZE / 2;
        }

        // quantum evolution (simple demo) 
        applyHadamard(q);   // evolve state
        normalize(q);

        double p0 = prob0(q);
        double p1 = prob1(q);

        // Map probabilities to color
        uint8_t r = static_cast<uint8_t>(p0 * 255); // |0> : red
        uint8_t b = static_cast<uint8_t>(p1 * 255); // |1> : blue
        uint8_t g = static_cast<uint8_t>((1.0 - std::abs(p0 - p1)) * 255); // balance : green

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        SDL_RenderDrawPoint(renderer, x, y);
        SDL_RenderPresent(renderer);

        SDL_Delay(50);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
