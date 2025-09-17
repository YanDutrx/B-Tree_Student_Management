/*
NOME: Yan Felipe Dutra Martins
Curso: Ciência da Computação
Faculdade: FCT UNESP
*/

/*
Informacoes: O trabalho a seguir é referente à disciplina de Estrutura de Dados II

ENUNCIADO:
- Neste trabalho, você deverá implementar uma árvore B em linguagem C. A árvore deve suportar as operações de busca, inserção e remoção, conforme descrito em sala e apresentado nos slides (CLRS).
- Além disso, a árvore B deverá ser dinâmica, ou seja, o grau mínimo t deve ser configurável
e determinado pelo usuário durante a execução do programa (alocação dinâmica). 
- A arvore B deve ser utilizada para a resolução de um problema prático. Sejam criativos!!!!
*/

#include<stdio.h> //Entrada e saída de dados, além da leitura de arquivos
#include<stdlib.h> //Alocacao dinamica
#include<stdbool.h> //Biblioteca para usaremos booleanos
#include<string.h> //Manipulacao de string
#include<time.h> //Usaremos para a geracao de pseudoaleatorios
#include <locale.h> //Para permitir acentuacoes de nomes (Tipo Bárbara, Cazé ou Caçula)

#define NOME_ARQUIVO 20 //Macro de pre-processamento
#define TAM_NOMES 25

int T; //Variavel GLOBAL (Visivel a todos os blocos) que será uma constante a ser definida
//O int T definira o grau mínimo da árvore (T-1) e sera definido de forma dinâmica

typedef struct Aluno
{
    int matricula; //Sera chave primaria utilizada em buscas
    char nome[TAM_NOMES];
    char curso[TAM_NOMES];  
} Aluno;

typedef struct No
{
    int num_chaves;
    bool eh_folha;
    Aluno *chaves; //Sera definido de forma dinamica, por isso o ponteiro em vez de chaves[MAX]
    struct No **filhos; //Ponteiro de ponteiro para filhos (Semelhante à Struct No*filhos[MAX+1])
    char nome[NOME_ARQUIVO];
} ArvoreB;

//Iremos utilizar as funcoes de arquivos (biblioteca stdio)

//Geracao de nome de arquivos aleatorios
char *Gerar_Nome_Aleatorio() //Como cada no estará em um arquivo, utilizaremos isso para nomear os diferentes arquivos
{
    static char nome[NOME_ARQUIVO]; //Tava dando erro, aparentemente tem que ser static para manter a informacao na pilha e nao ter dangling pointer

    for(int i = 0; i < 15; i++)
    {
        //No alfabeto, tem-se 26 letras (0 a 25)
        int posicao_a_ser_andada = rand() % 26;
        nome[i] = 'a' + posicao_a_ser_andada; //Como a eh a primeira letra do alfabeto, geraremos letras aleatorias a partir dele
    }
    nome[15] = '\0';
    strcat(nome, ".txt"); //Concatena com a extensao .txt o novo arquivo

    return nome;
}

bool Verifica_Alocacao_Arquivo(FILE *ptr)
{
    if (ptr == NULL) //Ternario: return ptr == NULL ? false : true;
        return false;
    return true;
}

//Escrever em arquivo de texto
void Escreve_Arquivo_Texto(ArvoreB *arvore)
{
    if (arvore == NULL) //O Ponteiro para a arvore nao foi criado e nao tem nenhuma area de memoria (malloc)
        return;
    
    FILE *arquivo = fopen(arvore -> nome, "w"); //Tenta abrir o arquivo com um ponteiro, no modo escrita
    if (!Verifica_Alocacao_Arquivo(arquivo))
    {
        printf("ERRO AO ACESSAR O ARQUIVO %s\n", arvore -> nome);
        return; //Termino o procedimento
    }

   //Printando o numero de chaves e se eh folha ou nao --> FPRINTF(local_print, "informacao", dados)
   fprintf(arquivo, "NUMERO DE CHAVES: %d\n EH NO FOLHA: %d\n", arvore -> num_chaves, arvore -> eh_folha);

   //Escrevendo os alunos nas chaves dos nós
   //Usaremos fprintf para printar as informacoes (gravar) em um arquivo
   for (int index = 0; index < arvore ->  num_chaves; index++) 
        fprintf(arquivo, "NOME: %s\nCURSO: %s\nMATRICULA: %d\n", arvore->chaves[index].nome, arvore->chaves[index].curso, arvore -> chaves[index].matricula);
    
    fclose(arquivo);

    //Se o no nao eh folha, teremos que guardar os demais filhos, ate chegar ao fim (a um caso base == no Folha)
    if (arvore -> eh_folha == false)
    {
        for (int index = 0; index <= arvore -> num_chaves; index++)
            Escreve_Arquivo_Texto(arvore->filhos[index]); //Grava cada filho daquele no
    }
}

