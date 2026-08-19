# -------------------------------------------------------------
# CONFIGURAÇÃO DE QUANTIDADE DE EXECUÇÕES E DISTÂNCIAS
TOTAL_RUNS=10
DISTANCIAS=(1 2 3 5 7 10)
# -------------------------------------------------------------

echo "=== Iniciando automação de simulações ns-3 ==="
echo "Total de rodadas por distância: $TOTAL_RUNS"
echo "Distâncias avaliadas: ${DISTANCIAS[*]}"
echo "----------------------------------------------"

for DISTANCIA in "${DISTANCIAS[@]}"
do
    echo "=============================================="
    echo ">>> INICIANDO SIMULAÇÕES PARA DISTÂNCIA: ${DISTANCIA}m <<<"
    echo "=============================================="

    for (( run=1; run<=$TOTAL_RUNS; run++ ))
    do
        echo "=> [Distância: ${DISTANCIA}m] Rodada #$run de $TOTAL_RUNS..."

        # Executa o Cenário SEM interferência co-canal
        echo "   [Cenário] Mesmo Canal: FALSE"
        ./ns3 run "wifi-interferencia-co-canal --dist=$DISTANCIA --mesmoCanal=false --run=$run"

        # Executa o Cenário COM interferência co-canal
        echo "   [Cenário] Mesmo Canal: TRUE"
        ./ns3 run "wifi-interferencia-co-canal --dist=$DISTANCIA --mesmoCanal=true --run=$run"
        
        echo "----------------------------------------------"
    done
done

echo "=== Simulações concluídas e gravadas com sucesso! ==="