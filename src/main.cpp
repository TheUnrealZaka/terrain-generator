#include <raylib.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iostream>
using namespace std;

/*
Generador de terreno usando Diamond-Square y Perlin Noise.
Incluye clases con los algoritmos y usa Raylib para visualizar.
Autores: TheUnrealZaka y Jowey7
*/
class DiamondSquare {
private:
    vector<vector<float>> map; // Mapa de alturas dinámico
    int size; // Dimensión del mapa = 2^n + 1
public:
    // Constructor: n (exponente para tamaño), roughness (factor inicial de ruido)
    DiamondSquare(int n, float roughness, int seed) {
        size = (1 << n) + 1; // 2^n + 1
        map.assign(size, vector<float>(size, 0.0f));
        srand(seed); // Semilla aleatoria
        // Inicializar los cuatro rincones con valores aleatorios [0,1]
        map[0][0] = (float)rand() / RAND_MAX;
        map[0][size-1] = (float)rand() / RAND_MAX;
        map[size-1][0] = (float)rand() / RAND_MAX;
        map[size-1][size-1] = (float)rand() / RAND_MAX;
        // Ejecutar recursivamente los pasos Diamond-Square
        diamondSquare(0, 0, size-1, roughness);
    }
    // Función recursiva: (x0,y0) esquina superior izquierda del subcuadrado,
    // 'step' es la longitud del subcuadrado, 'offset' es la dispersión actual.
    void diamondSquare(int x0, int y0, int step, float offset) {
        int half = step / 2;
        if (half < 1) return; // Caso base
        // Paso Diamond: punto central = promedio de 4 esquinas + ruido
        float avg = (map[y0][x0] + map[y0][x0+step] +
                     map[y0+step][x0] + map[y0+step][x0+step]) / 4.0f;
        map[y0+half][x0+half] = avg + (((float)rand() / RAND_MAX)*2.0f - 1.0f) * offset;
        // Paso Square: puntos medios de cada lado (promedio de vecinos + ruido)
        // Lado superior
        avg = (map[y0][x0] + map[y0][x0+step] + map[y0+half][x0+half]) / 3.0f;
        map[y0][x0+half] = avg + (((float)rand() / RAND_MAX)*2.0f - 1.0f) * offset;
        // Lado inferior
        avg = (map[y0+step][x0] + map[y0+step][x0+step] + map[y0+half][x0+half]) / 3.0f;
        map[y0+step][x0+half] = avg + (((float)rand() / RAND_MAX)*2.0f - 1.0f) * offset;
        // Lado izquierdo
        avg = (map[y0][x0] + map[y0+step][x0] + map[y0+half][x0+half]) / 3.0f;
        map[y0+half][x0] = avg + (((float)rand() / RAND_MAX)*2.0f - 1.0f) * offset;
        // Lado derecho
        avg = (map[y0][x0+step] + map[y0+step][x0+step] + map[y0+half][x0+half]) / 3.0f;
        map[y0+half][x0+step] = avg + (((float)rand() / RAND_MAX)*2.0f - 1.0f) * offset;
        // Recursión en los 4 subcuadrantes, reduciendo la dispersión (offset)
        diamondSquare(x0,       y0,       half, offset * 0.5f);
        diamondSquare(x0+half,  y0,       half, offset * 0.5f);
        diamondSquare(x0,       y0+half,  half, offset * 0.5f);
        diamondSquare(x0+half,  y0+half,  half, offset * 0.5f);
    }
    // Acceso al mapa generado
    vector<vector<float>>& getMap() { return map; }
    int getSize() { return size; }
};

// Estructura auxiliar para vectores 2D (usamos Vector2 de Raylib)
struct Vec2 { float x, y; };