//Funcoes presentes na arvoreB (Criar_No, Busca, insercao e remocao)

bool verifica_alocacao_arvore(ArvoreB *raiz)
{
    if (raiz == NULL) //Conseguiriamos fazer por ternario tambem: return raiz == NULL ? false : true;
        return false;
    return true;
}

ArvoreB *Criar_No()
{
    ArvoreB *novoNo = (ArvoreB*) malloc(sizeof(ArvoreB));

    if(!verifica_alocacao_arvore(novoNo))
    {
        printf("\nERRO NA ALOCACAO\n");
        exit(EXIT_FAILURE);
    }

    novoNo -> num_chaves = 0;
    novoNo -> eh_folha = true; //Todo no, eh, a priori, uma folha

    novoNo -> chaves = (Aluno*)malloc((2*T-1)*sizeof(Aluno)); //Para todas as chaves, aloca dinamicamente (devido à exigencia do enunciado)
    novoNo -> filhos = (ArvoreB**) malloc((2*T)*sizeof(ArvoreB*)); //Corrigido malloc: (2*T)*sizeof(ArvoreB*)

    for (int i = 0; i < 2*T; i++) //Seta todos os ponteiros (2T-1+1 = 2T) como NULL
        novoNo -> filhos[i] = NULL;

    strcpy(novoNo -> nome, Gerar_Nome_Aleatorio()); //O novoNo recebe um nome aleatorio
    Escreve_Arquivo_Texto(novoNo); //Escreve no disco o arquivo .txt contendo as informacoes do No

    return novoNo; //retorna um ponteiro para a nova área de memória criada
}

//Funcao para a busca, a chave utilizada sera a matricula (chave primaria, que nao se repete)
ArvoreB *Busca_Em_ArvoreB(ArvoreB *arvore, int matricula_procurada)
{
    if (arvore == NULL) //Caso base recursao
        return NULL; //Nao eh possivel realizar a busca em uma arvore que nao tem elemetnos

    //3 casos de busca - 1 esta no No presente, 2 nao esta no no e ja eh fim da arvore e 3 descer para o no filho
    int index = 0;
    while (index < arvore -> num_chaves && matricula_procurada > arvore -> chaves[index].matricula)
        index++; //percorre internamente aquele No
    
    //1 caso - encontramos a matricula naquele determinado no
    if (index < arvore -> num_chaves && matricula_procurada == arvore -> chaves[index].matricula)
        return arvore; //Retonarmos o No

    //2 caso - no folha
    if (arvore -> eh_folha == true) //Chegamos ao fim da árvore
        return NULL; //Elemento nao presente
    else
        return Busca_Em_ArvoreB(arvore->filhos[index], matricula_procurada); //Chama recursivamente o proximo No a ser procurado
}

//Funcao para dividir NoCheio (Particionameto)
void SplitChild(ArvoreB *No_Pai, int index_split) //REcebe o no pai do no que sera quebrado e o indice do filho cheio
{
    //Primeiramente, realocamos os ponteiros, ja que passamos o no Pai como parametro, e nao o no que sera quebrado
    ArvoreB *No_Filho = No_Pai -> filhos[index_split];

    //Agora, criaremos o NovoNo que guardara os elementos à direita da mediana (valores maiores que ela)
    ArvoreB *novoNo = Criar_No();
    novoNo -> eh_folha = No_Filho -> eh_folha; //Aprende com o outro no se é filho ou nao

    novoNo -> num_chaves = T - 1; //recebera T - 1 chaves

    //Sera necessario copiar do no cheio para o novoNo (valores maiores que a mediana)
    for (int index = 0; index < T - 1; index++)
        novoNo -> chaves [index] = No_Filho -> chaves [T + index]; //recebe os elementos a partir da mediana
    
    //Se o novoNo nao for folha, sera necessario realocar os ponteiros e depois a chave
    if (novoNo -> eh_folha == false)
    {
        for (int j = 0; j < T; j++) //realoca a partir de T ate 2T os ponteiros
            novoNo -> filhos[j] = No_Filho -> filhos [T + j]; //recebe os elementos maiores que a mediana (corrigido: usar No_Filho)
    }

    No_Filho -> num_chaves = T - 1; //reduz para a quantidade de valores menores que a mediana

    //Reajustaremos agora os ponteiros do no pai, e depois seus valores
    for (int index_pai = No_Pai -> num_chaves; index_pai >= index_split; index_pai--)//Percorrendo a arvore de tras ate chegar na posicao da mediana
        No_Pai -> filhos [index_pai + 1] = No_Pai -> filhos [index_pai]; //anda um elemento à frente (deslocamento)
    
    No_Pai -> filhos [index_split + 1] = novoNo; //Agora aponta para os elementos maiores que a mediana que vai subir

    //Reajustando as chaves do No Pai agora
    for (int index_pai = No_Pai -> num_chaves - 1; index_pai >= index_split; index_pai--) //corrigido limites do loop
        No_Pai -> chaves [index_pai + 1] = No_Pai -> chaves [index_pai];
    No_Pai -> chaves [index_split] = No_Filho -> chaves[T - 1]; //Quem vai subir é a mediana (corrigido: usar No_Filho)
    No_Pai -> num_chaves++;

    //Salvando as modificacoes em disco
    Escreve_Arquivo_Texto(No_Pai);
    Escreve_Arquivo_Texto(No_Filho);
    Escreve_Arquivo_Texto(novoNo);
}

