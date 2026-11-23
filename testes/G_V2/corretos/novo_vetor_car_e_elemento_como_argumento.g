funcao [
    proximo(c: car): car { escreva c; retorne c; }
    idx(): int { retorne 1; }
]
principal [ s[3]: car; ] {
    s[0] = 'a'; s[idx()] = 'b'; s[2] = '\n';
    proximo(s[0]);
    proximo(s[1]);
    proximo(s[2]);
}
