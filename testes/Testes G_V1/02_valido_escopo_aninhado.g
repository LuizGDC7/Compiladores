principal {
    val: int;
} {
    val = 10;
    escreva "Escopo externo val: ";
    escreva val;
    novalinha;

    {
        val: car;
    } {
        val = 'X';
        escreva "Escopo interno val: ";
        escreva val;
        novalinha;
    }

    escreva "Escopo externo val de volta: ";
    escreva val;
    novalinha;
}
