#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

// ============================================================================
// --- Constantes Globais ---
// ============================================================================
#define NUM_TERRITORIOS 10
#define NUM_MISSOES 3
#define MAX_NOME_LEN 50
#define COR_JOGADOR "AZUL" // Cor do exército do jogador principal

// ============================================================================
// --- Estrutura de Dados ---
// ============================================================================

/**
 * @brief Representa um território no mapa do jogo.
 */
typedef struct {
    char nome[MAX_NOME_LEN];
    char dono[MAX_NOME_LEN]; // Cor do exército que domina (ex: "AZUL", "VERDE")
    int tropas;
} Territorio;

// ============================================================================
// --- Protótipos das Funções ---
// ============================================================================

// Funções de setup e gerenciamento de memória:
Territorio* alocarMapa();
void inicializarTerritorios(Territorio* mapa);
void liberarMemoria(Territorio** mapa);

// Funções de interface com o usuário:
void exibirMenuPrincipal();
void exibirMapa(const Territorio* mapa); // Leitura: const
void exibirMissao(int idMissao);

// Funções de lógica principal do jogo:
void faseDeAtaque(Territorio* mapa, const char* corJogador); // Modifica mapa: sem const
int simularAtaque(Territorio* atacante, Territorio* defensor, const char* corJogador);
int sortearMissao();
int verificarVitoria(const Territorio* mapa, int idMissao, const char* corJogador); // Leitura: const

// Função utilitária:
void limparBufferEntrada();
int rolarDado();

// ============================================================================
// --- Função Principal (main) ---
// ============================================================================
int main() {
    // 1. Configuração Inicial (Setup):
    setlocale(LC_ALL, "Portuguese"); // Define o locale para português.
    srand(time(NULL));                 // Inicializa a semente para números aleatórios.

    Territorio* mapa = alocarMapa(); // Aloca a memória.
    if (mapa == NULL) {
        printf("ERRO: Falha ao alocar memória para o mapa. O jogo não pode começar.\n");
        return 1;
    }

    inicializarTerritorios(mapa);        // Preenche os dados iniciais.
    const char* corJogador = COR_JOGADOR; // Define a cor do jogador.
    int idMissao = sortearMissao();    // Sorteia a missão secreta.
    int escolha = -1;
    int vitoria = 0;

    printf("⚔️ Bem-vindo ao WAR Estruturado! Sua cor é **%s**.\n", corJogador);
    printf("🎯 Sua Missão Secreta (ID %d):\n", idMissao);
    exibirMissao(idMissao);
    
    printf("\n--- Pressione ENTER para iniciar ---\n");
    limparBufferEntrada();

    // 2. Laço Principal do Jogo (Game Loop):
    do {
        printf("\n"
               "==============================================\n"
               "             **NOVA RODADA**\n"
               "==============================================\n");

        exibirMapa(mapa);
        exibirMenuPrincipal();

        printf("➡️ Sua escolha: ");
        if (scanf("%d", &escolha) != 1) {
            escolha = -1; // Sinaliza uma entrada inválida
            limparBufferEntrada();
        } else {
            limparBufferEntrada();
        }

        switch (escolha) {
            case 1:
                printf("\n--- FASE DE ATAQUE ---\n");
                faseDeAtaque(mapa, corJogador);
                break;
            case 2:
                printf("\n--- VERIFICAÇÃO DE VITÓRIA ---\n");
                vitoria = verificarVitoria(mapa, idMissao, corJogador);
                if (vitoria) {
                    printf("🎉 **PARABÉNS! VOCÊ CUMPRIU SUA MISSÃO E VENCEU O JOGO!** 🎉\n");
                } else {
                    printf("😔 Sua missão ainda não foi cumprida. Continue jogando!\n");
                }
                break;
            case 0:
                printf("\n👋 Encerrando o jogo...\n");
                break;
            default:
                printf("\n❌ Opção inválida. Tente novamente.\n");
                break;
        }
        
        // Pausa a execução se o jogo ainda estiver rodando
        if (escolha != 0 && vitoria == 0) {
            printf("\n--- Pressione ENTER para continuar ---\n");
            getchar(); 
        }

    } while (escolha != 0 && vitoria == 0);

    // 3. Limpeza:
    liberarMemoria(&mapa); // Passagem por referência para modificar o ponteiro 'mapa' para NULL.

    return 0;
}

// ============================================================================
// --- Implementação das Funções ---
// ============================================================================

/**
 * @brief Aloca dinamicamente a memória para o vetor de territórios.
 * @return Um ponteiro para a memória alocada ou NULL em caso de falha.
 */
