global [ g[3]: int; ]
funcao [
    preenche(v[]: int, base: int): int [ i: int; ] {
        i = 0;
        enquanto (i < 3) { v[i] = base + i; i = i + 1; }
        retorne 0;
    }
]
principal [ a[3], b[3]: int; ] {
    preenche(g, 10);
    preenche(a, 20);
    preenche(b, 30);
    b = a;          /* b passa a apontar para os elementos de a */
    b[1] = 99;
    escreva a[1]; novalinha;
    a = g;          /* a passa a apontar para o vetor global */
    escreva a[2]; novalinha;
}
