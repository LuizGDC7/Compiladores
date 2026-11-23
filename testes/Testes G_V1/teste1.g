principal {
    x, y, soma: int;
    c: car;
} {
    x = 10;
    y = 20;
    soma = x + y;
    escreva "Resultado da soma: ";
    escreva soma;
    novalinha;

    c = 'A';
    escreva "Caractere: ";
    escreva c;
    novalinha;

    se (soma > 15) entao
        escreva "Soma e maior que 15";
        novalinha;
    senao
        escreva "Soma nao e maior que 15";
        novalinha;
    fimse
}