Territorio* alocarMapa() {
    // calloc inicializa a memória com zero, útil para estruturas.
    Territorio* mapa = (Territorio*)calloc(NUM_TERRITORIOS, sizeof(Territorio));
    return mapa;
}

/**
 * @brief Preenche os dados iniciais de cada território no mapa.
 * @param mapa Ponteiro para o vetor de territórios (passagem por referência).
 */
void inicializarTerritorios(Territorio* mapa) {
    // Simulação de alocação inicial para o mapa
    char* nomes[] = {"Brasil", "Argentina", "Peru", "Alaska", "Quebec", "California", "Aral", "Siberia", "Japao", "Australia"};
    char* donos[] = {"AZUL", "VERMELHO", "AZUL", "VERDE", "VERMELHO", "VERDE", "AZUL", "VERMELHO", "VERDE", "VERMELHO"};
    int tropas[] = {2, 3, 2, 3, 2, 3, 2, 3, 2, 3};

    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        strcpy(mapa[i].nome, nomes[i]);
        strcpy(mapa[i].dono, donos[i]);
        mapa[i].tropas = tropas[i];
    }
}

/**
 * @brief Libera a memória alocada para o mapa e seta o ponteiro para NULL.
 * @param mapa Ponteiro para o ponteiro do mapa (referência ao ponteiro).
 */
void liberarMemoria(Territorio** mapa) {
    if (*mapa != NULL) {
        free(*mapa);
        *mapa = NULL; // É crucial setar o ponteiro para NULL após liberar.
        printf("✅ Memória do mapa liberada com sucesso.\n");
    }
}

/**
 * @brief Imprime na tela o menu de ações.
 */
void exibirMenuPrincipal() {
    printf("\n"
           "--- Menu de Ações ---\n"
           "1. ⚔️  Atacar Território\n"
           "2. 🏆  Verificar Condição de Vitória\n"
           "0. 🚪  Sair do Jogo\n");
}

/**
 * @brief Mostra o estado atual de todos os territórios.
 * @param mapa Ponteiro constante para o vetor de territórios (leitura).
 */
void exibirMapa(const Territorio* mapa) {
    printf("\n"
           "| ID | %-20s | %-10s | %-6s |\n"
           "|----|----------------------|------------|--------|\n", 
           "Território", "Dono", "Tropas");

    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        printf("| %-2d | %-20s | %-10s | %-6d |\n", 
               i, mapa[i].nome, mapa[i].dono, mapa[i].tropas);
    }
    printf("|----|----------------------|------------|--------|\n");
}

/**
 * @brief Exibe a descrição da missão atual do jogador.
 * @param idMissao ID da missão sorteada.
 */
void exibirMissao(int idMissao) {
    switch (idMissao) {
        case 1:
            printf("    -> **Missão 1:** Conquistar 6 territórios do continente Sul-Americano.\n");
            break;
        case 2:
            printf("    -> **Missão 2:** Destruir completamente o exército **VERDE**.\n");
            break;
        case 3:
            printf("    -> **Missão 3:** Conquistar e manter 15 territórios de qualquer continente.\n");
            break;
        default:
            printf("    -> Missão desconhecida.\n");
            break;
    }
}

/**
 * @brief Sorteia e retorna um ID de missão aleatório para o jogador.
 * @return O ID da missão sorteada (entre 1 e NUM_MISSOES).
 */
int sortearMissao() {
    return (rand() % NUM_MISSOES) + 1;
}

/**
 * @brief Gerencia a fase de ataque, solicitando entrada e chamando a simulação.
 * @param mapa Ponteiro para o vetor de territórios.
 * @param corJogador Cor do exército do jogador.
 */
void faseDeAtaque(Territorio* mapa, const char* corJogador) {
    int idAtacante, idDefensor;

    printf("Selecione o ID do seu território **atacante** (0 a %d): ", NUM_TERRITORIOS - 1);
    scanf("%d", &idAtacante);
    limparBufferEntrada();

    if (idAtacante < 0 || idAtacante >= NUM_TERRITORIOS || 
        strcmp(mapa[idAtacante].dono, corJogador) != 0 || mapa[idAtacante].tropas < 2) {
        printf("🚫 Ataque inválido: Território atacante precisa ser seu e ter pelo menos 2 tropas.\n");
        return;
    }

    printf("Selecione o ID do território **defensor** (0 a %d): ", NUM_TERRITORIOS - 1);
    scanf("%d", &idDefensor);
    limparBufferEntrada();

    if (idDefensor < 0 || idDefensor >= NUM_TERRITORIOS || idAtacante == idDefensor || 
        strcmp(mapa[idDefensor].dono, corJogador) == 0) {
        printf("🚫 Ataque inválido: Território defensor precisa ser inimigo e diferente do atacante.\n");
        return;
    }

    // Passagem por referência dos territórios específicos
    Territorio* atacante = &mapa[idAtacante]; 
    Territorio* defensor = &mapa[idDefensor];
    
    printf("\n🔥 **INÍCIO DA BATALHA:** %s (%d) VS %s (%d)\n", 
           atacante->nome, atacante->tropas, defensor->nome, defensor->tropas);
           
    simularAtaque(atacante, defensor, corJogador);
}

