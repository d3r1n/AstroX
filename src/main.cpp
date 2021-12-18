//---------
// Includes
//---------

// === Standart Library ===
#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <string>
#include <math.h>
#include <iterator>

// === Other Libraries ===
#include "raylib.h"
#include "custom_button.hpp"

//-------------------
// Constant Variables
//-------------------

// === Player ===
#define PLAYER_SIZE  0.2f
#define PLAYER_SPEED 300.0f
#define PLAYER_ROTATION_SPEED 250.0f
#define PLAYER_ACCELERATION 0.5f
#define PLAYER_MAX_ACCELERATION 2.5f
#define DRAG 0.02f

// === Bullet ===
#define BULLET_SPEED 400.0f
#define BULLET_SIZE 0.4f
#define SHOOTING_DELAY 0.1f // 0.1 seconds

// === Asteroid ===
#define ASTEROID_BIG_SIZE 0.5f
#define ASTEROID_MEDIUM_SIZE 0.3f
#define ASTEROID_SMALL_SIZE 0.1f
#define ASTEROID_MAX_SPEED 200.0f
#define ASTEROID_MIN_SPEED 150.0f

using namespace std::chrono;

//------
// Types
//------
struct Player {
    Vector2 position;
    Vector2 speed;
    float acceleration;
    float rotation;
};

struct Bullet {
	Vector2 position;
	Vector2 speed;
	float rotation;
	float radius;
};

struct Asteroid {
	Texture2D texture;
	Vector2 position;
	Vector2 speed;
	float rotation;
	float radius;
	int status; // 2 = big, 1 = medium, 0 = small
};

struct StartText {
	Vector2 position;
	Texture2D texture;
};

//--------
// Globals
//--------

// === Game Variables ===
static const int screenWidth = 1000;
static const int screenHeight = 800;

static bool startScreen = true;
static bool gameOver = false;
static bool pause = false;
static bool victory = false;
static bool debug = false;

static int score = 0;
static int lives = 3;

// === Player ===
static Player player = { 0 };
static Texture2D playerTexture;
static Texture2D playerTextureFlight;
static bool playerFlying = false;
static float shipHeight = 0.0f;

// === User Interface ===
static CustomButton btnPause;
static Texture2D btnPauseTexture;
static Texture2D btnPauseTextureHover;

static CustomButton btnDebug;
static Texture2D btnDebugTexture;
static Texture2D btnDebugTextureHover;

static StartText startText;
static Texture2D gameOverTexture;

static CustomButton startButton;
static Texture2D startButtonTexture;
static Texture2D startButtonTextureHover;

// === Bullets ===
static Texture2D bulletTexture;
static std::vector<Bullet> bullets;

// === Asteroids ===
static Texture2D asteroidTexture1;
static Texture2D asteroidTexture2;
static Texture2D asteroidTexture3;
static std::vector<Asteroid> asteroids;

static double shotTime = -SHOOTING_DELAY;


// === Function prototypes ===
static void InitGame();         								// Initialize game
static void UpdateGame();       								// Update game (one frame)
static void DrawGame();        									// Draw game (one frame)
static void UnloadGame();       								// Unload game
static void UpdateDrawFrame();  								// Update and Draw (one frame)

static void UpdatePlayer();     								// Update player
static void DrawPlayer(); 										// Draw player

static void DrawDebugInfo();									// Draw debug info
static void DisplayScore();										// Display score
static void DrawPlayerLives();									// Draw player lives

static void Shoot();											// Shoot
static void UpdateBullets(std::vector<Bullet> &bullets);		// Update bullets
static void DrawBullet(Bullet bullet);				    		// Draw bullet
static void DrawBullets(std::vector<Bullet> bullets);			// Draw bullets

static void SpawnAsteroid(int status, Vector2 position);		// Spawn asteroid
static void DrawAsteroid(Asteroid asteroid);					// Draw asteroid
static void DrawAsteroids(std::vector<Asteroid> asteroids);		// Draw asteroids
static void UpdateAsteroids(std::vector<Asteroid> &asteroids);	// Update asteroids

