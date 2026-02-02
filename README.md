# VORTEX
Um framework para o descarregamento (offloading) de objetos detectados por veículos autônomos em um continuum de computação edge-cloud via redes LTE/4G e NR/5G, projetado para ser executado no ambiente de simulação integrado: OMNeT++ 5.6.2 como núcleo de eventos discretos, Simu5G 1.1.0 para modelagem da pilha 5G, INET 4.2.2 para os protocolos de rede, Veins 5.2 para acoplamento entre rede e mobilidade, e SUMO 1.8.0 para geração do tráfego veicular.

## Descrição

O VORTEX é uma heurística online e totalmente distribuída, projetada para aproximar o objetivo de otimização da primeira etapa, ou seja, maximizar o número de objetos descarregados durante seu tempo de vida útil. O VORTEX opera com informações incompletas e depende exclusivamente de parâmetros disponíveis localmente, como tamanho do objeto, tempo de vida útil restante, taxa de uplink instantânea e capacidade de armazenamento anunciada pelo nó de borda. O algoritmo toma decisões de descarregamento em nível de objeto em tempo real, possibilitando a implantação prática em cenários veiculares altamente dinâmicos.

## Representação

![Canva Presentation - 2026-02-01 115348](https://github.com/user-attachments/assets/79297041-1b92-41ba-b916-5cb205623be2)



A Figura ilustra o cenário de comunicação hierárquico considerado pelo VORTEX. 

