# Simulação de interferência co-canal usando ns-3

Este projeto tem como objetivo simular e analisar o fenômeno de interferência co-canal em redes Wi-Fi, utilizando o simulador de redes ns-3. A proposta consiste em representar um cenário residencial composto por diferentes redes Wi-Fi operando em canais que podem causar interferência entre si, permitindo observar os impactos dessa interferência na comunicação entre os dispositivos.

A partir da simulação, foram analisadas métricas de desempenho da rede, como throughput, atraso e perda de pacotes, buscando compreender como a interferência entre redes que compartilham o mesmo canal pode afetar a qualidade da comunicação.

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

Execute o comando abaixo para preparar o projeto do ns3 para compilação habilitando exemplos e testes automatizados

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

Copiar pasta scripts, e arquivos da pasta `scratch` desse repositório para o seu projeto local do ns3

### 13. Executar simulação

```
./ns3 run "wifi-interferencia-co-canal --dist=1 --mesmoCanal=true --run=1"
```
onde `--dist=1` representa a distancia entre os APs e as Stations de ambas as redes, `--mesmoCanal=true` define se as redes utilizam o mesmo canal (true), ou canais diferente (false), e `--run=1` identifica a sequência de execução dentro da seed pre-fixada. Por padrão, `mesmoCanal=true`, `dist=1` e `run=1`.
