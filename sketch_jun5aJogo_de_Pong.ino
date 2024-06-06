#include <Wire.h>
#include <U8g2lib.h>
#include <Arduino.h>

// Pinos dos botões
const int buttonPins[4] = {6, 7, 8, 9};

// Pino do buzzer (opcional)
const int buzzerPin = 10;

// Instância do display OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Variáveis para os paddles e a bola
int paddle1Y = 24;
int paddle2Y = 24;
const int paddleHeight = 16;
const int paddleWidth = 2;
int ballX = 64;
int ballY = 32;
int ballSpeedX = 2;
int ballSpeedY = 1;

// Variáveis para o placar
int score1 = 0;
int score2 = 0;

// Variável para verificar o vencedor
bool gameOver = false;

// Função para inicializar o display OLED
void setupDisplay() {
  display.begin();
  display.clearBuffer();
  display.setFont(u8g2_font_ncenB08_tr);
}

// Função para desenhar os paddles e a bola na tela
void drawGame() {
  display.clearBuffer();
  // Desenha os paddles
  display.drawBox(0, paddle1Y, paddleWidth, paddleHeight);
  display.drawBox(126, paddle2Y, paddleWidth, paddleHeight);
  // Desenha a bola
  display.drawBox(ballX, ballY, 2, 2);
  // Desenha o placar
  display.setCursor(54, 10);
  display.print(score1);
  display.setCursor(74, 10);
  display.print(score2);
  // Verifica se houve um vencedor
  if (gameOver) {
    display.setCursor(10, 30);
    display.print("Parabens!");
    display.setCursor(10, 50);
    display.print("Pres. qualquer botao para reiniciar");
  }
  display.sendBuffer();
}

// Função para atualizar a posição da bola
void updateBall() {
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Rebater a bola nas paredes superior e inferior
  if (ballY <= 0 || ballY >= 62) {
    ballSpeedY = -ballSpeedY;
  }

  // Verificar colisão com paddle 1
  if (ballX <= 2 && ballY >= paddle1Y && ballY <= paddle1Y + paddleHeight) {
    ballSpeedX = -ballSpeedX;
    playBuzzer();  // Tocar som quando a bola rebater na barra
  }

  // Verificar colisão com paddle 2
  if (ballX >= 124 && ballY >= paddle2Y && ballY <= paddle2Y + paddleHeight) {
    ballSpeedX = -ballSpeedX;
    playBuzzer();  // Tocar som quando a bola rebater na barra
  }

  // Se a bola passar dos paddles, atualizar o placar e reiniciar a posição da bola
  if (ballX < 0) {
    score2++;
    playScoreBuzzer();  // Tocar som quando marcar ponto
    resetBall();
  } else if (ballX > 128) {
    score1++;
    playScoreBuzzer();  // Tocar som quando marcar ponto
    resetBall();
  }

  // Verificar se um jogador alcançou 4 pontos
  if (score1 >= 4 || score2 >= 4) {
    gameOver = true;
  }
}

// Função para ler os botões e controlar os paddles
void readButtons() {
  // Controla paddle 1 (botões 0 e 1)
  if (digitalRead(buttonPins[0]) == LOW) {
    paddle1Y -= 2;
    if (paddle1Y < 0) {
      paddle1Y = 0;
    }
  } else if (digitalRead(buttonPins[1]) == LOW) {
    paddle1Y += 2;
    if (paddle1Y > 48) {
      paddle1Y = 48;
    }
  }

  // Controla paddle 2 (botões 2 e 3)
  if (digitalRead(buttonPins[2]) == LOW) {
    paddle2Y -= 2;
    if (paddle2Y < 0) {
      paddle2Y = 0;
    }
  } else if (digitalRead(buttonPins[3]) == LOW) {
    paddle2Y += 2;
    if (paddle2Y > 48) {
      paddle2Y = 48;
    }
  }
}

// Função para tocar o buzzer quando a bola rebate
void playBuzzer() {
  if (buzzerPin > 0) {
    tone(buzzerPin, 1000); // Toca um som de 1 kHz
    delay(50); // Espera 50 ms
    noTone(buzzerPin); // Desliga o buzzer
  }
}

// Função para tocar o buzzer quando marca ponto
void playScoreBuzzer() {
  if (buzzerPin > 0) {
    tone(buzzerPin, 1500); // Toca um som de 1.5 kHz
    delay(200); // Espera 200 ms
    noTone(buzzerPin); // Desliga o buzzer
  }
}

// Função para reiniciar a posição da bola
void resetBall() {
  ballX = 64;
  ballY = 32;
  ballSpeedX = ballSpeedX > 0 ? 2 : -2;
  ballSpeedY = ballSpeedY > 0 ? 1 : -1;
}

void setup() {
  // Inicializa os pinos dos botões como entradas com pull-up interno
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(buzzerPin, OUTPUT);

  // Inicializa o display
  setupDisplay();
}

void loop() {
  // Verifica se o jogo está encerrado e aguarda um botão ser pressionado para iniciar um novo jogo
  if (gameOver) {
    if (digitalRead(buttonPins[0]) == LOW || digitalRead(buttonPins[1]) == LOW ||
        digitalRead(buttonPins[2]) == LOW || digitalRead(buttonPins[3]) == LOW) {
      // Reinicia as variáveis do jogo
      score1 = 0;
      score2 = 0;
      gameOver = false;
    }
  } else {
    // Executa o jogo apenas se não estiver
    readButtons();   // Ler botões para movimentar os paddles
    updateBall();    // Atualizar a posição da bola
    drawGame();      // Desenhar o jogo na tela
    delay(20);       // Pequeno delay para controle de velocidade
  }
}
