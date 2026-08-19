# Simulação de interferência co-canal usando ns-3

Este projeto tem como objetivo simular e analisar o fenômeno de interferência co-canal em redes Wi-Fi, utilizando o simulador de redes ns-3. A proposta consiste em representar um cenário residencial composto por duas redes (A e B), compostas respectivamente por um acess point e uma estação, operando em canais que podem causar interferência entre si, permitindo observar os impactos dessa interferência na comunicação entre os dispositivos.

A partir da simulação, foram analisadas métricas de desempenho da rede, como throughput, atraso e taxa de perda de pacotes, buscando compreender como a interferência entre redes que compartilham o mesmo canal pode afetar a qualidade da comunicação.

               REDE A                                  REDE B

       STA1  ───────────  AP1  <────── d ──────>  AP2  ───────────  STA2
      estação          access point          access point          estação

## Instruções de execução

> [!IMPORTANT]
> Caso você já esteja utilizando um ambiente Linux nativamente, os passos relacionados à instalação do WSL e à extensão do VS Code não são necessários e podem ser pulados.

### 1. Instalar wsl
O WSL (Windows Subsystem for Linux) é um recurso que permite executar um ambiente Linux diretamente dentro do Windows, sem precisar instalar uma máquina virtual tradicional. Isso é essencial para o projeto porque o ns-3 é desenvolvido principalmente voltado para essa plataforma. Para instala-ló, abra o powershell com permissões de administrador e execute o seguinte comando

```
wsl --install
```

### 2. Instalar distribuição Ubuntu

Convencionalmente o comando padrão de instalação do wsl já instala automaticamente a distribuição Ubuntu, contudo, em alguns caso esse procedimento não é efetuado. Se o `wsl --install` não instalar automaticamente uma distribuição, é possível instala-la pela Microsoft Store ou pelo próprio powershell usando o comando:

```
wsl --install -d Ubuntu
```

### 3. Configurar usuário e senha
Após a conclusão da instalação do WSL e sua respectiva distribuição, o windows irá reiniciar. Ao retornar abrirá automaticamente o terminal da distribuição instalada solicitando nome de usuário e senha. Essas informações poderam ser requisitadas para executar os posteriores comandos e instalação das depedências necessárias

### 4. Atualizar sistema da distribuição

Dentro do terminal da distribuição instalada execute o comando abaixo para consultar a lista de pacotes atualizada e atualizar os pacotes pre instalados do sistema.

```
sudo apt update && sudo apt upgrade -y
```

### 5. Instalar ferramentas necessárias para compilar o ns-3

Execute o comando abaixo para instalar as dependências necessárias à compilação e execução de simulações em ns3:

```
sudo apt install -y build-essential git cmake ninja-build python3-dev libboost-all-dev
```

| Pacote | Descrição |
| --- | --- |
| build-essential | Instala ferramentas básicas de compilação, como gcc, g++ e make |
| git | Permite trabalhar com repositórios Git |
| cmake | Sistema utilizado para configurar a compilação do projeto |
| ninja-build | Ferramenta utilizada pelo sistema de build do ns-3 para realizar a compilação |
| python3-dev | Arquivos necessários para desenvolvimento/integração com Python |

### 6. Criar pasta para o ns3

```
cd ~
mkdir ns3-environment && cd ns3-environment
```

### 7. Baixar o ns3

```
wget https://www.nsnam.org/releases/ns-3.45.tar.bz2
```

### 8. Descompactar o arquivo do ns3

```
tar xfj ns-3.45.tar.bz2
```

### 9. Configurar o ns3

Após a conclusão do processo de descompactação, os arquivos do simulador ns3 ficarão armazenados internamente em `ns3-environment`, sobre a pasta `ns-3.45`. Utilize o comando `cd ns-3.45` para navegar até a pasta raiz do simulador, e execute o comando abaixo para preparar o projeto para compilação habilitando exemplos e testes automatizados

```
./ns3 configure --enable-examples --enable-tests
```

### 10. Compilar ns3

```
./ns3 build
```

A primeira compilação pode demorar bastante, especialmente em máquinas de poucos núcleos, contudo, depois de compilado, alterações em um arquivo de simulação normalmente não exigem recompilar todo o projeto.

### 11. Instalar extensão do WSL no VS Code