void Insere_Em_No_Nao_Cheio (ArvoreB *arvore, Aluno aluno)
{
    //Inserir no folha, inserir em No nao folha nao cheio e inserir em No nao folha cheio

    int indice = arvore -> num_chaves - 1;

    if (arvore -> eh_folha)
    {
        while (indice >= 0 && aluno.matricula < arvore -> chaves[indice].matricula){
            arvore -> chaves [indice + 1] = arvore -> chaves [indice];
            indice--;
        }

        //Apos encontrar a posicao correta, ocuparemos pos_correta - 1 no vetor
        indice++; //Incrementando para adicionar na posicao certa
        arvore -> chaves [indice] = aluno;
        arvore -> num_chaves++;
        Escreve_Arquivo_Texto(arvore);
    }
    else //Nao eh no folha (ou seja, eh no interno) e precisamos encontrar o no filho correto
    {
        while (indice >= 0 && aluno.matricula < arvore -> chaves[indice].matricula)
            indice--; //decrementa o indice ate achar a posicao correta
        indice++; //a posicao correta eh indice + 1
        
        //Metodo CLRS --> quebra preventiva
        if (arvore -> filhos[indice]->num_chaves == 2*T - 1) //corrigido: checar num_chaves do filho
        {
            SplitChild(arvore, indice); //Quebra o no filho e sobe a mediana para o no pai

            if (aluno.matricula > arvore -> chaves[indice].matricula) //Encontra para qual no filho ira descer
                indice++;
        }
     Insere_Em_No_Nao_Cheio(arvore->filhos[indice], aluno);
    }
}

//Tratando casos de raizes
ArvoreB *Insere_Arvore_B(ArvoreB *arvore, Aluno aluno)
{
    //3 casos - sem raiz, raiz cheia e raiz nao cheia

    if (arvore == NULL)
    {
        arvore = Criar_No();
        arvore -> chaves[0] = aluno;
        arvore -> num_chaves++;
        return arvore;
    }

    if (arvore -> num_chaves == 2*T - 1)
    {
        ArvoreB *nova_raiz = Criar_No();
        nova_raiz -> eh_folha = false; //A nova raiz sera conectada a antiga raiz, nao sendo folha
        nova_raiz -> filhos[0] = arvore; //a nova raiz eh conectada à antiga raiz
        SplitChild(nova_raiz, 0); 
        //Agora, teremos que comparar se o no vai à direita ou à esquerda da nova raiz

        int flag_esq_ou_dir = 0;

        if (aluno.matricula > nova_raiz -> chaves[0].matricula)
            flag_esq_ou_dir++; //significa que a matricula eh maior que a matricula da mediana
        Insere_Em_No_Nao_Cheio(nova_raiz -> filhos[flag_esq_ou_dir], aluno); //corrigido: usar nova_raiz
        return nova_raiz; //retornar nova raiz
    }
    else //No interno nao esta cheio
    {
        Insere_Em_No_Nao_Cheio(arvore, aluno);
        return arvore;
    }
}