// Clase para generar ruido Perlin 2D
class PerlinNoise {
private:
    vector<int> perm; // Tabla de permutación
    Vec2 grads[8];    // 8 vectores de gradiente unitarios
    // Función de interpolación suave (*fade*)
    float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
public:
    // Constructor: inicializa con semilla
    PerlinNoise(int seed) {
        perm.resize(512);
        // Inicializar permutación básica
        for(int i = 0; i < 256; i++) perm[i] = i;
        // Mezclar usando la semilla
        srand(seed);
        for(int i = 255; i > 0; i--) {
            int j = rand() % (i+1);
            swap(perm[i], perm[j]);
            perm[i+256] = perm[i];
        }
        for(int i = 0; i < 256; i++) perm[i+256] = perm[i];
        // Definir gradientes (E, W, N, S, NE, NW, SE, SW)
        float invSqrt2 = 1 / sqrt(2);
        grads[0] = {1, 0};
        grads[1] = {-1, 0};
        grads[2] = {0, 1};
        grads[3] = {0, -1};
        grads[4] = {invSqrt2, invSqrt2};
        grads[5] = {-invSqrt2, invSqrt2};
        grads[6] = {invSqrt2, -invSqrt2};
        grads[7] = {-invSqrt2, -invSqrt2};
    }
    // Función de ruido Perlin 2D en (x,y)
    float noise(float x, float y) {
        // Coordenadas enteras de la celda
        int xi = (int)floor(x) & 255;
        int yi = (int)floor(y) & 255;
        // Fracción dentro de la celda [0,1]
        float xf = x - floor(x);
        float yf = y - floor(y);
        // Obtener índices gradiente de las 4 esquinas
        int gi00 = perm[perm[xi] + yi] & 7;
        int gi01 = perm[perm[xi] + yi + 1] & 7;
        int gi10 = perm[perm[xi + 1] + yi] & 7;
        int gi11 = perm[perm[xi + 1] + yi + 1] & 7;
        // Vectores gradiente
        Vec2 g00 = grads[gi00];
        Vec2 g01 = grads[gi01];
        Vec2 g10 = grads[gi10];
        Vec2 g11 = grads[gi11];
        // Desplazamiento al punto
        float x0 = xf,   y0 = yf;
        float x1 = xf-1, y1 = yf;
        float x2 = xf,   y2 = yf-1;
        float x3 = xf-1, y3 = yf-1;
        // Producto punto gradiente x desplazamiento
        float dot00 = g00.x * x0 + g00.y * y0;
        float dot10 = g10.x * x1 + g10.y * y1;
        float dot01 = g01.x * x2 + g01.y * y2;
        float dot11 = g11.x * x3 + g11.y * y3;
        // Interpolación en X
        float u = fade(xf);
        float v = fade(yf);
        float ix0 = dot00 + u * (dot10 - dot00);
        float ix1 = dot01 + u * (dot11 - dot01);
        // Interpolación en Y
        float value = ix0 + v * (ix1 - ix0);
        return value; // Rango aproximado [-1,1]
    }
};

// Función para mapear altura [0,1] a color
Color heightToColor(float h) {
    if (h < 0.3f) return BLUE;    // Agua (azul)
    else if (h < 0.5f) return GREEN; // Llanuras (verde)
    else if (h < 0.7f) return BROWN; // Montañas (marrón)
    else return WHITE;           // Picos (blanco)
}

int main() {
    // Adjust parameters for smoother terrain
    int n = 10;              // Keep map size at 257x257
    float roughness = 0.7f; // Reduced roughness for smoother terrain
    int seed = time(NULL);
    float freq = 0.03f;     // Reduced frequency for more gradual changes
    
    // Generar mapa con Diamond-Square
    DiamondSquare ds(n, roughness, seed);
    auto &heightMapDS = ds.getMap();
    int mapSize = ds.getSize();
    int scale = 1;
    // Generar mapa con Perlin Noise (para comparación)
    PerlinNoise perlin(seed);
    vector<vector<float>> heightMapPN(mapSize, vector<float>(mapSize));
    for(int y = 0; y < mapSize; y++) {
        for(int x = 0; x < mapSize; x++) {
            // Normalizar resultado [-1,1] a [0,1]
            float val = perlin.noise(x * freq, y * freq);
            heightMapPN[y][x] = (val + 1.0f) * 0.5f;
        }
    }
    // Inicializar ventana Raylib
    InitWindow(mapSize*scale, mapSize*scale, "Procedural Terrain Generator");
    SetTargetFPS(60);
    // Dibujar mapa (usando Diamond-Square en este ejemplo)
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for(int y = 0; y < mapSize; y++) {
            for(int x = 0; x < mapSize; x++) {
                // Apply some smoothing by averaging with neighbors
                float h = heightMapDS[y][x];
                if (x > 0 && x < mapSize-1 && y > 0 && y < mapSize-1) {
                    h = (h + 
                         heightMapDS[y-1][x] + 
                         heightMapDS[y+1][x] + 
                         heightMapDS[y][x-1] + 
                         heightMapDS[y][x+1]) / 5.0f;
                }
                h = std::max(0.0f, std::min(1.0f, h));
                DrawRectangle(x*scale, y*scale, scale, scale, heightToColor(h));
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