int random_int(int range_from, int range_to);					// Generate random int
double random_dbl(double range_from, double range_to);			// Generate random double
float random_flt(float range_from, float range_to);				// Generate random float


int main()
{
    InitWindow(screenWidth, screenHeight, "AstroX");
    SetTargetFPS(120);
    InitGame();

    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }

    UnloadGame();        
    CloseWindow(); 

    return 0;
}

//-----------
// Functions
//-----------

// Initialize game variables
void InitGame()
{
    bool correctRange = false;
    gameOver = false;
	pause = false;
	victory = false;
	debug = false;

	score = 0;
	lives = 3;


    // Initialization player
	playerTextureFlight = LoadTexture("./assets/Flight.png");
	playerTexture = LoadTexture("./assets/Spaceship.png");
    player.position = Vector2 {screenWidth/2 - (float) playerTexture.width * PLAYER_SIZE, screenHeight / 2 - (float) playerTexture.height * PLAYER_SIZE};
    player.speed = Vector2 {0, 0};
    player.acceleration = 0;
    player.rotation = 0;
	shipHeight = (1.2 * PLAYER_SIZE) / tanf(20*DEG2RAD);

	// Initialization buttons
	btnPauseTexture = LoadTexture("./assets/pause_btn.png");
	btnPauseTextureHover = LoadTexture("./assets/pause_btn_hover.png");

	btnPause = CustomButton(Vector2{ (float)(0), 0 }, 0.30f, btnPauseTexture, "", 20, BLACK );
	btnPause.setPosition(Vector2{ (float)(screenWidth - (btnPauseTexture.width * btnPause.scale) - 15), 15 });

	btnDebugTexture = LoadTexture("./assets/debug_btn.png");
	btnDebugTextureHover = LoadTexture("./assets/debug_btn_hover.png");

	btnDebug = CustomButton(Vector2{ (float)(0), 0 }, 0.30f, btnDebugTexture, "", 20, BLACK );
	btnDebug.setPosition(Vector2{ (float)(screenWidth - (btnDebugTexture.height * btnDebug.scale) - (btnPauseTexture.width * btnPause.scale) - 30), 15});

	startText.texture = LoadTexture("./assets/astrox.png");
	startText.position = Vector2{(float)screenWidth/2 - (startText.texture.width/2), (float)screenHeight/2 - (startText.texture.height/2) - 150};

	gameOverTexture = LoadTexture("./assets/game_over_banner.png");

	startButtonTexture = LoadTexture("./assets/start_btn.png");
	startButtonTextureHover = LoadTexture("./assets/start_btn_hover.png");

	startButton = CustomButton(Vector2{ (float)(0), 0 }, 0.30f, startButtonTexture, "", 20, BLACK );
	startButton.setPosition(Vector2{ (float)(screenWidth/2 - (startButtonTexture.width * startButton.scale) / 2), (float)(screenHeight/2 - (startButtonTexture.height * startButton.scale) / 2) });

	// Initialization bullets
	bulletTexture = LoadTexture("./assets/bullet.png");
	bullets.clear();

	// Initialization asteroids
	asteroidTexture1 = LoadTexture("./assets/asteroid_1.png");
	asteroidTexture2 = LoadTexture("./assets/asteroid_2.png");
	asteroidTexture3 = LoadTexture("./assets/asteroid_3.png");
	asteroids.clear();
}

