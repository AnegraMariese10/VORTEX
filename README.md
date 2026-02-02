# VORTEX
Um framework para o descarregamento (offloading) de objetos detectados por veículos autônomos em um continuum de computação edge-cloud via redes LTE/4G e NR/5G, projetado para ser executado no ambiente de simulação integrado: OMNeT++ 5.6.2 como núcleo de eventos discretos, Simu5G 1.1.0 para modelagem da pilha 5G, INET 4.2.2 para os protocolos de rede, Veins 5.2 para acoplamento entre rede e mobilidade, e SUMO 1.8.0 para geração do tráfego veicular.

## Descrição

O VORTEX é uma heurística online e totalmente distribuída, projetada para aproximar o objetivo de otimização da primeira etapa, ou seja, maximizar o número de objetos descarregados durante seu tempo de vida útil. O VORTEX opera com informações incompletas e depende exclusivamente de parâmetros disponíveis localmente, como tamanho do objeto, tempo de vida útil restante, taxa de uplink instantânea e capacidade de armazenamento anunciada pelo nó de borda. O algoritmo toma decisões de descarregamento em nível de objeto em tempo real, possibilitando a implantação prática em cenários veiculares altamente dinâmicos.

## Cenário de Descarregamento Hierárquico

![Diagrama do VORTEX](https://github.com/AnegraMariese10/VORTEX/blob/main/vortex-overview.jpg)


A Figura ilustra o cenário de comunicação hierárquico considerado pelo VORTEX.  A arquitetura é composta por quatro camadas: veículos, estações base, nós de borda e uma nuvem centralizada. Os veículos descarregam os objetos detectados para uma estação base selecionada usando conectividade 4G/LTE ou 5G/NR. Cada estação base está conectada a um nó de borda por meio de um link de backhaul, enquanto os nós de borda estão conectados à nuvem através da rede central.

Após receber um objeto, a estação base o encaminha para o nó de borda associado. Antes de armazenar o objeto, o nó de borda verifica se já existe uma versão anterior do mesmo objeto, com base no tipo de objeto e na posição espacial aproximada. Se existir um registro anterior, a informação desatualizada é encaminhada para a nuvem e substituída pela observação mais recente. Esse mecanismo impede que dados obsoletos persistam na borda e garante que a percepção cooperativa seja suportada por informações atualizadas. Objetos cujo tempo de vida útil expirou são sistematicamente encaminhados para a nuvem, que se presume fornecer capacidade de armazenamento virtualmente ilimitada.


## Resultados


 A Figura abaixo apresenta a latência mediana (com IC de 95%) e o resultado do teste de Dunn, em dois painéis: à esquerda, dos objetos descarregados na borda; à direita, dos objetos descarregados na nuvem. A mediana foi adotada por se tratar de uma medida robusta à assimetria e à presença de valores extremos, comuns em métricas de latência. 

 ![Latência do VORTEX](latenciaMediana.png.png)

Considerando os objetos descarregados na borda, o VORTEX apresenta a menor latência mediana, com diferenças estatisticamente significativas em relação aos demais algoritmos ao nível de 5% (teste de Kruskal--Wallis seguido do pós-teste de Dunn). No gráfico da nuvem, as diferenças tornam-se ainda mais evidentes: o VORTEX mantém latência mediana significativamente inferior aos demais algoritmos ao nível de 5%, o que é consistente com a política de priorização de objetos com maior criticidade temporal e com a consequente redução de espera dos objetos que acabam sendo encaminhadas para a nuvem. 

## Pŕoximos Passos

* Modelos de aprendizado de máquina para prever a seleção dos objetos, baseada nas características.



