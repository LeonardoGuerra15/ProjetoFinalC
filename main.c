#include <stdio.h>
#include <stdlib.h>
#include <time.h> 


struct data {
    int dia;
    int mes;
    int ano;
    int hora;
    int minuto;
    int segundo;
};
typedef struct data Data;


struct movimentacao {
    float valor;
    char descricao[256];
    int cod;
    Data dat;
};
typedef struct movimentacao Movimentacao;

/* Transforma uma movimentacao em texto. */
char * movimentacao2str(Movimentacao * mov);
/* Leh do usuario os dados de um movimentacao. */
void readMovimentacao(Movimentacao * mov);
/* Retorna um struc tm com a data e hora locais. */
struct tm * getDataHora();
/* Verifica se uma Movimentacao eh do mes atual. */
int sameMonthLocaltime(Data dat);
/* Verifica se uma Movimentacao eh do ano atual. */
int sameYearLocaltime(Data dat);
/* Escreve relatorio mensal em HTML */
void geraRelatorioMensal(Movimentacao * movs, int quantidade);
/* Escreve relatorio anual em HTML */
void geraRelatorioAnual(Movimentacao * movs, int quantidade);

int main(int argc, char ** argv){
    Movimentacao mov[1000];
    int quantidadeMov = 0;
    float saldo = 0;
    int i;
    char * str;
    float saldoExtrato;

    int opcao = 0;
    while (opcao != 7){
        printf("GERENCIADOR FINANCEIRO\n\n1) Nova movimentacao.\n");
        printf("2) Gera relatorio anual.\n3) Gera relatorio mensal\n");
        printf("4) Saldo\n5) Extrato Mensal\n6) Extrato anual.\n");
        printf("7) Sair.\n# ");
        scanf(" %d", &opcao);

        if (opcao == 1){
            readMovimentacao(&(mov[quantidadeMov]));
            if (mov[quantidadeMov].cod != 5)
                mov[quantidadeMov].valor *=  -1;
            saldo += mov[quantidadeMov].valor;
            quantidadeMov++;
        } else if (opcao == 2){
            geraRelatorioAnual(mov, quantidadeMov);
        } else if (opcao == 3){
            geraRelatorioMensal(mov, quantidadeMov);
        } else if (opcao == 4){
            printf("Saldo = R$ %.2f\n\n", saldo);
        } else if (opcao == 5){
            printf("Extrato Mensal\n\n");
            saldoExtrato = 0;
            for (i=0; i<quantidadeMov; i++){
                if (sameMonthLocaltime(mov[i].dat) == 1){
                    str = movimentacao2str(&(mov[i]));
                    printf("%s\n", str);
                    free(str);
                    saldoExtrato += mov[i].valor;
                }
            }
            printf("Saldo do Mes = R$ %.2f\n\n", saldoExtrato);
        } else if (opcao == 6){
            printf("Extrato Anual\n\n");
            saldoExtrato = 0;
            for (i=0; i<quantidadeMov; i++){
                if (sameYearLocaltime(mov[i].dat) == 1){
                    str = movimentacao2str(&(mov[i]));
                    printf("%s\n", str);
                    free(str);
                    saldoExtrato += mov[i].valor;
                }
            }
            printf("Saldo do Ano = R$ %.2f\n\n", saldoExtrato);
        } 
    }
    printf("Encerrando...\n");

    return 0;
}

/* Transforma uma movimentacao em texto. */
char * movimentacao2str(Movimentacao * mov){
    char * str = malloc(sizeof(char)*512);
    char categoria[32];
    if (mov->cod == 1){
        sprintf(categoria, "%s", "Moradia");
    } else  if (mov->cod == 2){
        sprintf(categoria, "%s", "Estudos");
    } else  if (mov->cod == 3){
        sprintf(categoria, "%s", "Transporte");
    } else  if (mov->cod == 4){
        sprintf(categoria, "%s", "Alimentacao");
    } else  if (mov->cod == 5){
        sprintf(categoria, "%s", "Trabalho");
    }
    float valor =  mov->valor;
    if (mov->cod != 5)
        valor *= -1;
    sprintf(str, "%s da categoria %s em %02d/%02d/%04d no horario %02d:%02d:%02d = R$ %.2f"
        , mov->descricao,  categoria, mov->dat.dia, mov->dat.mes, mov->dat.ano
        , mov->dat.hora, mov->dat.minuto, mov->dat.segundo, valor);

    return str;
}

