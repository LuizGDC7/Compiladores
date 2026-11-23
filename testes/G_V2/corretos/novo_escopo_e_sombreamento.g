global [ x: int; ]
funcao [
    f(a: int): int {
        a = a + 1;
        [ a: car; ] { a = 'z'; escreva a; novalinha; }
        retorne a;
    }
]
principal [ y: int; ] {
    x = 5;
    [ x: car; ] { x = 'k'; escreva x; novalinha; }
    y = f(x);
    escreva x; novalinha;
    escreva y; novalinha;
}
