#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define MAX_ALIMENTOS 100
#define MAX_DESC 150
#define NOME_ARQUIVO "alimentos.csv"

// Requisito 2: Implementação das categorias por meio de um enumerado.
typedef enum {
    CEREAIS_E_DERIVADOS,
    VERDURAS_HORTALICAS_E_DERIVADOS,
    FRUTAS_E_DERIVADOS,
    GORDURAS_E_OLEOS,
    PESCADOS_E_FRUTOS_DO_MAR,
    CARNES_E_DERIVADOS,
    LEITE_E_DERIVADOS,
    BEBIDAS,
    OVOS_E_DERIVADOS,
    PRODUTOS_ACUCARADOS,
    MISCELANEAS,
    OUTROS_ALIMENTOS_INDUSTRIALIZADOS,
    ALIMENTOS_PREPARADOS,
    LEGUMINOSAS_E_DERIVADOS,
    NOZES_E_SEMENTES,
    CATEGORIA_INVALIDA // Para tratamento de erros
} Categoria;

// Vetor de strings para exibir os nomes das categorias
const char* nomesCategorias[] = {
    "Cereais e derivados",
    "Verduras, hortaliças e derivados",
    "Frutas e derivados",
    "Gorduras e óleos",
    "Pescados e frutos do mar",
    "Carnes e derivados",
    "Leite e derivados",
    "Bebidas (alcoólicas e não alcoólicas)",
    "Ovos e derivados",
    "Produtos açucarados",
    "Miscelâneas",
    "Outros alimentos industrializados",
    "Alimentos preparados",
    "Leguminosas e derivados",
    "Nozes e sementes"
};
const int NUM_CATEGORIAS = 15;


// Estrutura para armazenar os dados de um alimento
typedef struct {
    int numero;
    char descricao[MAX_DESC];
    float umidade;
    int energia;
    float proteina;
    float carboidrato;
    Categoria categoria;
} Alimento;

// Requisito 3: Estrutura para ordenação eficiente em memória
typedef struct {
    double valor; // O valor que será usado para ordenar (umidade, energia, etc.)
    const Alimento* alimentoPtr; // Ponteiro para o alimento original (não pode ser modificado)
} ItemOrdenavel;


// --- Protótipos das Funções ---
void exibirMenu();
Categoria stringParaCategoria(const char* str);
int carregarDados(Alimento alimentos[], const char* nomeArquivo);
void bubbleSort(ItemOrdenavel itens[], int n, char ordem);
void listarCategorias();
void listarAlimentosPorNome(const Alimento alimentos[], int numAlimentos);
void listarAlimentosPorEnergia(const Alimento alimentos[], int numAlimentos);
void listarTopN(const Alimento alimentos[], int numAlimentos, char criterio);
void limparBufferEntrada();
int obterCategoriaDoUsuario(Categoria* categoria);
int obterN(int* n);


// --- Função Principal ---
int main() {
    SetConsoleOutputCP(65001);

    Alimento alimentos[MAX_ALIMENTOS];
    int numAlimentos = carregarDados(alimentos, NOME_ARQUIVO);

    if (numAlimentos == 0) {
        return 1; // Encerra se não conseguiu carregar dados
    }

    int opcao;
    do {
        exibirMenu();
        if (scanf("%d", &opcao) != 1) {
            printf("\nOpção inválida! Por favor, digite um número.\n");
            limparBufferEntrada();
            opcao = -1; // Força a repetição do loop
            continue;
        }
        limparBufferEntrada(); // Limpa o buffer após ler o número

        switch (opcao) {
            case 1: listarCategorias(); break;
            case 2: listarAlimentosPorNome(alimentos, numAlimentos); break;
            case 3: listarAlimentosPorEnergia(alimentos, numAlimentos); break;
            case 4: listarTopN(alimentos, numAlimentos, 'U'); break; // Umidade
            case 5: listarTopN(alimentos, numAlimentos, 'E'); break; // Energia
            case 6: listarTopN(alimentos, numAlimentos, 'P'); break; // Proteína
            case 7: listarTopN(alimentos, numAlimentos, 'C'); break; // Carboidrato
            case 8: listarTopN(alimentos, numAlimentos, 'R'); break; // Relação Energia/Proteína
            case 9: listarTopN(alimentos, numAlimentos, 'K'); break; // Relação Energia/Carboidrato
            case 10: printf("\nEncerrando o programa. Até logo!\n"); break;
            default: printf("\nOpção inválida! Tente novamente.\n"); break;
        }
    } while (opcao != 10);

    return 0;
}

// --- Implementação das Funções ---

// Converte a string da categoria do arquivo para o tipo enum Categoria
Categoria stringParaCategoria(const char* str) {
    for (int i = 0; i < NUM_CATEGORIAS; i++) {
        if (strcmp(str, nomesCategorias[i]) == 0) {
            return (Categoria)i;
        }
    }
    return CATEGORIA_INVALIDA;
}

