principal {
    a, b, res: int;
    letter: car;
} {
    a = 15;
    b = 5;
    res = a + b * 2;
    escreva "Resultado: ";
    escreva res;
    novalinha;

    letter = 'K';
    escreva "Caractere: ";
    escreva letter;
    novalinha;

    se (res > 20) entao {
        escreva "Sucesso: Resultado > 20";
        novalinha;
    } senao {
        escreva "Falha";
        novalinha;
    } fimse
}
