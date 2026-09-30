#include <nds.h>
#include <stdio.h>
#include <string.h>

#define MAX_MESSAGES 12
#define MAX_MSG_LEN  60

char messages[MAX_MESSAGES][MAX_MSG_LEN];
int msg_count = 0;

// Añadir mensaje al historial
void add_message(const char* user, const char* text) {
    if (msg_count >= MAX_MESSAGES) {
        // Mover mensajes hacia arriba
        for (int i = 0; i < MAX_MESSAGES - 1; i++) {
            strcpy(messages[i], messages[i + 1]);
        }
        msg_count = MAX_MESSAGES - 1;
    }
    snprintf(messages[msg_count], MAX_MSG_LEN, "%s: %s", user, text);
    msg_count++;
}

// Dibujar historial en pantalla superior
void draw_chat() {
    consoleClear();
    iprintf("\x1b[0;0H=== DS CHAT ===\n\n");

    for (int i = 0; i < msg_count; i++) {
        iprintf("%s\n", messages[i]);
    }
}

int main(void) {
    // Pantalla superior = chat
    videoSetMode(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);

    // Pantalla inferior = teclado
    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    // Inicializar teclado
    keyboardDemoInit();
    keyboardShow();

    // Mensajes de ejemplo (como en tu foto)
    add_message("DSi IA", "Hola! Soy DSi IA.");
    add_message("DSi IA", "Pregunta lo que quieras.");
    add_message("TU", "quien te creo");
    add_message("DSi IA", "Daniel deidad ff");

    draw_chat();

    char input[64] = {0};
    int input_pos = 0;

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        int keys = keysDown();

        // Leer teclado
        int key = keyboardUpdate();

        if (key > 0) {
            if (key == DVK_ENTER) {
                // Enviar mensaje
                if (input_pos > 0) {
                    add_message("TU", input);
                    draw_chat();

                    // Aquí más adelante mandaremos el mensaje al servidor
                    // Por ahora solo lo mostramos

                    // Limpiar input
                    memset(input, 0, sizeof(input));
                    input_pos = 0;
                    consoleClear(); // limpiar pantalla inferior
                    iprintf("\x1b[0;0HEscribir:\n");
                }
            }
            else if (key == DVK_BACKSPACE) {
                if (input_pos > 0) {
                    input_pos--;
                    input[input_pos] = 0;
                }
            }
            else if (input_pos < 50 && key >= 32 && key <= 126) {
                input[input_pos++] = key;
                input[input_pos] = 0;
            }

            // Mostrar lo que se está escribiendo
            iprintf("\x1b[2;0H%s ", input);
        }

        if (keys & KEY_B) {
            // Salir (opcional)
            break;
        }
    }

    return 0;
}
