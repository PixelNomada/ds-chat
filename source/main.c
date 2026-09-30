#include <nds.h>
#include <dswifi9.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX_MSG 14
#define MAX_LEN 50

char chatlog[MAX_MSG][64];
int chat_count = 0;

char my_name[16] = "Mau";
char my_code[12] = "";          // Código de amigo
char server_ip[32] = "TU_IP_AQUI";  // <-- CAMBIA ESTO
int server_port = 7777;
int sock = -1;

// Añadir mensaje al chat
void add_msg(const char* text) {
    if (chat_count >= MAX_MSG) {
        for (int i = 0; i < MAX_MSG-1; i++)
            strcpy(chatlog[i], chatlog[i+1]);
        chat_count = MAX_MSG-1;
    }
    strncpy(chatlog[chat_count], text, 63);
    chatlog[chat_count][63] = 0;
    chat_count++;
}

// Dibujar pantalla de chat (estilo foto)
void draw_chat() {
    consoleClear();
    iprintf("\x1b[0;0H      DS CHAT\n");
    iprintf("========================\n");

    for (int i = 0; i < chat_count; i++) {
        iprintf("%s\n", chatlog[i]);
    }
}

// Conectar a WiFi + servidor
bool connect_to_server() {
    add_msg("Conectando WiFi...");
    draw_chat();

    if (!Wifi_InitDefault(WFC_CONNECT)) {
        add_msg("Error WiFi");
        draw_chat();
        return false;
    }

    add_msg("WiFi OK. Conectando servidor...");
    draw_chat();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        add_msg("Error socket");
        return false;
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(server_port);
    sa.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        add_msg("No se pudo conectar");
        return false;
    }

    // Login
    char login[64];
    sprintf(login, "LOGIN|%s\n", my_name);
    send(sock, login, strlen(login), 0);

    add_msg("Conectado!");
    return true;
}

// Recibir mensajes del servidor (no bloqueante)
void receive_messages() {
    if (sock < 0) return;

    char buf[128];
    int len = recv(sock, buf, sizeof(buf)-1, MSG_DONTWAIT);
    if (len > 0) {
        buf[len] = 0;
        // Quitar salto de línea
        char* p = strchr(buf, '\n');
        if (p) *p = 0;

        if (strncmp(buf, "CODE|", 5) == 0) {
            strcpy(my_code, buf + 5);
            char tmp[64];
            sprintf(tmp, "Tu codigo: %s", my_code);
            add_msg(tmp);
        }
        else if (strncmp(buf, "MSG|", 4) == 0) {
            add_msg(buf + 4);
        }
        else {
            add_msg(buf);
        }
        draw_chat();
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

    keyboardDemoInit();
    keyboardShow();

    add_msg("Bienvenido a DS Chat");
    add_msg("Escribe tu nombre y ENTER");
    draw_chat();

    char input[48] = {0};
    int pos = 0;
    bool connected = false;
    bool name_set = false;

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        int keys = keysDown();

        receive_messages();

        int key = keyboardUpdate();

        if (key > 0) {
            if (key == DVK_ENTER) {
                if (!name_set) {
                    // Primer ENTER = poner nombre
                    if (pos > 0) {
                        strncpy(my_name, input, 15);
                        my_name[15] = 0;
                        name_set = true;
                        memset(input, 0, sizeof(input));
                        pos = 0;

                        // Conectar
                        connected = connect_to_server();
                        draw_chat();
                    }
                }
                else if (connected && pos > 0) {
                    // Enviar mensaje
                    char packet[80];
                    sprintf(packet, "MSG|%s\n", input);
                    send(sock, packet, strlen(packet), 0);

                    char local[64];
                    sprintf(local, "TU: %s", input);
                    add_msg(local);
                    draw_chat();

                    memset(input, 0, sizeof(input));
                    pos = 0;
                }
            }
            else if (key == DVK_BACKSPACE) {
                if (pos > 0) {
                    pos--;
                    input[pos] = 0;
                }
            }
            else if (pos < 40 && key >= 32 && key < 127) {
                input[pos++] = (char)key;
                input[pos] = 0;
            }

            // Mostrar lo que se escribe
            iprintf("\x1b[23;0HEscribir: %s ", input);
        }

        if (keys & KEY_B) break; // Salir
        if (keys & KEY_X && connected) {
            // Ejemplo: agregar amigo (más adelante mejoramos)
            add_msg("Usa: /add CODIGO");
            draw_chat();
        }
    }

    if (sock >= 0) close(sock);
    return 0;
}