//Procedimento para imprimir IN ORDER (Em ordem) a Árvore B --> Esquerda, No_Pai, Direita
void Imprimir_In_Ordem (ArvoreB *arvore) //Sera recursivo para percorrermos toda a arvore
{
    if (arvore == NULL) //Caso base de recursao, chegamos a um filho NULL (Passamos de um NO folha)
        return;
    
    int index = 0; //Index para percorrer os nós da árvore
    for (index = 0; index < arvore -> num_chaves; index++) //Meu index tem que andar num_chaves + 1 filhos da arvore
    {
        Imprimir_In_Ordem(arvore -> filhos[index]); //Anda ate o extremo esquerdo (in order) da arvore
        printf("\n NOME: %s | CURSO: %s | MATRICULA: %d\n", arvore -> chaves[index].nome, arvore -> chaves[index].curso, arvore -> chaves[index].matricula);
    }
    Imprimir_In_Ordem (arvore -> filhos[index]); //Visita o filho À extrema direita (temos num_chaves + 1 ponteiros)
}

//Funcoes auxiliares para realizar a remoção 

//Encontra o indice de insercao (A primeira chave >= matricula a ser inserida)
int Encontra_Chave(ArvoreB *arvore, int matricula_procurada)
{
    int index = 0;
    while (index < arvore -> num_chaves && matricula_procurada > arvore -> chaves[index].matricula) //Anda com o índice internamente à arvore
        index++;
    
    return index;
}

//EM REMOCAO: 3 casos
//1 - Eh no folha
//2 - Eh no interno (filho esq > T chaves) | (filho dir > T chaves) | (filho esq == filho dir == T - 1 chaves)
//3 - Se chave nao esta no No, descemos para o proximo

//Funcoes auxiliares

//Funcao para obter o elemento predecessor ("Empresta a chave anterior")
Aluno Obtem_Predecessor(ArvoreB *No_Pai, int index) //Requisito 2 A
{
    ArvoreB *atual = No_Pai -> filhos [index]; //Anda para o no anterior
    while (atual -> eh_folha == false ) //enquanto nao for folha
        atual = atual -> filhos [atual->num_chaves]; //Vai para o extremo direito (último elemento)
    return atual -> chaves [atual -> num_chaves - 1]; //Retorna o elemento predecessor
}

//Funcao para obter o elemento posterior ("Empresta a proxima chave")
Aluno Obtem_Posterior(ArvoreB *No_Pai, int index) //Requisito 2B
{
    ArvoreB *atual = No_Pai -> filhos[index + 1]; //Anda para o filho à direita (filho maior)
    while (atual -> eh_folha == false) //Ando pela arvore enquanto nao eh folha
        atual = atual -> filhos[0]; //anda para o elemento mais à esquerda do no filho (imediatamente posterior)
    return atual -> chaves[0]; //Obtem o elemento imediatamente posterior ao que sera removido
}

//Funcao para encontrar o indice em que a matricula esta
//(Observacao: ja existe Encontra_Chave acima; mantive apenas uma versao)

//Para o caso 2 C, em que é necessario fundir dois nós, faremos o procedimento de FUSAO (merge)
// (filho esq == filho dir == T - 1 chaves)

void Merge(ArvoreB *No_Pai, int index)
{
    ArvoreB *filho = No_Pai -> filhos[index]; //Conecta o filho ao pai para nao perder o No
    ArvoreB *irmao = No_Pai -> filhos[index + 1];

    //A chave do pai desce para o filho
    filho -> chaves [T-1] = No_Pai -> chaves[index]; //O filho recebe a mediana que sera o no Pai

    //Copia a chave do irmao para o filho
    for (int i = 0; i < irmao -> num_chaves; i++)
        filho -> chaves [T + i] = irmao -> chaves [i]; //o filho recebera as chaves à direita da mediana (corrigido indice)

    //Se nao for folha, sera necessario copiar os filhos do irmao
    if (!filho->eh_folha)
    {
        for (int j = 0; j < irmao -> num_chaves + 1; j++)
            filho -> filhos [T + j] = irmao -> filhos[j]; //Copia os filhos do irmao para o no filho
    }

    filho -> num_chaves += irmao -> num_chaves + 1; //o numero de chaves do filho é incrementado e tambem recebera a mediana

    //Desvinculando as chaves do no pai
    //Movimenta chaves à direita de index para esquerda (shift left)
    for (int i = index + 1; i < No_Pai -> num_chaves; i++)
        No_Pai -> chaves [i - 1] = No_Pai -> chaves [i];

    //Movimenta ponteiros de filhos à direita para esquerda
    for (int j = index + 2; j <= No_Pai->num_chaves; j++)
        No_Pai -> filhos [j - 1] = No_Pai -> filhos [j];

    No_Pai -> num_chaves--;

    //Como movemos as chaves do no irmao (maiores que a mediana) podemos limpa-lo
    free(irmao->chaves); //Desaloca a area de memoria
    free(irmao->filhos); //Desaloca os ponteiros para os filhos --> Evita dangling pointers
    free(irmao); //Libera o espaco do no ocupado

    //Atualizando o arquivo
    Escreve_Arquivo_Texto(No_Pai);
    Escreve_Arquivo_Texto(filho);
}

