funcao [
    fat(n: int): int {
        se (n <= 1) entao retorne 1; senao retorne n * fat(n-1); fimse
    }
]
principal [ x: int; ] {
    leia x;
    escreva fat(x);
    novalinha;
}