// Update game (one frame)
void UpdateGame(void)
{
	if (startScreen) {

		if (startButton.isClicked()) {
			startScreen = false;
		}

		if (startButton.isHovered()) {
			startButton.setTexture(startButtonTextureHover);
		} else {
			startButton.setTexture(startButtonTexture);
		}
	}

    if (!gameOver)
    {

		bool shoot = false;

        if (IsKeyPressed('P') || btnPause.isClicked()) pause = !pause;

		if (btnPause.isHovered()) {
			btnPause.setTexture(btnPauseTextureHover);
		}
		else {
			btnPause.setTexture(btnPauseTexture);
		}

		if (btnDebug.isHovered()) {
			btnDebug.setTexture(btnDebugTextureHover);
		}
		else {
			btnDebug.setTexture(btnDebugTexture);
		}

		if (btnDebug.isClicked()) debug = !debug;

        if (!pause)
        {

            UpdatePlayer();
						

			if (IsKeyDown(KEY_SPACE)) {
				double delta = GetTime() - shotTime;

				if (delta > SHOOTING_DELAY) {
					shoot = true;
                	shotTime = GetTime();
				}				
			}

			if (shoot) Shoot();

			UpdateBullets(bullets);

			UpdateAsteroids(asteroids);

			if (asteroids.size() == 0) {
				for (int i = 0; i < random_int(2, 6); i++) {
					SpawnAsteroid(2, Vector2{ (float)random_int(0, screenWidth), (float)random_int(0, screenHeight) });	
				}
			}
		}
		else
    	{
			if (IsKeyPressed(KEY_ENTER))
			{
				UnloadGame();
				InitGame();
			}
    	}
	}
	else {
		if (IsKeyPressed(KEY_ENTER))
		{
			UnloadGame();
			InitGame();
		}
	}
}

// Draw game (one frame)
void DrawGame()
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

		if (startScreen)
		{
			// Draw Start UI
			DrawTexture(startText.texture, startText.position.x, startText.position.y, WHITE);
			startButton.Draw();
		}

		if (!gameOver && !startScreen)
		{
			// Draw bullets
			DrawBullets(bullets);

			// Draw asteroids
			DrawAsteroids(asteroids);

			// Draw Player
			DrawPlayer();

			// Draw score
			DisplayScore();

			// Draw Player Lives
			DrawPlayerLives();

			// Draw button
			btnPause.Draw();
			btnDebug.Draw();

			if (debug) {
				// Draw debug info
				DrawDebugInfo();
			}
		}
		else if (gameOver)
		{
			// Draw game over banner
			DrawTexture(gameOverTexture, (screenWidth/2) - (gameOverTexture.width/2), (screenHeight/2) - (gameOverTexture.height/2), WHITE);
		}

        if (!gameOver)
        {
            if (victory) DrawText("VICTORY", screenWidth/2 - MeasureText("VICTORY", 20)/2, screenHeight/2, 20, LIGHTGRAY);
            if (pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, Color{150, 150, 150, 255});
        }

    EndDrawing();
}

// Unload game variables
void UnloadGame()
{
	UnloadTexture(playerTexture);
	UnloadTexture(startText.texture);
	UnloadTexture(btnPauseTexture);
	UnloadTexture(btnPauseTextureHover);
	UnloadTexture(btnDebugTexture);
	UnloadTexture(btnDebugTextureHover);
	UnloadTexture(startButtonTexture);
	UnloadTexture(startButtonTextureHover);
	UnloadTexture(bulletTexture);

}

// Update and Draw (one frame)
void UpdateDrawFrame()
{
    UpdateGame();
    DrawGame();
}

void DrawDebugInfo() {
	// Draw FPS
	DrawText("FPS:", 10, 10, 20, BLACK);
	DrawText(std::to_string(GetFPS()).c_str(), MeasureText("FPS:", 20) + 20, 10, 20, GREEN);
				
	// Draw player acceleration
	DrawText("Acceleration:", 10, 30, 20, BLACK);
	DrawText(std::to_string(player.acceleration).c_str(), MeasureText("Acceleration:", 20) + 20, 30, 20, BLACK);

	// Draw player rotation		
	DrawText("Rotation:", 10, 50, 20, BLACK);
	DrawText(std::to_string(player.rotation).c_str(), MeasureText("Rotation:", 20) + 20, 50, 20, BLACK);

	// Draw Bullet count		
	DrawText("Bullet count:", 10, 70, 20, BLACK);
	DrawText(std::to_string(bullets.size()).c_str(), MeasureText("Bullet count:", 20) + 20, 70, 20, BLACK);
}