//Funcoes para emprestar chave do irmao esquerdo
void Pegar_Emprestado_Anterior(ArvoreB *No_Pai, int idx)
{
    ArvoreB *filho = No_Pai->filhos[idx];
    ArvoreB *irmao = No_Pai->filhos[idx-1];

    // Move todas as chaves do filho para a direita (abrir espaço na posição 0)
    for (int i = filho->num_chaves - 1; i >= 0; i--)
        filho->chaves[i+1] = filho->chaves[i];

    if (!filho->eh_folha) {
        for (int i = filho->num_chaves; i >= 0; i--)
            filho->filhos[i+1] = filho->filhos[i];
    }

    // Coloca a chave do pai no primeiro elemento do filho
    filho->chaves[0] = No_Pai->chaves[idx-1];

    // Move a última chave do irmão para o pai
    No_Pai->chaves[idx-1] = irmao->chaves[irmao->num_chaves - 1];

    if (!irmao->eh_folha)
        filho->filhos[0] = irmao->filhos[irmao->num_chaves];

    filho->num_chaves += 1;
    irmao->num_chaves -= 1;

    Escreve_Arquivo_Texto(No_Pai);
    Escreve_Arquivo_Texto(filho);
    Escreve_Arquivo_Texto(irmao);
}

//Funcoes para emprestar chave do irmao direito
void Pegar_Emprestado_proximo(ArvoreB *No_Pai, int idx)
{
    ArvoreB *filho = No_Pai->filhos[idx];
    ArvoreB *irmao = No_Pai->filhos[idx+1];

    // Coloca a chave do pai no final do filho
    filho->chaves[filho->num_chaves] = No_Pai->chaves[idx];

    if (!filho->eh_folha)
        filho->filhos[filho->num_chaves + 1] = irmao->filhos[0];

    // Move a primeira chave do irmão para o pai
    No_Pai->chaves[idx] = irmao->chaves[0];

    // Shift left nas chaves do irmão
    for (int i = 1; i < irmao->num_chaves; i++)
        irmao->chaves[i-1] = irmao->chaves[i];

    if (!irmao->eh_folha) {
        for (int i = 1; i <= irmao->num_chaves; i++)
            irmao->filhos[i-1] = irmao->filhos[i];
    }

    filho->num_chaves++;
    irmao->num_chaves--;

    Escreve_Arquivo_Texto(No_Pai);
    Escreve_Arquivo_Texto(filho);
    Escreve_Arquivo_Texto(irmao);
}