/**
 * @brief Executa a lógica de uma batalha entre dois territórios.
 * @param atacante Ponteiro para o território atacante.
 * @param defensor Ponteiro para o território defensor.
 * @param corJogador Cor do exército do jogador.
 * @return 1 se o território foi conquistado, 0 caso contrário.
 */
int simularAtaque(Territorio* atacante, Territorio* defensor, const char* corJogador) {
    int vitoriasAtacante = 0;
    int vitoriasDefensor = 0;
    int resultado;
    
    // Simulação simples de dado (3 dados para atacante, 2 para defensor)
    // Para simplificar, vamos simular 1 dado por lado por vez
    int dadoAtacante = rolarDado();
    int dadoDefensor = rolarDado();

    printf("  🎲 Dados: Atacante %d vs Defensor %d\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        printf("  🛡️ O defensor perdeu 1 tropa.\n");
        defensor->tropas--;
        vitoriasAtacante = 1;
    } else {
        printf("  💥 O atacante perdeu 1 tropa.\n");
        atacante->tropas--;
        vitoriasDefensor = 1;
    }

    printf("  🔄 Tropas restantes: %s (%d) | %s (%d)\n", 
           atacante->nome, atacante->tropas, defensor->nome, defensor->tropas);
    
    // Conquista
    if (defensor->tropas <= 0) {
        printf("👑 **CONQUISTA!** O território %s foi tomado!\n", defensor->nome);
        
        // Atualiza o dono do território
        strcpy(defensor->dono, atacante->dono); 
        
        // Move uma tropa do atacante para o defensor (pela regra do WAR)
        atacante->tropas--; 
        defensor->tropas = 1; 

        return 1;
    } else if (atacante->tropas < 2) {
        // Ataque falha se o atacante não tiver tropas suficientes para continuar
        printf("⚠️ Ataque encerrado: O atacante não tem mais tropas suficientes (mínimo 2).\n");
        return 0;
    }

    // Se a simulação fosse mais complexa, poderia perguntar se o jogador quer continuar atacando.
    return 0;
}

/**
 * @brief Verifica se o jogador cumpriu os requisitos de sua missão atual.
 * @param mapa Ponteiro constante para o vetor de territórios (leitura).
 * @param idMissao ID da missão a ser verificada.
 * @param corJogador Cor do exército do jogador.
 * @return 1 (verdadeiro) se a missão foi cumprida, e 0 (falso) caso contrário.
 */
int verificarVitoria(const Territorio* mapa, int idMissao, const char* corJogador) {
    int territoriosJogador = 0;
    int sulAmericanos = 0;
    int temVerde = 0;
    
    // Contagem de territórios e verificação da cor "VERDE"
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        if (strcmp(mapa[i].dono, corJogador) == 0) {
            territoriosJogador++;
        }
        
        // Simulação: IDs 0, 1, 2 são da América do Sul (Brasil, Argentina, Peru)
        if ((i >= 0 && i <= 2) && strcmp(mapa[i].dono, corJogador) == 0) {
             sulAmericanos++;
        }

        if (strcmp(mapa[i].dono, "VERDE") == 0) {
            temVerde = 1; // Encontrou pelo menos 1 território com exército VERDE
        }
    }

    switch (idMissao) {
        case 1: // Conquistar 6 territórios do continente Sul-Americano (Simulando com 3)
            // Note: Para a simulação, usaremos 3, pois só temos 10 no total.
            printf("-> Verificando Missão 1: Conquistar 3 territórios da América do Sul.\n");
            return (sulAmericanos >= 3);
            
        case 2: // Destruir completamente o exército VERDE
            printf("-> Verificando Missão 2: Destruir o exército VERDE.\n");
            return (!temVerde); // Se 'temVerde' for 0 (falso), a missão é cumprida (true)

        case 3: // Conquistar e manter 15 territórios (Simulando com 6)
            printf("-> Verificando Missão 3: Conquistar 6 territórios no total.\n");
            return (territoriosJogador >= 6);

        default:
            return 0;
    }
}

/**
 * @brief Função utilitária para rolar um dado simples (1 a 6).
 * @return O resultado da rolagem do dado.
 */
int rolarDado() {
    return (rand() % 6) + 1;
}


/**
 * @brief Função utilitária para limpar o buffer de entrada do teclado.
 */
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}