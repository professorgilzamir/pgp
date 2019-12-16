#include <cmath>
#include <vector>
#include <iostream>

using namespace std;

const int width = 10;
const int height = 10;

const int MX = width - 1;
const int MY = height - 1;

char screen[1024][1024];


void initialize() {
	for (int i = 0; i < width; i ++ ){
		for (int j = 0; j < height; j ++) {
			screen[i][j] = ' ';
		}
	}
}

void show() {
	for (int i = 0; i < width; i ++ ){
		for (int j = 0; j < height; j ++) {
			cout<<screen[i][j];
		}
		cout<<endl;
	}
}


//INICIO DAS FUNCOES DE MAPEAMENTO ENTRE COORDENADAS DO UNIVERSO E COORDENADAS DA TELA
int toXScr(float x, float minXSRU, float maxXSRU){
	return  (int)(x * width/(maxXSRU-minXSRU) + 0.5f);
}

int toYScr(float y, float minYSRU, float maxYSRU) {
	return (int)(y * (-height)/(maxYSRU-minYSRU) + 0.5f);
}

float toXSRU(int x, float minXSRU, float maxXSRU) {
	float amp = maxXSRU - minXSRU;
	return  amp * x/width + minXSRU;
}

float toYSRU(int y, float minYSRU, float maxYSRU) {
	float amp = maxYSRU - minYSRU;
	return  -(amp * y/height + minYSRU);
}
//FIM DAS FUNCOES DE MAPEAMENTO ENTRE COORDENADAS DO UNIVERSO E COORDENADAS DA TELA

void __bresenham(float x1, float y1, float x2, float y2, bool inv1 = false){
	int sx1 = toXScr(x1, 0, 10);
	int sy1 = toYScr(y1, 0, 10);
	int sx2 = toXScr(x2, 0, 10);
	int sy2 = toXScr(y2, 0, 10);
	int dx = sx2 - sx1;
	int dy = sy2 - sy1;
	int p = 2 * dy - dx;
	int y = sy1;
	int signalx = 1;
	int signaly = 1;
	for (int x = sx1; x <= sx2; x += signalx) {
		if (!inv1){
			screen[y][x] = '*';
		} else {
			screen[x][y] = '*';
		}
		if ( p < 0 ) {
			p = p + 2 * dy;
		} else {
			y = y + signaly;
			p = p + 2 * dy - 2 * dx;
		}
	}
}



int main(int argc, char *argv[]) {
	show();
	bresenham(10.0f, 10.0f, 0.0f, 0.0f);
	show();
}