Ao utilizar uma distirbuição linux pelo WSL no windows é recomendável instalar a extensão WSL no vs code. Ela permite que o VS Code trabalhe diretamente com os arquivos e ferramentas do ambiente Linux, possibilitando usar o terminal integrado para intergir e executar os arquivos do projeto. Para abrir o projeto no vs code com a extensão instalada, execute via terminal o comando abaixo estando na pasta do projeto

```
. code
```

### 12. Copiar arquivos desse repositório

Copie a pasta `scratch` desse repositório para a pasta `ns-3.45` do seu projeto local. Ainda na pasta `ns-3.45`, crie uma nova pasta entitulada `scripts`, ela irá armazenar o arquivo .csv gerado contendo as métricas registradas pelas execuções das nossas simulações. Por fim, copie o arquivo `executar_simulacoes.sh` para a pasta `ns-3.45`, ele nos permitirá executar múltiplas simulaçõs variando aspectos dos cenários estabelecido a partir de um único comando no terminal.

### 13. Executar simulação

#### 1. Executar um cenário de simulação individual

Para executar apenas um cenário específico de simulação, a partir da pasta raiz do simulador efetue o seguinte comando:

```
./ns3 run "wifi-interferencia-co-canal --dist=1 --mesmoCanal=true --run=1"
```
onde `--dist` representa a distancia entre os APs e as Stations de ambas as redes, `--mesmoCanal` define se as redes utilizarão o mesmo canal (true), ou canais diferentes (false), para se comunicarem, e `--run` identifica a sequência de execução dentro da seed pre-fixada. Por padrão, senão específicados, `mesmoCanal=true`, `dist=1` e `run=1`.

A simulação utiliza uma seed fixada, permitindo aleatoriedade nas condições de propagação dos pacotes sem perder a reprodutibilidade. A alternância do valor de `--run` seleciona sequências diferentes, que podem produzir resultados distintos. Assim, é possível recuperar um resultado específico informando o mesmo número de `run` utilizado anteriormente.

> [!NOTE]
> A execução da simulação utilizando essa forma não permite armazenar suas métricas obtidas de forma automatizada em um arquivo csv. As métricas coletadas são apenas exibidas em um formato estruturado e simplificado no próprio terminal, facilitando a conferência de um único cenário de execução por vez.

#### 2. Executar múltiplas cenários de simulações pre configurados

O arquivo `executar_simulacoes.sh` automatiza a execução de vários cenários, comparando os resultados com e sem interferência co-canal em diferentes distâncias e com múltiplas repetições. Essa é a forma recomendada de executar as simulações porque reduz a necessidade de iniciar cada cenário manualmente, evita erros na configuração dos parâmetros e garante que todas as combinações sejam executadas de maneira padronizada. Além disso, o script registra automaticamente as métricas em um arquivo CSV, facilitando a organização, a comparação e uma posterior análise mais abrangente e consistente dos resultados.

Antes da primeira execução, conceda permissão de execução ao script:

```
chmod +x executar_simulacoes.sh
```

Em seguida, execute-o a partir da pasta raiz do ns-3, onde o arquivo foi copiado:

```
./executar_simulacoes.sh
```

O script possui os seguintes parâmetros pré-configurados:

- `TOTAL_RUNS=10`: executa cada cenário 10 vezes para cada distância, ajudando a obter resultados mais representativos.
- `DISTANCIAS=(1 2 3 5 7 10)`: define as distâncias, em metros, avaliadas entre as redes.

Para cada distância e rodada, o script executa os cenários com `--mesmoCanal=false` (sem interferência co-canal) e `--mesmoCanal=true` (com interferência co-canal), para os fluxos de transmissão de dados de ambas as redes (A e B). Com a configuração padrão, são realizadas 10 rodadas para cada uma das 6 distâncias, nos dois fluxos de dados, totalizando 240 simulações. Para alterar a quantidade de repetições ou as distâncias, edite `TOTAL_RUNS` e `DISTANCIAS` no início do arquivo antes de executá-lo.

## Retorno esperado

Apresentação simplificada, diretamente no terminal, das métricas coletadas durante a simulação, caso tenha optado pela execução de um cenário individual (forma 1), ou um arquivo CSV estruturado com as métricas obtidas de cada cenário, caso adotada a execução de múltiplos cenários (forma 2).