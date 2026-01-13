#include <Keypad.h>
#include <LiquidCrystal.h>

/* ================== TECLADO ================== */

char teclas[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte linhas[4] = {6, 7, 8, 9};
byte colunas[4] = {10, 13, A0, A1};

Keypad teclado = Keypad(makeKeymap(teclas), linhas, colunas, 4, 4);

/* ================== LCD ================== */

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* ================== CRONÔMETROS ================== */

// Agora temos apenas dois cronômetros
unsigned long tempos[2] = {0, 0};
unsigned long ultimoMillis = 0;
int timeAtivo = -1;

/* ================== SENHAS ================== */

String bufferSenha = "";

const String senhas[2] = {
  "0101", // AlphaYankee
  "0202"  // BravoJuliet
};

const char* nomesTimes[2] = {
  "AlphaYankee",
  "BravoJuliet"
};

/* ================== CONTROLE DE ESTADO ================== */

bool mostrandoMensagem = false;
bool digitandoSenha = false;
unsigned long inicioMensagem = 0;
const unsigned long duracaoMensagem = 1000;

/* ================== FUNÇÕES ================== */

int validaSenha(String senha) {
  for (int i = 0; i < 2; i++) {
    if (senha == senhas[i]) {
      return i;
    }
  }
  return -1;
}

void exibeTempo(unsigned long tempo) {

  if (digitandoSenha || mostrandoMensagem) return;

  unsigned long totalSegundos = tempo / 1000;
  int minutos = totalSegundos / 60;
  int segundos = totalSegundos % 60;

  lcd.setCursor(0, 1);
  lcd.print("Tempo: ");

  if (minutos < 10) lcd.print("0");
  lcd.print(minutos);
  lcd.print(":");
  if (segundos < 10) lcd.print("0");
  lcd.print(segundos);
}

/* ================== SETUP ================== */

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cronometro");
  ultimoMillis = millis();
}

/* ================== LOOP ================== */

void loop() {

  /* ---- Mensagem temporária ---- */
  if (mostrandoMensagem) {
    if (millis() - inicioMensagem >= duracaoMensagem) {
      mostrandoMensagem = false;
      lcd.clear();

      if (timeAtivo != -1) {
        lcd.setCursor(0, 0);
        lcd.print(nomesTimes[timeAtivo]);
      }
    }
  }

  /* ---- Atualização do cronômetro ---- */
  if (timeAtivo != -1) {
    unsigned long agora = millis();
    tempos[timeAtivo] += agora - ultimoMillis;
    ultimoMillis = agora;

    exibeTempo(tempos[timeAtivo]);
  }

  /* ---- Leitura do teclado ---- */
  if (!mostrandoMensagem) {

    char tecla = teclado.getKey();

    if (tecla) {

      if (tecla >= '0' && tecla <= '9') {

        digitandoSenha = true;
        bufferSenha += tecla;

        lcd.setCursor(0, 1);
        lcd.print("Senha: ");
        lcd.print(bufferSenha);
        lcd.print("    ");
      }

      if (bufferSenha.length() == 4) {

        int novoTime = validaSenha(bufferSenha);

        if (novoTime != -1) {

          timeAtivo = novoTime;
          ultimoMillis = millis();

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(nomesTimes[timeAtivo]);

        } else {

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Senha invalida");

          mostrandoMensagem = true;
          inicioMensagem = millis();
        }

        bufferSenha = "";
        digitandoSenha = false;
      }
    }
  }
}
