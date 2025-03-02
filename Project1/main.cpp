﻿#include <allegro5/allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include "constantes.h"
#include "desenho.h"
#include "definicao.h"
#include "placar.h"
#include "movimento.h"
#include "structs.h"
#include "jogos.h"

int main(int argc, char** argv)
{
	//MAPA
	int xoff = 0;
	int yoff = 0;

	int mapColums = 20;
	int mapSize = 400;
	int titleSize = 80;

	//2 = algo relacionado ao minigame 4 = npc pedrito  1 = paredes  0 = grama 3 = rua/calçada não defini muito bem oq é
	//4 Pedrito cima
	/*
	5 =Talonito direita
	6 = Nikito esquerda
	*/

	int map[] = {
				  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,4,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,5,0,1,
				  1,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,1,
				  1,0,3,3,3,3,3,3,3,6,3,3,3,3,3,3,3,3,0,1,
				  1,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,1,
				  1,0,8,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,7,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,1,
				  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
				  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

	//PROJECT VARIABLES

	bool done = false;//finalizar o loop 
	bool render = false;//desenhar
	bool inGame = false;

	//animacao player/ ball1
	bool draw = true, active = false;
	int dir = DOWN, sourceX = 80, sourceY = 0;

	
	bool collision = false;//colisao
	Sprite ball1;//player
	Sprite ball2;//npc

	

	ball2.x = width / 2;
	ball2.y = height / 2;

	//local de fala
	bool local_fala = false, explicacao = true;

	int npc_fala = 0;//verificar com qual npc esta interagindo

	int contador = 0;//contar as quantidades de fala

	//allegro variables
	ALLEGRO_KEYBOARD_STATE keyState;//animacao
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_DISPLAY* displayTenis = NULL;
	ALLEGRO_DISPLAY* displayBoxe = NULL;
	ALLEGRO_DISPLAY* displaySurf = NULL;
	ALLEGRO_DISPLAY* displaySkate = NULL;
	ALLEGRO_EVENT_QUEUE* event_queue = NULL;
	ALLEGRO_TIMER* timer;
	ALLEGRO_TIMER* playerTimer;
	ALLEGRO_FONT* font18 = NULL;
	ALLEGRO_FONT* font50 = NULL;
	ALLEGRO_BITMAP* bgSheet = NULL;
	ALLEGRO_BITMAP* npc = NULL;
	//som 
	ALLEGRO_SAMPLE* trilhaSonora = NULL;
	ALLEGRO_SAMPLE_INSTANCE* inst_trilhaSonora = NULL;


	//ALLEGRO INIT FUNCTIONS

	if (!al_init())
	{
		return -1;
	}

	display = al_create_display(width, height);

	if (!display)
	{
		return -1;
	}

	//ADDONS INSTALL

	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_install_mouse();
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(10);

	//PROJECT INIT
		//som
	trilhaSonora = al_load_sample("trilhaSonora.ogg");
	inst_trilhaSonora = al_create_sample_instance(trilhaSonora);
	al_attach_sample_instance_to_mixer(inst_trilhaSonora, al_get_default_mixer());
	al_set_sample_instance_playmode(inst_trilhaSonora, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(inst_trilhaSonora, 0.1);


	bgSheet = al_load_bitmap("background02.png"); //mapa  paleta de sprites para o mapa

	npc = al_load_bitmap("bicho_aleatorio.png");//imagem do npc

	//local mapa
	xoff = -80 * 4.5; //movimentação do mapa
	yoff = -80 * 5.5;

	font18 = al_load_font("arial.ttf", 18, 0);//font de texto
	font50 = al_load_font("arial.ttf", 50, 0);//font de texto

	ball1.image = al_load_bitmap("playerImg.png");// imagem o player  animacao

	//pegar parametros do tamanho dos sprites
	ball1.w = al_get_bitmap_width(ball1.image);
	ball1.h = al_get_bitmap_height(ball1.image);

	ball1.x = width / 2 - 80;
	ball1.y = ((height * 4) / 5) - 190;

	ball2.w = 75;
	ball2.h = 75;

	ball1.bx = ball1.w / 2 - 5;
	ball1.by = ball1.h / 2 - 10;

	ball2.bx = ball2.w / 2 - 5;
	ball2.by = ball2.h / 2 - 10;

	//TIMER INIT AND STARTUP

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / 60);
	playerTimer = al_create_timer(1.0 / 15);

	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_timer_event_source(playerTimer));//animacao
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_start_timer(timer);
	while (!done)
	{
		printf("Tenis - %d, Skate - %d, Surf -%d, Boxe -%d", pontoTenis, pontoSkate, pontoSurf, pontoBoxe);
		while (inGame == false)
		{
			int somaPontos = pontoBoxe + pontoSkate + pontoSurf + pontoTenis;
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);
			al_get_keyboard_state(&keyState);//animacao
			
			//movimentação
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch (ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_ESCAPE:
					inGame = true;
					done = true;
					break;
				case ALLEGRO_KEY_LEFT:
					if (contador > 0)
						keys[LEFT] = false;
					else
						keys[LEFT] = true;
					break;
				case ALLEGRO_KEY_RIGHT:
					if (contador > 0)
						keys[RIGHT] = false;
					else
						keys[RIGHT] = true;
					break;
				case ALLEGRO_KEY_UP:
					if (contador > 0)
						keys[UP] = false;
					else
						keys[UP] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					if (contador > 0)
						keys[DOWN] = false;
					else
						keys[DOWN] = true;
					break;
				case ALLEGRO_KEY_A:
					if (contador > 0)
						keys[LEFT] = false;
					else
						keys[LEFT] = true;
					break;
				case ALLEGRO_KEY_D:
					if (contador > 0)
						keys[RIGHT] = false;
					else
						keys[RIGHT] = true;
					break;
				case ALLEGRO_KEY_W:
					if (contador > 0)
						keys[UP] = false;
					else
						keys[UP] = true;
					break;
				case ALLEGRO_KEY_S:
					if (contador > 0)
						keys[DOWN] = false;
					else
						keys[DOWN] = true;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = true;
					break;
				case ALLEGRO_KEY_Z:
					keys[Z] = true;
					if (local_fala)
					{
						contador++;
					}
					break;
				}
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_UP)
			{
				switch (ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = false;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = false;
					break;
				case ALLEGRO_KEY_UP:
					keys[UP] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = false;
					break;
				case ALLEGRO_KEY_A:
					keys[LEFT] = false;
					break;
				case ALLEGRO_KEY_D:
					keys[RIGHT] = false;
					break;
				case ALLEGRO_KEY_W:
					keys[UP] = false;
					break;
				case ALLEGRO_KEY_S:
					keys[DOWN] = false;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = false;
					break;
				case ALLEGRO_KEY_Z:
					keys[Z] = false;
					break;
				}
			}
			//tempo fps
			else if (ev.type == ALLEGRO_EVENT_TIMER)
			{
				render = true;
				active = true;

				if (keys[UP] )
				{
					ball1.y -= 2;
					yoff += keys[UP] * 5;
					dir = UP;//animacao
				}
				else if (keys[DOWN] )
				{
					ball1.y += 2;
					yoff -= keys[DOWN] * 5;
					dir = DOWN;//animacao
				}
				else if (keys[LEFT] )
				{
					ball1.x -= 2;
					xoff += keys[LEFT] * 5;
					dir = LEFT;//animacao
				}
				else if (keys[RIGHT] )
				{
					ball1.x += 2;
					xoff -= keys[RIGHT] * 5;
					dir = RIGHT;//animacao
				}
				else if (explicacao)
				{
					contador++;
					explicacao = false;
				}
				else
				{
					active = false;//animacao
				}



				//colisoes
				//npc de cima  // npc fala 1
				if (ball1.x + (ball1.bx / 2) > ((75.4 * 8) + xoff) - ball2.bx &&
					ball1.x + 10 < ((75 * 8) + xoff) + ball2.bx &&
					ball1.y + (ball1.by / 2) > ((65 * 3) + yoff) - ball2.by &&
					ball1.y < ((65 * 3) + yoff) + ball2.by)
				{
					collision = true;
				}
				//npc da esquerda
				else if (ball1.x + (ball1.bx / 2) > ((68 * 3) + xoff) - ball2.bx &&
					ball1.x + 10 < ((66 * 3) + xoff) + ball2.bx &&
					ball1.y + (ball1.by / 2) > ((83 * 12) + yoff) - ball2.by &&
					ball1.y < ((83 * 12) + yoff) + ball2.by)
					collision = true;
				//nc da direita
				else if (ball1.x + (ball1.bx / 2) > ((78 * 18) + xoff) - ball2.bx &&
					ball1.x + 10 < ((77.5 * 18) + xoff) + ball2.bx &&
					ball1.y + (ball1.by / 2) > ((76 * 9) + yoff) - ball2.by &&
					ball1.y < ((75 * 9) + yoff) + ball2.by)
					collision = true;

				//npc do meio
				else if (ball1.x + (ball1.bx / 2) > ((76 * 10) + xoff) - ball2.bx &&
					ball1.x + 10 < ((76 * 10) + xoff) + ball2.bx &&
					ball1.y + (ball1.by / 2) > ((84 * 10) + yoff) - ball2.by &&
					ball1.y < ((84 * 10) + yoff) + ball2.by)
					collision = true;

				//npc do comeco
				else if (ball1.x + (ball1.bx / 2) > ((77 * 12) + xoff) - ball2.bx &&
					ball1.x + 10 < ((76 * 12) + xoff) + ball2.bx &&
					ball1.y + (ball1.by / 2) > ((77.8 * 17) + yoff) - ball2.by &&
					ball1.y < ((77.4 * 17) + yoff) + ball2.by)
					collision = true;

				//paredes
				else if (ball1.x + (ball1.bx / 2) > ((80 * 19) + xoff) ||
					ball1.x + 10 < 80 + xoff ||
					ball1.y + (ball1.by / 2) > (80 * 19) + yoff ||
					ball1.y < (80 * 1) + yoff)
					collision = true;
				else
				{
					collision = false;
				}




				//falas e indação de qual npc esta interagindo no momento
				//local fala do npc de cima
				if (ball1.x + (ball1.bx / 2) > ((70 * 8) + xoff) - ball2.bx &&
					ball1.x + 10 < ((81 * 8) + xoff) + ball2.bx &&
					ball1.y + (ball1.by / 2) > ((52 * 3) + yoff) - ball2.by &&
					ball1.y < ((78 * 3) + yoff) + ball2.by)
				{
					npc_fala = 1;
					local_fala = true;
				}
				//esquerda
				else if (ball1.x + (ball1.bx / 2) > ((55 * 3) + xoff) - ball2.bx &&
					ball1.x + 10 < ((78 * 3) + xoff) + ball2.bx &&
					ball1.y + (ball1.by / 2) > ((80 * 12) + yoff) - ball2.by &&
					ball1.y < ((86 * 12) + yoff) + ball2.by)
				{
					npc_fala = 2;
					local_fala = true;
				}
				//direita
				else if (ball1.x + (ball1.bx / 2) > ((74 * 18) + xoff) - ball2.bx &&
					ball1.x + 10 < ((80 * 18) + xoff) + ball2.bx &&
					ball1.y + (ball1.by / 2) > ((76 * 9) + yoff) - ball2.by &&
					ball1.y < ((83 * 9) + yoff) + ball2.by)
				{
					npc_fala = 3;
					local_fala = true;
				}//meio
				else if (ball1.x + (ball1.bx / 2) > ((70 * 10) + xoff) - ball2.bx &&
					ball1.x + 10 < ((82 * 10) + xoff) + ball2.bx &&
					ball1.y + (ball1.by / 2) > ((78 * 10) + yoff) - ball2.by &&
					ball1.y < ((90 * 10) + yoff) + ball2.by)
				{
					npc_fala = 4;
					local_fala = true;
				}
				//comeco
				else if (ball1.x + (ball1.bx / 2) > ((73 * 12) + xoff) - ball2.bx &&
					ball1.x + 10 < ((80 * 12) + xoff) + ball2.bx &&
					ball1.y + (ball1.by / 2) > ((74 * 17) + yoff) - ball2.by &&
					ball1.y < ((80 * 17) + yoff) + ball2.by)
				{
					npc_fala = 5;
					local_fala = true;
				}
				else
				{
					local_fala = false;
					npc_fala = 0;
				}
				//animacao
				if (active)
				{
					sourceX += ball1.w / 3;
				}
				else
					sourceX = 80;

				if (sourceX >= ball1.w)
					sourceX = 0;
				sourceY = dir;
				draw = true;

			}


			//RENDER
			if (render && al_is_event_queue_empty(event_queue))
			{
				render = false;

				for (int i = 0; i < mapSize; i++)// desenha o mapa MAPA
				{
					al_draw_bitmap_region(bgSheet, titleSize * map[i], 0, titleSize, titleSize,
						xoff + titleSize * (i % mapColums), yoff + titleSize * (i / mapColums), 0);

				}


				//animacao
				if (draw)
				{
					al_start_timer(playerTimer);
					al_draw_bitmap_region(ball1.image, sourceX, sourceY * ball1.h / 4, 80, 80, ball1.x, ball1.y, NULL);

				}



				if (local_fala)//fala
				{
					al_play_sample_instance(inst_trilhaSonora);
					if (contador > 0)
					{	 //desenhar o local para as falas
						al_draw_filled_rectangle(0, 0, width, height, al_map_rgba_f(0, 0, 0, 0.7));
						al_draw_filled_rectangle(0, (height * 4) / 6, width, height, al_map_rgba_f(0, 0, 0, 0.9));
					}
					//fala npc cima >>tenis
					if (contador == 1 && npc_fala == 1) {
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Pedrito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Vejo que voce esta interessado em tenis. Ja que e assim vou te explicar as regras, mas primeiro ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "voce pode me chamar de 'Pedrito'. Voce sabia que o saque mais rapido ja registrado pela Associacao");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "dos Tenistas Profissionais foi dado em 2012 pelo australiano Samuel Groth, com incriveis 263km/h ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 640, NULL, "durante o torneio de Busan, na Coreia do Sul? E ainda encaixou mais 2 saques de 253km/h e de 255km/h!");

					}
					else if (contador == 2 && npc_fala == 1)

					{
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Pedrito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Durante o jogo agora voce vai precisar marcar alguns set para vencer do seu adversario. Para se ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "movimentar para esquerda e direita utilize as teclas 'A'(esquerda) e 'D'(direita). Voce so ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "precisa chegar perto o suficiente para rebater. Quem fizer 3 pontos primeiro marca um set ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 640, NULL, "e quem fizer 2 sets primeiro vence. Boa sorte! ");
					}
					else if (contador == 3 && npc_fala == 1)
					{
						al_stop_sample_instance(inst_trilhaSonora);
						contador = 0;
						inGame = true;
						displayTenis = al_create_display(widthTenis, heightTenis);
						tenis(displayTenis);
						al_flush_event_queue(event_queue);
						al_set_target_backbuffer(display);
						inGame = false;
					}

					else if (contador > 3 && npc_fala == 1) {
						al_rest(0.5);
						contador = 0;
					}



					//fala npc esquerda>>skate
					if (contador == 1 && npc_fala == 2) {
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Nikito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Parece que voce quer andar de Skate! Sabia que ele surgiu na California, Estados Unidos, nos anos 60?");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "Foi inventado por alguns surfistas, como uma brincadeira para um dia no qual nao havia ondas no mar, ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "sendo assim eles utilizaram rodinhas de patins. So em 1965 foram fabricados os primeiros skates e");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 640, NULL, "realizados os primeiros campeonatos.");
						
					}

					else if (contador == 2 && npc_fala == 2)
					{
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Nikito");//continuar com a fala depois //feito
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Agora como jogar: para ir para esquerda e direita use as teclas 'A' e 'D' ou seta esquerda e direita.");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "A tecla 'W', a seta pra cima ou o espaco te fazem pular. E se quiser sair do minigame e so pressionar a tecla 'esc'.");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "ou clicar no 'x'. No minigame voce vai precisar desviar de 2 tipos de obstaculos: o buraco que pode ser pulado, ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 640, NULL, "e o cone que nao pode ser pulado por cima. Voce precisa sobreviver por 1 min para ganhar, boa sorte!");
					}

					else if (contador == 3 && npc_fala == 2) {
						al_stop_sample_instance(inst_trilhaSonora);
						inGame = true;
						contador = 0;
						displaySkate = al_create_display(widthSkate, heightSkate);
						for (int i = 0; i <= 6; i++)
							keys[i] = false;
						skate(displaySkate);
						al_flush_event_queue(event_queue);
						al_set_target_backbuffer(display);
						inGame = false;	
					}
					else if (contador > 3 && npc_fala == 2) {
						al_rest(0.5);
						contador = 0;
					}

					//fala npc direita>>surf
					if (contador == 1 && npc_fala == 3) {
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Talonito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Temos um desafiante aqui! Pode me chamar de Talonito. Sabia que o surf e conhecido como o ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "'esporte dos Deuses'? Na Polinesia somente os reis podiam pratica-lo em pe na prancha enquanto os ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "suditos deveriam ficar deitados. Por isso que nao se deve subestimar o surf!");

					}
					else if (contador == 2 && npc_fala == 3)

					{
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Talonito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Agora, para vencer esse desafio voce precisa coletar 5 medalhas. Voce pode se movimentar utilizando ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "as teclas 'W,S,D,A' para ir pra cima, baixo, direita e esquerda. Boa sorte!");
					}
					else if (contador == 3 && npc_fala == 3)
					{
						al_stop_sample_instance(inst_trilhaSonora);
						contador = 0;
						inGame = true;
						displaySurf = al_create_display(widthSurf, heightSurf);
						surf(displaySurf);
						al_flush_event_queue(event_queue);
						al_set_target_backbuffer(display);
						inGame = false;
					}

					else if (contador > 3 && npc_fala == 3) {
						al_rest(0.5);
						contador = 0;
					}



					//fala npc meio
					if (contador == 1 && npc_fala == 4) {
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530 , ALLEGRO_ALIGN_CENTRE, "Shodito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Para andar pressione as teclas 'w,s,a,d' e para interagir com os npcs pressione a tecla 'z'");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "Oi, voce pode me chamar de 'Shodito', e irei te falar onde estao os lugares para voce jogar");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "e tambem vou falar sobre umas curiosidades das olimpiadas que pode te interessar, eu acho.");

					}
					else if (contador == 2 && npc_fala == 4)

					{
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Shodito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Voce sabia que os JOGOS Olimpicos se originaram na Grecia Antiga na cidade de Olimpia onde tiveram ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "um forte vinculo religioso e eram dedicados a Zeus(Deus dos deuses dentro da mitologia grega)?");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "As argolas na bandeira das olimpiadas representam os 5 continentes habitados no mundo.");
					}
					else if (contador == 3 && npc_fala == 4)
					{
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Shodito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Agora vou falar onde voce pode jogar, falando com o cara la de cima voce pode jogar um ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "pouco de tenis, mas se quiser pode jogar quantas vezes quiser. Indo pela direita voce ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "pode surfar falando com o Talonito. Na direcao de baixo pode praticar um boxe com o ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 640, NULL, "Branquito e na esquerda pode andar de skate com o Nikito.");

					}
					else if (contador == 4 && npc_fala == 4)
					{
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Shodito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Para vencer o jogo e so vencer os 4 mini-jogos! ");
					}
					else if (contador == 5 && npc_fala == 4) {
						contador = 0;
					}
					//fala npc comeco // baixo>>>boxe
					if (contador == 1 && npc_fala == 5)
					{
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Branquito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Que tal tentar jogar uma partida de Boxe? Pode me chamar de Branquito! Voce sabia que ele era ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "popular na Inglaterra nos seculos XVIII e XIX e que as lutas eram feitas com as maos descobertas? ");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "Ele so foi reformado em 1867 com as Regras de Quensberry, que previam rounds de tres minutos.");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 640, NULL, "E o uso de luvas so entraram em vigor em 1872.");
					}
					else if (contador == 2 && npc_fala == 5)
					{
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Branquito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "Deixa eu te explicar como funcionam as regras aqui: o jogo e baseado em prever a habilidade do inimigo, voce");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "tem 3 opcoes de habilidade, sendo elas 'jeb', 'chute' e 'combo'. Cada um tem uma certa vantagem ");//continuar com a fala depois
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "sobre a outra. Para nocautear o inimigo, tera que acertar 3 golpes nele.");

					}
					else if (contador == 3 && npc_fala == 5)
					{
						al_draw_text(font18, al_map_rgb(255, 255, 0), width / 12, 530, ALLEGRO_ALIGN_CENTRE, "Branquito");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 580, NULL, "A ordem de vantagens e:");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 600, NULL, "JEB > CHUTE");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 620, NULL, "CHUTE > COMBO");
						al_draw_text(font18, al_map_rgb(255, 255, 0), 15, 640, NULL, "COMBO > JEB ");

					}
					else if (contador == 4 && npc_fala == 5)
					{
						al_stop_sample_instance(inst_trilhaSonora);
						inGame = true;
						displayBoxe = al_create_display(widthBoxe, heightBoxe);
						boxe(displayBoxe);
						al_flush_event_queue(event_queue);
						al_set_target_backbuffer(display);
						inGame = false;
						contador++;
					}
					else if(contador > 4 && npc_fala == 5){
						contador = 0;
						al_rest(0.5);
					}
				}

				if (collision)//quando acontecer o colisao o player volta para o lado oposto que veio
				{
					
					if (keys[RIGHT] == true)
					{
						keys[RIGHT] = false;
						ball1.x -= 10;
						xoff += 17;
					}
					else if (keys[LEFT] == true)
					{
						keys[LEFT] = false;
						ball1.x += 10;
						xoff -= 17;
					}
					else if (keys[DOWN] == true)
					{
						keys[DOWN] = false;
						ball1.y -= 10;
						yoff += 17;
					}

					else if (keys[UP] == true)
					{
						keys[UP] = false;
						ball1.y += 10;
						yoff -= 17;
					}

				}

				if (somaPontos >= 4) {
					inGame = true;
					done = true;
				}

				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
			}
		}
	}
	//DESTROY ALLEGRO OBJECTS
	if (pontoBoxe + pontoSkate + pontoSurf + pontoTenis == 4) {
		al_clear_to_color(al_map_rgb(0, 255, 0));
		al_draw_text(font50, al_map_rgb(255, 255, 255), width / 2, height / 2, ALLEGRO_ALIGN_CENTER, "Parabéns!");
		al_draw_text(font50, al_map_rgb(255, 255, 255), width / 2, height / 2 + 20, ALLEGRO_ALIGN_CENTER, "Você venceu os 4 jogos, tornou-se digno de receber uma medalha!");
		al_flip_display();
		al_rest(5.0);
	}

	al_destroy_font(font18);
	al_destroy_timer(timer);
	al_destroy_timer(playerTimer);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);

	return 0;
}


