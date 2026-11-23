#!/bin/bash
#
# [G-V2] Bateria de testes do compilador g-v2. Uso: ./run_tests.sh
#
#   1. testes/G_V2/corretos: devem compilar. Se houver simulador MIPS, o programa
#      também é executado com a entrada .in e a saída é comparada com .out.
#   2. testes/G_V2/erros_*: a saída deve ser exatamente a linha do .esperado.
#   3. Arquivos originais do professor: conferidos com originais.esperado.
#
# Simulador: usa a variável MIPS_SIM, se definida (comando que recebe o .asm como
# último argumento); caso contrário, usa o spim, se instalado.

GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

cd "$(dirname "$0")" || exit 1

BASE="testes/G_V2"
passed=0
failed=0
not_run=0

pass() { passed=$((passed + 1)); echo -e "  ${GREEN}PASSOU${NC} $1"; }
fail() { failed=$((failed + 1)); echo -e "  ${RED}FALHOU${NC} $1"; [ -n "$2" ] && echo "         $2"; }

if [ -n "$MIPS_SIM" ]; then
    SIM="$MIPS_SIM"
elif command -v spim > /dev/null 2>&1; then
    SIM="spim -quiet -file"
else
    SIM=""
fi

echo -e "${BLUE}=== Bateria de Testes do Compilador G-V2 ===${NC}"

echo -e "\n${YELLOW}[1/4] Compilando o projeto...${NC}"
if ! make > /dev/null 2>&1; then
    echo -e "${RED}ERRO: falha ao compilar o executável ./g-v2${NC}"
    exit 1
fi

TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

echo -e "\n${YELLOW}[2/4] Programas corretos${NC}"
for src in "$BASE"/corretos/*.g; do
    name=$(basename "$src" .g)
    asm="$TMP_DIR/$name.asm"
    if ! ./g-v2 "$src" > "$asm" 2>&1; then
        fail "$name" "$(head -1 "$asm")"
        continue
    fi
    if [ -z "$SIM" ]; then
        not_run=$((not_run + 1))
        pass "$name (somente compilação)"
        continue
    fi
    if timeout 20 $SIM "$asm" < "${src%.g}.in" > "$TMP_DIR/$name.saida" 2>&1 &&
       cmp -s "$TMP_DIR/$name.saida" "${src%.g}.out"; then
        pass "$name (compilação e execução)"
    else
        fail "$name" "saída da execução difere de ${src%.g}.out"
    fi
done

echo -e "\n${YELLOW}[3/4] Programas com erro${NC}"
for src in "$BASE"/erros_semanticos/*.g "$BASE"/erros_lexicos_sintaticos/*.g; do
    name=$(basename "$src" .g)
    expected=$(cat "${src%.g}.esperado")
    output=$(./g-v2 "$src" 2>&1)
    status=$?
    if [ $status -ne 0 ] && [ "$output" == "$expected" ]; then
        pass "$name -> $expected"
    else
        fail "$name" "esperado: '$expected' | obtido: '$(echo "$output" | head -1)'"
    fi
done

echo -e "\n${YELLOW}[4/4] Arquivos originais (sem alteração)${NC}"
while IFS='|' read -r file expected; do
    src="testes/$file"
    output=$(./g-v2 "$src" 2>&1)
    status=$?
    if [ "$expected" == "OK" ]; then
        if [ $status -eq 0 ]; then pass "$(basename "$src") -> OK"
        else fail "$(basename "$src")" "$(echo "$output" | head -1)"; fi
    elif [ $status -ne 0 ] && [ "$output" == "$expected" ]; then
        pass "$(basename "$src") -> $expected"
    else
        fail "$(basename "$src")" "esperado: '$expected' | obtido: '$(echo "$output" | head -1)'"
    fi
done < "$BASE/originais.esperado"

echo -e "\n${BLUE}=== Resultado: ${passed} passaram, ${failed} falharam ===${NC}"
if [ "$not_run" -gt 0 ]; then
    echo -e "${YELLOW}Aviso: ${not_run} programas não foram executados (instale o spim ou defina MIPS_SIM).${NC}"
fi
[ "$failed" -eq 0 ]
