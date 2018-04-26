*******************
COMPILANDO NO LINUX
*******************

Entre no diretório do exemplo que você quer compilar e execute o seguinte comando

```
$> make
```

Para executar o programa gerado, execute o programa do seguinte modo:

```
$> ./<nome_do_programa>
```

onde <nome_do_programa> é o nome do executável gerado na compilação. O nome do executável gerado é a parte do nome do diretório após a underline. Por exemplo, se estiver compilando o exemplo 2 do capítulo 2, o diretório do exemplo tem o seguinte nome cap2_ex2, assim, o nome do executável será ex2.


*********************
COMPILANDO NO MACOSX
*********************

O procedimento para compilar os exemplos no MacOSX é parecido com o procedimento para se fazer isso no Linux. Contudo, ao executar o comando make, deve-se especificar a plataforma alvo, que é o Linux padrão. No geral, o comando para compilar é:

```
$> make PLAT=PLATAFORMA
```

onde PLATAFORMA pode ser macosx ou linux. Portanto, para MacOSX, o comando para compilação é:

```
$> make PLAT=macosx
```

A nomeação do executável gerado segue as mesmas regras do caso do Linux.

*********************************************
COMPILANDO NO WINDOWS COM MSYS2 (VERSÃO BETA)
*********************************************
A melhor solução para compilar este projeto em Windows, sem muito trabalho e sem necessidade de adaptação, é instalar o msys2  (https://www.msys2.org). Depois de instalado o msys2, pressione a tecla Win+R e execute o seguinte comando:

```
C:\msys64\msys2_shell.cmd -mingw32
```

Na janela que se abrirá, teremos o shell do msys2 em 32 bits. Neste shell, execute os seguintes comandos

```
$>pacman -Syuu
$>pacman -S mingw-w64-i686-gcc
$>pacman -S mingw-w64-i686-mesa
$>pacman -S mingw-w64-i686-freeglut
$>pacman -S make
```

Após a instalação dos pacotes acima, entre no diretório do exemplo que você quer compilar e execute o seguinte comando:

```
$>make -f Makefile.windows
```

Para limpar uma compilação existente, execute o seguinte comando:

```
$>make -f Makefile.windows clean
```

No caso do Windows, o executável gerado terá extensão ".exe".
Este procedimento foi testado apenas no Windows 10.

Pelo terminal do msys2, é criado um sistema de arquivos à parte parecido com o
sistema de arquivos de uma distribuição Linux. À primeira vista, parece impossível
acessar via terminal o sistema de arquivos do Windows. Mas isso é possível! Para
acessar a raiz da unidade C por meio do terminal do msys2, por exemplo, digite o seguinte comando:

```
cd /c
```
Qualquer dúvida entre contato com gilzamir_gomes@uvanet.br.
