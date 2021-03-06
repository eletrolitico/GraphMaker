// GRAPH.cpp : Define o ponto de entrada para o aplicativo.
//

#include "stdafx.h"
#include "Graph.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>
#include <Windowsx.h>
#include <ctime>


#define MAX_LOADSTRING 100

// Variáveis Globais:
HINSTANCE hInst;                                // instância atual
WCHAR szTitle[MAX_LOADSTRING];                  // O texto da barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // o nome da classe da janela principal

												// Declarações de encaminhamento de funções incluídas nesse módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Peso(HWND, UINT, WPARAM, LPARAM);

typedef struct no {
	int  x, y;
	std::vector < std::pair<int,int> > adj;
}node;

std::vector<node> graph;
int selecionado = -1;


//Desenha
void draw(HDC hdc) {
	//Primeiro desenha as linhas
	for (int i = 0; i < (int)graph.size(); ++i) {
		node n = graph.at(i);
		for (int j = 0; j < (int)n.adj.size(); ++j) {
			int x1 = n.x, y1 = n.y, x2 = graph.at(n.adj.at(j).first).x, y2 = graph.at(n.adj.at(j).first).y;
			MoveToEx(hdc, x1, y1, NULL);
			LineTo(hdc, x2, y2);
			wchar_t buffer[256];
			wsprintfW(buffer, L"%d", n.adj.at(j).second);

			RECT rect;
			rect.left = (x1+x2)/2 - 20;
			rect.top = (y1 + y2) / 2 - 20;
			rect.right = (x1 + x2) / 2 + 20;
			rect.bottom = (y1 + y2) / 2 + 20;
			DrawText(hdc, buffer, -1, &rect,
				DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			
		}
	}
	//Depois desenha os circulos por cima
	for (int i = 0; i < (int)graph.size(); ++i) {
		node n = graph.at(i);

		if (i == selecionado)Ellipse(hdc, n.x - 23, n.y - 23, n.x + 23, n.y + 23);
		Ellipse(hdc, n.x - 20, n.y - 20, n.x + 20, n.y + 20);
		wchar_t buffer[256];
		wsprintfW(buffer, L"%d", i);

		RECT rect;
		rect.left = n.x - 20;
		rect.top = n.y - 20;
		rect.right = n.x + 20;
		rect.bottom = n.y + 20;
		DrawText(hdc, buffer, -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}
}

void addNode(int x, int y) {
	for (int i = 0; i < (int)graph.size(); ++i) {
		if (abs(x - graph.at(i).x) < 40 && abs(y - graph.at(i).y) < 40) {
			return;
		}
	}
	graph.push_back({ x,y,{} });
}

void removeNode(int x, int y) {
	int v = -1;
	for (int i = 0; i < (int)graph.size(); ++i) {
		if (abs(x - graph.at(i).x) < 20 && abs(y - graph.at(i).y) < 20) {
			v = i;
			break;
		}
	}
	if (v == -1)return;
	std::vector<node>::iterator it = graph.begin() + v;
	for (int i = 0; i < (int)graph.size(); ++i) {
		node &n = graph.at(i);
		for (int j = 0; j < (int)n.adj.size(); ++j) {
			if (n.adj.at(j).first == v) {
				std::vector<std::pair<int,int>>::iterator ite = n.adj.begin() + j;
				n.adj.erase(ite);
				break;
			}
		}
		for (int j = 0; j < (int)n.adj.size(); ++j) {
			if (n.adj.at(j).first>v)n.adj.at(j).first--;
		}
	}
	graph.erase(it);
}

int contem(int e, std::vector<std::pair<int,int>> vec) {
	for (int i = 0; i < (int)vec.size(); ++i) {
		if (vec.at(i).first == e)return i;
	}
	return -1;
}

void apagaVec(std::vector<std::pair<int,int>> &vet,int e) {
	unsigned int i;
	bool achou = false;
	for (i = 0; i < vet.size(); ++i) {
		if (vet.at(i).first == e) {
			achou = true;
			break;
		}
	}
	if (!achou)return;
	std::vector<std::pair<int,int>>::iterator it = vet.begin() + i;
	vet.erase(it);
}
int peso, dragged = -1;
int x, y;
bool sdrag;
void seleciona(HWND hWnd) {
	int v = -1;
	for (int i = 0; i < (int)graph.size(); ++i) {
		if (abs(x - graph.at(i).x) < 20 && abs(y - graph.at(i).y) < 20) {
			v = i;
			break;
		}
	}
	if (v == -1) {
		for (int i = 0; i < (int)graph.size(); ++i) {
			node n = graph.at(i);
			for (int j = 0; j < (int)n.adj.size(); ++j) {
				int x1 = n.x, y1 = n.y, x2 = graph.at(n.adj.at(j).first).x, y2 = graph.at(n.adj.at(j).first).y;
				if (abs(x - (x1 + x2) / 2) < 20 && abs(y - (y1 + y2) / 2 < 20)) {
					sdrag = false;
					DialogBox(hInst, MAKEINTRESOURCE(IDD_PESO), hWnd, Peso);
					graph.at(i).adj.at(j).second = peso;
					graph.at(n.adj.at(j).first).adj.at(contem(i, graph.at(n.adj.at(j).first).adj)  ).second = peso;
					return;
				}
			}
		}
		selecionado = -1;
		return;
	}
	dragged = v;
	if (selecionado == -1) {
		selecionado = v;
	}
	else {
		if (selecionado != v && contem(selecionado, graph.at(v).adj)==-1) {
			sdrag = false;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_PESO), hWnd, Peso);
			graph.at(v).adj.push_back({selecionado,peso});
			graph.at(selecionado).adj.push_back({v,peso});
		}
		else if (selecionado != v) {
			apagaVec(graph.at(v).adj, selecionado);
			apagaVec(graph.at(selecionado).adj, v);
		}
		selecionado = -1;
	}
}