void UpdatePlayer() {
	// Player logic: rotation
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) player.rotation -= PLAYER_ROTATION_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player.rotation += PLAYER_ROTATION_SPEED * GetFrameTime();
	if (player.rotation > 360) player.rotation -= 360;
	if (player.rotation < -360) player.rotation += 360;

    // Player logic: speed
    player.speed.x = sin(player.rotation*DEG2RAD)*PLAYER_SPEED * GetFrameTime() ;
    player.speed.y = cos(player.rotation*DEG2RAD)*PLAYER_SPEED * GetFrameTime();

    // Player logic: acceleration
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
		playerFlying = true;

        if (player.acceleration < PLAYER_MAX_ACCELERATION) player.acceleration += PLAYER_ACCELERATION * ( DRAG * 2);
		else if (player.acceleration > PLAYER_MAX_ACCELERATION) player.acceleration = PLAYER_MAX_ACCELERATION;
	}
    else
    {
		playerFlying = false;

        if (player.acceleration > 0) player.acceleration -= DRAG;
        else if (player.acceleration < 0) player.acceleration = 0;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        if (player.acceleration > 0) player.acceleration -= PLAYER_ACCELERATION / 2;
        else if (player.acceleration < 0) player.acceleration = 0;
    }

    // Player logic: movement
    player.position.x += (player.speed.x * player.acceleration);
    player.position.y -= (player.speed.y * player.acceleration);

    // Collision logic: player vs walls
    if (player.position.x > screenWidth + shipHeight) player.position.x = -(shipHeight);
    else if (player.position.x < -(shipHeight)) player.position.x = screenWidth + shipHeight;
    if (player.position.y > (screenHeight + shipHeight)) player.position.y = -(shipHeight);
    else if (player.position.y < -(shipHeight)) player.position.y = screenHeight + shipHeight;

	// Player Lives:
	if (lives == -1) {
		gameOver = true;
	}
}

void DrawPlayer() {
	// Draw player
	if (playerFlying) {
		
		DrawTexturePro(
			playerTextureFlight, 
			Rectangle {1, 1, (float)playerTextureFlight.width, (float)playerTextureFlight.height}, 
			Rectangle {player.position.x, player.position.y, (float)playerTextureFlight.width * PLAYER_SIZE, (float)playerTextureFlight.height * PLAYER_SIZE}, 
			Vector2 {(float)playerTextureFlight.width * PLAYER_SIZE / 2, (float)playerTextureFlight.height * PLAYER_SIZE / 2}, 
			player.rotation,
			WHITE
		);
	}
	else {
		DrawTexturePro(
			playerTexture, 
			Rectangle {1, 1, (float)playerTexture.width, (float)playerTexture.height}, 
			Rectangle {player.position.x, player.position.y, (float)playerTexture.width * PLAYER_SIZE, (float)playerTexture.height * PLAYER_SIZE}, 
			Vector2 {(float)playerTexture.width * PLAYER_SIZE / 2, (float)playerTexture.height * PLAYER_SIZE / 2}, 
			player.rotation,
			WHITE
		);
	}

	if (debug) {
		DrawCircleV(player.position,  playerTexture.width * PLAYER_SIZE / 2, Color{ 61, 168, 255, 175 });
	}
}


void Shoot() {
	// Bullet logic: spawn
	Bullet bullet;
	bullet.position = Vector2 { player.position.x + sin(player.rotation*DEG2RAD)*shipHeight, player.position.y - cos(player.rotation*DEG2RAD)*shipHeight };
	bullet.speed.x = BULLET_SPEED*sin(player.rotation*DEG2RAD)*PLAYER_SPEED * GetFrameTime();
	bullet.speed.y = BULLET_SPEED*cos(player.rotation*DEG2RAD)*PLAYER_SPEED * GetFrameTime();
	bullet.rotation = player.rotation;
	bullet.radius = bulletTexture.width * BULLET_SIZE / 2;

	// Add bullet to list
	bullets.push_back(bullet);
}

void UpdateBullets(std::vector<Bullet> &bullets) {
	// Bullet logic: movement
	for (int i = 0; i < bullets.size(); i++) {
		// Bullet logic: collision with screen borders
		if (bullets[i].position.x < -bullets[i].radius || bullets[i].position.x > screenWidth + bullets[i].radius || bullets[i].position.y < -bullets[i].radius || bullets[i].position.y > screenHeight + bullets[i].radius) {
			bullets.erase(bullets.begin() + i);
		}
		else {
			// Movement
			bullets[i].position.x += bullets[i].speed.x * GetFrameTime();
			bullets[i].position.y -= bullets[i].speed.y * GetFrameTime();
		}
	}
}

