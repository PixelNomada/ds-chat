#include <nds.h>
#include <dswifi9.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_MSG 12
#define MAX_LEN 50

char chatlog[MAX_MSG][64];
int chat_count = 0;

char my_name[16] = "Mau";
char my_code[12] = "";
char server_ip[32] = "127.0.0.1";  // <-- CAMBIA ESTA IP por la de tu servidor
int server_port = 7777;
int sock = -1;

// Añadir mensaje al historial
void add_msg(const char* text) {
    if (chat_count >= MAX_MSG) {
        for (int i = 0; i < MAX_MSG - 1; i++) {
            strcpy(chatlog[i], chatlog[i + 1]);
        }
        chat_count = MAX_MSG - 1;
    }
    strncpy(chatlog[chat_count], text, 63);
    chatlog[chat_count][63] = '\0';
    chat_count++;
}

// Dibujar el chat (pantalla superior)
void draw_chat(void) {
    consoleClear();
    printf("\x1b[0;0H      DS CHAT\n");
    printf("========================\n");

    for (int i = 0; i < chat_count; i++) {
        printf("%s\n", chatlog[i]);
    }
}

// Conectar a WiFi y al servidor
bool connect_to_server(void) {
    add_msg("Conectando WiFi...");
    draw_chat();

    if (!Wifi_InitDefault(WFC_CONNECT)) {
        add_msg("Error: No se pudo conectar al WiFi");
        draw_chat();
        return false;
    }

    add_msg("WiFi OK. Conectando al servidor...");
    draw_chat();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        add_msg("Error creando socket");
        draw_chat();
        return false;
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(server_port);
    sa.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        add_msg("Error: No se pudo conectar al servidor");
        draw_chat();
        close(sock);
        sock = -1;
        return false;
    }

    // Enviar login
    char login[64];
    snprintf(login, sizeof(login), "LOGIN|%s\n", my_name);
    send(sock, login, strlen(login), 0);

    add_msg("Conectado al servidor!");
    draw_chat();
    return true;
}

// Recibir mensajes del servidor
void receive_messages(void) {
    if (sock < 0) return;

    char buf[128];
    int len = recv(sock, buf, sizeof(buf) - 1, MSG_DONTWAIT);

    if (len > 0) {
        buf[len] = '\0';

        // Quitar salto de línea
        char* p = strchr(buf, '\n');
        if (p) *p = '\0';
        p = strchr(buf, '\r');
        if (p) *p = '\0';

        if (strncmp(buf, "CODE|", 5) == 0) {
            strncpy(my_code, buf + 5, sizeof(my_code) - 1);
            char tmp[64];
            snprintf(tmp, sizeof(tmp), "Tu codigo: %s", my_code);
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

    // Inicializar teclado
    keyboardDemoInit();
    keyboardShow();

    add_msg("Bienvenido a DS Chat");
    add_msg("Escribe tu nombre y pulsa ENTER");
    draw_chat();

    char input[48] = {0};
    int pos = 0;
    bool connected = false;
    bool name_set = false;

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        u16 keys = keysDown();

        receive_messages();

        int key = keyboardUpdate();

        if (key > 0) {
            if (key == DVK_ENTER) {
                if (!name_set) {
                    // Primer ENTER = guardar nombre y conectar
                    if (pos > 0) {
                        strncpy(my_name, input, sizeof(my_name) - 1);
                        my_name[sizeof(my_name) - 1] = '\0';
                        name_set = true;

                        memset(input, 0, sizeof(input));
                        pos = 0;

                        connected = connect_to_server();
                        draw_chat();
                    }
                }
                else if (connected && pos > 0) {
                    // Enviar mensaje
                    char packet[80];
                    snprintf(packet, sizeof(packet), "MSG|%s\n", input);
                    send(sock, packet, strlen(packet), 0);

                    char local[64];
                    snprintf(local, sizeof(local), "TU: %s", input);
                    add_msg(local);
                    draw_chat();

                    memset(input, 0, sizeof(input));
                    pos = 0;
                }
            }
            else if (key == DVK_BACKSPACE) {
                if (pos > 0) {
                    pos--;
                    input[pos] = '\0';
                }
            }
            else if (pos < 40 && key >= 32 && key < 127) {
                input[pos++] = (char)key;
                input[pos] = '\0';
            }

            // Mostrar lo que se está escribiendo
            printf("\x1b[23;0HEscribir: %s ", input);
        }

        if (keys & KEY_B) {
            break; // Salir
        }
    }

    if (sock >= 0) {
        close(sock);
    }

    return 0;
}