void remAresta(int u, int v,std::vector<std::tuple<int,int,int>> &vet) {
	for (int i = 0; i < (int)vet.size(); ++i) {
		if (std::get<0>(vet.at(i)) == u && std::get<1>(vet.at(i)) == v) {
			std::vector<std::tuple<int, int, int>>::iterator it = vet.begin() + i;
			vet.erase(it);
		}
	}
}

void salva() {
	const int tam = graph.size();
	std::ofstream myfile;
	myfile.open("matriz.txt");
	myfile.flush();
	for (int i = 0; i < tam; i++) {
		for (int j = 0; j < tam; ++j) {
			int c = contem(j, graph.at(i).adj);
			if (c != -1) {
				int val = graph.at(i).adj.at(c).second;
				if (j>0)myfile << " " << val;
				else myfile << val;
			}
			else {
				if (j > 0)myfile << " " << 0;
				else myfile << 0;
			}
		}
		myfile << std::endl;
	}
	
	std::vector<std::tuple<int, int, int>> lista;
	for (int i = 0; i < (int)graph.size(); ++i) {
		for (int j = 0; j < (int)graph.at(i).adj.size(); ++j) {
			lista.push_back({ i,graph.at(i).adj.at(j).first,graph.at(i).adj.at(j).second });
		}
	}
	for (int i = 0; i < (int)lista.size(); ++i) {
		remAresta(std::get<1>(lista.at(i)), std::get<0>(lista.at(i)),lista);
	}
	myfile << std::endl;
	myfile.close();
	myfile.open("lista.txt");
	myfile.flush();
	for (int i = 0; i < (int)lista.size(); ++i) {
		myfile << std::get<0>(lista.at(i))<< " " << std::get<1>(lista.at(i)) << " " << std::get<2>(lista.at(i)) << std::endl;
	}
	myfile.close();
}

void drag(int x1, int y1) {
	if (dragged == -1) {
		sdrag = false;
		return;
	}
	int dx = x1 - x;
	int dy = y1 - y;
	graph.at(dragged).x += dx;
	graph.at(dragged).y += dy;
	selecionado = -1;
	x = x1; y = y1;
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Inicializar cadeias de caracteres globais
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_GRAPH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Realize a inicialização do aplicativo:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPH));

	MSG msg;

	// Loop de mensagem principal:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNÇÃO: MyRegisterClass()
//
//  FINALIDADE: Registra a classe de janela.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPH));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GRAPH);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNÇÃO: InitInstance(HINSTANCE, int)
//
//   FINALIDADE: Salva o identificador de instância e cria a janela principal
//
//   COMENTÁRIOS:
//
//        Nesta função, o identificador de instâncias é salvo em uma variável global e
//        crie e exiba a janela do programa principal.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Armazenar o identificador de instância em nossa variável global

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
long long ini;
//
//  FUNÇÃO: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  FINALIDADE: Processa as mensagens para a janela principal.
//
//  WM_COMMAND  - processar o menu do aplicativo
//  WM_PAINT    - Pintar a janela principal
//  WM_DESTROY  - postar uma mensagem de saída e retornar
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Analise as seleções do menu:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_ARQUIVO_SALVAR:
			salva();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT clinetrect;
		int cx, cy;

		GetClientRect(hWnd, &clinetrect);
		cx = clinetrect.right - clinetrect.left;
		cy = clinetrect.bottom - clinetrect.top;

		HDC memdc = ::CreateCompatibleDC(hdc);
		unsigned bpp = ::GetDeviceCaps(hdc, BITSPIXEL);
		HBITMAP hBmp = ::CreateBitmap(cx, cy, 1, bpp, NULL);
		HBITMAP hTmpBmp = (HBITMAP)::SelectObject(memdc, (HGDIOBJ)hBmp);
		//do all your drawing here to the memdc
		::FillRect(memdc, &clinetrect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		draw(memdc);

		//when you are done drawing, update the main DC
		::BitBlt(hdc, 0, 0, cx, cy, memdc, 0, 0, SRCCOPY);
		::SelectObject(memdc, (HGDIOBJ)hTmpBmp);
		::DeleteDC(memdc);


		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDBLCLK:
		addNode(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		InvalidateRect(hWnd, 0, TRUE);
		break;
	case WM_MBUTTONDOWN:
		removeNode(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		InvalidateRect(hWnd, 0, TRUE);
		break;
	case WM_LBUTTONDOWN:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		dragged = -1;
		sdrag = true;
		seleciona(hWnd);
		InvalidateRect(hWnd, 0, TRUE);
		ini = clock();
		break;
	case WM_LBUTTONUP:
		sdrag = false;
		dragged = -1;
		InvalidateRect(hWnd, 0, TRUE);
		break;
	case WM_MOUSEMOVE:
		if (sdrag) {
			drag(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			if ((clock() - ini)*1000 / CLOCKS_PER_SEC > 32) {
				InvalidateRect(hWnd, 0, TRUE);
				ini = clock();
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Manipulador de mensagem para a caixa 'sobre'.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Peso(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			TCHAR szBuf[40];
			GetDlgItemText(hDlg, IDC_EDIT1, szBuf, 39);
			peso = _ttoi(szBuf);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