void DrawBullet(Bullet bullet) {
	DrawTexturePro(
		bulletTexture,
		Rectangle {1, 1, (float)bulletTexture.width, (float)bulletTexture.height},
		Rectangle {bullet.position.x, bullet.position.y, (float)bulletTexture.width * BULLET_SIZE, (float)bulletTexture.height * BULLET_SIZE},
		Vector2 {(float)bulletTexture.width * BULLET_SIZE / 2, (float)bulletTexture.height * BULLET_SIZE / 2},
		bullet.rotation,
		WHITE
	);

	if (debug) {
		DrawCircleV(bullet.position, bullet.radius, Color{ 0, 228, 48, 175 });
	}
}

void DrawBullets(std::vector<Bullet> bullets) {
	for (int i = 0; i < bullets.size(); i++) {
		DrawBullet(bullets[i]);
	}
}

void SpawnAsteroid(int status, Vector2 position) {
	// Asteroid logic: spawn
	std::vector<Texture2D> asteroidTextures = { asteroidTexture1, asteroidTexture2, asteroidTexture3 }; // Asteroid textures
	Texture2D rand_text = asteroidTextures[random_int(0, asteroidTextures.size() - 1)]; // Random asteroid texture

	Asteroid asteroid; 
	asteroid.texture = rand_text;
	asteroid.rotation = random_int(0, 360); // Random rotation

	asteroid.speed = Vector2 { // random speed
		(float)random_dbl(ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED)*sin(asteroid.rotation*DEG2RAD), 
		(float)random_dbl(ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED)*cos(asteroid.rotation*DEG2RAD) 
	};

	asteroid.position = position;
	asteroid.status = status;
	
	switch (status) {
		case 2:
			asteroid.radius = (float)(rand_text.width * ASTEROID_BIG_SIZE) / 2;
			break;

		case 1:
			asteroid.radius = (float)(rand_text.width * ASTEROID_MEDIUM_SIZE) / 2;
			break;

		case 0:
			asteroid.radius = (float)(rand_text.width * ASTEROID_SMALL_SIZE) / 2;
			break;
	}
	
	asteroids.push_back(asteroid); // Add asteroid to list
}

void DrawAsteroid(Asteroid asteroid) {

	// Draw asteroid according to its status
	switch (asteroid.status) {
		case 2:
			DrawTexturePro(
                asteroid.texture,
               	Rectangle {1, 1, (float)asteroid.texture.width, (float)asteroid.texture.height},
                Rectangle {asteroid.position.x, asteroid.position.y, (float)asteroid.texture.width * ASTEROID_BIG_SIZE, (float)asteroid.texture.height * ASTEROID_BIG_SIZE},
                Vector2 {(float)asteroid.texture.width * ASTEROID_BIG_SIZE / 2, (float)asteroid.texture.height * ASTEROID_BIG_SIZE / 2},
            	asteroid.rotation,
               	WHITE
			);
			break;

		case 1:
			DrawTexturePro(
				asteroid.texture,
				Rectangle {1, 1, (float)asteroid.texture.width, (float)asteroid.texture.height},
				Rectangle {asteroid.position.x, asteroid.position.y, (float)asteroid.texture.width * ASTEROID_MEDIUM_SIZE, (float)asteroid.texture.height * ASTEROID_MEDIUM_SIZE},
				Vector2 {(float)asteroid.texture.width * ASTEROID_MEDIUM_SIZE / 2, (float)asteroid.texture.height * ASTEROID_MEDIUM_SIZE / 2},
				asteroid.rotation,
				WHITE
			);
			break;

		case 0:
			DrawTexturePro(
				asteroid.texture,
				Rectangle {1, 1, (float)asteroid.texture.width, (float)asteroid.texture.height},
				Rectangle {asteroid.position.x, asteroid.position.y, (float)asteroid.texture.width * ASTEROID_SMALL_SIZE, (float)asteroid.texture.height * ASTEROID_SMALL_SIZE},
				Vector2 {(float)asteroid.texture.width * ASTEROID_SMALL_SIZE / 2, (float)asteroid.texture.height * ASTEROID_SMALL_SIZE / 2},
				asteroid.rotation,
				WHITE
			);
			break;
	}

	if (debug) {
		DrawCircleV(asteroid.position, asteroid.radius, Color{255, 71, 96, 175});
	}
}