//Agora, finalmente, a funcao de remocao de um elemento na arvore
//3 casos
void Remover_Matricula (ArvoreB **raiz_ref, ArvoreB *arvore, int matricula_removida)
{
    if (arvore == NULL) //Caso base da recursao --> Chegar ao fim da arvore ou arvore vazia
        return;

    int indice = Encontra_Chave(arvore, matricula_removida);

    //Caso a chave esteja no No em que encontramos
    if (indice < arvore -> num_chaves && matricula_removida == arvore -> chaves[indice].matricula)
    {
        //O No pode ser folha, mas tambem pode nao ser folha
        if (arvore -> eh_folha)
        {
            for (int i = indice + 1; i < arvore -> num_chaves; i++)
                arvore -> chaves [i - 1] = arvore -> chaves [i]; //Copia o proximo para o anteriro (corrigido indices)
            arvore -> num_chaves --; //Perdemos uma chave
            Escreve_Arquivo_Texto(arvore); //Atualiza o registro
            return;
        }
        else //Caso 2 - No interno: 3 subcasos
        {
            if (arvore -> filhos [indice] -> num_chaves >= T) //Caso 2A - filho esquerdo tem pelo menos T chaves
            {
                Aluno predecessor = Obtem_Predecessor(arvore, indice);
                arvore -> chaves [indice] = predecessor; //Insere o predecessor na ordem certa
                Remover_Matricula(raiz_ref, arvore->filhos[indice], predecessor.matricula); //remove o elemento que antes estava no no filho
                return;
            }
            else if (arvore -> filhos [indice + 1] -> num_chaves>= T) //Caso 2B - filho direito tem pelo menos T chaves
            {
                Aluno posterior = Obtem_Posterior(arvore, indice);
                arvore -> chaves [indice] = posterior;
                Remover_Matricula(raiz_ref, arvore->filhos[indice+1], posterior.matricula); //Recursivamente remove o elemento do no filho imediatamente à direita.
                return;
            }
            else //Caso os nós à direita e à esquerda tenham T - 1 nós (sera necessario fundi-los)
            {
                Merge(arvore, indice); //Junta os dois filhos em um, resultando em 2*T - 1 elementos
                // Depois do merge, o filho na posição 'indice' contém as chaves mescladas
                Remover_Matricula(raiz_ref, arvore -> filhos [indice], matricula_removida); //REmove a matricula do No fIlHO (NovoNo raiz)
                return;
            }
        }
    }
    //Caso 3 - Matricula nao esta nesse no, devemos descer na arvore para continuar procurando
    else
    {
        if (arvore -> eh_folha)
        {
            printf("\nMATRICULA NAO ENCONTRADA NA ARVORE\n");
            return;
        }

        // Descemos para o filho apropriado
        bool ultimo_filho = (indice == arvore->num_chaves);

        // Se o filho onde vamos descer tem menos do que T chaves, precisamos garantir que ele tenha pelo menos T antes de descer (pegando empresgado ou merge)
        if (arvore -> filhos[indice] -> num_chaves < T)
        {
            // Verifica se o irmap esquerdo pode emprestar
            if (indice > 0 && arvore->filhos[indice - 1]->num_chaves >= T)
            {
                Pegar_Emprestado_Anterior(arvore, indice); //Se sim, pega emprestado
            }
            // Verifica se o irmao direito pode emprestar
            else if (indice < arvore->num_chaves && arvore->filhos[indice + 1]->num_chaves >= T)
            {
                Pegar_Emprestado_proximo(arvore, indice); //Se sim, peha emprestado
            }
            // Caso nenhum irmão possa emprestar, fazemos merge (fusao)
            else
            {
                if (indice < arvore->num_chaves) //Merge com os filhos
                {
                    Merge(arvore, indice);
                }
                else
                {
                    Merge(arvore, indice - 1); //merge com os filhos d aposciao anterior
                    indice--; // Após merge, o filho correto é o anterior ?? (Ver com o Danillao)
                }
            }
        }

        //Caso especial: a raiz ficou sem chaves, entao ajustamos ponteiro da raiz externa
        if (*raiz_ref != NULL && (*raiz_ref)->num_chaves == 0) //Se minha raiz existe e nao tem elementos
        {
            ArvoreB *old_root = *raiz_ref;
            if (!old_root->eh_folha)
            {
                *raiz_ref = old_root->filhos[0];
            }
            else
            {
                *raiz_ref = NULL;
            }
        }

        // Depois de garantirmos que o filho tem >= T chaves, descemos recursivamente
        if (ultimo_filho && indice > arvore->num_chaves)
            Remover_Matricula(raiz_ref, arvore->filhos[indice - 1], matricula_removida);
        else
            Remover_Matricula(raiz_ref, arvore->filhos[indice], matricula_removida);
    }
}

//Funcao para liberar toda a arvore (recursiva)
void Free_Arvore(ArvoreB *raiz)
{
    if (raiz == NULL) 
        return;
    if (!raiz->eh_folha)  //Libera no por no, assim como em grafos que o cotonete comentou
    {
        for (int i = 0; i <= raiz->num_chaves; i++)
            Free_Arvore(raiz->filhos[i]);
    }

    free(raiz->chaves); 
    free(raiz->filhos);
    free(raiz);
}

char *geraCursoAleatorio()
{
    int resto = rand() % 5; //Restos possiveis == int [0,4]
    switch (resto)
    {
        case 0:
            return "COMPUTACAO";
            break;
        
        case 1:
            return "FISIOTERAPIA";
            break;
        
        case 2: 
            return "ARQUITETURA E URBANISMO";
            break;

        case 3:
            return "MATEMATICA";
            break;
        
        case 4:
            return "GEOGRAFIA";
            break;
        
        default: //Nao tem como ser default
            break;
    }
}

void GeraNomeBEEEEMaleatorio(Aluno *aluno)
{
    int random = rand() % 26; //Possiveis restos de 0 a 25

    for (int i = 0; i < 12; i++)
    {
        aluno->nome[i] = 'A' + random; //Gera letra maiuscula entre A e Z
        random = rand() % 26;
    }
    aluno->nome[12] = '\0'; //Concatena o caractere de fim de string
}

