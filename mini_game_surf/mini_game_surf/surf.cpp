#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "objetos.h"

// --------------- VARI�VEIS GLOBAIS --------------------
const int LARGURA = 800;
const int ALTURA = 600;
const int FPS = 60;
const int QTD_OBSTACULOS = 50;
enum TECLAS { CIMA, BAIXO, DIREITA, ESQUERDA };
// ------------------------------------------------------

// --------------- PROT�TIPO DE FUN��ES -----------------
// Personagem 
void initPersonagem(personagem& personagemSurf);
void desenhaPersonagem(personagem& personagemSurf);
// Movimenta��o Personagem
void personagemMoveCima(personagem& personagemSurf);
void personagemMoveBaixo(personagem& personagemSurf);
void personagemMoveEsquerda(personagem& personagemSurf);
void personagemMoveDireira(personagem& personagemSurf);
// Mar
void initMar(mar& onda);
void desenhaMar(mar& onda);
// Obst�culos
void initObstaculos(obstaculos vetorObstaculos[], int quantidade);
void liberaObstaculos(obstaculos vetorObstaculos[], int quantidade);
void moveObstaculos(obstaculos vetorObstaculos[], int quantidade);
void desenhaObstaculos(obstaculos vetorObstaculos[], int quantidade);
// Colis�o
void colidirObstaculos(obstaculos vetorObstaculos[], int quantidade, personagem& personagem_surf);
// ------------------------------------------------------
int main() // Fun��o inicial
{
	// ------------- VARI�VEIS DO JOGO ----------------------
	// Cria a vari�vel da fila de entos
	ALLEGRO_EVENT_QUEUE *filaEventos = NULL; 
	ALLEGRO_TIMER* timer = NULL;
	// -----------------------------------------------------
	
	// Vari�veis para o loop principal
	bool fim = false;
	bool desenha = true;
	bool teclas[] = { false, false, false, false }; // O enum ser� �til aqui. Onde, em vez de usar n�mero para se obter o valor false, � usado os nomes que est�o no enum.
	// -------------- INICIALIZA��O DE OBJETOS ------------------
	personagem personagemSurf;
	obstaculos vetorObstaculos[QTD_OBSTACULOS];
	mar onda;

	// ----------------------------------------------------------
	
	// -------------- INICIALIZA��O DO ALLEGRO E DO DISPLAY ------------------
	// Cria um ponteiro inicializado com NULL. 
	ALLEGRO_DISPLAY *display = NULL;

	if (!al_init()) {
		al_show_native_message_box(NULL, "AVISO!", "ERRO:", "Allegro n�o pode ser inicializar", NULL, NULL);
		return -1;
	}

	display = al_create_display(LARGURA, ALTURA);

	if (!display) {
		al_show_native_message_box(NULL, "AVISO!", "ERROR!", "N�o foi poss�vel inicializar o display", NULL, NULL);
		return -1;
	}
	// -------------------------------------------------------------------------
	
	// ---------------------- INICIALIZA��O DOS ADDONS -----------------------------
	// Toda fun��o do Allegro precisa ser inicializada pelos addons.
	al_install_keyboard();
	al_install_mouse();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	// -----------------------------------------------------------------------------

	// ----------------------- CRIAR FILA DE EVENTOS E DEMAIS DISPOSITIVOS ---------
	// No campo das vari�veis foi criado o ponteiro fila_evento com valor NULL.
	// Agora, para sua utiliza��o, � atribuido a fun��o que cria eventos a esse ponteiro.
	filaEventos = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);
	// -----------------------------------------------------------------------------

	// ----------------------- REGISTRO DE SOURCES ---------------------------------
	// Agora � necess�rio que aja um registro dos eventos, qualquer que forem criados, para serem
	// atribuidos ao pontiero fila_evento. Assim:
	al_register_event_source(filaEventos, al_get_display_event_source(display)); // a fun��o al_get_display_event_source pede um parametro que, no caso, � o ponteiro display.
	al_register_event_source(filaEventos, al_get_keyboard_event_source()); // Aqui a fun��o al_register_event_source permite que o ponteiro fila_evento receba os dados do teclado pelo al_get_keyboard_event_source().
	al_register_event_source(filaEventos, al_get_mouse_event_source()); // Igual ao anterio, mas para o mouse.
	al_register_event_source(filaEventos, al_get_timer_event_source(timer));
	
	// ----------------------- FUN��ES INICIAIS -------------------------------------
	initMar(onda);
	initPersonagem(personagemSurf);
	initObstaculos(vetorObstaculos, QTD_OBSTACULOS);
	

	// ----------------------- LOOP PRINCIPAL ---------------------------------------
	al_start_timer(timer);

	while (!fim) 
	{
		ALLEGRO_EVENT ev; // Esse tipo ALLEGRO_EVENT cria uma vari�vel que aceita "qualquer tipo de evento" estabelecido.
		al_wait_for_event(filaEventos, &ev); // A fun��o al_wait_for_event aguarda uma a��o (entrada de dados). Quando entra dados, o ev recebe e manda para o ponteiro fila_evento.

		// ------------------- L�GICA DO JOGO
		// Como a vari�vel ev aceita qualquer tipo de entrada de dados, o if a seguir faz a compara��o dos dados de entrada para executar seu comando.
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			fim = true;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {/* ALLEGRO_EVENT_KEY_DOWN � o reconhecimento de que um bot�o qualquer foi apertado */
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_ESCAPE:
					fim = true;
					break;
				case ALLEGRO_KEY_UP:
					teclas[CIMA] = true; // Aplica��o do enum dentro do vetor. O valor original do vetor � false...
					break;
				case ALLEGRO_KEY_DOWN:
					teclas[BAIXO] = true;
					break;
				case ALLEGRO_KEY_RIGHT:
					teclas[DIREITA] = true;
					break;
				case ALLEGRO_KEY_LEFT:
					teclas[ESQUERDA] = true;
					break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					teclas[CIMA] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					teclas[BAIXO] = false;
					break;
				case ALLEGRO_KEY_RIGHT:
					teclas[DIREITA] = false;
					break;
				case ALLEGRO_KEY_LEFT:
					teclas[ESQUERDA] = false;
					break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_TIMER) {
			// ------------------- MOVIMENTO DO OBJETO -----------------
			
			desenha = true;

			if (teclas[CIMA])
				personagemMoveCima(personagemSurf);
			if (teclas[BAIXO])
				personagemMoveBaixo(personagemSurf);
			if (teclas[ESQUERDA])
				personagemMoveEsquerda(personagemSurf);
			if (teclas[DIREITA])
				personagemMoveDireira(personagemSurf);

			liberaObstaculos(vetorObstaculos, QTD_OBSTACULOS);
			moveObstaculos(vetorObstaculos, QTD_OBSTACULOS);
			colidirObstaculos(vetorObstaculos, QTD_OBSTACULOS, personagemSurf);
		}

		if (desenha && al_is_event_queue_empty(filaEventos)) {
			// -------------- DESENHO DO OBJETO ----------------
			desenha = false;
			
			desenhaMar(onda);
			desenhaPersonagem(personagemSurf);
			desenhaObstaculos(vetorObstaculos, QTD_OBSTACULOS);


		
			// ------------------- MOSTRA NO DISPLAY E BACKBUFFER
			
			al_flip_display(); // Mostra o que � feito no display.
			al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpa o display a cada movimenta��o.
		}
		
	}

	// --------------------- DESTRUI��O DOS EVENTOS USADOS
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(filaEventos);

	return 0;
}