void DrawAsteroids(std::vector<Asteroid> asteroids) {
	for (int i = 0; i < asteroids.size(); i++) {
		DrawAsteroid(asteroids[i]);
	}
}

void UpdateAsteroids(std::vector<Asteroid> &asteroids) {
	if (!startScreen) {
		for (int i = 0; i < asteroids.size(); i++) {
			// Movement
			asteroids[i].position.x += asteroids[i].speed.x * GetFrameTime();
			asteroids[i].position.y -= asteroids[i].speed.y * GetFrameTime();

			// Check if asteroid is out of screen bounds 
			if (asteroids[i].position.x > screenWidth + asteroids[i].radius) asteroids[i].position.x = -(asteroids[i].radius);
			else if (asteroids[i].position.x < -(asteroids[i].radius)) asteroids[i].position.x = screenWidth + asteroids[i].radius;
			if (asteroids[i].position.y > (screenHeight + asteroids[i].radius)) asteroids[i].position.y = -(asteroids[i].radius);
			else if (asteroids[i].position.y < -(asteroids[i].radius)) asteroids[i].position.y = screenHeight + asteroids[i].radius;

			// Check if asteroid is colliding with player
			if (CheckCollisionCircles(asteroids[i].position, asteroids[i].radius, player.position, playerTexture.width * PLAYER_SIZE / 2.5)) {
				asteroids.erase(asteroids.begin() + i);
				lives--;
			}

			// Check if asteroid is colliding with bullets
			for (int j = 0; j < bullets.size(); j++) {
				if (CheckCollisionCircles(asteroids[i].position, asteroids[i].radius, bullets[j].position, bullets[j].radius)) {
					bullets.erase(bullets.begin() + j);
					
					if (asteroids[i].status == 2) {

						for (int k = 0; k < random_int(2, 3); k++) {
							SpawnAsteroid(1, asteroids[i].position);
						}
						
						asteroids.erase(asteroids.begin() + i);
					}
					else if (asteroids[i].status == 1) {
						
						for (int k = 0; k < random_int(2, 3); k++) {
							SpawnAsteroid(0, asteroids[i].position);
						}

						asteroids.erase(asteroids.begin() + i);
					}
					else if (asteroids[i].status == 0) {
						asteroids.erase(asteroids.begin() + i);
						score += 1;
					}

				}
			}
		}
	}
}

void DisplayScore() {
	DrawText(
		std::to_string(score).c_str(),
		screenWidth / 2 - MeasureText(std::to_string(score).c_str(), 80) / 2,
		40,
		80,
		BLACK
	);
}

void DrawPlayerLives() {
	if (!debug) {	
		for (int i = 1; i <= lives; i++) {

			DrawTextureEx(
				playerTexture,
				Vector2 {
					(float)((i * playerTexture.width * PLAYER_SIZE)),
					20.0f,
				},
				0.0f,
				PLAYER_SIZE,
				WHITE
			);
		}
	}
	else {
		for (int i = 1; i <= lives; i++) {

			DrawTextureEx(
				playerTexture,
				Vector2 {
					(float)((i * playerTexture.width * PLAYER_SIZE)),
					100.0f,
				},
				0.0f,
				PLAYER_SIZE,
				WHITE
			);
		}
	}
}

int random_int(int range_from, int range_to) {
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>    distr(range_from, range_to);
    return distr(generator);
}

double random_dbl(double range_from, double range_to) {
	std::random_device                  rand_dev;
	std::mt19937                        generator(rand_dev());
	std::uniform_real_distribution<double>    distr(range_from, range_to);
	return distr(generator);
}

float random_flt(float range_from, float range_to) {
	std::random_device                  rand_dev;
	std::mt19937                        generator(rand_dev());
	std::uniform_real_distribution<float>    distr(range_from, range_to);
	return distr(generator);
}