/* Leh do usuario os dados de um movimentacao. */
void readMovimentacao(Movimentacao * mov){
    printf("Entre com a descricao da movimentacao:\n# ");
    scanf(" %[^\n]", mov->descricao);
    printf("Entre com o valor: ");
    scanf(" %f", &(mov->valor));
    int cod = 0;
    while (cod < 1 || cod > 5){
        printf("Entre com a categoria.\n\n1) Moradia\n2) Estudos\n3) Transporte\n");
        printf("4) Alimentacao\n5) Trabalho\n# ");
        scanf(" %d", &cod);
        if (cod < 1 || cod > 5)
            printf("Codigo Invalido! Entre com um valor entre 1 e 5.\n\n");
    }
    mov->cod = cod;

    /* Captura a data e hora locais. */
    struct tm * timeinfo = getDataHora();

    mov->dat.dia = timeinfo->tm_mday;
    mov->dat.mes = timeinfo->tm_mon+1;
    mov->dat.ano = timeinfo->tm_year+1900;
    mov->dat.hora = timeinfo->tm_hour;
    mov->dat.minuto = timeinfo->tm_min;
    mov->dat.segundo = timeinfo->tm_sec;
}

struct tm * getDataHora(){
    time_t rawtime;
    time (&rawtime);
    return localtime (&rawtime);
}

/* Verifica se uma Movimentacao eh do mes atual. */
int sameMonthLocaltime(Data dat){
    struct tm * timeinfo = getDataHora();
    if (timeinfo->tm_mon+1 == dat.mes && timeinfo->tm_year+1900==dat.ano){
        return 1;
    } else {
        return 0;
    }
}

/* Verifica se uma Movimentacao eh do mes atual. */
int sameYearLocaltime(Data dat){
    struct tm * timeinfo = getDataHora();
    if (timeinfo->tm_year+1900==dat.ano){
        return 1;
    } else {
        return 0;
    }
}

/* Escreve relatorio mensal em HTML */
void geraRelatorioMensal(Movimentacao * movs, int quantidade){
    FILE * pFile;
    pFile = fopen ("mensal.html","w");
    if (pFile==NULL) {
        perror("Erro ao tentar escrever o arquivo \"mensal.html\"!");
    } else {
        fputs ("<!DOCTYPE html>\n\
            <html dir=\"ltr\" lang=\"pt-BR\">\n\
            <head>\n\
            <meta charset=\"utf-8\">\n\
            <title>Relatório Mensal</title>\n\
            <meta name=\"description\" content=\"GERENCIADOR FINANCEIRO - Relatório\">\n\
            <meta name=\"author\" content=\"Leonardo Guerra\">\n\
            </head>\n\
            <body>\n<h1>GERENCIADOR FINANCEIRO</h1><br><h2>Relatório Mensal</h2>", pFile);
        int i;
        char * linha;
        float saldo = 0;
        for (i=0; i<quantidade; i++){
            if (sameMonthLocaltime(movs[i].dat) == 1){
                linha = movimentacao2str(&(movs[i]));
                saldo += movs[i].valor;
                fprintf (pFile, "%s\n<br>",linha);
            }
        }
        fprintf(pFile, "Saldo do Mes = R$ %.2f\n<br>", saldo);
        fputs("\n</body>\n</html>", pFile);
        fclose (pFile);
    }
   
}

/* Escreve relatorio anual em HTML */
void geraRelatorioAnual(Movimentacao * movs, int quantidade){
    FILE * pFile;
    pFile = fopen ("anual.html","w");
    if (pFile==NULL) {
        perror("Erro ao tentar escrever o arquivo \"anual.html\"!");
    } else {
        fputs ("<!DOCTYPE html>\n\
            <html dir=\"ltr\" lang=\"pt-BR\">\n\
            <head>\n\
            <meta charset=\"utf-8\">\n\
            <title>Relatório Anual</title>\n\
            <meta name=\"description\" content=\"GERENCIADOR FINANCEIRO - Relatório\">\n\
            <meta name=\"author\" content=\"Leonardo Guerra\">\n\
            </head>\n\
            <body>\n<h1>GERENCIADOR FINANCEIRO</h1><br><h2>Relatório Anual</h2>", pFile);
        int i;
        char * linha;
        float saldo = 0;
        for (i=0; i<quantidade; i++){
            if (sameYearLocaltime(movs[i].dat) == 1){
                linha = movimentacao2str(&(movs[i]));
                saldo += movs[i].valor;
                fprintf (pFile, "%s\n<br>",linha);
            }
        }
        fprintf(pFile, "Saldo do Ano = R$ %.2f\n<br>", saldo);
        fputs("\n</body>\n</html>", pFile);
        fclose (pFile);
    }
   
}
