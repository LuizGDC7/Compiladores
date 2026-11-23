principal {
    x: int;
} {
    x = 100;
    escreva "Valor no bloco externo: ";
    escreva x;
    novalinha;

    {
        x: car;
    } {
        x = 'Z';
        escreva "Valor no bloco interno: ";
        escreva x;
        novalinha;
    }

    escreva "Valor de volta no bloco externo: ";
    escreva x;
    novalinha;
}
