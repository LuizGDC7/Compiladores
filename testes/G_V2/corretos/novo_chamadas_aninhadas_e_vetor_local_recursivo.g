funcao [
    soma(a: int, b: int): int { retorne a + b; }
    um(): int { retorne 1; }
    desce(n: int): int [ v[2]: int; ] {
        v[0] = n;
        v[um()] = n * 2;
        se (n > 0) entao
            v[1] = v[1] + desce(n - 1);
        fimse
        retorne v[0] + v[1];
    }
]
principal [ w[3]: int; ] {
    w[soma(1, um())] = soma(soma(1, 2), soma(3, 4));
    escreva w[2]; novalinha;
    escreva desce(3); novalinha;
}