// Carrega os dados do arquivo CSV para o vetor de alimentos
int carregarDados(Alimento alimentos[], const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    char linha[512];
    int contador = 0;

    // Pula a linha do cabeçalho
    fgets(linha, sizeof(linha), arquivo);

    while (fgets(linha, sizeof(linha), arquivo) && contador < MAX_ALIMENTOS) {
        char* token;
        
        token = strtok(linha, ";");
        alimentos[contador].numero = atoi(token);

        token = strtok(NULL, ";");
        strcpy(alimentos[contador].descricao, token);

        token = strtok(NULL, ";");
        alimentos[contador].umidade = atof(token);

        token = strtok(NULL, ";");
        alimentos[contador].energia = atoi(token);

        token = strtok(NULL, ";");
        alimentos[contador].proteina = atof(token);
        
        token = strtok(NULL, ";");
        alimentos[contador].carboidrato = atof(token);

        token = strtok(NULL, "\n"); // Lê até o final da linha
        alimentos[contador].categoria = stringParaCategoria(token);

        contador++;
    }

    fclose(arquivo);
    printf(">> %d alimentos carregados com sucesso!\n", contador);
    return contador;
}

// Exibe o menu de opções para o usuário
void exibirMenu() {
    printf("\n---------- MENU DE OPÇÕES ----------\n");
    printf("1. Listar todas as categorias\n");
    printf("2. Listar alimentos de uma categoria (ordem alfabética)\n");
    printf("3. Listar alimentos de uma categoria (ordem de energia)\n");
    printf("4. Listar TOP N alimentos por Umidade\n");
    printf("5. Listar TOP N alimentos por Energia\n");
    printf("6. Listar TOP N alimentos por Proteína\n");
    printf("7. Listar TOP N alimentos por Carboidrato\n");
    printf("8. Listar TOP N alimentos por Relação Energia/Proteína\n");
    printf("9. Listar TOP N alimentos por Relação Energia/Carboidrato\n");
    printf("10. Encerrar o programa\n");
    printf("------------------------------------\n");
    printf("Escolha uma opção: ");
}

// Requisito 4: Implementação do algoritmo de ordenação (Bubble Sort)
void bubbleSort(ItemOrdenavel itens[], int n, char ordem) {
    int i, j;
    ItemOrdenavel temp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            int deveTrocar = 0;
            if (ordem == 'D') { // Descendente
                if (itens[j].valor < itens[j + 1].valor) {
                    deveTrocar = 1;
                }
            } else { // Ascendente
                if (itens[j].valor > itens[j + 1].valor) {
                    deveTrocar = 1;
                }
            }
            if (deveTrocar) {
                temp = itens[j];
                itens[j] = itens[j + 1];
                itens[j + 1] = temp;
            }
        }
    }
}

// Opção a: Lista todas as categorias
void listarCategorias() {
    printf("\n--- Categorias de Alimentos ---\n");
    for (int i = 0; i < NUM_CATEGORIAS; i++) {
        printf("%d. %s\n", i + 1, nomesCategorias[i]);
    }
}

// Opção b: Lista alimentos de uma categoria em ordem alfabética
void listarAlimentosPorNome(const Alimento alimentos[], int numAlimentos) {
    Categoria categoria;
    if (!obterCategoriaDoUsuario(&categoria)) return;

    // Estrutura auxiliar apenas com ponteiros para ordenação por string
    const Alimento* ptrsAlimentos[MAX_ALIMENTOS];
    int contadorItens = 0;

    for (int i = 0; i < numAlimentos; i++) {
        if (alimentos[i].categoria == categoria) {
            ptrsAlimentos[contadorItens] = &alimentos[i];
            contadorItens++;
        }
    }
    
    if (contadorItens == 0) {
        printf("\nNenhum alimento encontrado para esta categoria.\n");
        return;
    }

    // Ordenação Bubble Sort específica para strings
    for (int i = 0; i < contadorItens - 1; i++) {
        for (int j = 0; j < contadorItens - i - 1; j++) {
            if (strcmp(ptrsAlimentos[j]->descricao, ptrsAlimentos[j + 1]->descricao) > 0) {
                const Alimento* temp = ptrsAlimentos[j];
                ptrsAlimentos[j] = ptrsAlimentos[j + 1];
                ptrsAlimentos[j + 1] = temp;
            }
        }
    }

    printf("\n--- Alimentos em '%s' (Ordem Alfabética) ---\n", nomesCategorias[categoria]);
    for (int i = 0; i < contadorItens; i++) {
        printf("- %s\n", ptrsAlimentos[i]->descricao);
    }
}