void geraMatriculaAleatoria(Aluno *aluno)
{
    int random = rand() % 1000; //Restos possiveis --> de 0 a 999
    aluno -> matricula = random;
}

//Funcao para gerar alunos aleatoriamente
Aluno *Gera_Aluno_Aleatorio()
{
    Aluno *pseudo_aluno = (Aluno*) malloc(sizeof(Aluno));

    if (!pseudo_aluno) //Verificando se foi alocado 
        exit(EXIT_FAILURE); //Caso haja erro (pseudoaluno == NULL)
    
    strcpy(pseudo_aluno->curso, geraCursoAleatorio()); //Copia o conteudo do curso aleatorio gerado para o novo aluno criado
    GeraNomeBEEEEMaleatorio(pseudo_aluno);
    geraMatriculaAleatoria(pseudo_aluno);

    return pseudo_aluno; //Retorna um ponteiro para a nova area de memoria alocada
}

// Função auxiliar para limpar o buffer de entrada
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
}

//Para prevenirmos a criacao de matriculas duplicadas (ja que isso afetaria a busca de chaves, retornando somente a primeir)
bool AhMatriculaExiste(ArvoreB *arvore, int matricula)
{
    if (arvore == NULL) //Chegamos em um no nulo
        return false; //Podemos inserir, pois a matricula nao exitse 
    ArvoreB *Onde_inserir = Busca_Em_ArvoreB(arvore, matricula);

    if (Onde_inserir == NULL) //A matricula nao existe
        return false; //Entao retornamos false (se for false, significa que poderemos inserir determinada chave primaria)
    else
        return true; //A matricula existe, logo, TRUE
}