// ------------------------ FUN��ES ---------------------------------
// Personagem
void initPersonagem(personagem& personagemSurf) {
	personagemSurf.ID = JOGADOR;
	personagemSurf.posX = 20;
	personagemSurf.posY = (ALTURA / 2) + (ALTURA / 4);
	personagemSurf.bordaX = 30;
	personagemSurf.bordaY = 30;
	personagemSurf.velocidade = 7;
	personagemSurf.vida = 5;
	personagemSurf.pontos = 0;
}
void desenhaPersonagem(personagem& personagemSurf) {
	al_draw_filled_rectangle(personagemSurf.posX, personagemSurf.posY, personagemSurf.posX + 30, personagemSurf.posY + 30, al_map_rgb(255, 0, 0));
}
// Movimenta��o Personagem
void personagemMoveCima(personagem& personagemSurf) {
	personagemSurf.posY -= personagemSurf.velocidade;

	if (personagemSurf.posY < ALTURA / 2)
		personagemSurf.posY = ALTURA / 2;
}
void personagemMoveBaixo(personagem& personagemSurf) {
	personagemSurf.posY += personagemSurf.velocidade;

	if (personagemSurf.posY + 30 > ALTURA)
		personagemSurf.posY = ALTURA - 30;
}
void personagemMoveEsquerda(personagem& personagemSurf) {
	personagemSurf.posX -= personagemSurf.velocidade;

	if (personagemSurf.posX < 0)
		personagemSurf.posX = 0;
}
void personagemMoveDireira(personagem& personagemSurf) {
	personagemSurf.posX += personagemSurf.velocidade;
	
	if (personagemSurf.posX + 30> LARGURA )
		personagemSurf.posX = LARGURA - 30;
}

