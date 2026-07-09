# ShiftGL - Frameshift Mutation Visualizer

ShiftGL é uma aplicação educacional para visualizar os efeitos de mutações do tipo frameshift em sequências codificantes e em suas proteínas resultantes. O projeto combina uma interface gráfica em Java/Swing com um motor gráfico nativo em C++ usando OpenGL, GLFW e FreeType.

A aplicação recebe um identificador de transcrito no formato `NM\_...` e uma mutação em notação HGVS no formato `c....`, processa a alteração sobre a região CDS do transcrito, traduz a sequência de referência e a sequência mutante para proteína e exibe uma visualização comparativa da mutação.

## Sumário

* [Funcionalidades](#funcionalidades)
* [Estrutura do projeto](#estrutura-do-projeto)
* [Arquivos de dados](#arquivos-de-dados)
* [Dependências](#dependências)
* [Execução no Windows](#execução-no-windows)
* [Execução no Linux](#execução-no-linux)
* [Funcionamento geral](#funcionamento-geral)
* [Formatos aceitos de entrada](#formatos-aceitos-de-entrada)
* [Scripts auxiliares para Windows](#scripts-auxiliares-para-windows)
* [Solução de problemas](#solução-de-problemas)

## Funcionalidades

* Interface gráfica em Java/Swing.
* Entrada de ID de transcrito RefSeq no formato `NM\_...`.
* Entrada de mutação em notação HGVS começando por `c.`.
* Validação básica dos campos de entrada.
* Busca da sequência do transcrito em arquivo FASTA.
* Busca das posições CDS em arquivo TSV.
* Aplicação da mutação sobre a região codificante.
* Tradução da sequência CDS para sequência proteica.
* Comparação entre proteína de referência e proteína mutante.
* Detecção de frameshift com base na variação líquida de bases.
* Identificação da primeira posição divergente entre as proteínas.
* Visualização nativa com OpenGL:

  * sequência de referência;
  * sequência mutante;
  * códons;
  * aminoácidos;
  * ponto de divergência;
  * destaque de base inserida;
  * movimentação horizontal da câmera por barra de rolagem ou teclado.

## Estrutura do projeto

```text
frameshift-renderer/
├── java/
│   ├── data/
│   │   ├── Antonio-Bold.ttf
│   │   ├── codons.tsv
│   │   ├── refseqHumanFullNM.fasta        # necessário para processamento real
│   │   └── nm\_cds\_positions.tsv           # necessário para processamento real
│   │
│   └── src/
│       ├── jni/
│       │   └── OpenGLBridge.java
│       │
│       ├── main/
│       │   └── MainWindow.java
│       │
│       ├── model/
│       │   ├── FileLoader.java
│       │   ├── HGVSParser.java
│       │   ├── Mutation.java
│       │   ├── MutationType.java
│       │   ├── ProteinResult.java
│       │   ├── Transcript.java
│       │   └── Translator.java
│       │
│       └── service/
│           ├── MutationService.java
│           └── ValidationService.java
│
├── native/
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── engine.h
│   │   ├── graphics\_math.h
│   │   ├── jni\_OpenGLBridge.h
│   │   ├── shaders\_impl.h
│   │   ├── text\_renderer.h
│   │   ├── glad/
│   │   │   └── glad.h
│   │   └── KHR/
│   │       └── khrplatform.h
│   │
│   ├── resources/
│   │   └── fonts/
│   │       └── Antonio-Bold.ttf
│   │
│   ├── shaders/
│   │   ├── shape.frag
│   │   ├── shape.vert
│   │   ├── text.frag
│   │   └── text.vert
│   │
│   └── src/
│       ├── bridge.cpp
│       ├── glad.c
│       ├── renderer.cpp
│       └── text\_renderer.cpp
│
├── run.sh
├── build\_windows.bat
├── run\_windows.bat
├── run\_native\_test\_windows.bat
├── deps\_windows\_vcpkg.bat
└── clean\_windows.bat
```

### Camada Java

A pasta `java/src` contém a aplicação principal:

* `main/MainWindow.java`: janela principal da aplicação, campos de entrada, botões e área de resultado.
* `jni/OpenGLBridge.java`: ponte Java/JNI para carregar a biblioteca nativa `motor` e enviar dados ao renderizador OpenGL.
* `model/FileLoader.java`: leitura dos arquivos de dados em `java/data`.
* `model/HGVSParser.java`: interpretação da mutação HGVS.
* `model/Mutation.java`: representação interna da mutação.
* `model/MutationType.java`: enumeração dos tipos de mutação.
* `model/Transcript.java`: representação do transcrito, sequência e posições CDS.
* `model/Translator.java`: aplicação da mutação, tradução para proteína e comparação de divergências.
* `model/ProteinResult.java`: objeto de resultado enviado para a interface e para o renderizador.
* `service/MutationService.java`: fluxo central de processamento da mutação.
* `service/ValidationService.java`: validação dos formatos de entrada.

### Camada nativa C++

A pasta `native` contém o motor gráfico:

* `src/bridge.cpp`: implementação JNI chamada pelo Java.
* `src/renderer.cpp`: inicialização do GLFW/OpenGL e renderização da comparação visual.
* `src/text\_renderer.cpp`: renderização de texto via FreeType.
* `src/glad.c`: carregador OpenGL GLAD.
* `include/engine.h`: estrutura compartilhada dos dados de renderização.
* `include/jni\_OpenGLBridge.h`: cabeçalho JNI correspondente à classe Java `OpenGLBridge`.
* `shaders/`: shaders GLSL para formas e texto.
* `resources/fonts/Antonio-Bold.ttf`: fonte usada pelo renderizador nativo.

O CMake gera dois alvos principais:

* `motor`: biblioteca compartilhada carregada pelo Java via `System.loadLibrary("motor")`.

  * No Windows: `motor.dll`.
  * No Linux: `libmotor.so`.
* `ShiftGL\_Test`: executável nativo de teste para verificar a janela OpenGL sem depender da interface Java.

## Arquivos de dados

A aplicação procura os arquivos de dados a partir da pasta de trabalho `java`. Portanto, os arquivos devem estar em:

```text
java/data/
```

Arquivos esperados:

```text
java/data/codons.tsv
java/data/Antonio-Bold.ttf
java/data/refseqHumanFullNM.fasta
java/data/nm\_cds\_positions.tsv
```

### `codons.tsv`

Tabela de tradução de códons. Cada linha deve conter um códon e seu aminoácido, separados por tabulação:

```text
ATG	M
TAA	STOP
```

### `refseqHumanFullNM.fasta`

Arquivo FASTA contendo sequências de transcritos RefSeq. O carregador procura uma entrada cujo identificador comece por `>` e corresponda ao ID informado pelo usuário, por exemplo:

```text
>NM\_001037732.3 ...
ATG...
```

O código compara também a versão base do identificador. Assim, `NM\_001037732` pode corresponder a `NM\_001037732.3`.

### `nm\_cds\_positions.tsv`

Tabela com metadados de CDS. O código espera pelo menos quatro colunas separadas por tabulação:

```text
NM\_ID	NP\_ID	CDS\_START	CDS\_END
```

Exemplo:

```text
NM\_001037732.3	NP\_001032809.2	123	1456
```

### Observação importante

O programa pode compilar sem `refseqHumanFullNM.fasta` e `nm\_cds\_positions.tsv`, mas o processamento real de mutações depende desses arquivos. Sem eles, a interface pode abrir, mas o programa exibirá mensagens como:

```text
Transcrito não encontrado
Sequência não encontrada
```

## Dependências

### Dependências gerais

* Java Development Kit, preferencialmente JDK 17 ou superior.
* CMake 3.10 ou superior.
* Compilador C/C++ compatível com C++17.
* OpenGL 3.3 ou superior.
* GLFW 3.
* FreeType.

### Windows

Recomendado:

* Windows 10 ou Windows 11.
* JDK 17 ou 21 de 64 bits.
* CMake adicionado ao `PATH`.
* Visual Studio Build Tools 2022.
* Workload do Visual Studio: `Desktop development with C++`.
* MSVC v143 x64/x86 build tools.
* Windows 10 SDK ou Windows 11 SDK.
* vcpkg em `C:\\vcpkg`.
* Pacotes vcpkg:

  * `freetype:x64-windows`
  * `glfw3:x64-windows`

### Linux

Pacotes típicos em distribuições Debian, Ubuntu e Linux Mint:

```bash
sudo apt update
sudo apt install build-essential cmake openjdk-17-jdk libglfw3-dev libfreetype6-dev libgl1-mesa-dev mesa-common-dev
```

Em Fedora:

```bash
sudo dnf install gcc gcc-c++ cmake java-17-openjdk-devel glfw-devel freetype-devel mesa-libGL-devel
```

Em Arch Linux, Manjaro ou CachyOS:

```bash
sudo pacman -S base-devel cmake jdk17-openjdk glfw freetype2 mesa
```

## Execução no Windows

### 1\. Instalar Visual Studio Build Tools 2022

Instale o Visual Studio Build Tools 2022 e marque:

```text
Desktop development with C++
```

Garanta que estejam instalados:

```text
MSVC v143 - VS 2022 C++ x64/x86 build tools
Windows 10 SDK ou Windows 11 SDK
C++ CMake tools for Windows
```

### 2\. Instalar JDK

Instale um JDK de 64 bits. Exemplos:

* Eclipse Temurin JDK 17 ou 21.
* Microsoft Build of OpenJDK 17 ou 21.
* Oracle JDK 17 ou 21.

O diretório do JDK deve conter:

```text
bin\\java.exe
bin\\javac.exe
include\\jni.h
include\\win32\\jni\_md.h
```

Se necessário, defina `JAVA\_HOME`:

```powershell
$env:JAVA\_HOME="C:\\Program Files\\Java\\jdk-21"
```

Não use como `JAVA\_HOME`:

```text
C:\\Program Files\\Common Files\\Oracle\\Java
```

Esse caminho normalmente aponta para atalhos de execução da Oracle e não contém os headers JNI necessários.

### 3\. Instalar vcpkg

No PowerShell:

```powershell
cd C:\\
git clone https://github.com/microsoft/vcpkg
cd C:\\vcpkg
.\\bootstrap-vcpkg.bat
```

No PowerShell, scripts da pasta atual precisam ser chamados com `./` ou `.\\`. Por isso, use:

```powershell
.\\bootstrap-vcpkg.bat
```

em vez de:

```powershell
bootstrap-vcpkg.bat
```

### 4\. Instalar GLFW e FreeType via vcpkg

```powershell
cd C:\\vcpkg
.\\vcpkg install freetype:x64-windows glfw3:x64-windows
```

Verifique:

```powershell
.\\vcpkg list
```

A saída deve conter entradas como:

```text
freetype:x64-windows

glfw3:x64-windows
```

### 5\. Definir variáveis de ambiente no terminal

No mesmo terminal em que executará o projeto:

```powershell
$env:VCPKG\_ROOT="C:\\vcpkg"
$env:VCPKG\_TARGET\_TRIPLET="x64-windows"
```

Opcionalmente, para tornar `VCPKG\_ROOT` permanente:

```powershell
\[Environment]::SetEnvironmentVariable("VCPKG\_ROOT", "C:\\vcpkg", "User")
```

Depois disso, feche e abra o terminal novamente.

### 6\. Executar o projeto

Na raiz do projeto, onde ficam as pastas `java` e `native`:

```powershell
cd "C:\\Users\\OLMP\\Documents\\frameshift-renderer"
.\\clean\_windows.bat
.\\run\_windows.bat
```

Se tudo estiver correto, o script irá:

1. verificar a estrutura do projeto;
2. localizar CMake;
3. localizar um JDK real com headers JNI;
4. carregar o ambiente do Visual Studio Build Tools;
5. selecionar o gerador CMake adequado, normalmente `NMake Makefiles`;
6. usar o vcpkg em `C:\\vcpkg`;
7. compilar a biblioteca nativa `motor.dll`;
8. compilar as classes Java;
9. executar `main.MainWindow`.

A interface gráfica Java deverá abrir ao final do processo.

## Execução no Linux

No Linux, o projeto pode ser compilado e executado diretamente pelo script run.sh, localizado na raiz do repositório.



Antes de executar, garanta que o script tenha permissão de execução:



chmod +x run.sh



Em seguida, execute:



./run.sh



O script realiza as seguintes etapas:



compila a biblioteca nativa C++ na pasta native/build;

compila os arquivos Java na pasta java/build;

executa a interface principal da aplicação pela classe:

main.MainWindowCompilação manual no Linux

A partir da raiz do projeto:

```bash
cd native
mkdir -p build
cd build
cmake -DCMAKE\_BUILD\_TYPE=Release ..
cmake --build .
```

Depois compile o Java:

```bash
cd ../../java
mkdir -p build
find src -name "\*.java" > build/sources.txt
javac -encoding UTF-8 -d build @build/sources.txt
```

Execute a aplicação a partir da pasta `java`:

```bash
java -Djava.library.path=../native/build -cp build main.MainWindow
```

A execução a partir da pasta `java` é importante porque `FileLoader.java` procura os dados em:

```text
data/
```

ou seja, em:

```text
java/data/
```

### Script Linux sugerido

Caso queira atualizar o `run.sh`, use uma versão como esta:

```bash
#!/usr/bin/env bash
set -e

ROOT\_DIR="$(cd "$(dirname "$0")" \&\& pwd)"

cd "$ROOT\_DIR/native"
mkdir -p build
cd build
cmake -DCMAKE\_BUILD\_TYPE=Release ..
cmake --build .

cd "$ROOT\_DIR/java"
mkdir -p build
find src -name "\*.java" > build/sources.txt
javac -encoding UTF-8 -d build @build/sources.txt

java -Djava.library.path="$ROOT\_DIR/native/build" -cp build main.MainWindow
```

Dê permissão de execução:

```bash
chmod +x run.sh
./run.sh
```

## Funcionamento geral

O fluxo principal da aplicação é:

1. O usuário informa o ID do transcrito.
2. O usuário informa a mutação HGVS.
3. A interface chama `ValidationService` para validar os formatos básicos.
4. `MutationService` coordena o processamento.
5. `FileLoader` procura os metadados CDS no arquivo `nm\_cds\_positions.tsv`.
6. `FileLoader` procura a sequência correspondente no arquivo `refseqHumanFullNM.fasta`.
7. `HGVSParser` interpreta a mutação.
8. `Transcript` extrai a região CDS da sequência do transcrito.
9. `Translator` aplica a mutação sobre a CDS.
10. `Translator` traduz a CDS de referência e a CDS mutante em proteína.
11. `Translator` compara as duas proteínas e encontra posições divergentes.
12. O resultado aparece na interface Java.
13. `OpenGLBridge.prepare(...)` envia os dados para o motor nativo C++.
14. Ao clicar em `Ver Mutação`, a janela OpenGL é aberta.
15. O renderizador C++ mostra a comparação visual entre referência e mutante.

## Formatos aceitos de entrada

### ID do transcrito

O campo de transcrito aceita IDs no formato:

```text
NM\_123456
NM\_123456.1
NM\_001037732.3
```

A validação usada é equivalente a:

```regex
^NM\_\\d+(\\.\\d+)?$
```

### Mutação HGVS

O campo HGVS deve começar com:

```text
c.
```

O parser implementa os seguintes padrões principais:

### Inserção

```text
c.183\_184insA
```

Formato geral:

```text
c.<pos1>\_<pos2>ins<sequência>
```

### Deleção simples

```text
c.183del
c.183delA
```

Formato geral:

```text
c.<pos>del
c.<pos>del<sequência>
```

### Deleção de intervalo

```text
c.183\_186del
c.183\_186delATGC
```

Formato geral:

```text
c.<pos1>\_<pos2>del
c.<pos1>\_<pos2>del<sequência>
```

### Delins

```text
c.183\_186delATGCinsA
```

Formato geral:

```text
c.<pos1>\_<pos2>del<sequência\_deletada>ins<sequência\_inserida>
```

### Observações sobre frameshift

O projeto considera frameshift quando a variação líquida de bases não é múltipla de 3:

```text
número de bases inseridas - número de bases deletadas
```

Se esse valor não for divisível por 3, a mutação desloca a matriz de leitura.

## Scripts auxiliares para Windows

Os scripts `.bat` devem ficar na raiz do projeto, no mesmo nível de `java` e `native`.

### `run\_windows.bat`

Compila e executa a aplicação.

Uso:

```powershell
.\\run\_windows.bat
```

É o script principal para o usuário final.

### `build\_windows.bat`

Apenas compila o projeto, sem abrir a aplicação.

Uso:

```powershell
.\\build\_windows.bat
```

Ele compila:

* `motor.dll`;
* `ShiftGL\_Test.exe`;
* classes Java em `java/build`.

### `run\_native\_test\_windows.bat`

Compila e executa apenas o teste nativo OpenGL.

Uso:

```powershell
.\\run\_native\_test\_windows.bat
```

Esse script é útil para verificar se OpenGL, GLFW, FreeType e as DLLs nativas estão funcionando antes de testar a integração com Java.

### `deps\_windows\_vcpkg.bat`

Instala dependências nativas via vcpkg.

Uso:

```powershell
$env:VCPKG\_ROOT="C:\\vcpkg"
.\\deps\_windows\_vcpkg.bat
```

Ele instala:

```text
glfw3
freetype
```

no triplet configurado, normalmente:

```text
x64-windows
```

### `clean\_windows.bat`

Remove artefatos de build gerados pelos scripts Windows.

Uso:

```powershell
.\\clean\_windows.bat
```

Remove, quando existirem:

```text
java/build
native/build-windows-msvc-nmake
native/build-windows-msvc-ninja
native/build-windows-mingw
native/build-windows-mingw-ninja
native/build-windows-manual
native/build-windows-cmake-wrapper
.frameshift\_native\_build\_dir.txt
```

## Solução de problemas

### `Could NOT find Freetype`

Causa provável: FreeType não está instalado no vcpkg usado pelo CMake.

Solução:

```powershell
cd C:\\vcpkg
.\\vcpkg install freetype:x64-windows glfw3:x64-windows
```

Depois rode o projeto com:

```powershell
$env:VCPKG\_ROOT="C:\\vcpkg"
$env:VCPKG\_TARGET\_TRIPLET="x64-windows"
.\\clean\_windows.bat
.\\run\_windows.bat
```

### O CMake usa o vcpkg errado do Visual Studio

Se o erro mencionar um caminho como:

```text
C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/vcpkg
```

mas suas dependências foram instaladas em:

```text
C:\\vcpkg
```

force o vcpkg correto:

```powershell
Remove-Item Env:CMAKE\_TOOLCHAIN\_FILE -ErrorAction SilentlyContinue
$env:VCPKG\_ROOT="C:\\vcpkg"
$env:VCPKG\_TARGET\_TRIPLET="x64-windows"
.\\clean\_windows.bat
.\\run\_windows.bat
```

### `Unable to find a valid Visual Studio instance`

Causa provável: Visual Studio Build Tools não está instalado ou não tem o workload C++.

Instale:

```text
Visual Studio Build Tools 2022
Desktop development with C++
MSVC v143
Windows SDK
```

Depois feche e reabra o terminal.

### `nmake failed` ou `CMAKE\_CXX\_COMPILER not set`

Causa provável: CMake foi instalado, mas não há compilador C/C++ disponível no terminal.

Solução recomendada:

```text
Visual Studio Build Tools 2022 + Desktop development with C++
```

Alternativamente, use MSYS2/MinGW, mas nesse caso as dependências precisam estar instaladas para o mesmo ambiente MinGW.

### `Invalid character escape '\\P'` no `FindJNI.cmake`

Esse erro ocorre quando o CMake/JNI interpreta caminhos Windows com barras invertidas, como:

```text
C:\\Program Files\\...
```

Os scripts Windows fornecidos contornam esse problema passando os includes JNI explicitamente e normalizando caminhos. Use os scripts atualizados em vez de chamar diretamente o `CMakeLists.txt` nativo no Windows.

### `java.exe not found` ou `javac.exe not found`

Causa provável: há apenas um JRE ou o `JAVA\_HOME` aponta para um local incorreto.

Verifique:

```powershell
java -version
javac -version
```

Se necessário:

```powershell
$env:JAVA\_HOME="C:\\Program Files\\Java\\jdk-21"
.\\run\_windows.bat
```

O diretório deve conter:

```text
include\\jni.h
include\\win32\\jni\_md.h
bin\\java.exe
bin\\javac.exe
```

### `UnsatisfiedLinkError: no motor in java.library.path`

Causa provável: o Java não encontrou `motor.dll` ou `libmotor.so`.

No Windows, use:

```powershell
.\\run\_windows.bat
```

No Linux, execute a aplicação informando o caminho da biblioteca nativa:

```bash
cd java
java -Djava.library.path=../native/build -cp build main.MainWindow
```

### A interface abre, mas o processamento falha

Causa provável: arquivos de dados ausentes.

Confirme se existem:

```text
java/data/refseqHumanFullNM.fasta
java/data/nm\_cds\_positions.tsv
java/data/codons.tsv
```

Sem os arquivos FASTA e TSV de metadados CDS, o programa não consegue localizar o transcrito e a região codificante.

### A janela OpenGL não abre

Possíveis causas:

* driver de vídeo desatualizado;
* OpenGL 3.3 indisponível;
* DLLs do vcpkg não estão no `PATH`;
* `motor.dll` foi compilada, mas suas dependências não foram encontradas.

Teste primeiro:

```powershell
.\\run\_native\_test\_windows.bat
```

Se o teste nativo abrir, o problema provavelmente está na integração Java/JNI. Se não abrir, o problema está na camada C++/OpenGL.

## Observações de desenvolvimento

* A aplicação Java carrega a biblioteca nativa com:

```java
System.loadLibrary("motor");
```

* O nome da biblioteca deve ser:

  * `motor.dll` no Windows;
  * `libmotor.so` no Linux.
* A classe principal atual é:

```text
main.MainWindow
```

* O `run.sh` original do repositório pode precisar ser atualizado para essa classe principal.
* O diretório de trabalho recomendado para executar a interface Java é:

```text
java/
```

porque os arquivos de dados são carregados por caminho relativo:

```text
data/...
```

## 

