#!/bin/bash

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}===========================================${NC}"
echo -e "${BLUE}   Bateria de Testes do Compilador G-V1    ${NC}"
echo -e "${BLUE}===========================================${NC}"

# Recompilar compilador
echo -e "\n${YELLOW}[1/2] Compilando o projeto...${NC}"
make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo -e "${RED}ERRO: Falha ao compilar o executável ./g-v1${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Executável ./g-v1 gerado com sucesso!${NC}"

echo -e "\n${YELLOW}[2/2] Executando testes...${NC}\n"

test_files=(
    "testes/01_valido_basico.g"
    "testes/02_valido_escopo_aninhado.g"
    "testes/03_valido_laco_while.g"
    "testes/04_erro_lexico_caractere.g"
    "testes/05_erro_lexico_comentario.g"
    "testes/06_erro_lexico_string.g"
    "testes/07_erro_sintatico.g"
    "testes/08_erro_semantico_nao_declarada.g"
    "testes/09_erro_semantico_redeclarada.g"
    "testes/10_erro_semantico_tipo_incompativel.g"
)

for test in "${test_files[@]}"; do
    if [ ! -f "$test" ]; then
        continue
    fi

    filename=$(basename "$test")
    echo -e "${BLUE}▶ Testando: ${filename}${NC}"
    
    # Executar compilador
    output=$(./g-v1 "$test" 2>&1)
    status=$?

    if [[ "$filename" == *"valido"* ]]; then
        if [ $status -eq 0 ]; then
            echo -e "   ${GREEN}PASSOU: Compilação concluída com sucesso (Assembly MIPS gerado).${NC}"
        else
            echo -e "   ${RED}FALHOU: Esperava sucesso, mas ocorreu erro:${NC}"
            echo "   $output"
        fi
    else
        if [ $status -ne 0 ]; then
            echo -e "   ${GREEN}PASSOU: Erro esperado detectado com sucesso:${NC}"
            echo "   $output"
        else
            echo -e "   ${RED}FALHOU: Esperava erro, mas compilou sem erros.${NC}"
        fi
    fi
    echo ""
done

echo -e "${BLUE}===========================================${NC}"
echo -e "${GREEN}      Bateria de Testes Finalizada!        ${NC}"
echo -e "${BLUE}===========================================${NC}"