// Mar
void initMar(mar& onda) {
	onda.posX = 0;
	onda.posY = ALTURA / 2;
}
void desenhaMar(mar& onda) {
	al_draw_filled_rectangle(onda.posX, onda.posY, onda.posX + LARGURA, onda.posY + (ALTURA / 2), al_map_rgb(0, 0, 255));
}

// Obst�culos
void initObstaculos(obstaculos vetorObstaculos[], int quantidade) {
	for (int i = 0; i < quantidade; i++) {
		vetorObstaculos[i].ID = INIMIGO;
		vetorObstaculos[i].velocidade = 5;
		vetorObstaculos[i].bordaX = 8;
		vetorObstaculos[i].bordaY = 8;
		vetorObstaculos[i].ativo = true;
	}
}
void liberaObstaculos(obstaculos vetorObstaculos[], int quantidade) {
	for (int i = 0; i < quantidade; i++) {
		if (!vetorObstaculos[i].ativo) {
			if (rand() % 500 == 0) {
				vetorObstaculos[i].posX = LARGURA;
				vetorObstaculos[i].posY = rand() % (ALTURA / 2) + (ALTURA / 2);
				vetorObstaculos[i].ativo = true;
				break;
			}
		}
	}
}
void moveObstaculos(obstaculos vetorObstaculos[], int quantidade) {
	for (int i = 0; i < quantidade; i++) {
		if (vetorObstaculos[i].ativo) {
			vetorObstaculos[i].posX -= vetorObstaculos[i].velocidade;

			if (vetorObstaculos[i].posX < 0)
				vetorObstaculos[i].ativo = false;
		}
	}
}
void desenhaObstaculos(obstaculos vetorObstaculos[], int quantidade) {
	for (int i = 0; i < quantidade; i++) {
		if (vetorObstaculos[i].ativo) {
			al_draw_filled_circle(vetorObstaculos[i].posX, vetorObstaculos[i].posY, 10, al_map_rgb(255, 0, 0));
		}
	}
}
// Colis�o
void colidirObstaculos(obstaculos vetorObstaculos[], int quantidade, personagem& personagem_surf) {
	for (int i = 0; i < quantidade; i++) {
		if (vetorObstaculos[i].ativo) {
			if (personagem_surf.posX + personagem_surf.bordaX > vetorObstaculos[i].posX - vetorObstaculos[i].bordaX &&
				personagem_surf.posY + personagem_surf.bordaY > vetorObstaculos[i].posY - vetorObstaculos[i].bordaY &&
				personagem_surf.posX < vetorObstaculos[i].posX + vetorObstaculos[i].bordaX &&
				personagem_surf.posY < vetorObstaculos[i].posY + vetorObstaculos[i].bordaY) {

				vetorObstaculos[i].ativo = false;
				personagem_surf.vida--;
			}
		}
	}
}
// -------------------------------------------------------------------