int main()
{
    setlocale(LC_ALL, "Portuguese"); //Permitindo acentuacoes
    srand(time(NULL)); //Alimentando a seed de pseudoaleatório com o tempo atual

    printf("\n**** OBS: O VALOR T NOS INDICA A QUANTIDADE MINIMA (T-1) E MAXIMA (2*T-1) DE CHAVES EM UM NO****");
    printf("\nInsira o valor de T (grau minimo da arvore B eh t >= 2): ");
    scanf("%d", &T);

    while(T < 2) //Força a árvore ter, no mínimo, 3 filhos (T - 1 = 2 => t == 3 ponteiros no mínimo)
    {
        printf("\nNAO QUEREMOS UMA ARVORE DEGENERADA (LISTA ENCADEADA)!");
        printf("\nValor invalido para T. Insira um valor maior ou igual a 2: ");
        scanf("%d", &T);
    }

    ArvoreB *raiz = NULL; //Cria a raiz da árvore B (aloca memória) e referencia como NULL (Ainda não aponta para nada)

    int opcao = 0,
        qtde_aleatoria_alunos = 0,
        matricula_busca;

    Aluno aluno_inserir;
    
    ArvoreB *resultado_busca;

    int index;
    
    do {
        printf("\n\nx-x-x-x-x MENU ARVORE B x-x-x-x-x\n");

        printf("1 - Inserir um aluno\n");
        printf("2 - Buscar aluno por matricula\n");
        printf("3 - Remover aluno por matricula\n");
        printf("4 - Imprimir arvore EM ORDEM (Esquerda -> Pai -> Direita)\n");
        printf("5 - Gerar alunos aleatorios\n");
        printf("6 - Sair\nDigite qual opcao voce quer realizar: ");
        scanf("%d", &opcao);

        getchar(); //Para nao prejudicar a leitura de Strings (\n)

        switch(opcao)
        {
            case 1:
                printf("\nOPCAO 1 - INSERIR ALUNO \n");
                printf("Matricula: ");
                scanf("%d", &aluno_inserir.matricula);
                getchar();

                if (AhMatriculaExiste(raiz, aluno_inserir.matricula)) //Para evitar duplicatas
                {
                printf("ERRO! A matricula (chave unica) %d ja foi cadastrada... Insira outra chave para o aluno desejado.\n", aluno_inserir.matricula);
                break;
                }

                printf("Nome: ");
                fgets(aluno_inserir.nome, TAM_NOMES, stdin);
                aluno_inserir.nome[strcspn(aluno_inserir.nome, "\n")] = 0; // Remove o \n para a funcao limpabuffer() --> nao prejudica na proxima eitura
                printf("Curso: ");
                fgets(aluno_inserir.curso, TAM_NOMES, stdin);
                aluno_inserir.curso[strcspn(aluno_inserir.curso, "\n")] = 0; // Remove o \n para a funcao limpabuffer() --> nao prejudica na proxima leitura
                
                raiz = Insere_Arvore_B(raiz, aluno_inserir);
                printf("Aluno inserido com SUCESSO!!\n");
                break;
                
            case 2:
                printf("\nOPCAO 2 - BUSCAR ALUNO (MATRICULA)\n");
                printf("Digite a matricula: ");
                scanf("%d", &matricula_busca);
                limparBuffer(); //Para nao prejudicar caso haja uma leitura de string ou caractere
                
                resultado_busca = Busca_Em_ArvoreB(raiz, matricula_busca);
                if (resultado_busca != NULL) //Se eh !=  NULL, encontramos o no em que esta
                {
                    // Encontrar a posição dentro do nó
                    index = Encontra_Chave(resultado_busca, matricula_busca);
                    if (index < resultado_busca->num_chaves && 
                        resultado_busca->chaves[index].matricula == matricula_busca) //Criterios de achada a informacao
                    {
                        printf("\nALUNO ENCONTRADO:\n");
                        printf("Nome: %s\n", resultado_busca->chaves[index].nome);
                        printf("Curso: %s\n", resultado_busca->chaves[index].curso);
                        printf("Matricula: %d\n", resultado_busca->chaves[index].matricula);
                    }
                    else
                    {
                        printf("Aluno de matricula %d NAO encontrado!\n", matricula_busca); //Caso seja == NULL, nao esta presente na arvore
                    }
                }
                else //Se a arvore esta vazia, nao ha nada para ser encontrado ou buscado
                {
                    printf("Aluno com matricula %d nao encontrado!\n", matricula_busca);
                }
                break;
                
            case 3:
                printf("\nOPCAO 3 - REMOVER ALUNO (matricula)\n");
                printf("Digite a matricula: "); //A remocao ocorre por matricula pois eh ela que esta sendo meu referenciamento de insercao na arvore (ordenacao)
                scanf("%d", &matricula_busca);
                limparBuffer();
                Remover_Matricula(&raiz, raiz, matricula_busca);
                printf("Remocao REALIZADA!\n");
                break;
                
            case 4:
                printf("\nOPCAO 4 - IMPRIMIR ARVORE (MENOR, MEIO, MAIOR)\n"); //Em ordem
                if (raiz == NULL) //so EH POSSIVEL imprimir caso exista uma raiz
                    printf("Arvore vazia!\n");
                else
                    Imprimir_In_Ordem(raiz); //EM ordem
                break;
                
            case 5:
                printf("\nOPCAO 5 - GERA ALUNOS ALEATORIOS\n");
                printf("Quantidade de alunos a gerar (max 1000): "); //Coloquei o maximo 1000 pois a partir disso ja comecou a ficar mto lento esse programa
                scanf("%d", &qtde_aleatoria_alunos);
                limparBuffer();
                
                if (qtde_aleatoria_alunos > 1000)
                {
                    printf("OVERFLOW GARANTIDO DE CHAVES (MATRICULAS)! O LIMITE  1000.\n");
                    qtde_aleatoria_alunos = rand()%1000 + 1; // 001 a 1000
                    printf("GERANDO (ALEATORIAMENTE) A QUANTIDADE DE %d alunos", qtde_aleatoria_alunos + 1);
                }
                
                for (int i = 0; i < qtde_aleatoria_alunos; i++)
                {
                    Aluno *aluno_aleatorio = Gera_Aluno_Aleatorio();
                    while (AhMatriculaExiste(raiz, aluno_aleatorio->matricula))
                    {
                        free(aluno_aleatorio); //Limpa o antigo dado para nao estourar a memoria (ja que alocamos na heap por meio de ponteiors)
                        aluno_aleatorio = Gera_Aluno_Aleatorio(); //Continua gerando ate achar uma chave que nao esteja ocupada
                    }

                    raiz = Insere_Arvore_B(raiz, *aluno_aleatorio); //Se saiu do loop eh por que a matricula gerada foi uma chave unica
                    free(aluno_aleatorio); //Apos inserir, posso desalocar o ponteiro (informacao ja salva em arvore)
                }
                printf("%d alunos aleatorios inseridos\n", qtde_aleatoria_alunos);
                break;
                
            case 6:
                printf("\n...ENCERRANDO O PROGRAMA...\n");
                break;
                
            default:
                printf("\nOpcao INVALIDA! DIGITE UM NUMERO DE 1 A 6\n");
                break;
        }
        
    } while (opcao != 6);

    Free_Arvore(raiz); //Desalocando a memoria RAM que aquele determinado no ocupa
    
    return 0;
}