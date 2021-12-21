#define is_Down(b) input->buttons[b].isDown
#define pressed(b) (input->buttons[b].isDown && input->buttons[b].changed)
#define released(b) (!input->buttons[b].isDown && input->buttons[b].changed)

enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY
};

Gamemode CurrentGamemode = GM_MENU;
int hotButton = 0;
bool enemyAI;
bool secondAI;

//playerDP is the velocity of the player, or the player derivative
float player1P = 0.f, player1Dp = 0.f, player2P = 0.f, player2Dp = 0.f;
int player1Score = 0, player2Score = 0;
float arenaHalfSizeX = 85, arenaHalfSizeY = 45;
float playerHalfSizeX = 2.5, playerHalfSizeY = 12;
float ballPX = 0, ballPY = 0, ballDpX = 75, ballDpY = 0, ballHalfSize = 1;

void simulatePlayer(float* p, float* dp, float ddp, float dt) {
	ddp -= *dp * 10.f;//Friction

	//Normal movement 
	*p = *p + *dp * dt + ddp * dt * dt * .5f;
	*dp = *dp + ddp * dt;

	//Movement without acceleration
	//*p = *p + (ddp / 15) * dt;

	if (*p + playerHalfSizeY > arenaHalfSizeY) {
		*p = arenaHalfSizeY - playerHalfSizeY;
		*dp *= 0;
	}
	else if (*p - playerHalfSizeY < -arenaHalfSizeY) {
		*p = -arenaHalfSizeY + playerHalfSizeY;
		*dp *= 0;
	}
}

bool colidedHitboxes(float p1X, float p1Y, float hs1X, float hs1Y,
	float p2X, float p2Y, float hs2X, float hs2Y) {
	return p1X + hs1X > p2X - hs2X &&
		p1X + hs1X < p2X + hs2X &&
		p1Y + hs1Y > p2Y - hs2Y &&
		p1Y + hs1Y < p2Y + hs2Y;
}

void simulateGame(Input* input, float dt) {
	ClearScreen(0x000000);
	DrawRect(0, 0, 86, 46, 0xFFFFFF);
	DrawRect(0, 0, 85, 45, 0x000000);

	if (pressed(BUTTON_ESCAPE)) {
		running = false;
	}

	if (CurrentGamemode == GM_GAMEPLAY) {
		//playerDdp is the derivative of the dirivative or how the velocity is changing. 
		//The higher the number the faster the place accelerates
		float player1Ddp = 0.f, player2Ddp = 0.f;

		if (!enemyAI) {
			if (is_Down(BUTTON_UP)) {
				player1Ddp += 1750;
			}
			if (is_Down(BUTTON_DOWN)) {
				player1Ddp -= 1750;
			}
		}
		else {
			//if (ballPY > player1P+2.f) player1Ddp += 1800;
			//if (ballPY < player1P-2.f) player1Ddp -= 1800;
			player1Ddp = (ballPY - player1P) * 100;
			if (player1Ddp > 1300) player1Ddp = 1300;
			if (player1Ddp < -1300) player1Ddp = -1300;
		}

		if (!secondAI) {
			if (is_Down(BUTTON_W)) {
				player2Ddp += 1750;
			}
			if (is_Down(BUTTON_S)) {
				player2Ddp -= 1750;
			}
		}
		else {
			//if (ballPY > player2P+2.f) player2Ddp += 1300;
			//if (ballPY < player2Pp-2.f) player2Ddp -= 1300;
			player2Ddp = (ballPY - player2P) * 100;
			if (player2Ddp > 1300) player2Ddp = 1300;
			if (player2Ddp < -1300) player2Ddp = -1300;
		}

		simulatePlayer(&player1P, &player1Dp, player1Ddp, dt);
		simulatePlayer(&player2P, &player2Dp, player2Ddp, dt);

		//Simulate Ball
		{
			ballPX += ballDpX * dt;
			ballPY += ballDpY * dt;

			if (colidedHitboxes(ballPX, ballPY, ballHalfSize, ballHalfSize, 80, player1P, playerHalfSizeX, playerHalfSizeY)) {
				ballPX = 80 - playerHalfSizeX - ballHalfSize;
				ballDpX *= -1;
				ballDpY = (ballPY - player1P) * 1.5 + (player1Dp * .75f);
			}
			else if (colidedHitboxes(ballPX, ballPY, ballHalfSize, ballHalfSize, -80, player2P, playerHalfSizeX, playerHalfSizeY)) {
				ballPX = -80 + playerHalfSizeX + ballHalfSize;
				ballDpX *= -1;
				ballDpY = (ballPY - player2P) * 1.5 + (player2Dp * .75f);
			}

			if (ballPY + ballHalfSize > arenaHalfSizeY) {
				ballPY = arenaHalfSizeY - ballHalfSize;
				ballDpY *= -1;
			}
			else if (ballPY - ballHalfSize < -arenaHalfSizeY) {
				ballPY = -arenaHalfSizeY + ballHalfSize;
				ballDpY *= -1;
			}

			if (ballPX - ballHalfSize < -arenaHalfSizeX) {
				ballPX = 0;
				ballPY = 0;
				ballDpX *= -1;
				ballDpY = 0;
				player2Score++;
			}
			else if (ballPX + ballHalfSize > arenaHalfSizeX) {
				ballPX = 0;
				ballPY = 0;
				ballDpX *= -1;
				ballDpY = 0;
				player1Score++;
			}
		}

		//Rendering
		DrawRect(ballPX, ballPY, ballHalfSize, ballHalfSize, 0xFFFFFF);

		DrawNumber(player1Score, -10, 40, 1.f, 0xFFFFFF);
		DrawNumber(player2Score, 10, 40, 1.f, 0XFFFFFF);

		DrawRect(80, player1P, playerHalfSizeX, playerHalfSizeY, 0xFFFFFF);
		DrawRect(-80, player2P, playerHalfSizeX, playerHalfSizeY, 0xFFFFFF);

	}
	else {
		if (pressed(BUTTON_LEFT)) {
			hotButton--;
			if (hotButton < 0) {
				hotButton = 2;
			}
		}
		if (pressed(BUTTON_RIGHT)) {
			hotButton++;
			if (hotButton > 2) {
				hotButton = 0;
			}
		}
		if (pressed(BUTTON_ENTER)) {
			CurrentGamemode = GM_GAMEPLAY;
			switch (hotButton) {
				case 0:
					enemyAI = true;
					secondAI = true;
					player2P = -30;
					player1P = 30;
					break;
				case 1:
					enemyAI = true;
					secondAI = false;
					player1P = 30;
					break;
				case 2:
					enemyAI = false;
					secondAI = false;
					break;
			}
		}
		switch (hotButton) {
			case 0:
				DrawNumber(0, -60, 0, 10, 0xFF0000);
				DrawNumber(1, -10, 0, 10, 0xFFFFFF);
				DrawNumber(2, 60, 0, 10, 0xFFFFFF);
				break;
			case 1:
				DrawNumber(0, -60, 0, 10, 0xFFFFFF);
				DrawNumber(1, -10, 0, 10, 0xFF0000);
				DrawNumber(2, 60, 0, 10, 0xFFFFFF);
				break;
			case 2:
				DrawNumber(0, -60, 0, 10, 0xFFFFFF);
				DrawNumber(1, -10, 0, 10, 0xFFFFFF);
				DrawNumber(2, 60, 0, 10, 0xFF0000);
				break;
		}
	}
}