// Opção c: Lista todos os alimentos de uma categoria por energia
void listarAlimentosPorEnergia(const Alimento alimentos[], int numAlimentos) {
    Categoria categoria;
    if (!obterCategoriaDoUsuario(&categoria)) return;

    ItemOrdenavel itensParaOrdenar[MAX_ALIMENTOS];
    int contadorItens = 0;

    for (int i = 0; i < numAlimentos; i++) {
        if (alimentos[i].categoria == categoria) {
            itensParaOrdenar[contadorItens].alimentoPtr = &alimentos[i];
            itensParaOrdenar[contadorItens].valor = (double)alimentos[i].energia;
            contadorItens++;
        }
    }

    if (contadorItens == 0) {
        printf("\nNenhum alimento encontrado para esta categoria.\n");
        return;
    }

    bubbleSort(itensParaOrdenar, contadorItens, 'D');

    printf("\n--- Alimentos em '%s' (Ordem de Energia) ---\n", nomesCategorias[categoria]);
    printf("%-50s | %s\n", "Descrição", "Energia (Kcal)");
    printf("------------------------------------------------------------------\n");
    for (int i = 0; i < contadorItens; i++) {
        printf("%-50s | %d\n", itensParaOrdenar[i].alimentoPtr->descricao, itensParaOrdenar[i].alimentoPtr->energia);
    }
}


// Opções d a i: Lista os N melhores alimentos por um critério específico
void listarTopN(const Alimento alimentos[], int numAlimentos, char criterio) {
    Categoria categoria;
    int n;
    
    if (!obterCategoriaDoUsuario(&categoria)) return;
    if (!obterN(&n)) return;

    ItemOrdenavel itensParaOrdenar[MAX_ALIMENTOS];
    int contadorItens = 0;
    char tituloCriterio[50];
    
    for (int i = 0; i < numAlimentos; i++) {
        if (alimentos[i].categoria == categoria) {
            itensParaOrdenar[contadorItens].alimentoPtr = &alimentos[i];
            
            switch(criterio) {
                case 'U': 
                    strcpy(tituloCriterio, "Umidade (%)");
                    itensParaOrdenar[contadorItens].valor = alimentos[i].umidade;
                    break;
                case 'E':
                    strcpy(tituloCriterio, "Energia (Kcal)");
                    itensParaOrdenar[contadorItens].valor = alimentos[i].energia;
                    break;
                case 'P':
                    strcpy(tituloCriterio, "Proteína (g)");
                    itensParaOrdenar[contadorItens].valor = alimentos[i].proteina;
                    break;
                case 'C':
                    strcpy(tituloCriterio, "Carboidrato (g)");
                    itensParaOrdenar[contadorItens].valor = alimentos[i].carboidrato;
                    break;
                case 'R':
                    strcpy(tituloCriterio, "Energia/Proteína");
                    if (alimentos[i].proteina > 0.0) {
                        itensParaOrdenar[contadorItens].valor = (double)alimentos[i].energia / alimentos[i].proteina;
                    } else {
                        itensParaOrdenar[contadorItens].valor = 0.0;
                    }
                    break;
                case 'K':
                     strcpy(tituloCriterio, "Energia/Carboidrato");
                    if (alimentos[i].carboidrato > 0.0) {
                        itensParaOrdenar[contadorItens].valor = (double)alimentos[i].energia / alimentos[i].carboidrato;
                    } else {
                        itensParaOrdenar[contadorItens].valor = 0.0;
                    }
                    break;
            }
            contadorItens++;
        }
    }

    if (contadorItens == 0) {
        printf("\nNenhum alimento encontrado para esta categoria.\n");
        return;
    }
    
    bubbleSort(itensParaOrdenar, contadorItens, 'D');

    printf("\n--- Top %d Alimentos em '%s' por %s ---\n", n, nomesCategorias[categoria], tituloCriterio);
    printf("%-50s | %s\n", "Descrição", tituloCriterio);
    printf("----------------------------------------------------------------------\n");

    int limite = (n < contadorItens) ? n : contadorItens;
    for (int i = 0; i < limite; i++) {
        printf("%-50s | %.2f\n", itensParaOrdenar[i].alimentoPtr->descricao, itensParaOrdenar[i].valor);
    }
}

// Função utilitária para limpar o buffer de entrada (stdin)
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Função auxiliar para obter a categoria do usuário com validação
int obterCategoriaDoUsuario(Categoria* categoria) {
    int escolhaCat;
    listarCategorias();
    printf("Escolha a categoria pelo número: ");
    if (scanf("%d", &escolhaCat) != 1 || escolhaCat < 1 || escolhaCat > NUM_CATEGORIAS) {
        printf("Número de categoria inválido.\n");
        limparBufferEntrada();
        return 0;
    }
    limparBufferEntrada();
    *categoria = (Categoria)(escolhaCat - 1);
    return 1;
}

// Função auxiliar para obter o valor de N do usuário com validação
int obterN(int* n) {
    printf("Digite o valor de N (quantos alimentos listar): ");
    if (scanf("%d", n) != 1 || *n <= 0) {
        printf("Valor de N inválido. Deve ser um número positivo.\n");
        limparBufferEntrada();
        return 0;
    }
    limparBufferEntrada();
    return 1;
}